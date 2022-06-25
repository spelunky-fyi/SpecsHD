
#include <Windows.h>
#include <algorithm>
#include <format>
#include <functional>
#include <unordered_set>

#include "3rdparty/imgui/imgui.h"

#include "hd.h"
#include "ui.h"

// Global States
DWORD gBaseAddress = NULL;
ImDrawList *gOverlayDrawList = NULL;

CameraState *gCameraState = NULL;
GlobalState *gGlobalState = NULL;
Entity *gSelectedEntity = NULL;

int gWindowedMode = 0;
int gDisplayWidth = 0;
int gDisplayHeight = 0;

const size_t gNumAudioNames = 230;
const char *gAudioNames[gNumAudioNames] = {
    "alien_jump.wav",
    "alienexplosion.wav",
    "angry_kali.wav",
    "ani_squeak.wav",
    "ankhbreak.wav",
    "ankhflash.wav",
    "ankhflashback.wav",
    "anubisII_appear.wav",
    "anubisII_summons.wav",
    "armor_break.wav",
    "arrowhitwall.wav",
    "arrowshot.wav",
    "batflap.wav",
    "batoneflap.wav",
    "bee.wav",
    "bee2.wav",
    "bee3.wav",
    "blockfall.wav",
    "blocksmash.wav",
    "bomb_glue.wav",
    "bomb_timer.wav",
    "bone_shatter.wav",
    "boomerang_loop.wav",
    "bouldercoming.wav",
    "boulderhit.wav",
    "boulderhit2.wav",
    "boulderhit3.wav",
    "boulderhit4.wav",
    "bounce.wav",
    "bounce_light.wav",
    "camera.wav",
    "cape.wav",
    "catch_boomerang.wav",
    "chaching.wav",
    "char_unlock.wav",
    "chestopen.wav",
    "chime.wav",
    "chime3.wav",
    "chimp_attack.wav",
    "chimp_bounce.wav",
    "chute.wav",
    "coinsdrop.wav",
    "collect.wav",
    "crateopen.wav",
    "cricket.wav",
    "crushhit.wav",
    "crysknife.wav",
    "damsel_dog.wav",
    "damsel_female.wav",
    "damsel_male.wav",
    "damsel_sloth.wav",
    "damsel_water.wav",
    "demon.wav",
    "deposit.wav",
    "dm_go.wav",
    "dm_jump.wav",
    "dm_point.wav",
    "dm_ready.wav",
    "dm_winner.wav",
    "doorcrack.wav",
    "doorcrack2.wav",
    "doorglow.wav",
    "down.wav",
    "eggplant.wav",
    "end_chest.wav",
    "eruption.wav",
    "eyeblink.wav",
    "fadein.wav",
    "fadeout.wav",
    "fly_loop.wav",
    "forcefield.wav",
    "freezeray.wav",
    "frog1.wav",
    "frog2.wav",
    "frog3.wav",
    "frog_big_land.wav",
    "frog_bomb_charge.wav",
    "frog_mini.wav",
    "frozen.wav",
    "fwboom.wav",
    "fwshot.wav",
    "gem1.wav",
    "gem2.wav",
    "gem3.wav",
    "gem4.wav",
    "gem5.wav",
    "ghostloop.wav",
    "gold_poop.wav",
    "grab.wav",
    "grunt01.wav",
    "grunt02.wav",
    "grunt03.wav",
    "grunt04.wav",
    "grunt05.wav",
    "grunt06.wav",
    "heartbeat.wav",
    "hit.wav",
    "homing1.wav",
    "homing2.wav",
    "horsehead.wav",
    "ice_crack1.wav",
    "ice_crack2.wav",
    "ice_crack3.wav",
    "idol_get6.wav",
    "ihear_water.wav",
    "immortal_bounce.wav",
    "imp_flap.wav",
    "intodoor.wav",
    "item_drop.wav",
    "itemsplash.wav",
    "jelly_get.wav",
    "jetpack_loop.wav",
    "jump.wav",
    "kaboom.wav",
    "kaboombass.wav",
    "kiss.wav",
    "knifeattack.wav",
    "knifepickup.wav",
    "land.wav",
    "lasergun.wav",
    "lava_splash.wav",
    "lavastream.wav",
    "lick.wav",
    "lobbydrum.wav",
    "maindooropen.wav",
    "mantrapbite.wav",
    "match.wav",
    "menu_enter.wav",
    "menu_hor_l.wav",
    "menu_hor_r.wav",
    "menu_ret.wav",
    "menu_selection.wav",
    "menu_selection2.wav",
    "menu_swipe.wav",
    "menu_ver.wav",
    "metal_clank.wav",
    "mine_timer.wav",
    "mm_amb.wav",
    "mm_click.wav",
    "mm_door1.wav",
    "mm_door2.wav",
    "mm_door3.wav",
    "monkey_stealing.wav",
    "msgup.wav",
    "newshatter.wav",
    "oxface.wav",
    "pageget.wav",
    "pageturn.wav",
    "pause_in.wav",
    "pause_out.wav",
    "penguin.wav",
    "pickup.wav",
    "pushblock.wav",
    "queenblast.wav",
    "ropecatch.wav",
    "ropetoss.wav",
    "rubble.wav",
    "rubble2.wav",
    "rubble3.wav",
    "rubble_bone1.wav",
    "rubble_bone2.wav",
    "rubble_bone3.wav",
    "rubble_ice1.wav",
    "rubble_ice2.wav",
    "rubble_ice3.wav",
    "rubble_metal1.wav",
    "rubble_metal2.wav",
    "rubble_metal3.wav",
    "rubble_vase1.wav",
    "rubble_vase2.wav",
    "rubble_vase3.wav",
    "sacrifice.wav",
    "scarab_get.wav",
    "scorpion.wav",
    "scrollhit.wav",
    "secret.wav",
    "shatter.wav",
    "shop_bells.wav",
    "shopwheel.wav",
    "shotgun.wav",
    "shotgunpump.wav",
    "skeleton_arise.wav",
    "snail_bubble.wav",
    "snail_bubble_burst.wav",
    "snakebite.wav",
    "snowball.wav",
    "spider_jump.wav",
    "spidershot.wav",
    "spike_hit.wav",
    "splash.wav",
    "splat.wav",
    "spring.wav",
    "squish.wav",
    "sr_frogburp.wav",
    "succubus.wav",
    "talkbutton.wav",
    "tank.wav",
    "teleport.wav",
    "throw_item.wav",
    "tikifire.wav",
    "tikispike.wav",
    "torchgust.wav",
    "torchlight.wav",
    "torchlightshort.wav",
    "turretlaser.wav",
    "ufo_loop.wav",
    "ufo_shot.wav",
    "uhoh.wav",
    "up.wav",
    "vanish.wav",
    "volcanoshot.wav",
    "vomitflies.wav",
    "vsnake_sizzle.wav",
    "waterstream.wav",
    "webshot.wav",
    "whip.wav",
    "worm_block_destroy.wav",
    "worm_block_regen.wav",
    "worm_contact.wav",
    "worm_contact2.wav",
    "worm_contact3.wav",
    "worm_eats.wav",
    "worm_tounge_wiggle.wav",
    "yama_faceoff.wav",
    "yama_slam.wav",
    "yamaspew.wav",
    "yeti_roar.wav",
    "yeti_toss.wav",
    "zap.wav",
    "zombie_jump.wav",
};

