#include "zoom.h"

#include <d3d9.h>

#include <hddll/hd.h>
#include <hddll/hddll.h>
#include <hddll/memory.h>

// ---------------------------------------------------------------------------
// How zoom works
//
// gViewScale (>= 1.0) is the user-facing zoom factor. applyZoom() pushes it
// into several pieces of game state every frame; initZoom() puts persistent
// code patches in place so that scaling has effect. Each piece below has its
// own section in this file:
//
//   - Projection matrix: scaling g_flOrthoProjectionMatrix m[0]/m[5] by 1/Z
//     uniformly scales BOTH position and size of everything that goes through
//     the game's shader (HUD/menus included -- that's the known wart).
//   - Frustum culls: every render-path function compares world distance
//     against shared 12.0 / 7.0 half-viewport literals before drawing. We
//     redirect each cull site's operand to read our zoom-scaled pads.
//   - Composite + light quad scale: the composite blit and the per-light
//     screen quad both source their full-screen-quad scale from the shared
//     _DAT_00335cf0 / _DAT_00335c30. We redirect those reads so both quads
//     stretch with zoom in lockstep with the world.
//   - Composite-blit projection restore: the scaled projection from above
//     would shrink the now-larger composite quad right back to native size
//     in NDC. We hook composite_scene_to_backbuffer's entry and re-upload a
//     native-scale matrix so the world draws keep the scaled projection but
//     the composite blit gets native. The cf0/c30 redirect and this hook are
//     a coordinated PAIR -- removing either reintroduces the "shrinking
//     viewport" symptom.
//   - Scene textures: D3D_TEXTURE_1..7 are 1280x720 / 512x256 at native. We
//     recreate them at 2x or 4x as zoom rises so quality survives the
//     composite stretching them back over the window.
//   - Level light radius: type-177 entity's radius is in tile units; we
//     multiply it by gViewScale so the lit area keeps the same fraction of
//     the (now-wider) view.
//
// restoreZoom() reverts everything before the DLL unloads.
// ---------------------------------------------------------------------------

// Game's live IDirect3DDevice9 pointer. Ghidra symbol LPDIRECT3DDEVICE9_DEVICE
// @ 0x0034619c -> base-relative 0x14619c.
#define GAME_DEVICE_OFFSET 0x14619C

// ---- frustum-cull pads ----------------------------------------------------

// Zoom-scaled visibility-cull pads. The render path frustum-culls every tile /
// entity / particle against a fixed half-viewport: the shared literals 12.0
// (X) and 7.0 (Y). initZoom() redirects every cull site's operand to read
// these instead; applyZoom() scales them with the zoom. Native 12.0 / 7.0 ->
// no-op at 1.0x. Must keep stable addresses while patched -> file-scope.
static float gCullPadX = 12.0f;
static float gCullPadY = 7.0f;

// Every camera frustum-cull site in the render path. Each is a 6-byte x87
// instruction (D8 /0 FADD, D8 /3 FCOMP, D9 /0 FLD) whose disp32 operand (at
// instruction + 2) reads the shared 12.0 / 7.0 half-viewport literal. RVAs are
// base-relative; verified against Spelunky.exe 1.47.
struct CullSite {
  DWORD operandRva; // base-relative address of the disp32 operand
  bool isX;         // true -> gCullPadX (12.0), false -> gCullPadY (7.0)
};
static const CullSite gCullSites[] = {
    {0x8878C, true},  {0x88818, true}, // build_visible_entity_list (FADD)
    {0x887BA, false}, {0x88830, false},
    {0xBF69, true},   {0xBF9D, false},  // FUN_0020bec0  entity draw
    {0xC33F, true},   {0xC373, false},  // FUN_0020c290  entity draw
    {0xD4C0, true},   {0xD4F4, false},  // draw_entity   main entity draw
    {0xDC16, true},   {0xDC44, false},  // FUN_0020db60  entity-kind-4 draw
    {0xE2FA, true},   {0xE31E, false},  // FUN_0020e270  entity draw
    {0xFC1B, true},   {0xFC4C, false},  // draw_particle (FLD)
    {0xF09F, true},   {0xF0BE, false},  // build_fluid_vertex_buffer
    {0xF1AA, true},   {0xF1C9, false},  // build_background_geometry
    {0x10739, true},  {0x10768, false}, // build_floor_vertex_buffer
};
static const int gCullSiteCount = sizeof(gCullSites) / sizeof(gCullSites[0]);

