
#include "mods_tab.h"

#include "../drawing.h"
#include "../game_hooks.h"
#include "../state.h"
#include <hddll/memory.h>
#include <hddll/utils.h>
#include <misc/cpp/imgui_stdlib.h>

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
    hddll::applyPatches(gDarkModePatches, !gModsState.DarkMode);
  };

  ImGui::Separator();
  if (ImGui::Checkbox("The Full Spelunky", &gModsState.TheFullSpelunky)) {
    if (gModsState.TheFullSpelunky && !hookUnlockCoffinsJmpBackAddr) {
      // Hook Coffin Assignments
      int hookLen = 6;
      DWORD hookAddr = hddll::gBaseAddress + 0xe8860;
      hookUnlockCoffinsJmpBackAddr = hookAddr + hookLen;
      hddll::hook((void *)hookAddr, hookUnlockCoffins, hookLen);
      hddll::applyPatches(gFullSpelunkyPatches);
    }
    if (!gModsState.TheFullSpelunky && hookUnlockCoffinsJmpBackAddr) {
      hddll::applyPatches(gFullSpelunkyPatches, true);
      hddll::unhook((void *)(hddll::gBaseAddress + 0xe8860));
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
      hddll::applyPatches(gBiglunkyPatches);
      hddll::applyRelativePatches(gBiglunkyRelativePatches);
    } else {
      hddll::applyPatches(gBiglunkyPatches, true);
      hddll::applyRelativePatches(gBiglunkyRelativePatches, true);
    }
  };

  ImGui::Separator();
  if (ImGui::Checkbox("Uplunky (Beta)", &gModsState.Uplunky)) {
    hddll::applyPatches(gUplunkyPatches, !gModsState.Uplunky);
    hddll::applyRelativePatches(gUplunkyRelativePatches, !gModsState.Uplunky);
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
        DWORD hookAddr = hddll::gBaseAddress + 0x569a5;
        hookWhipJmpBackAddr = hookAddr + hookLen;
        hookWhipSkipWhippingAddr = hddll::gBaseAddress + 0x56a80;
        hddll::hook((void *)hookAddr, hookWhip, hookLen);
      }
    } else {
      if (hookWhipJmpBackAddr) {
        hddll::applyPatches(gTunnelManPatches, true);
        hddll::unhook((void *)(hddll::gBaseAddress + 0x569a5));
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
        DWORD hookAddr = hddll::gBaseAddress + 0x6ae01;
        hookSeedLevelJmpBackAddr = hookAddr + hookLen;
        hddll::hook((void *)hookAddr, hookSeedLevel, hookLen);
        hddll::applyPatches(gSeededModePatches);
        if (gSeededModeState.useDailySeeding) {
          hddll::applyPatches(gSeededModeDailySeedingPatches);
        }
      }
    } else {
      if (hookSeedLevelJmpBackAddr) {
        hddll::applyPatches(gSeededModePatches, true);
        hddll::applyPatches(gSeededModeDailySeedingPatches, true);
        hddll::unhook((void *)(hddll::gBaseAddress + 0x6ae01));
        hookSeedLevelJmpBackAddr = NULL;
      }
    }
  }
  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::InputScalar("Seed##SeededMode", ImGuiDataType_U32,
                         &gSeededModeState.seed)) {
    updateExportedSeed();
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::Checkbox("Use Daily Seeding##SeededMode",
                      &gSeededModeState.useDailySeeding)) {
    if (gModsState.SeededMode && gSeededModeState.useDailySeeding) {
      hddll::applyPatches(gSeededModeDailySeedingPatches);
    } else {
      hddll::applyPatches(gSeededModeDailySeedingPatches, true);
    }
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  ImGui::Checkbox("Random Seed On Restart##SeededMode",
                  &gSeededModeState.randomSeedOnRestart);

  int itemSelectedIdx = std::clamp(
      static_cast<int>(gSeededModeState.advanceOnRestart) - 1, 0, 19);
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

  auto isDisabled = hddll::gGlobalState->screen_state != 0 ||
                    hddll::gGlobalState->play_state != 0;
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
      hddll::ltrim(gSeededModeState.exportSeed);
      hddll::rtrim(gSeededModeState.exportSeed);
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
          i--;
          continue;
        }

        uint8_t currentLevel = std::get<0>(gSeededModeState.levelSeeds[i]);
        int itemSelectedIdx =
            std::clamp(static_cast<int>(currentLevel) - 1, 0, 21);
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
          updateExportedSeed();
        }
      }

      ImGui::EndTable();
    }
  }
}