struct EnabledEntities {
  bool activeEntities = false;
  bool floorEntities = false;
  bool floorBgEntities = false;
  bool backgroundEntities = false;
  bool unknown1400 = false;
  bool foregroundEntities = false;
  bool lightEmittingEntities = false;
  bool _4cStructEntities = false;

  int excludeEntityInput = -1;
  std::unordered_set<uint32_t> excluded = {171, 177};
};

struct DebugState {
  bool EnableTileBorders = false;
  bool EnableBinBorders = false;
  bool EnablePacifistOverlay = false;

  EnabledEntities Ids;
  EnabledEntities Hitboxes;
  EnabledEntities Selection;

  bool DrawSelectedEntHitbox = false;
  bool DrawClosestEntHitbox = false;
  bool DrawClosestEntId = false;
};
DebugState gDebugState = {};

struct SpawnState {
  int SpawnEntityInput = 0;
  bool ClickToSpawn = false;
};
SpawnState gSpawnState = {};

void patchReadOnlyCode(HANDLE process, DWORD addr, void *value, size_t size) {
  DWORD oldrights;
  VirtualProtectEx(process, (LPVOID)addr, size, PAGE_EXECUTE_READWRITE,
                   &oldrights);

  WriteProcessMemory(process, (LPVOID)addr, value, size, NULL);
  VirtualProtectEx(process, (LPVOID)addr, size, oldrights, &oldrights);
}

void specsOnInit() {
  gBaseAddress = (size_t)GetModuleHandleA(NULL);
  setupOffsets(gBaseAddress);

  gDebugState.Selection.activeEntities = true;
  gDebugState.Selection.floorEntities = true;

  auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                 PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                 PROCESS_CREATE_THREAD,
                             0, GetCurrentProcessId());

  BYTE patch[] = {0x4a};
  patchReadOnlyCode(process, gBaseAddress + 0x135B2A, patch, 1);

  BYTE patch2[] = {0xF0};
  patchReadOnlyCode(process, gBaseAddress + 0x1366C6, patch2, 1);
  CloseHandle(process);
}

struct PlayerState {
  bool LockHealth = false;
  int LockedHealthAmount = 0;

  bool LockBombs = false;
  int LockedBombsAmount = 0;

  bool LockRopes = false;
  int LockedRopesAmount = 0;
};
PlayerState gPlayersState[4] = {{}, {}, {}, {}};

// If we want to normalize the screen position to 0,0 at the top-left
// if (gWindowedMode == 2) {
//   RECT windowRect;
//   GetWindowRect(ui::window, &windowRect);
//   screen.x = screen.x + windowRect.left;
//   screen.y = screen.y + windowRect.top;
// }

ImVec2 screenToGame(ImVec2 screen) {

  auto x =
      (screen.x - ((float)gDisplayWidth / 2)) * (20 / (float)gDisplayWidth) +
      gCameraState->camera_x;
  auto y =
      (screen.y - ((float)gDisplayHeight / 2)) * -(20 / (float)gDisplayWidth) +
      gCameraState->camera_y;

  return {x, y};
}

ImVec2 gameToScreen(ImVec2 game) {
  auto x = (game.x - gCameraState->camera_x) / (20 / (float)gDisplayWidth) +
           ((float)gDisplayWidth / 2);
  auto y = (game.y - gCameraState->camera_y) / -(20 / (float)gDisplayWidth) +
           ((float)gDisplayHeight / 2);
  return {x, y};
}

void drawEntityHitbox(Entity *ent,
                      ImU32 color = ImGui::GetColorU32({255.f, 0.0f, 238.0f,
                                                        0.7f})) {
  auto screen = gameToScreen({ent->x, ent->y});
  ImVec2 topLeft =
      gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 topRight =
      gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 bottomRight =
      gameToScreen({ent->x + ent->hitbox_x, ent->y - ent->hitbox_down});
  ImVec2 bottomLeft =
      gameToScreen({ent->x - ent->hitbox_x, ent->y - ent->hitbox_down});

  gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft, color,
                            1.f);
}

