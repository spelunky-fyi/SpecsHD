#pragma once

#include <cstdint>

struct LockableU8 {
  bool IsLocked = false;
  uint8_t LockedValue = 0;
};

struct LevelsState {
  LockableU8 FloodedMines = {};
  LockableU8 SkinIsCrawling = {};

  LockableU8 DeadAreRestless = {};
  LockableU8 RushingWater = {};
  LockableU8 HauntedCastle = {};
  LockableU8 TikiVillage = {};
  LockableU8 BlackMarket = {};

  LockableU8 WetFur = {};
  LockableU8 MotherShip = {};
  LockableU8 Worm = {};

  LockableU8 CityOfGold = {};

  LockableU8 AltarSpawned = {};
  LockableU8 IdolSpawned = {};
  LockableU8 DamselSpawned = {};
  LockableU8 GhostSpawned = {};
  LockableU8 VaultSpawnedInArea = {};
};

extern LevelsState gLevelsState;

void warpToLevel(uint32_t level);
void resetRun();
void ensureLockedLevelsState();
void drawLevelTab();