// Original disp32 values captured before initZoom() redirected each site, so
// restoreZoom() can revert the game's code before the DLL unloads.
static DWORD gCullPatchOriginals[gCullSiteCount] = {0};
static bool gCullPatchApplied = false;

// Native projection-matrix scale, captured by applyZoom() on the first valid
// frame so the slider is relative to it and restoreZoom() can put it back.
static float gBaseProjM00 = 0.0f;
static float gBaseProjM11 = 0.0f;

// ---- composite + light quad upscale ---------------------------------------
//
// composite_scene_to_backbuffer and FUN_0020cd70 (the per-light "blit the
// light/shadow texture as a screen quad") both source their full-screen-quad
// scale from the shared _DAT_00335cf0 (X) / _DAT_00335c30 (Y). At zoom Z the
// world ends up rendered into the central 1/Z of the scene texture, so we
// scale these UP by Z to stretch that central 1/Z to fill the window. We
// redirect every cf0/c30 operand in the two functions to read our own
// zoom-scaled floats (ae0 is composite-only and we leave it alone).
//
// Pairs with the composite-blit projection restore below: scaling cf0/c30
// makes the quad bigger in world space, but the scaled projection would
// shrink it right back to native in NDC. The hook restores native projection
// just for the composite draw so the bigger quad actually renders bigger.
// Both pieces are required -- removing either reintroduces the "shrinking
// viewport" symptom. Without the cf0/c30 part of the pair specifically, the
// per-light shadow/glow quad stays at native size while the world is scaled
// and the shadow appears to shrink toward the center.
static const DWORD COMPOSITE_CF0_OPERAND_RVA =
    0x10DCA; // FMUL [335cf0] in composite
static const DWORD COMPOSITE_C30_OPERAND_RVA =
    0x10DD4; // FLD  [335c30] in composite
static const DWORD LIGHT_CF0_OPERAND_RVA =
    0xCDA3; // FLD  [335cf0] in FUN_0020cd70
static const DWORD LIGHT_C30_OPERAND_RVA =
    0xCDB4; // FLD  [335c30] in FUN_0020cd70
static const DWORD COMPOSITE_CF0_VALUE_RVA = 0x135CF0;
static const DWORD COMPOSITE_C30_VALUE_RVA = 0x135C30;
static float gCompositeScaleCf0 = 0.0f;
static float gCompositeScaleC30 = 0.0f;
static float gNativeCompositeCf0 = 0.0f;
static float gNativeCompositeC30 = 0.0f;
static DWORD gOriginalCompositeCf0Operand = 0;
static DWORD gOriginalCompositeC30Operand = 0;
static DWORD gOriginalLightCf0Operand = 0;
static DWORD gOriginalLightC30Operand = 0;
static bool gCompositeScalePatched = false;

// ---- scene render-target upscale ------------------------------------------
//
// The game's render-target textures are too small to keep the world sharp
// under projection scaling: at zoom Z the world fills only the central 1/Z of
// each texture, which then gets stretched to the window. The fix is to
// enlarge them so there are more texels to fill at every zoom level. We
// recreate them once on the first frame after init.
//
//   D3D_TEXTURE_1, 2: 1280x720, D3DFMT_X8R8G8B8 (0x16)  -- main scene buffers
//   D3D_TEXTURE_3:    1280x720, D3DFMT_A8R8G8B8 (0x15)  -- scene with alpha
//   D3D_TEXTURE_4:     512x256, D3DFMT_R5G6B5   (0x17)  -- lighting/shadow
//   D3D_TEXTURE_5, 7:  512x256, D3DFMT_X8R8G8B8 (0x16)  -- effect buffers
//   D3D_TEXTURE_6:     512x256, D3DFMT_A8R8G8B8 (0x15)  -- effect with alpha
//
// All are render targets (Usage = D3DUSAGE_RENDERTARGET) created in
// initialize_d3d9 with these exact dimensions.
//
// The scale is dynamic with zoom so we don't pay 4x fillrate / ~55-205MB of
// VRAM when the user isn't zoomed. Discrete steps with hysteresis to avoid
// thrashing as the slider crosses a threshold.