void drawEntityHitboxDefault(Entity *ent) { drawEntityHitbox(ent); }

void drawPacifistOverlay() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->owner == Ownership::Unowned) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", (int)ent->owner);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              out.c_str());
  }
}

void drawBinBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 4.f) {
    gOverlayDrawList->AddLine(gameToScreen({x, -4.f}), gameToScreen({x, 120.f}),
                              IM_COL32_WHITE);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 4.f) {
    gOverlayDrawList->AddLine(gameToScreen({-4.f, y}), gameToScreen({50.f, y}),
                              IM_COL32_WHITE);
  }
}

void drawTileBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 1.f) {
    gOverlayDrawList->AddLine(gameToScreen({x + -.5f, -4.5f}),
                              gameToScreen({x + -.5f, 120.5f}), IM_COL32_WHITE,
                              0.5f);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 1.f) {
    gOverlayDrawList->AddLine(gameToScreen({-4.5f, y + 0.5f}),
                              gameToScreen({50.5f, y + 0.5f}), IM_COL32_WHITE,
                              0.5f);
  }
}

void drawEntityId(Entity *ent) {
  auto screen = gameToScreen({ent->x, ent->y});
  auto out = std::format("{}", ent->entity_type);
  gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5,
                            ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                            out.c_str());
}

using EntityCallback = std::function<void(Entity *e)>;
void forEntities(std::unordered_set<uint32_t> excludedEntities,
                 EntityCallback callback, Entity **entities, size_t count) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (excludedEntities.contains(ent->entity_type)) {
      continue;
    }

    callback(ent);
  }
}

bool findEntityArray(Entity *searchEnt, Entity **entities, size_t count) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (searchEnt == ent)
      return true;
  }
  return false;
}

bool findEntity(Entity *searchEnt) {
  if (searchEnt == NULL) {
    return false;
  }
  return (
      findEntityArray(searchEnt, gGlobalState->entities->entities_active,
                      gGlobalState->entities->entities_active_count) ||
      findEntityArray(searchEnt, gGlobalState->entities->array_1400,
                      gGlobalState->entities->array_1400_count) ||
      findEntityArray(
          searchEnt, gGlobalState->entities->entities_foreground,
          gGlobalState->entities->array_entities_foreground_count) ||
      findEntityArray(
          searchEnt, gGlobalState->entities->entities_foreground,
          gGlobalState->entities->array_entities_foreground_count) ||
      findEntityArray(searchEnt,
                      gGlobalState->entities->entities_light_emitting,
                      gGlobalState->entities->entities_light_emitting_count) ||
      findEntityArray(searchEnt, gGlobalState->level_state->entity_floors,
                      4692) ||

      findEntityArray(searchEnt, gGlobalState->level_state->entity_floors_bg,
                      4692) ||

      findEntityArray(searchEnt, gGlobalState->level_state->entity_backgrounds,
                      gGlobalState->level_state->entity_backgrounds_count) ||
      findEntityArray(searchEnt, gGlobalState->_4cstruct->entities, 160));
}

void forEnabledEntities(EnabledEntities &enabledEnts, EntityCallback callback) {

  // Active
  if (enabledEnts.activeEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_active,
                gGlobalState->entities->entities_active_count);
  }

  // 1400
  if (enabledEnts.unknown1400) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->array_1400,
                gGlobalState->entities->array_1400_count);
  }
  // Foreground
  if (enabledEnts.foregroundEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_foreground,
                gGlobalState->entities->array_entities_foreground_count);
  }

  // Light Emitting
  if (enabledEnts.lightEmittingEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_light_emitting,
                gGlobalState->entities->entities_light_emitting_count);
  }

  // Floors
  if (enabledEnts.floorEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->level_state->entity_floors, 4692);
  }
  if (enabledEnts.floorBgEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->level_state->entity_floors_bg, 4692);
  }

  // Backgrounds
  if (enabledEnts.backgroundEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->level_state->entity_backgrounds,
                gGlobalState->level_state->entity_backgrounds_count);
  }

  if (enabledEnts._4cStructEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->_4cstruct->entities, 160);
  }
}

float dist(ImVec2 pos1, ImVec2 pos2) {
  return sqrt(pow(pos2.x - pos1.x, 2.0f) + pow(pos2.y - pos1.y, 2.0f));
}

