
#include "seeded_mode.h"


SeededModeState gSeededModeState = {};

#include "../drawing.h"
#include "../game_hooks.h"
#include "../seeded_items.h"
#include "../tabs/level_tab.h"
#include <hddll/entities.h>
#include <hddll/utils.h>

std::vector<hddll::Patch> gSeededModePatches = {
    // Seed Kali Drops
    {0x1531c, {0x90, 0x90}, {0x74, 0x0c}},

    // Seed Item Drops
    {0x214ac, {0x90, 0x90}, {0x74, 0x08}},

    // Seed Crate Opens
    {0x3332f, {0x90, 0x90}, {0x74, 0x08}},

    // Seed Chest Opens
    {0x33810, {0x90, 0x90}, {0x74, 0x08}},

    // Seed Monster Drops
    {0x36d69, {0x90, 0x90}, {0x74, 0x08}},

    // Restore Stolen Bytes from Hook
    {0x6ae01, {}, {0x52, 0xff, 0xd6, 0x8b, 0x44, 0x24, 0x18}},

    // Never allow coffins
    {0xe887c, {0x0}, {0x1}},

    // Force Dark Levels
    {0x6afa6, {0x90, 0x90}, {0x74, 0x10}},
    {0x6bae1, {0x0}, {0x1}},
};

std::vector<hddll::Patch> gSeededModeDailySeedingPatches = {
    // Remove Roulette Wheel
    {0xdeef8,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x84, 0x78, 0x00, 0x00, 0x00}},
};

const char *levelItems[] = {
    "1-1", "1-2", "1-3", "1-4", "2-1",   "2-2",   "2-3", "2-4",
    "3-1", "3-2", "3-3", "3-4", "4-1",   "4-2",   "4-3", "4-4",
    "5-1", "5-2", "5-3", "5-4", "3-UFO", "3-4.2",
};

const uint8_t MAX_SEED_OVERRIDES = 22;

void updateExportedSeed() {
  std::string seed = std::format("{}", gSeededModeState.seed);
  for (auto [level, levelSeed] : gSeededModeState.levelSeeds) {
    seed += std::format(";{}:{}", level, levelSeed);
  }
  gSeededModeState.exportSeed = seed;
}

void loadFromExportSeed() {
  auto seed = gSeededModeState.exportSeed;
  auto parts = hddll::split(seed, ';');
  try {
    if (parts.size() > 0) {
      gSeededModeState.seed = std::stoi(parts[0]);
    }
    if (parts.size() > 1) {
      gSeededModeState.levelSeeds.clear();
      auto levelSeeds =
          std::vector<std::string>(parts.begin() + 1, parts.end());
      for (auto levelSeed : levelSeeds) {
        auto levelSeedParts = hddll::split(levelSeed, ':');
        if (levelSeedParts.size() == 2) {
          gSeededModeState.levelSeeds.push_back(
              {std::stoi(levelSeedParts[0]), stoul(levelSeedParts[1])});
        }
      }
    }
  } catch (const std::exception &) {
    // Invalid input, ignore
  }
}

std::unordered_set<uint8_t> getUsedLevelsForSeed() {
  std::unordered_set<uint8_t> usedLevels = {};
  for (auto [level, seed] : gSeededModeState.levelSeeds) {
    usedLevels.insert(level);
  }

  return usedLevels;
}

uint8_t getNextAvailableLevelForSeed() {
  std::unordered_set<uint8_t> usedLevels = getUsedLevelsForSeed();
  for (uint8_t i = 1; i <= MAX_SEED_OVERRIDES; i++) {
    if (usedLevels.find(i) == usedLevels.end()) {
      return i;
    }
  }

  return 0;
}

std::vector<uint8_t> getAvailableLevelsForSeed() {
  std::unordered_set<uint8_t> usedLevels = getUsedLevelsForSeed();
  std::vector<uint8_t> availableLevels = {};
  for (uint8_t i = 1; i <= MAX_SEED_OVERRIDES; i++) {
    if (usedLevels.find(i) == usedLevels.end()) {
      availableLevels.push_back(i);
    }
  }

  return availableLevels;
}

uint32_t getRandomSeed() {
  return (static_cast<uint32_t>(std::rand()) << 16) |
         static_cast<uint32_t>(std::rand());
}

void chooseRandomSeed() { gSeededModeState.seed = getRandomSeed(); }

// Seeded mode stops the game assigning coffin characters (see "Never allow
// coffins"), so coffin_char stays -1 and no coffin room is placed. Setting
// it to the Robot before rooms are placed lets the game's own coffin
// placement put the mothership coffin down like an unmodded run would.
static bool gForcedMothershipCoffin = false;

void prePlaceRoomsSeededMode() {
  auto isMothership = hddll::gGlobalState->level == 11 &&
                      hddll::gGlobalState->is_mothership;

  if (gSeededModeState.forceMothershipCoffin && isMothership) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_ROBOT;
    // The game loads the coffin texture when it assigns coffin_char during
    // the level transition, which is before this. Load it again like the
    // game does after its own late assignments (e.g. Carl in the BM).
    hddll::LoadCoffinTexture(hddll::gGlobalState->_34struct);
    gForcedMothershipCoffin = true;
  } else if (gForcedMothershipCoffin) {
    // Don't let a coffin that couldn't be placed carry over to later levels
    hddll::gGlobalState->_34struct->coffin_char = -1;
    gForcedMothershipCoffin = false;
  }
}

void advanceLevel() {

  if (!gModsState.SeededMode) {
    return;
  }

  auto isDisabled = hddll::gGlobalState->screen_state != 0 ||
                    hddll::gGlobalState->play_state != 0;

  if (isDisabled)
    return;

  if (hddll::gGlobalState->level < gSeededModeState.advanceOnRestart) {
    warpToLevel(hddll::gGlobalState->level);
    return;
  }
}