// Active scale. 1 = original game textures, no upscale, no overhead.
static int gCurrentSceneScale = 1;
// User cap on upscale (1, 2, or 4 -- 4 ≈ 205 MB total GPU memory across all
// 7 textures). Default 4 = "auto pick best for zoom". Declared extern in
// zoom.h so the debug UI can bind to it.
int gMaxSceneScale = 4;

struct SceneTextureSlot {
  DWORD texturePtrRva;
  DWORD surfacePtrRva;
  int nativeW;
  int nativeH;
  D3DFORMAT format;
  IDirect3DTexture9
      *originalTexture; // captured on upscale, swapped back on restore
  IDirect3DSurface9 *originalSurface;
};

static SceneTextureSlot gSceneSlots[] = {
    {0x15A55C, 0x15A560, 1280, 720, D3DFMT_X8R8G8B8, nullptr, nullptr}, // 1
    {0x15A584, 0x15A588, 1280, 720, D3DFMT_X8R8G8B8, nullptr, nullptr}, // 2
    {0x15A58C, 0x15A590, 1280, 720, D3DFMT_A8R8G8B8, nullptr, nullptr}, // 3
    {0x15A564, 0x15A568, 512, 256, D3DFMT_R5G6B5, nullptr, nullptr}, // 4 light
    {0x15A56C, 0x15A570, 512, 256, D3DFMT_X8R8G8B8, nullptr, nullptr}, // 5
    {0x15A574, 0x15A578, 512, 256, D3DFMT_A8R8G8B8, nullptr, nullptr}, // 6
    {0x15A57C, 0x15A580, 512, 256, D3DFMT_X8R8G8B8, nullptr, nullptr}, // 7
};
static const int gSceneSlotCount = sizeof(gSceneSlots) / sizeof(gSceneSlots[0]);

// ---- level-light radius scale ---------------------------------------------
//
// Entity type 177 (0xb1) is the level light -- the circle of visibility that
// follows the player on dark levels. Its radius (entity + 0x88) is in tile
// units. With zoom, the player sees more tiles, but the radius stays the
// same, so the lit area shrinks relative to the view. We scale it with
// gViewScale so the lit area keeps the same fraction of the view (and grows
// in tile units when zoomed out).
//
// Three render paths read this field:
//   - render_frame's per-light shadow-caster cull (decides which casters affect
//     this light)
//   - build_floor_vertex_buffer / FUN_002103e0 (sizes the glow quad)
//   - FUN_0020cd70 (uploads to the shader as part of the light's 4-float param)
// Modifying the field once covers all three consistently.
static const DWORD LEVEL_LIGHT_ENTITY_TYPE = 177;
static const DWORD LIGHT_RADIUS_OFFSET = 0x88;
static const DWORD ENTITY_TYPE_OFFSET = 0xC;

static hddll::Entity *gTrackedLight = nullptr;
static float gTrackedLightNativeRadius = 0.0f;
static float gTrackedLightLastSetValue = 0.0f;

static hddll::Entity *findLevelLightEntity() {
  if (!hddll::gGlobalState || !hddll::gGlobalState->entities)
    return nullptr;
  hddll::EntityStruct *es = hddll::gGlobalState->entities;
  uint32_t count = es->entities_active_count;
  if (count > 1280)
    count = 1280; // sanity clamp against garbage values
  for (uint32_t i = 0; i < count; i++) {
    hddll::Entity *e = es->entities_active[i];
    if (!e)
      continue;
    uint32_t type = *reinterpret_cast<uint32_t *>(reinterpret_cast<char *>(e) +
                                                  ENTITY_TYPE_OFFSET);
    if (type == LEVEL_LIGHT_ENTITY_TYPE)
      return e;
  }
  return nullptr;
}