void drawOverlayWindow() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowSize(io.DisplaySize);
  ImGui::SetNextWindowPos({0, 0});
  ImGui::Begin(
      "Overlay", NULL,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
          ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
          ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
          ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs |
          ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  gOverlayDrawList = ImGui::GetWindowDrawList();

  // check if selected entity exists
  if (!findEntity(gSelectedEntity)) {
    gSelectedEntity = NULL;
  }

  Entity *closestEnt = NULL;
  if (ImGui::IsWindowHovered()) {
    if (io.MouseClicked[1]) {
      auto player = gGlobalState->player1;
      if (player) {
        auto pos = screenToGame(io.MousePos);
        player->x = pos.x;
        player->y = pos.y;
      }
    }

    if (gSpawnState.ClickToSpawn && io.MouseClicked[0]) {
      auto gamePos = screenToGame(io.MousePos);
      if (gSpawnState.SpawnEntityInput > 0) {
        gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                  gSpawnState.SpawnEntityInput, true);
      }
    }

    auto gamePos = screenToGame(io.MousePos);
    if (gDebugState.DrawClosestEntHitbox || gDebugState.DrawClosestEntId ||
        io.MouseClicked[2]) {
      float closestEntDist = 1;
      if (io.MouseClicked[2]) {
        gSelectedEntity = NULL;
      }
      EntityCallback getClosestEnt = [&](Entity *e) {
        auto eDist = dist(gamePos, ImVec2(e->x, e->y));
        if (eDist < closestEntDist) {
          closestEnt = e;
          closestEntDist = eDist;
        }
      };
      forEnabledEntities(gDebugState.Selection, getClosestEnt);
      if (closestEnt && io.MouseClicked[2]) {
        gSelectedEntity = closestEnt;
      }
    }

    if (gSelectedEntity != NULL && io.MouseDown[2] &&
        io.MouseDownDuration[2] > 0.2f) {
      gSelectedEntity->x = std::lerp(gSelectedEntity->x, gamePos.x, 1.f);
      gSelectedEntity->y = std::lerp(gSelectedEntity->y, gamePos.y, 1.f);
      if ((int)gSelectedEntity->entity_kind > 0 &&
          (int)gSelectedEntity->entity_kind < 5) {
        auto ent = (EntityActive *)gSelectedEntity;
        ent->time_in_air = 0.f;
      }
    }
  }

  gOverlayDrawList->AddText(
      ImGui::GetFont(), ImGui::GetFontSize() + 5, {148.f, 40.f},
      ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.05f}), "SpecsHD");

  if (gDebugState.EnableTileBorders) {
    drawTileBorders();
  }

  if (gDebugState.EnableBinBorders) {
    drawBinBorders();
  }

  if (gDebugState.EnablePacifistOverlay) {
    drawPacifistOverlay();
  }

  forEnabledEntities(gDebugState.Hitboxes, &drawEntityHitboxDefault);

  forEnabledEntities(gDebugState.Ids, &drawEntityId);

  if (gSelectedEntity != NULL && gDebugState.DrawSelectedEntHitbox) {
    drawEntityHitbox(gSelectedEntity,
                     ImGui::GetColorU32({1.0f, 1.0f, 1.0f, .9f}));
  }
  if (closestEnt) {
    if (gDebugState.DrawClosestEntHitbox)
      drawEntityHitbox(closestEnt, ImGui::GetColorU32({0.0f, 1.0f, .5f, .9f}));
    if (gDebugState.DrawClosestEntId) {
      drawEntityId(closestEnt);
    }
  }
  ImGui::End();
}

void drawSpawnTab() {
  ImGui::InputInt("Spawn Entity", &gSpawnState.SpawnEntityInput);
  ImGui::Checkbox("Click to spawn", &gSpawnState.ClickToSpawn);

  if (ImGui::Button("Spawn")) {
    if (gSpawnState.SpawnEntityInput > 0) {
      gGlobalState->SpawnEntity(gGlobalState->player1->x,
                                gGlobalState->player1->y,
                                gSpawnState.SpawnEntityInput, true);
    }
  }
}

void warpToLevel(uint32_t level) {
  gGlobalState->level = level;
  gGlobalState->screen_state = 3;
}

void RectFilled(ImVec2 &size, ImU32 col = IM_COL32_WHITE, float rounding = 0.f,
                ImDrawFlags flags = 0) {

  ImGui::Dummy(size);

  if (!ImGui::IsItemVisible()) {
    return;
  }

  auto p0 = ImGui::GetItemRectMin();
  auto p1 = ImGui::GetItemRectMax();
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(p0, p1, true);
  draw_list->AddRectFilled(p0, p1, col, rounding, flags);
  draw_list->PopClipRect();
}

void drawLevelTab() {

  auto isDisabled =
      gGlobalState->screen_state != 0 || gGlobalState->play_state != 0;

  if (isDisabled) {
    ImGui::BeginDisabled();
  }
  ImGui::Text("");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("Next Level")) {
    warpToLevel(gGlobalState->level);
  }

  ImGui::Text("Mines");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("1-1"))
    warpToLevel(0);
  ImGui::SameLine();
  if (ImGui::Button("1-2"))
    warpToLevel(1);
  ImGui::SameLine();
  if (ImGui::Button("1-3")) {
    warpToLevel(2);
  }
  ImGui::SameLine();
  if (ImGui::Button("1-4")) {
    warpToLevel(3);
  }

  ImGui::Text("Jungle");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("2-1"))
    warpToLevel(4);
  ImGui::SameLine();
  if (ImGui::Button("2-2"))
    warpToLevel(5);
  ImGui::SameLine();
  if (ImGui::Button("2-3")) {
    warpToLevel(6);
  }
  ImGui::SameLine();
  if (ImGui::Button("2-4")) {
    warpToLevel(7);
  }

  ImGui::Text("Ice Caves");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("3-1"))
    warpToLevel(8);
  ImGui::SameLine();
  if (ImGui::Button("3-2"))
    warpToLevel(9);
  ImGui::SameLine();
  if (ImGui::Button("3-3")) {
    warpToLevel(10);
  }
  ImGui::SameLine();
  if (ImGui::Button("3-4")) {
    warpToLevel(11);
  }

  ImGui::Text("Temple");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("4-1"))
    warpToLevel(12);
  ImGui::SameLine();
  if (ImGui::Button("4-2"))
    warpToLevel(13);
  ImGui::SameLine();
  if (ImGui::Button("4-3")) {
    warpToLevel(14);
  }
  ImGui::SameLine();
  if (ImGui::Button("Olmec")) {
    warpToLevel(15);
  }

  ImGui::Text("Hell");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("5-1"))
    warpToLevel(16);
  ImGui::SameLine();
  if (ImGui::Button("5-2"))
    warpToLevel(17);
  ImGui::SameLine();
  if (ImGui::Button("5-3")) {
    warpToLevel(18);
  }
  ImGui::SameLine();
  if (ImGui::Button("Yama")) {
    warpToLevel(19);
  }

  if (isDisabled) {
    ImGui::EndDisabled();
  }

  ImVec2 size = {5.f, 5.f};
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Level Map")) {
    for (auto idx = 0; idx < 4692; idx++) {
      auto ent = gGlobalState->level_state->entity_floors[idx];

      auto col = IM_COL32(183, 183, 183, 255);
      // Empty
      if (ent == NULL) {
        col = IM_COL32(0, 0, 0, 0);
      } else if (ent->entity_type == 3 || ent->entity_type == 2) {
        // Doors
        col = IM_COL32(59, 196, 0, 255);
      } else if (ent->entity_type == 4 || ent->entity_type == 5) {
        // Ladders
        col = IM_COL32(133, 133, 133, 100);
      } else if (ent->entity_type == 25) {
        // Bedrock
        col = IM_COL32(90, 90, 90, 255);
      } else if (ent->entity_type == 27) {
        // Water
        col = IM_COL32(43, 114, 214, 100);
      } else if (ent->entity_type == 36) {
        // Lava
        col = IM_COL32(214, 54, 43, 100);
      } else if (ent->entity_type == 91) {
        // Acid
        col = IM_COL32(43, 214, 77, 100);
      }

      if (idx % 46 > 0) {

        ImGui::SameLine(0.f, 4.f);
      }
      RectFilled(size, col);
    }
  }
}

