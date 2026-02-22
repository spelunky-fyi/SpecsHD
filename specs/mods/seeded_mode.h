#pragma once

#include <unordered_set>

#include "../memory.h"
#include "../state.h"

struct SeededModeState {
  uint32_t seed = 1;
  std::vector<std::tuple<uint32_t, uint32_t>> levelSeeds = {};
  bool useDailySeeding = false;
  bool randomSeedOnRestart = false;
  uint32_t advanceOnRestart = 1;
  bool enabledSeedSearch = false;
  std::string exportSeed = "";
};

extern SeededModeState gSeededModeState;

extern std::vector<Patch> gSeededModePatches;
extern std::vector<Patch> gSeededModeDailySeedingPatches;

extern const char *levelItems[];

void chooseRandomSeed();
void advanceLevel();

void updateExportedSeed();
void loadFromExportSeed();

std::unordered_set<uint8_t> getUsedLevelsForSeed();
uint8_t getNextAvailableLevelForSeed();
std::vector<uint8_t> getAvailableLevelsForSeed();