// Snapshots / scales the level-light radius each frame. The native value is
// re-captured whenever the entity pointer changes (level transition / respawn)
// or whenever the game writes a value other than what we last set (some
// gameplay event changed the radius).
static void applyLevelLightScale() {
  hddll::Entity *light = findLevelLightEntity();
  if (!light) {
    gTrackedLight = nullptr;
    gTrackedLightNativeRadius = 0.0f;
    gTrackedLightLastSetValue = 0.0f;
    return;
  }

  float *radiusPtr = reinterpret_cast<float *>(reinterpret_cast<char *>(light) +
                                               LIGHT_RADIUS_OFFSET);
  float currentRadius = *radiusPtr;

  if (light != gTrackedLight) {
    gTrackedLight = light;
    gTrackedLightNativeRadius = currentRadius;
  } else if (currentRadius != gTrackedLightLastSetValue) {
    // Game updated the value (item pickup, scripted change, etc.) -- treat
    // whatever it set as the new native.
    gTrackedLightNativeRadius = currentRadius;
  }

  float newRadius = gTrackedLightNativeRadius * hddll::gViewScale;
  *radiusPtr = newRadius;
  gTrackedLightLastSetValue = newRadius;
}

static void restoreLevelLight() {
  if (gTrackedLight && gTrackedLightNativeRadius > 0.0f) {
    float *radiusPtr = reinterpret_cast<float *>(
        reinterpret_cast<char *>(gTrackedLight) + LIGHT_RADIUS_OFFSET);
    *radiusPtr = gTrackedLightNativeRadius;
  }
  gTrackedLight = nullptr;
  gTrackedLightNativeRadius = 0.0f;
  gTrackedLightLastSetValue = 0.0f;
}

// ---- composite-blit projection restore ------------------------------------
//
// g_flOrthoProjectionMatrix is uploaded each frame to the persistent shader
// param DAT_003472bc and used for EVERY draw -- composite_scene_to_backbuffer
// included. Left scaled, it shrinks the composite quad in NDC by 1/Z, exactly
// cancelling the Z stretch we just installed on cf0/c30 above. We hook
// composite_scene_to_backbuffer's entry and re-upload a native-scale matrix
// there so the world draws (earlier in render_frame) keep the scaled
// projection but the composite blit gets native -- the cf0/c30 stretch
// survives and the texture blits full-window.
//
// composite_scene_to_backbuffer @ RVA 0x10d70. First instruction is `SUB ESP,
// 0xd8` (6 bytes, relocation-safe -- no absolute addrs to fix up).
//
// IDirect3DXEffect *DAT_0035a520 -> RVA 0x15a520.    (the effect object)
// D3DXHANDLE       *DAT_003472bc -> RVA 0x1472bc.    (the projection param)
// ID3DXEffect::SetMatrixTranspose is at vtable offset 0xb0.
static const DWORD COMPOSITE_HOOK_RVA = 0x10D70;
static const int COMPOSITE_HOOK_LEN = 6;
static const DWORD EFFECT_PTR_RVA = 0x15A520;
static const DWORD PROJ_HANDLE_RVA = 0x1472BC;
static const int SET_MATRIX_TRANSPOSE_VTBL_OFFSET = 0xB0;
static DWORD gCompositeHookJmpBack = 0;
static bool gCompositeHookInstalled = false;

// Re-upload the native projection matrix to DAT_003472bc so the composite blit
// runs at native scale even though g_flOrthoProjectionMatrix is left scaled for
// the world pass.
static void restoreNativeProjectionForComposite() {
  if (!hddll::gProjectionMatrix || gBaseProjM00 == 0.0f)
    return;

  // Build a native-scale copy of the matrix (everything else identical).
  float native[16];
  memcpy(native, hddll::gProjectionMatrix, sizeof(native));
  native[0] = gBaseProjM00;
  native[5] = gBaseProjM11;

  void *effect =
      *reinterpret_cast<void **>(hddll::gBaseAddress + EFFECT_PTR_RVA);
  DWORD handle =
      *reinterpret_cast<DWORD *>(hddll::gBaseAddress + PROJ_HANDLE_RVA);
  if (!effect || !handle)
    return;

  // effect->vtbl->SetMatrixTranspose(effect, handle, native). __stdcall, 3
  // args.
  typedef long(__stdcall * SetMatrixTransposeFn)(void *, DWORD, const float *);
  void **vtbl = *reinterpret_cast<void ***>(effect);
  SetMatrixTransposeFn fn = reinterpret_cast<SetMatrixTransposeFn>(
      vtbl[SET_MATRIX_TRANSPOSE_VTBL_OFFSET / 4]);
  fn(effect, handle, native);
}