void ensureLockedAmountsForPlayer(EntityPlayer *player, PlayerData &data,
                                  PlayerState *state) {
  if (state->LockHealth) {
    // If you ressurect a player it gets into a bad state.
    if (player->health > 0) {
      player->health = state->LockedHealthAmount;
    }
  }

  if (state->LockBombs) {
    data.bombs = state->LockedBombsAmount;
  }

  if (state->LockRopes) {
    data.ropes = state->LockedRopesAmount;
  }
}

void ensureLockedAmounts() {
  if (gGlobalState->player1) {
    ensureLockedAmountsForPlayer(gGlobalState->player1,
                                 gGlobalState->player1_data, &gPlayersState[0]);
  }

  if (gGlobalState->player2) {
    ensureLockedAmountsForPlayer(gGlobalState->player2,
                                 gGlobalState->player2_data, &gPlayersState[1]);
  }

  if (gGlobalState->player3) {
    ensureLockedAmountsForPlayer(gGlobalState->player3,
                                 gGlobalState->player3_data, &gPlayersState[2]);
  }

  if (gGlobalState->player4) {
    ensureLockedAmountsForPlayer(gGlobalState->player4,
                                 gGlobalState->player4_data, &gPlayersState[3]);
  }
}

void drawPlayerTab(EntityPlayer *player, PlayerData &data, PlayerState *state) {
  if (!player) {
    ImGui::Text("No Player Entity");
    return;
  }

  if (ImGui::Button("Max Health/Bombs/Ropes")) {
    player->health = 99;
    state->LockedHealthAmount = player->health;
    data.bombs = 99;
    state->LockedBombsAmount = data.bombs;
    data.ropes = 99;
    state->LockedRopesAmount = data.ropes;
  }

  ImGui::Text("Locked?");
  ImGui::SameLine(80.0f);
  ImGui::Text("Amount");

  if (ImGui::Checkbox("##LockHealth", &state->LockHealth)) {
    if (state->LockHealth) {
      state->LockedHealthAmount = player->health;
    }
  };
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Health", &player->health)) {
    player->health = std::clamp(player->health, 0, 99);
    state->LockedHealthAmount = player->health;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockBombs", &state->LockBombs)) {
    if (state->LockBombs) {
      state->LockedBombsAmount = data.bombs;
    }
  }
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Bombs", &data.bombs)) {
    data.bombs = std::clamp(data.bombs, 0, 99);
    state->LockedBombsAmount = data.bombs;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockRopes", &state->LockRopes)) {
    if (state->LockRopes) {
      state->LockedRopesAmount = data.ropes;
    }
  }
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Ropes", &data.ropes)) {
    data.ropes = std::clamp(data.ropes, 0, 99);
    state->LockedRopesAmount = data.ropes;
  }
  ImGui::PopItemWidth();
  ImGui::Separator();

  ImGui::Checkbox("Compass", &data.has_compass);
  ImGui::Checkbox("Parachute", &data.has_parachute);

  ImGui::Checkbox("Jetpack", &data.has_jetpack);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Jetpack")) {
    gGlobalState->SpawnEntity(player->x, player->y, 522, true);
  }

  ImGui::Checkbox("Climbing Gloves", &data.has_climbing_gloves);
  ImGui::Checkbox("Pitcher's Mitt", &data.has_pitchers_mitt);
  ImGui::Checkbox("Spring Shoes", &data.has_spring_shoes);
  ImGui::Checkbox("Spike Shoes", &data.has_spike_shoes);
  ImGui::Checkbox("Spectacles", &data.has_spectacles);
  ImGui::Checkbox("Kapala", &data.has_kapala);
  ImGui::Checkbox("Hedjet", &data.has_hedjet);
  ImGui::Checkbox("Udjat Eye", &data.has_udjat);
  ImGui::Checkbox("Book of the Dead", &data.has_book_of_dead);
  ImGui::Checkbox("Ankh", &data.has_ankh);
  ImGui::Checkbox("Paste", &data.has_paste);

  ImGui::Checkbox("Cape", &data.has_cape);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Cape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 521, true);
  }

  ImGui::Checkbox("Vlad's Cape", &data.has_vlads_cape);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##VladsCape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 532, true);
  }

  ImGui::Checkbox("Crysknife", &data.has_crysknife);
  ImGui::Checkbox("Vlad's Amulet", &data.has_vlads_amulet);
  ImGui::Checkbox("White Flag", &data.has_white_flag);
}

