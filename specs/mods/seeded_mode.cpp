
#include "seeded_mode.h"

#include <fstream>

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

static void searchStraightDown() {
  auto entranceRoomNumber =
      hddll::gGlobalState->level_state->entrance_room_x +
      hddll::gGlobalState->level_state->entrance_room_y * 4;

  auto roomType =
      hddll::gGlobalState->level_state->room_types[entranceRoomNumber];
  auto roomType2 =
      hddll::gGlobalState->level_state->room_types[entranceRoomNumber + 4];
  auto roomType3 =
      hddll::gGlobalState->level_state->room_types[entranceRoomNumber + 8];
  auto exitRoomNumber = hddll::gGlobalState->level_state->exit_room_x +
                        hddll::gGlobalState->level_state->exit_room_y * 4;

  if (roomType != 2 || roomType2 != 2 || roomType3 != 2 ||
      entranceRoomNumber + 12 != exitRoomNumber) {
    resetRun();
  }
}

static void searchNoEggy13() {
  auto hasBox = false;
  auto hasIdol = false;
  auto hasAltar = false;

  auto boxPos = 0;
  auto idolPos = 0;
  auto altarPos = 0;

  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = hddll::gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_kind == hddll::EntityKind::KIND_ITEM) {
      auto entity_item = (hddll::EntityItem *)ent;
      if (entity_item->field52_0x1f0 == 1 and entity_item->entity_type == 248) {
        hasBox = true;
        boxPos = hddll::GetRoomForPosition(ent->x, ent->y) / 4;
      }
    }
  }

  for (auto idx = 0; idx < hddll::ENTITY_FLOORS_COUNT; idx++) {
    auto ent = hddll::gGlobalState->level_state->entity_floors[idx];
    if (!ent) {
      continue;
    }
    if (ent->entity_type == 35) {
      hasAltar = true;
      altarPos = hddll::GetRoomForPosition(ent->x, ent->y) / 4;
    }
    if (ent->entity_type == 15) {
      hasIdol = true;
      idolPos = hddll::GetRoomForPosition(ent->x, ent->y) / 4;
    }
  }

  if (hasBox && hasAltar && hasIdol && boxPos <= idolPos &&
      idolPos == altarPos) {
    std::ofstream file("1-3-seeds.txt", std::ios_base::app);
    if (file.is_open())
      file << lastSeed << std::endl;
    resetRun();
  } else {
    resetRun();
  }
}

static int getFirstTileForRoomIdx(int roomIdx) {
  const int startIdx = 141;
  const int roomHeight = 8;
  const int roomWidth = 10;
  const int rowWidth = 46;

  auto roomY = roomIdx / 4;
  auto roomX = roomIdx % 4;

  return startIdx + (roomY * roomHeight * rowWidth) + (roomX * roomWidth);
}

static int getTileByCoord(int x, int y) {
  const int startIdx = 141;
  const int rowWidth = 46;

  return hddll::ENTITY_FLOORS_COUNT - (y * rowWidth) + x;
}

static void searchDaR() {
  auto spawnedBM = hddll::gGlobalState->spawned_black_market_entrance;

  auto hasHC = false;
  auto hcRoom = 0;

  for (auto idx = 0; idx < 48; idx++) {
    auto room_type = hddll::gGlobalState->level_state->room_types[idx];
    if (room_type == 47) {
      hcRoom = idx;
      hasHC = true;
      break;
    }
  }

  if (!hasHC || spawnedBM) {
    resetRun();
    return;
  }

  auto roomBelowHC = hcRoom + 4;
  auto tileStart = getFirstTileForRoomIdx(roomBelowHC);

  for (auto idx = 0; idx < 10; idx++) {
    auto tile =
        hddll::gGlobalState->level_state->entity_floors[tileStart + idx];
    if (!tile || tile->entity_type == 4) {
      continue;
    } else {
      resetRun();
      return;
    }
  }

  for (auto idx = 0; idx < 10; idx++) {
    auto tile =
        hddll::gGlobalState->level_state->entity_floors[tileStart + 46 + idx];
    if (!tile) {
      resetRun();
      return;
    }
    if (tile->entity_type == 9097 || tile->entity_type == 4) {
      continue;
    } else {
      resetRun();
      return;
    }
  }

  std::ofstream file("haunted-castle-black-market-seeds.txt",
                     std::ios_base::app);
  if (file.is_open())
    file << lastSeed << std::endl;
  resetRun();
}

static int hasBombChest() {
  auto bombCount = 0;
  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = hddll::gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type == 100) {
      auto entity_types = getChestItemsForSeed(ent->z_depth_as_int);
      bombCount += std::count(entity_types.begin(), entity_types.end(), 107);
    }
  }

  return bombCount;
}

