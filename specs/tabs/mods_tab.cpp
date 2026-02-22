
#include "mods_tab.h"

#include "../drawing.h"
#include "../game_hooks.h"
#include "../memory.h"
#include "../state.h"
#include "../utils.h"
#include "../3rdparty/imgui/misc/cpp/imgui_stdlib.h"

#include "../mods/biglunky.h"
#include "../mods/dark_mode.h"
#include "../mods/full_spelunky.h"
#include "../mods/seeded_mode.h"
#include "../mods/tunnel_man.h"
#include "../mods/uplunky.h"

#include "level_tab.h"

void drawModsTab() {
  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::Checkbox("Dark Mode", &gModsState.DarkMode)) {
    applyPatches(gDarkModePatches, !gModsState.DarkMode);
  };

  ImGui::Separator();
  if (ImGui::Checkbox("The Full Spelunky", &gModsState.TheFullSpelunky)) {
    if (gModsState.TheFullSpelunky && !hookUnlockCoffinsJmpBackAddr) {
      // Hook Coffin Assignments
      int hookLen = 6;
      DWORD hookAddr = gBaseAddress + 0xe8860;
      hookUnlockCoffinsJmpBackAddr = hookAddr + hookLen;
      hook((void *)hookAddr, hookUnlockCoffins, hookLen);
      applyPatches(gFullSpelunkyPatches);
    }
    if (!gModsState.TheFullSpelunky && hookUnlockCoffinsJmpBackAddr) {
      applyPatches(gFullSpelunkyPatches, true);
      unhook((void *)(gBaseAddress + 0xe8860));
      hookUnlockCoffinsJmpBackAddr = NULL;
    }
  };
  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  ImGui::Checkbox("Show Character Overlay##The Full Spelunky",
                  &gFullSpelunkyState.showCharacterOverlay);

  ImGui::Separator();
  if (ImGui::Checkbox("Biglunky", &gModsState.Biglunky)) {
    if (gModsState.Biglunky) {
      applyPatches(gBiglunkyPatches);
      applyRelativePatches(gBiglunkyRelativePatches);
    } else {
      applyPatches(gBiglunkyPatches, true);
      applyRelativePatches(gBiglunkyRelativePatches, true);
    }
  };

  ImGui::Separator();
  if (ImGui::Checkbox("Uplunky (Beta)", &gModsState.Uplunky)) {
    applyPatches(gUplunkyPatches, !gModsState.Uplunky);
    applyRelativePatches(gUplunkyRelativePatches, !gModsState.Uplunky);
    if (gModsState.Uplunky) {
      resetUplunkyState();
    }
  };

  ImGui::Separator();
  if (ImGui::Checkbox("Tunnel Man", &gModsState.TunnelMan)) {
    if (gModsState.TunnelMan) {
      resetTunnelManState();

      if (!hookWhipJmpBackAddr) {

        int hookLen = 7;
        DWORD hookAddr = gBaseAddress + 0x569a5;
        hookWhipJmpBackAddr = hookAddr + hookLen;
        hookWhipSkipWhippingAddr = gBaseAddress + 0x56a80;
        hook((void *)hookAddr, hookWhip, hookLen);
      }
    } else {
      if (hookWhipJmpBackAddr) {
        applyPatches(gTunnelManPatches, true);
        unhook((void *)(gBaseAddress + 0x569a5));
        hookWhipJmpBackAddr = NULL;
        hookWhipSkipWhippingAddr = NULL;
      }
    }
  };

  ImGui::Separator();
  if (ImGui::Checkbox("Seeded", &gModsState.SeededMode)) {
    if (gModsState.SeededMode) {
      if (!hookSeedLevelJmpBackAddr) {
        int hookLen = 7;
        DWORD hookAddr = gBaseAddress + 0x6ae01;
        hookSeedLevelJmpBackAddr = hookAddr + hookLen;
        hook((void *)hookAddr, hookSeedLevel, hookLen);
        applyPatches(gSeededModePatches);
        if (gSeededModeState.useDailySeeding) {
          applyPatches(gSeededModeDailySeedingPatches);
        }
      }
    } else {
      if (hookSeedLevelJmpBackAddr) {
        applyPatches(gSeededModePatches, true);
        applyPatches(gSeededModeDailySeedingPatches, true);
        unhook((void *)(gBaseAddress + 0x6ae01));
        hookSeedLevelJmpBackAddr = NULL;
      }
    }
  }
  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::InputScalar("Seed##SeededMode", ImGuiDataType_U32,
                         &gSeededModeState.seed)) {
    gSeededModeState.seed =
        std::clamp(gSeededModeState.seed, (uint32_t)0, UINT32_MAX);
    updateExportedSeed();
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::Checkbox("Use Daily Seeding##SeededMode",
                      &gSeededModeState.useDailySeeding)) {
    if (gModsState.SeededMode && gSeededModeState.useDailySeeding) {
      applyPatches(gSeededModeDailySeedingPatches);
    } else {
      applyPatches(gSeededModeDailySeedingPatches, true);
    }
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  ImGui::Checkbox("Random Seed On Restart##SeededMode",
                  &gSeededModeState.randomSeedOnRestart);

  int itemSelectedIdx = gSeededModeState.advanceOnRestart - 1;
  const char *comboPreviewValue = levelItems[itemSelectedIdx];

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::BeginCombo("Advance On Restart", comboPreviewValue)) {
    for (int n = 0; n < 20; n++) {
      const bool is_selected = (itemSelectedIdx == n);

      if (ImGui::Selectable(levelItems[n], is_selected)) {
        gSeededModeState.advanceOnRestart = n + 1;
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  ImGui::Checkbox("Enable Seed Search", &gSeededModeState.enabledSeedSearch);

  auto isDisabled =
      gGlobalState->screen_state != 0 || gGlobalState->play_state != 0;
  if (isDisabled) {
    ImGui::BeginDisabled();
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::Button("Reset Run")) {
    resetRun();
  }
  if (isDisabled) {
    ImGui::EndDisabled();
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::Button("Add Level Seed##SeededMode")) {
    auto nextLevel = getNextAvailableLevelForSeed();
    if (nextLevel > 0) {
      gSeededModeState.levelSeeds.push_back({nextLevel, 1});
      updateExportedSeed();
    }
  }

  if (gSeededModeState.levelSeeds.size() > 0) {

    ImGui::Text("");
    ImGui::SameLine(20.0f * io.FontGlobalScale);
    if (ImGui::InputText("Import/Export", &gSeededModeState.exportSeed)) {
      ltrim(gSeededModeState.exportSeed);
      rtrim(gSeededModeState.exportSeed);
      loadFromExportSeed();
      return;
    }

    if (ImGui::BeginTable("##LevelSeedTable", 3)) {

      ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 1.0f);
      ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthStretch,
                              2.0f);
      ImGui::TableSetupColumn("Seed", ImGuiTableColumnFlags_WidthStretch, 4.0f);
      ImGui::TableHeadersRow();

      auto usedLevels = getUsedLevelsForSeed();

      for (size_t i = 0; i < gSeededModeState.levelSeeds.size(); i++) {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        if (ImGui::Button(std::format("X##RemoveLevelSeed{}", i).c_str())) {
          gSeededModeState.levelSeeds.erase(
              gSeededModeState.levelSeeds.begin() + i);
          updateExportedSeed();
          continue;
        }

        uint8_t currentLevel = std::get<0>(gSeededModeState.levelSeeds[i]);
        int itemSelectedIdx = currentLevel - 1;
        const char *comboPreviewValue = levelItems[itemSelectedIdx];

        ImGui::TableNextColumn();

        if (ImGui::BeginCombo(std::format("##LevelLevelSeed{}", i).c_str(),
                              comboPreviewValue)) {

          for (int n = 0; n < 22; n++) {
            const bool is_selected = (itemSelectedIdx == n);
            auto flags = ImGuiSelectableFlags_None;
            if (!is_selected && usedLevels.contains(n + 1)) {
              flags = ImGuiSelectableFlags_Disabled;
            }
            if (ImGui::Selectable(levelItems[n], is_selected, flags)) {
              std::get<0>(gSeededModeState.levelSeeds[i]) = n + 1;
              updateExportedSeed();
            }
            if (is_selected)
              ImGui::SetItemDefaultFocus();
          }
          ImGui::EndCombo();
        }

        ImGui::TableNextColumn();
        uint32_t &levelSeed = std::get<1>(gSeededModeState.levelSeeds[i]);
        if (ImGui::InputScalar(std::format("##SeedLevelSeed{}", i).c_str(),
                               ImGuiDataType_U32, &levelSeed)) {
          levelSeed = std::clamp(levelSeed, (uint32_t)0, UINT32_MAX);
          updateExportedSeed();
        }
      }

      ImGui::EndTable();
    }
  }
}