// Computes the desired texture upscale for a given view scale. Hysteresis on
// downscale only -- we want quality to follow zoom-out promptly, but not flap
// back to lower scale right at the threshold.
static int desiredSceneScaleForZoom(float viewScale) {
  int target;
  if (viewScale >= 2.5f)
    target = 4;
  else if (viewScale >= 1.25f)
    target = 2;
  else
    target = 1;

  // Hysteresis: hold the current higher scale until the zoom drops 0.15 below
  // the threshold so the slider can hover at the boundary without thrashing.
  if (target < gCurrentSceneScale) {
    if (gCurrentSceneScale == 4 && viewScale >= 2.35f)
      target = 4;
    else if (gCurrentSceneScale == 2 && viewScale >= 1.1f)
      target = 2;
  }

  if (target > gMaxSceneScale)
    target = gMaxSceneScale;
  return target;
}

// Switches the live scene render-target scale to the given value. scale == 1
// means "use the game's original textures, no upscale". scale > 1 means
// "create new upscaled render targets and put them in the globals". Called
// from applyZoom() between frames, so no swap happens mid-render.
static void setSceneTextureScale(int scale) {
  if (scale == gCurrentSceneScale)
    return;

  IDirect3DDevice9 *device = *reinterpret_cast<IDirect3DDevice9 **>(
      hddll::gBaseAddress + GAME_DEVICE_OFFSET);
  if (!device)
    return;

  for (int i = 0; i < gSceneSlotCount; i++) {
    SceneTextureSlot &slot = gSceneSlots[i];
    IDirect3DTexture9 **pTex = reinterpret_cast<IDirect3DTexture9 **>(
        hddll::gBaseAddress + slot.texturePtrRva);
    IDirect3DSurface9 **pSrf = reinterpret_cast<IDirect3DSurface9 **>(
        hddll::gBaseAddress + slot.surfacePtrRva);

    // If we were at the original scale, capture the originals before we
    // replace them so we can restore them later. We only do this on the first
    // transition off scale 1 -- once captured they stay captured.
    if (gCurrentSceneScale == 1 && slot.originalTexture == nullptr) {
      slot.originalTexture = *pTex;
      slot.originalSurface = *pSrf;
    } else if (*pTex != slot.originalTexture) {
      // Currently holds an upscaled texture we own; release it. After an
      // earlier scale>1 -> 1 transition, the slot's *pTex IS the captured
      // original again -- skip the Release in that case so we don't drop a
      // refcount we don't own. We'll just overwrite *pTex with the new
      // upscaled texture below, leaving the original safely held only by
      // slot.originalTexture until we need it again.
      if (*pSrf)
        (*pSrf)->Release();
      if (*pTex)
        (*pTex)->Release();
    }

    if (scale == 1) {
      // Switch back to the originals.
      *pTex = slot.originalTexture;
      *pSrf = slot.originalSurface;
      continue;
    }

    // Create at the new scale.
    IDirect3DTexture9 *newTex = nullptr;
    if (FAILED(device->CreateTexture(slot.nativeW * scale, slot.nativeH * scale,
                                     1, D3DUSAGE_RENDERTARGET, slot.format,
                                     D3DPOOL_DEFAULT, &newTex, nullptr))) {
      // Out of VRAM or unsupported size -- fall back to the originals so the
      // game keeps running.
      *pTex = slot.originalTexture;
      *pSrf = slot.originalSurface;
      gCurrentSceneScale = 1;
      gMaxSceneScale = 1; // don't keep trying
      return;
    }
    IDirect3DSurface9 *newSrf = nullptr;
    if (FAILED(newTex->GetSurfaceLevel(0, &newSrf))) {
      newTex->Release();
      *pTex = slot.originalTexture;
      *pSrf = slot.originalSurface;
      gCurrentSceneScale = 1;
      return;
    }
    *pTex = newTex;
    *pSrf = newSrf;
  }

  gCurrentSceneScale = scale;
}

// Convenience: pick the right scale for the current zoom.
static void updateSceneTextureScale() {
  setSceneTextureScale(desiredSceneScaleForZoom(hddll::gViewScale));
}

// Releases any upscaled textures and puts the originals back into the game's
// globals. Call before the DLL unloads.
static void restoreSceneTextures() {
  setSceneTextureScale(1);
  // Forget the captured originals so a re-attach starts fresh.
  for (int i = 0; i < gSceneSlotCount; i++) {
    gSceneSlots[i].originalTexture = nullptr;
    gSceneSlots[i].originalSurface = nullptr;
  }
}

