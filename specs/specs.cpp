
#include "specs.h"

#include <cstdlib>
#include <ctime>

#include "drawing.h"
#include "game_hooks.h"
#include "inputs.h"
#include "state.h"
#include "zoom.h"
#include <hddll/hddll.h>
#include <hddll/memory.h>
#include <hddll/ui.h>
#include <hddll/utils.h>

#include "mods/full_spelunky.h"
#include "mods/seeded_mode.h"
#include "mods/tunnel_man.h"

#include "tabs/debug_tab.h"
#include "tabs/level_tab.h"
#include "tabs/mods_tab.h"
#include "tabs/players_tab.h"
#include "tabs/selected_tab.h"
#include "tabs/settings_tab.h"
#include "tabs/spawn_tab.h"

static bool gToolWindowOpen = true;

namespace hddll {

void onInit() {
  srand((unsigned int)time(NULL));

  gConfig = Specs::Config::load();

  initHooks();
  initZoom();

  gDebugState.Selection.activeEntities = true;
  gDebugState.Selection.floorEntities = true;

  auto process = GetCurrentProcess();
}

void onDestroy() {
  restoreZoom();
  cleanUpHooks();
}

} // namespace hddll

static void handleKeyInput() {
  auto keys = gConfig->keys;

  if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Hide])) {
    gToolWindowOpen = !gToolWindowOpen;
  }

  if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Engine_Pause])) {
    gPaused = !gPaused;
    gPauseAt = gFrame;
  }

  if (gPaused && gFrame > gPauseAt) {
    if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Engine_Frame_Advance],
                            true)) {
      hddll::gGlobalState->pause_update = 0;
      gPauseAt = gFrame + 1;
    } else {
      hddll::gGlobalState->pause_update = 1;
    }
  } else {
    hddll::gGlobalState->pause_update = 0;
  }

  auto isDisabled = hddll::gGlobalState->screen_state != 0 ||
                    hddll::gGlobalState->play_state != 0;
  if (!isDisabled && Specs::IsKeyPressed(keys[Specs::KeyFeatures_Reset_Run])) {
    resetRun();
    return;
  }

  if (!isDisabled && Specs::IsKeyPressed(keys[Specs::KeyFeatures_Next_Level])) {
    warpToLevel(hddll::gGlobalState->level);
    return;
  }
}

static void drawToolWindow() {
  if (!gToolWindowOpen) {
    return;
  }
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2{0.f, 0.f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2{400.f, 500.f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Specs HD");

  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
    io.MouseDrawCursor = true;
    io.WantCaptureMouse = true;
  } else {
    io.MouseDrawCursor = false;
    io.WantCaptureMouse = false;
  }

  auto mouse_game = hddll::screenToGame(io.MousePos);

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

    if (ImGui::BeginTabItem("Mods")) {
      drawModsTab();
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

    if (ImGui::BeginTabItem("Settings")) {
      drawSettingsTab();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
}

static void onRunningFrame() {
  if (gModsState.TunnelMan) {
    onRunningFrameTunnelMan();
  }
  if (gModsState.TheFullSpelunky) {
    onRunningFrameFullSpelunky();
  }
}

static void onLevelStart() {
  if (gDebugState.DisableOlmecCutscene) {
    onLevelStartOlmec();
  }

  if (gModsState.TheFullSpelunky) {
    onLevelStartFullSpelunky();
  }
}

namespace hddll {

void onFrame() {
  if (!gGlobalState || !gCameraState)
    return;

  gFrame++;

  applyZoom();

  if (gGlobalState->screen_state == 0 && gScreenStatePrevious == 2) {
    onLevelStart();
  }

  handleKeyInput();
  ensureLockedAmounts();
  drawOverlayWindow();
  drawToolWindow();
  advanceLevel();

  if (gGlobalState->screen_state == 0 && gGlobalState->play_state == 0) {
    onRunningFrame();
  }

  gScreenStatePrevious = gGlobalState->screen_state;
}

} // namespace hddll
