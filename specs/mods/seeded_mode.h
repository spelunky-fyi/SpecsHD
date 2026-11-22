#pragma once

#include <unordered_set>

#include "../state.h"
#include <hddll/memory.h>

struct SeededModeState {
  uint32_t seed = 1;
  std::vector<std::tuple<uint32_t, uint32_t>> levelSeeds = {};
  bool useDailySeeding = false;
  bool forceMothershipCoffin = false;
  bool randomSeedOnRestart = false;
  uint32_t advanceOnRestart = 1;
  bool rollback = false;
  uint32_t rollbackLevels = 1;
  bool rollbackFlagplant = true;
  bool showRollbackOverlay = true;
  std::string exportSeed = "";
};

extern SeededModeState gSeededModeState;

extern std::vector<hddll::Patch> gSeededModePatches;
extern std::vector<hddll::Patch> gSeededModeDailySeedingPatches;

extern const char *levelItems[];

void chooseRandomSeed();
void advanceLevel();
void prePlaceRoomsSeededMode();

// screen_state while in the lobby (camp)
const uint32_t SCREEN_STATE_LOBBY = 22;

bool startRollback();
void resetRollback();
void resetRollbackFlag();
void preSeedLevelRollback();
size_t getRollbackSavedLevelCount();
// Name of the level dying now would go back to, or empty
std::string getRollbackDestination();
// What the last restart did, or empty
const std::string &getRollbackLastRestart();

void updateExportedSeed();
void loadFromExportSeed();

std::unordered_set<uint8_t> getUsedLevelsForSeed();
uint8_t getNextAvailableLevelForSeed();
std::vector<uint8_t> getAvailableLevelsForSeed();