static void __declspec(naked) hookCompositeStart() {
  __asm {
    ; stolen bytes: SUB ESP, 0xd8 (the original functions stack reserve)
    sub esp, 0xd8
    pushad
  }
  restoreNativeProjectionForComposite();
  __asm {
    popad
    jmp [gCompositeHookJmpBack]
  }
}

// ---------------------------------------------------------------------------

void initZoom() {
  auto process = GetCurrentProcess();

  // Redirect every frustum-cull operand at our zoom-scaled pads. Save the
  // originals first -- these point into this DLL and MUST be reverted
  // (restoreZoom) before the DLL unloads.
  DWORD cullPadXAddr = reinterpret_cast<DWORD>(&gCullPadX);
  DWORD cullPadYAddr = reinterpret_cast<DWORD>(&gCullPadY);
  for (int i = 0; i < gCullSiteCount; i++) {
    DWORD site = hddll::gBaseAddress + gCullSites[i].operandRva;
    gCullPatchOriginals[i] = *reinterpret_cast<DWORD *>(site);
    DWORD padAddr = gCullSites[i].isX ? cullPadXAddr : cullPadYAddr;
    hddll::patchReadOnlyCode(process, site, &padAddr, 4);
  }
  gCullPatchApplied = true;

  // Capture native composite scale constants and redirect the operands so the
  // composite quad AND the per-light shadow/glow quad both scale with the zoom.
  gNativeCompositeCf0 =
      *reinterpret_cast<float *>(hddll::gBaseAddress + COMPOSITE_CF0_VALUE_RVA);
  gNativeCompositeC30 =
      *reinterpret_cast<float *>(hddll::gBaseAddress + COMPOSITE_C30_VALUE_RVA);
  gCompositeScaleCf0 = gNativeCompositeCf0;
  gCompositeScaleC30 = gNativeCompositeC30;
  DWORD cf0Addr = reinterpret_cast<DWORD>(&gCompositeScaleCf0);
  DWORD c30Addr = reinterpret_cast<DWORD>(&gCompositeScaleC30);
  gOriginalCompositeCf0Operand = *reinterpret_cast<DWORD *>(
      hddll::gBaseAddress + COMPOSITE_CF0_OPERAND_RVA);
  gOriginalCompositeC30Operand = *reinterpret_cast<DWORD *>(
      hddll::gBaseAddress + COMPOSITE_C30_OPERAND_RVA);
  gOriginalLightCf0Operand =
      *reinterpret_cast<DWORD *>(hddll::gBaseAddress + LIGHT_CF0_OPERAND_RVA);
  gOriginalLightC30Operand =
      *reinterpret_cast<DWORD *>(hddll::gBaseAddress + LIGHT_C30_OPERAND_RVA);
  hddll::patchReadOnlyCode(
      process, hddll::gBaseAddress + COMPOSITE_CF0_OPERAND_RVA, &cf0Addr, 4);
  hddll::patchReadOnlyCode(
      process, hddll::gBaseAddress + COMPOSITE_C30_OPERAND_RVA, &c30Addr, 4);
  hddll::patchReadOnlyCode(process, hddll::gBaseAddress + LIGHT_CF0_OPERAND_RVA,
                           &cf0Addr, 4);
  hddll::patchReadOnlyCode(process, hddll::gBaseAddress + LIGHT_C30_OPERAND_RVA,
                           &c30Addr, 4);
  gCompositeScalePatched = true;

  // Install the composite-blit hook so the final scene->window blit uses native
  // projection (otherwise scaling g_flOrthoProjectionMatrix shrinks that blit
  // too, which is the "shrinking viewport" we keep seeing).
  gCompositeHookJmpBack =
      hddll::gBaseAddress + COMPOSITE_HOOK_RVA + COMPOSITE_HOOK_LEN;
  gCompositeHookInstalled = hddll::hook(
      reinterpret_cast<void *>(hddll::gBaseAddress + COMPOSITE_HOOK_RVA),
      hookCompositeStart, COMPOSITE_HOOK_LEN);
}

