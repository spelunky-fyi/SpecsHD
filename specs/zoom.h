#pragma once

// Camera zoom feature.
//
// Spelunky HD has no native zoom. The whole world is drawn through one
// orthographic projection matrix (g_flOrthoProjectionMatrix). Scaling its
// m[0]/m[5] uniformly scales position+size of everything that goes through the
// game's shader -- but every render-path function also frustum-culls geometry
// against the shared 12.0/7.0 half-viewport literals, and the full-screen
// composite/light quads are sized from another set of shared constants, and
// the scene textures are sized for native resolution. The whole feature is
// the combination of patching all of those so they scale together.
//
// hddll::gViewScale is the single source of truth (1.0 = native, >1 zooms
// out). HDDLL only reads it; its screen<->game coordinate helpers honor it so
// debug overlays stay aligned. Everything in zoom.cpp is the policy that
// drives it and patches the game to match.

// Cap on the scene-texture upscale. Valid values are 1, 2, or 4. The active
// scale steps up from 1 -> 2 -> 4 as gViewScale crosses thresholds, but never
// goes above this cap. Lower it to limit GPU memory on constrained hardware:
// 1 = no upscale (zero overhead), 2 ~= 55 MB, 4 ~= 205 MB extra VRAM.
extern int gMaxSceneScale;

// Installs every persistent patch the zoom feature needs (cull-operand
// redirects, composite/light scale-operand redirects, and the camera-clamp
// hook). Call once from onInit(), after hddll::init() has set gBaseAddress.
void initZoom();

// Per-frame work: scales the projection matrix, cull pads, composite/light
// quad scale, scene-texture resolution, and level-light radius from
// hddll::gViewScale. Call once per frame from onFrame().
void applyZoom();

// Reverts every persistent change zoom made to game memory so the DLL can
// safely unload. Call from onDestroy() before the DLL unloads.
void restoreZoom();