void drawPlayersTab() {

  if (ImGui::BeginTabBar("Players")) {
    if (ImGui::BeginTabItem("Player 1")) {
      drawPlayerTab(gGlobalState->player1, gGlobalState->player1_data,
                    &gPlayersState[0]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 2")) {
      drawPlayerTab(gGlobalState->player2, gGlobalState->player2_data,
                    &gPlayersState[1]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 3")) {
      drawPlayerTab(gGlobalState->player3, gGlobalState->player3_data,
                    &gPlayersState[2]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 4")) {
      drawPlayerTab(gGlobalState->player4, gGlobalState->player4_data,
                    &gPlayersState[3]);
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
}

void drawAudioTab() {

  // auto player = *(&((*gGlobalState).player1) + gGlobalState->flag_player);
  auto player = gGlobalState->player1;
  if (!player) {
    ImGui::Text("Need Player Entity");
    return;
  }

  for (auto idx = 0; idx < gNumAudioNames; idx++) {
    if (ImGui::Button(gAudioNames[idx])) {
      player->PlaySound(gAudioNames[idx]);
    }
  }
}

void drawToggleEntityTab(const char *preText, EnabledEntities &enabledEnts) {
  ImGui::Checkbox(std::format("{} Active Entities", preText).c_str(),
                  &enabledEnts.activeEntities);
  ImGui::Checkbox(std::format("{} Light Emitting", preText).c_str(),
                  &enabledEnts.lightEmittingEntities);
  ImGui::Checkbox(
      std::format("{} Unknown 1400 (BG + Active?)", preText).c_str(),
      &enabledEnts.unknown1400);
  ImGui::Checkbox(std::format("{} Foreground Entities", preText).c_str(),
                  &enabledEnts.foregroundEntities);
  ImGui::Checkbox(std::format("{} Floor Entities", preText).c_str(),
                  &enabledEnts.floorEntities);
  ImGui::Checkbox(std::format("{} Floor Background Entities", preText).c_str(),
                  &enabledEnts.floorBgEntities);
  ImGui::Checkbox(std::format("{} Background Entities", preText).c_str(),
                  &enabledEnts.backgroundEntities);
  ImGui::Checkbox(std::format("{} 4c Struct Entities", preText).c_str(),
                  &enabledEnts._4cStructEntities);

  ImGui::InputInt(std::format("Exclude Entity {}", preText).c_str(),
                  &enabledEnts.excludeEntityInput);
  if (ImGui::Button(std::format("Exclude##Debug{}", preText).c_str())) {
    if (enabledEnts.excludeEntityInput >= 0) {
      enabledEnts.excluded.insert(enabledEnts.excludeEntityInput);
      enabledEnts.excludeEntityInput = 0;
    }
  }
  ImGui::Separator();
  for (auto ent_type : enabledEnts.excluded) {
    auto label = std::format("Remove {}##Debug{}", ent_type, preText);
    if (ImGui::Button(label.c_str())) {
      enabledEnts.excluded.erase(ent_type);
    }
  }
}

void drawDebugTab() {
  ImGuiIO &io = ImGui::GetIO();

  auto gameMouse = screenToGame(io.MousePos);
  ImGui::Text("Mouse: %f %f", io.MousePos.x, io.MousePos.y);
  ImGui::Text("Mouse (Game): %f %f", gameMouse.x, gameMouse.y);
  if (gGlobalState->player1) {
    auto screenPlayer =
        gameToScreen({gGlobalState->player1->x, gGlobalState->player1->y});
    ImGui::Text("Player (Screen): %f %f", screenPlayer.x, screenPlayer.y);
  }
  ImGui::Checkbox("Draw Tile Borders", &gDebugState.EnableTileBorders);
  ImGui::Checkbox("Draw Bin Borders", &gDebugState.EnableBinBorders);
  ImGui::Checkbox("Draw Owned Entities", &gDebugState.EnablePacifistOverlay);

  if (ImGui::CollapsingHeader("Draw Hitboxes")) {
    drawToggleEntityTab("Show", gDebugState.Hitboxes);
  }
  if (ImGui::CollapsingHeader("Draw Ids")) {
    drawToggleEntityTab("Draw", gDebugState.Ids);
  }
  if (ImGui::CollapsingHeader("Selectable Entities")) {
    ImGui::Checkbox("Draw Selected Entity Hitbox",
                    &gDebugState.DrawSelectedEntHitbox);
    ImGui::Checkbox("Draw Closest Entity Hitbox",
                    &gDebugState.DrawClosestEntHitbox);
    ImGui::Checkbox("Draw Closest Entity Id", &gDebugState.DrawClosestEntId);
    ImGui::Separator();
    drawToggleEntityTab("Enable", gDebugState.Selection);
  }
}

size_t sizeofEntityKind(EntityKind entityKind) {
  switch (entityKind) {
  case EntityKind::KIND_FLOOR:
    return sizeof(EntityFloor);
  case EntityKind::KIND_ACTIVE:
    return sizeof(EntityActive);
  case EntityKind::KIND_PLAYER:
    return sizeof(EntityPlayer);
  case EntityKind::KIND_MONSTER:
    return sizeof(EntityMonster);
  case EntityKind::KIND_ITEM:
    return sizeof(EntityItem);
  case EntityKind::KIND_BACKGROUND:
    return sizeof(EntityBackground);
  case EntityKind::KIND_EXPLOSION:
    return sizeof(EntityExplosion);
  case EntityKind::KIND_ENTITY:
    return sizeof(Entity);
  default:
    return 0;
  }
}

void drawCharBool(const char *label, char &flag) {
  bool bflag = flag ? 1 : 0;
  ImGui::Checkbox(label, &bflag);
  bflag ? flag = 1 : flag = 0;
}

void drawCharBool(const char *label, uint8_t &flag) {
  bool bflag = flag ? 1 : 0;
  ImGui::Checkbox(label, &bflag);
  bflag ? flag = 1 : flag = 0;
}

void drawSelectedEntityTab() {
  ImGuiIO &io = ImGui::GetIO();
  if (!gSelectedEntity) {
    ImGui::Text("No selected entity");
    return;
  }
  ImGui::Text("Address: 0x%X", (uint32_t)gSelectedEntity);
  ImGui::Text("Entity ID: %d", gSelectedEntity->entity_type);
  ImGui::Text("Entity kind: %d", gSelectedEntity->entity_kind);

  if (ImGui::CollapsingHeader("Position, hitbox, etc.")) {
    ImGui::InputFloat("Entity x", &gSelectedEntity->x);
    ImGui::InputFloat("Entity y", &gSelectedEntity->y);
    ImGui::SliderFloat("width", &gSelectedEntity->width, 0.0, 10.0);
    ImGui::SliderFloat("height", &gSelectedEntity->height, 0.0, 10.0);
    ImGui::SliderFloat("current_z", &gSelectedEntity->current_z, 0.0, 50.0);
    ImGui::SliderFloat("original_z", &gSelectedEntity->original_z, 0.0, 50.0);
    ImGui::SliderFloat("alpha", &gSelectedEntity->alpha, 0.0, 1.0);
    ImGui::SliderFloat("hitbox up", &gSelectedEntity->hitbox_up, 0.0, 5.0);
    ImGui::SliderFloat("hitbox down", &gSelectedEntity->hitbox_down, 0.0, 5.0);
    ImGui::SliderFloat("hitbox x", &gSelectedEntity->hitbox_x, 0.0, 5.0);
    ImGui::SliderAngle("angle", &gSelectedEntity->angle);
  }
  if (ImGui::CollapsingHeader("Flags1")) {
    drawCharBool("flag_deletion", gSelectedEntity->flag_deletion);
    drawCharBool("flag_horizontal_flip", gSelectedEntity->flag_horizontal_flip);
    drawCharBool("flag_3", gSelectedEntity->flag_3);
    drawCharBool("flag_4", gSelectedEntity->flag_4);
    drawCharBool("flag_5", gSelectedEntity->flag_5);
    drawCharBool("flag_6", gSelectedEntity->flag_6);
    drawCharBool("flag_7", gSelectedEntity->flag_7);
    drawCharBool("flag_8", gSelectedEntity->flag_8);
    drawCharBool("flag_9", gSelectedEntity->flag_9);
    drawCharBool("flag_10", gSelectedEntity->flag_10);
    drawCharBool("flag_11", gSelectedEntity->flag_11);
    drawCharBool("flag_12", gSelectedEntity->flag_12);
    drawCharBool("flag_13", gSelectedEntity->flag_13);
    drawCharBool("flag_14", gSelectedEntity->flag_14);
    drawCharBool("flag_15", gSelectedEntity->flag_15);
    drawCharBool("flag_16", gSelectedEntity->flag_16);
    drawCharBool("flag_17", gSelectedEntity->flag_17);
    drawCharBool("flag_18", gSelectedEntity->flag_18);
    drawCharBool("flag_19", gSelectedEntity->flag_19);
    drawCharBool("flag_20", gSelectedEntity->flag_20);
    drawCharBool("flag_21", gSelectedEntity->flag_21);
    drawCharBool("flag_22", gSelectedEntity->flag_22);
    drawCharBool("flag_23", gSelectedEntity->flag_23);
    drawCharBool("flag_24", gSelectedEntity->flag_24);
  }
  if ((uint32_t)gSelectedEntity->entity_kind > 0 &&
      (uint32_t)gSelectedEntity->entity_kind < 5 &&
      ImGui::CollapsingHeader("EntityActive Stuff")) {
    auto entityActive = reinterpret_cast<EntityActive *>(gSelectedEntity);
    ImGui::InputInt("Health", &entityActive->health);
    ImGui::InputInt("Favor given", &entityActive->favor_given);
    ImGui::InputFloat("Velocity x", &entityActive->velocity_x);
    ImGui::InputFloat("Velocity y", &entityActive->velocity_y);
    if (ImGui::CollapsingHeader("Flags2")) {
      for (size_t i = 0x1f0; i <= 0x218; ++i) {
        char *addr = ((char *)gSelectedEntity) + i;
        drawCharBool(std::format("Flag {:X}", i).c_str(), *addr);
      }
    }
  }
  if (ImGui::CollapsingHeader("Raw Entity Values")) {
    if (ImGui::BeginTable("Raw Bytes", 6)) {

      ImGui::TableSetupColumn("Offset");
      ImGui::TableSetupColumn("Bytes");
      ImGui::TableSetupColumn("Signed");
      ImGui::TableSetupColumn("Unsigned");
      ImGui::TableSetupColumn("Hex");
      ImGui::TableSetupColumn("Float");
      ImGui::TableHeadersRow();

      for (size_t i = 0; i < sizeofEntityKind(gSelectedEntity->entity_kind);
           i += 4) {

        ImGui::TableNextRow();

        char *addr = ((char *)gSelectedEntity) + i;
        ImGui::TableNextColumn();
        ImGui::Text("0x%X", i);
        {
          uint32_t a1, a2, a3, a4;
          a1 = (*(addr)) & (0xFF);
          a2 = (*(addr + 1)) & (0xFF);
          a3 = (*(addr + 2)) & (0xFF);
          a4 = (*(addr + 3)) & (0xFF);
          ImGui::TableNextColumn();
          ImGui::Text("%X %X %X %X", a1, a2, a3, a4);
        }
        ImGui::TableNextColumn();
        ImGui::Text("%d", *(int32_t *)addr);

        ImGui::TableNextColumn();
        ImGui::Text("%u", *(uint32_t *)addr);

        ImGui::TableNextColumn();
        ImGui::Text("0x%X", *(uint32_t *)addr);

        ImGui::TableNextColumn();
        ImGui::Text("%f", *(float *)addr);
      }

      ImGui::EndTable();
    }
  }
  if (gSelectedEntity->flag_deletion == 1) {
    gSelectedEntity = NULL;
  }
}

void drawGlobalStateTab() {
  ImGui::InputScalar("screen_state", ImGuiDataType_U32,
                     &gGlobalState->screen_state);
  ImGui::InputScalar("play_state", ImGuiDataType_U32,
                     &gGlobalState->play_state);
  ImGui::InputScalar("flag_player", ImGuiDataType_U32,
                     &gGlobalState->flag_player);
  ImGui::InputScalar("level", ImGuiDataType_U32, &gGlobalState->level);
  ImGui::InputScalar("level_track", ImGuiDataType_U32,
                     &gGlobalState->level_track);
  if (ImGui::CollapsingHeader("GlobalState Flags")) {
    drawCharBool("dark_level", gGlobalState->dark_level);
    drawCharBool("altar_spawned", gGlobalState->altar_spawned);
    drawCharBool("idol_spawned", gGlobalState->idol_spawned);
    drawCharBool("damsel_spawned", gGlobalState->damsel_spawned);
    drawCharBool("unknown_flag", gGlobalState->unknown_flag);
    drawCharBool("moai_unopened", gGlobalState->moai_unopened);
    drawCharBool("moai_broke_in", gGlobalState->moai_broke_in);
    drawCharBool("ghost_spawned", gGlobalState->ghost_spawned);
    drawCharBool("rescued_damsel", gGlobalState->rescued_damsel);
    drawCharBool("shopkeeper_triggered", gGlobalState->shopkeeper_triggered);
    drawCharBool("area_had_dark_level", gGlobalState->area_had_dark_level);
    drawCharBool("level_has_udjat", gGlobalState->level_has_udjat);
    drawCharBool("has_spawned_udjat", gGlobalState->has_spawned_udjat);
    drawCharBool("unused_flag", gGlobalState->unused_flag);
    drawCharBool("vault_spawned_in_area", gGlobalState->vault_spawned_in_area);
    drawCharBool("flooded_mines", gGlobalState->flooded_mines);
    drawCharBool("skin_is_crawling", gGlobalState->skin_is_crawling);
    drawCharBool("dead_are_restless", gGlobalState->dead_are_restless);
    drawCharBool("rushing_water", gGlobalState->rushing_water);
    drawCharBool("is_haunted_castle", gGlobalState->is_haunted_castle);
    drawCharBool("at_haunted_castle_exit",
                 gGlobalState->at_haunted_castle_exit);
    drawCharBool("tiki_village", gGlobalState->tiki_village);
    drawCharBool("spawned_black_market_entrance",
                 gGlobalState->spawned_black_market_entrance);
    drawCharBool("unused_flag2", gGlobalState->unused_flag2);
    drawCharBool("spawned_haunted_castle_entrance",
                 gGlobalState->spawned_haunted_castle_entrance);
    drawCharBool("mothership_spawned", gGlobalState->mothership_spawned);
    drawCharBool("moai_spawned", gGlobalState->moai_spawned);
    drawCharBool("is_blackmarket", gGlobalState->is_blackmarket);
    drawCharBool("at_blackmarket_exit", gGlobalState->at_blackmarket_exit);
    drawCharBool("is_wet_fur", gGlobalState->is_wet_fur);
    drawCharBool("is_mothership", gGlobalState->is_mothership);
    drawCharBool("at_mothership_exit", gGlobalState->at_mothership_exit);
    drawCharBool("is_city_of_gold", gGlobalState->is_city_of_gold);
    drawCharBool("at_city_of_gold_exit", gGlobalState->at_city_of_gold_exit);
    drawCharBool("is_worm", gGlobalState->is_worm);
  }
}

void drawToolWindow() {
  if (!ui::open) {
    return;
  }
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2{0.f, 0.f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2{400.f, 500.f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Specs HD");

  if (ImGui::IsWindowHovered()) {
    io.MouseDrawCursor = true;
    io.WantCaptureMouse = true;
  } else {
    io.MouseDrawCursor = false;
    io.WantCaptureMouse = false;
  }

  auto mouse_game = screenToGame(io.MousePos);

  if (ImGui::BeginTabBar("Specs HD")) {
    if (ImGui::BeginTabItem("Spawn")) {
      drawSpawnTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Level")) {
      drawLevelTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Players")) {
      drawPlayersTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Audio")) {
      drawAudioTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Debug")) {
      drawDebugTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Selected")) {
      drawSelectedEntityTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("GlobalState")) {
      drawGlobalStateTab();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
}

void specsOnFrame() {

  gCameraState =
      reinterpret_cast<CameraState *>(*((DWORD *)(gBaseAddress + 0x154510)));
  gGlobalState =
      reinterpret_cast<GlobalState *>(*((DWORD *)(gBaseAddress + 0x15446C)));

  gWindowedMode = static_cast<int>(*((DWORD *)(gBaseAddress + 0x15a52c)));
  gDisplayWidth = static_cast<int>(*((DWORD *)(gBaseAddress + 0x140a8c)));
  gDisplayHeight = static_cast<int>(*((DWORD *)(gBaseAddress + 0x140a90)));

  gGlobalState->N00001004 = 0; // 440629

  ensureLockedAmounts();

  drawOverlayWindow();
  drawToolWindow();
}