void applyZoom() {
  if (!hddll::gProjectionMatrix)
    return;

  // The game builds this orthographic matrix once at startup and re-uploads it
  // to the shader every frame, so writing it here takes effect next frame.
  // m[0] = 2/viewWidth, m[5] = 2/viewHeight; dividing both by the view scale
  // widens the view box -> zoom out. Capture the native values once so the
  // slider is relative to them and 1.0x is an exact no-op.
  if (gBaseProjM00 == 0.0f) {
    if (hddll::gProjectionMatrix[0] == 0.0f)
      return; // matrix not built by the game yet
    gBaseProjM00 = hddll::gProjectionMatrix[0];
    gBaseProjM11 = hddll::gProjectionMatrix[5];
  }

  if (hddll::gViewScale < 1.0f)
    hddll::gViewScale = 1.0f;

  hddll::gProjectionMatrix[0] = gBaseProjM00 / hddll::gViewScale;
  hddll::gProjectionMatrix[5] = gBaseProjM11 / hddll::gViewScale;

  // Grow the frustum-cull pads with the zoom. Geometry is culled in tile space,
  // so these stay tile-based; the wider view needs more of it kept.
  gCullPadX = 12.0f * hddll::gViewScale;
  gCullPadY = 7.0f * hddll::gViewScale;

  // Composite quad stretch + upscaled scene textures together: the world is
  // rendered at higher effective resolution into the 2x texture, then the
  // composite stretches the central 1/Z of that texture to fill the window.
  // The per-light shadow/glow quad in FUN_0020cd70 also reads these (cf0, c30)
  // so they stretch in lockstep with composite, keeping shadows the right
  // size relative to the world.
  gCompositeScaleCf0 = gNativeCompositeCf0 * hddll::gViewScale;
  gCompositeScaleC30 = gNativeCompositeC30 * hddll::gViewScale;

  // Pick the right scene-texture scale for the current zoom. No-op at
  // viewScale == 1.0 (originals stay in place; no extra VRAM / fillrate). The
  // call happens here in onFrame -- at EndScene, between frames -- so the
  // texture swap is never mid-render.
  updateSceneTextureScale();

  // Scale the level-light radius so its lit area scales with the view.
  applyLevelLightScale();
}

void restoreZoom() {
  // Revert the projection matrix so the game is not left zoomed after detach.
  if (hddll::gProjectionMatrix && gBaseProjM00 != 0.0f) {
    hddll::gProjectionMatrix[0] = gBaseProjM00;
    hddll::gProjectionMatrix[5] = gBaseProjM11;
  }

  // Remove the composite-blit hook.
  if (gCompositeHookInstalled) {
    hddll::unhook(
        reinterpret_cast<void *>(hddll::gBaseAddress + COMPOSITE_HOOK_RVA));
    gCompositeHookInstalled = false;
  }

  // Restore the level light's native radius before we walk away.
  restoreLevelLight();

  // Restore the original scene render targets (releases the upscaled ones).
  restoreSceneTextures();

  // Revert composite + light scale operands.
  if (gCompositeScalePatched) {
    auto process = GetCurrentProcess();
    hddll::patchReadOnlyCode(process,
                             hddll::gBaseAddress + COMPOSITE_CF0_OPERAND_RVA,
                             &gOriginalCompositeCf0Operand, 4);
    hddll::patchReadOnlyCode(process,
                             hddll::gBaseAddress + COMPOSITE_C30_OPERAND_RVA,
                             &gOriginalCompositeC30Operand, 4);
    hddll::patchReadOnlyCode(process,
                             hddll::gBaseAddress + LIGHT_CF0_OPERAND_RVA,
                             &gOriginalLightCf0Operand, 4);
    hddll::patchReadOnlyCode(process,
                             hddll::gBaseAddress + LIGHT_C30_OPERAND_RVA,
                             &gOriginalLightC30Operand, 4);
    gCompositeScalePatched = false;
  }

  // Revert every cull-site operand to its original disp32. They currently point
  // into this DLL; leaving them in place would dangle the instant the DLL
  // unloads and crash the game on the next frame.
  if (gCullPatchApplied) {
    auto process = GetCurrentProcess();
    for (int i = 0; i < gCullSiteCount; i++) {
      hddll::patchReadOnlyCode(process,
                               hddll::gBaseAddress + gCullSites[i].operandRva,
                               &gCullPatchOriginals[i], 4);
    }
    gCullPatchApplied = false;
  }
}
