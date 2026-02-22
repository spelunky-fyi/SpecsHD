
#include "specs.h"

#include <cstdlib>
#include <ctime>

#include "drawing.h"
#include "game_hooks.h"
#include "inputs.h"
#include "memory.h"
#include "state.h"
#include "ui.h"

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

void specsOnInit() {
  srand((unsigned int)time(NULL));

  gConfig = Specs::Config::load();

  gBaseAddress = (size_t)GetModuleHandleA(NULL);
  setupOffsets(gBaseAddress);
  initHooks();

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

void specsOnDestroy() { cleanUpHooks(); }

static void handleKeyInput() {
  auto keys = gConfig->keys;

  if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Hide])) {
    ui::open = !ui::open;
  }

  if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Engine_Pause])) {
    gPaused = !gPaused;
    gPauseAt = gFrame;
  }

  if (gPaused && gFrame > gPauseAt) {
    if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Engine_Frame_Advance],
                            true)) {
      gGlobalState->pause_update = 0;
      gPauseAt = gFrame + 1;
    } else {
      gGlobalState->pause_update = 1;
    }
  } else {
    gGlobalState->pause_update = 0;
  }

  auto isDisabled =
      gGlobalState->screen_state != 0 || gGlobalState->play_state != 0;
  if (!isDisabled && Specs::IsKeyPressed(keys[Specs::KeyFeatures_Reset_Run])) {
    resetRun();
    return;
  }

  if (!isDisabled && Specs::IsKeyPressed(keys[Specs::KeyFeatures_Next_Level])) {
    warpToLevel(gGlobalState->level);
    return;
  }
}

static void drawToolWindow() {
  if (!ui::open) {
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

void specsOnFrame() {

  gCameraState =
      reinterpret_cast<CameraState *>(*((DWORD *)(gBaseAddress + 0x154510)));
  gGlobalState =
      reinterpret_cast<GlobalState *>(*((DWORD *)(gBaseAddress + 0x15446C)));

  gWindowedMode = static_cast<int>(*((DWORD *)(gBaseAddress + 0x15a52c)));
  gDisplayWidth = static_cast<int>(*((DWORD *)(gBaseAddress + 0x140a8c)));
  gDisplayHeight = static_cast<int>(*((DWORD *)(gBaseAddress + 0x140a90)));

  gGlobalState->N00001004 = 0; // 440629
  gFrame++;

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
