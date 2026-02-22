
#include "seeded_mode.h"

#include <fstream>

SeededModeState gSeededModeState = {};

#include "../drawing.h"
#include "../entities.h"
#include "../seeded_items.h"
#include "../tabs/level_tab.h"
#include "../utils.h"

std::vector<Patch> gSeededModePatches = {
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

std::vector<Patch> gSeededModeDailySeedingPatches = {
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
  auto parts = split(seed, ';');
  if (parts.size() > 0) {
    gSeededModeState.seed = std::stoi(parts[0]);
  }
  if (parts.size() > 1) {
    gSeededModeState.levelSeeds.clear();
    auto levelSeeds = std::vector<std::string>(parts.begin() + 1, parts.end());
    for (auto levelSeed : levelSeeds) {
      auto levelSeedParts = split(levelSeed, ':');
      if (levelSeedParts.size() == 2) {
        gSeededModeState.levelSeeds.push_back(
            {std::stoi(levelSeedParts[0]), stoul(levelSeedParts[1])});
      }
    }
  }
}

std::unordered_set<uint8_t> getUsedLevelsForSeed() {
  std::unordered_set<uint8_t> usedLevels = {};
  for (auto [level, seed] : gSeededModeState.levelSeeds) {
    usedLevels.insert(level);
    updateExportedSeed();
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
  return std::rand() % UINT32_MAX * (UINT32_MAX / RAND_MAX);
}

void chooseRandomSeed() { gSeededModeState.seed = getRandomSeed(); }

static void searchSeeds() {
  extern uint32_t lastSeed;

  std::ofstream cratesFile;
  std::ofstream shopItemsFile;
  cratesFile.open("crates.txt", std::ios_base::app);
  shopItemsFile.open("shop_items.txt", std::ios_base::app);

  auto seedForLevel = lastSeed;
  auto formattedLevel = formatLevel(gGlobalState->level);

  std::vector<EntityItem *> shopItems;

  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type == 101) {
      auto entity_type = getCrateItemForSeed(ent->z_depth_as_int);
      auto entity_name = EntityTypeName(entity_type);
      cratesFile << seedForLevel << "," << formattedLevel << "," << entity_name
                 << std::endl;
    }

    if (ent->entity_kind == EntityKind::KIND_ITEM) {
      auto entity_item = (EntityItem *)ent;
      if (entity_item->field52_0x1f0 == 1) {
        shopItems.push_back(entity_item);
      }
    }
  }

  if (shopItems.size() > 0) {
    shopItemsFile << seedForLevel << "," << formattedLevel;
    for (auto entity_item : shopItems) {
      auto entity_name = EntityTypeName(entity_item->entity_type);
      shopItemsFile << "," << entity_name;
    }
    shopItemsFile << std::endl;
  }

  resetRun();
}

void advanceLevel() {

  if (!gModsState.SeededMode) {
    return;
  }

  auto isDisabled =
      gGlobalState->screen_state != 0 || gGlobalState->play_state != 0;

  if (isDisabled)
    return;

  if (gGlobalState->level < gSeededModeState.advanceOnRestart) {
    warpToLevel(gGlobalState->level);
    return;
  }

  if (gSeededModeState.enabledSeedSearch) {
    searchSeeds();
  }
}