static void searchHC() {
  auto spawnedBM = hddll::gGlobalState->spawned_black_market_entrance;
  if (!spawnedBM) {
    resetRun();
    return;
  }

  auto bmX = hddll::gGlobalState->level_state->alt_exit_x;
  auto bmY = hddll::gGlobalState->level_state->alt_exit_y;

  auto bmXGood = bmX >= 30.0 && bmX <= 31.0;
  auto bmYGood = bmY == 69.0;

  if (!bmXGood || !bmYGood) {
    resetRun();
    return;
  }

  std::ofstream file("haunted-castle-seeds.txt", std::ios_base::app);
  if (file.is_open())
    file << lastSeed << std::endl;
  resetRun();
}

static void searchWorm() {

  auto wormEggCount = 0;
  auto bacteriumCount = 0;
  auto ufoCount = 0;
  auto yetiCount = 0;
  auto skeletonCount = 0;

  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = hddll::gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type == 1046) {
      wormEggCount++;
    } else if (ent->entity_type == 1035) {
      bacteriumCount++;
    } else if (ent->entity_type == 1010) {
      ufoCount++;
    } else if (ent->entity_type == 1009) {
      yetiCount++;
    } else if (ent->entity_type == 1012) {
      skeletonCount++;
    }
  }

  std::ofstream file("worms.txt", std::ios_base::app);
  if (!file.is_open()) {
    resetRun();
    return;
  }
  file << lastSeed << "," << wormEggCount << "," << bacteriumCount << ","
       << ufoCount << "," << yetiCount << "," << skeletonCount << std::endl;
  resetRun();
}

static void search34() {
  if (hddll::gGlobalState->level == 9) {
    hddll::gGlobalState->is_worm = 1;
    warpToLevel(hddll::gGlobalState->level);
    return;
  } else if (hddll::gGlobalState->level == 10) {
    warpToLevel(hddll::gGlobalState->level);
    return;
  }

  auto rightMoship =
      hddll::gGlobalState->level_state->entity_floors[getTileByCoord(33, 96)];
  auto leftMoship =
      hddll::gGlobalState->level_state->entity_floors[getTileByCoord(12, 96)];

  if (rightMoship && rightMoship->entity_type == 81) {
    auto block =
        hddll::gGlobalState->level_state->entity_floors[getTileByCoord(32, 97)];
    auto blockAbove =
        hddll::gGlobalState->level_state->entity_floors[getTileByCoord(32, 98)];
    if (!block || block->entity_type != 9098 || blockAbove) {
      resetRun();
      return;
    }
  } else if (leftMoship && leftMoship->entity_type == 81) {
    auto block =
        hddll::gGlobalState->level_state->entity_floors[getTileByCoord(13, 97)];
    auto blockAbove =
        hddll::gGlobalState->level_state->entity_floors[getTileByCoord(13, 98)];
    if (!block || block->entity_type != 9098 || blockAbove) {
      resetRun();
      return;
    }
  } else {
    resetRun();
    return;
  }

  std::ofstream file("34-seeds.txt", std::ios_base::app);
  if (file.is_open())
    file << lastSeed << std::endl;
  resetRun();
}

static void searchSeeds() {
  extern uint32_t lastSeed;

  std::ofstream cratesFile;
  std::ofstream shopItemsFile;
  cratesFile.open("crates.txt", std::ios_base::app);
  shopItemsFile.open("shop_items.txt", std::ios_base::app);
  if (!cratesFile.is_open() || !shopItemsFile.is_open())
    return;

  auto seedForLevel = lastSeed;
  auto formattedLevel = hddll::formatLevel(hddll::gGlobalState->level);

  std::vector<hddll::EntityItem *> shopItems;

  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = hddll::gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type == 101) {
      auto entity_type = getCrateItemForSeed(ent->z_depth_as_int);
      auto entity_name = hddll::EntityTypeName(entity_type);
      cratesFile << seedForLevel << "," << formattedLevel << "," << entity_name
                 << std::endl;
    }

    if (ent->entity_kind == hddll::EntityKind::KIND_ITEM) {
      auto entity_item = (hddll::EntityItem *)ent;
      if (entity_item->field52_0x1f0 == 1) {
        shopItems.push_back(entity_item);
      }
    }
  }

  if (shopItems.size() > 0) {
    shopItemsFile << seedForLevel << "," << formattedLevel;
    for (auto entity_item : shopItems) {
      auto entity_name = hddll::EntityTypeName(entity_item->entity_type);
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

  auto isDisabled = hddll::gGlobalState->screen_state != 0 ||
                    hddll::gGlobalState->play_state != 0;

  if (isDisabled)
    return;

  if (hddll::gGlobalState->level < gSeededModeState.advanceOnRestart) {
    warpToLevel(hddll::gGlobalState->level);
    return;
  }

  if (gSeededModeState.enabledSeedSearch) {
    searchSeeds();
  }
}
