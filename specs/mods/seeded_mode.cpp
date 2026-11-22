
#include "seeded_mode.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <optional>

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

// Rollback
//
// Each level's starting state is saved as it loads, in the order the levels
// are played. Dying restarts the run straight into the level played
// rollbackLevels before the one died on and puts its saved state back.
//
// Levels are found by play order, not level number. The mothership is
// entered from 3-4 and is level 11 (3-3), with 3-4 played again after it, and
// the worm skips the levels it replaces.
//
// The run flags levels are generated from (moai spawned, black market
// entrance spawned, ...) are saved and restored as the level is seeded,
// before anything is rolled, so the level generates the same as the first
// time. The player's state and the run timer are restored just before the
// rooms are placed.

// GlobalState byte ranges saved with each level
const size_t RUN_FLAGS_BEGIN = offsetof(hddll::GlobalState, dark_level);
const size_t RUN_FLAGS_END = offsetof(hddll::GlobalState, pad_44062C);
const size_t RUN_TIMER_BEGIN = offsetof(hddll::GlobalState, total_minutes);
const size_t RUN_TIMER_END = offsetof(hddll::GlobalState, pad_445968);

// The game's money anticheat (0x45b410) runs as each level ends. It allows
// the money from its last check (stored as -7 * money) plus gold picked up
// since (stored as -12 * amount), held gold and 5000, and sets total_money to
// 1 if there's more. resetForRun zeroes both, so they're restored with the
// money or a rollback can trip it.
const size_t MONEY_CHECKPOINT = offsetof(hddll::GlobalState, pad_447350) + 0x8;
const size_t MONEY_PICKED_UP = offsetof(hddll::GlobalState, pad_445968) + 0x5C;
static_assert(MONEY_CHECKPOINT == 0x447358);
static_assert(MONEY_PICKED_UP == 0x4459C4);

// Which special level loads is decided from these before the level is
// seeded, so a rollback sets them before restarting into it
static constexpr uint8_t hddll::GlobalState::*SPECIAL_LEVEL_FLAGS[] = {
    &hddll::GlobalState::is_haunted_castle,
    &hddll::GlobalState::at_haunted_castle_exit,
    &hddll::GlobalState::is_blackmarket,
    &hddll::GlobalState::at_blackmarket_exit,
    &hddll::GlobalState::is_mothership,
    &hddll::GlobalState::at_mothership_exit,
    &hddll::GlobalState::is_city_of_gold,
    &hddll::GlobalState::at_city_of_gold_exit,
    &hddll::GlobalState::is_worm,
};

struct RollbackSnapshot {
  uint32_t level = 0;
  std::string name = "";

  // Saved as the level is seeded
  std::array<uint8_t, RUN_FLAGS_END - RUN_FLAGS_BEGIN> runFlags = {};
  std::array<uint8_t, std::size(SPECIAL_LEVEL_FLAGS)> specialLevelFlags = {};

  // Saved just before the rooms are placed
  bool hasPlayerState = false;
  hddll::PlayerData playerData = {};
  uint32_t money = 0;
  int32_t moneyCheckpoint = 0;
  int32_t moneyPickedUp = 0;
  int32_t favor = 0;
  std::array<uint8_t, RUN_TIMER_END - RUN_TIMER_BEGIN> runTimer = {};
};

// In the order the levels were played. While rolling back the last one is
// the level being restarted into.
static std::vector<RollbackSnapshot> gRollbackHistory = {};
static bool gRollingBack = false;
// Level the player was dead on when last seen in a level, or 0
static uint32_t gRollbackDiedOnLevel = 0;
// respawn_level from before a rollback pointed it at the target level
static std::optional<uint32_t> gRollbackRespawnLevel = {};
// Special level flags from before a rollback changed them, to put back if the
// reset was for going to the lobby instead of a restart
static std::array<uint8_t, std::size(SPECIAL_LEVEL_FLAGS)>
    gRollbackPrevSpecialLevelFlags = {};
// Already warped to the target level after the restart missed it
static bool gRollbackWarped = false;
// What the last restart did, for the Mods tab
static std::string gRollbackLastRestart = "";

static uint8_t *globalStateBytes(size_t offset) {
  return reinterpret_cast<uint8_t *>(hddll::gGlobalState) + offset;
}

static void restoreRespawnLevel() {
  if (gRollbackRespawnLevel) {
    hddll::gGlobalState->respawn_level = *gRollbackRespawnLevel;
    gRollbackRespawnLevel.reset();
  }
}

void resetRollback() {
  restoreRespawnLevel();
  gRollbackHistory.clear();
  gRollingBack = false;
  gRollbackDiedOnLevel = 0;
  gRollbackWarped = false;
  gRollbackLastRestart = "";
}

const std::string &getRollbackLastRestart() { return gRollbackLastRestart; }

static std::string getRollbackLevelName() {
  auto globalState = hddll::gGlobalState;
  auto name = hddll::formatLevel(static_cast<uint8_t>(globalState->level));
  if (globalState->is_worm) {
    return name + " W";
  } else if (globalState->is_blackmarket) {
    return name + " BM";
  } else if (globalState->is_haunted_castle) {
    return name + " HC";
  } else if (globalState->is_mothership) {
    return name + " MS";
  } else if (globalState->is_city_of_gold) {
    return name + " CoG";
  }
  return name;
}

// Index in gRollbackHistory a death on diedOnLevel goes back to
static std::optional<size_t> getRollbackTargetIndex(uint32_t diedOnLevel) {
  if (gRollbackHistory.empty()) {
    return {};
  }

  // The level died on is the last one saved, unless it started before
  // Rollback was turned on
  auto current = gRollbackHistory.size();
  if (gRollbackHistory.back().level == diedOnLevel) {
    current--;
  }

  auto levelsBack =
      (std::min)(static_cast<size_t>(gSeededModeState.rollbackLevels), current);
  return (std::min)(current - levelsBack, gRollbackHistory.size() - 1);
}

size_t getRollbackSavedLevelCount() { return gRollbackHistory.size(); }

std::string getRollbackDestination() {
  auto index = getRollbackTargetIndex(hddll::gGlobalState->level);
  return index ? gRollbackHistory[*index].name : "";
}

static void setSpecialLevelFlags(const RollbackSnapshot &snapshot) {
  for (size_t idx = 0; idx < std::size(SPECIAL_LEVEL_FLAGS); idx++) {
    hddll::gGlobalState->*SPECIAL_LEVEL_FLAGS[idx] =
        snapshot.specialLevelFlags[idx];
  }
}

static void updateRollbackDeath() {
  if (!gSeededModeState.rollback) {
    return;
  }

  // Some deaths (crushed, lava, ...) remove the player instead of leaving a
  // body, so a missing player counts as dead too. Only being seen alive again
  // (the ankh) clears it.
  auto player = hddll::gGlobalState->player1;
  auto alive = player && player->health > 0 && !player->flag_deletion;
  gRollbackDiedOnLevel = alive ? 0 : hddll::gGlobalState->level;
}

// Runs at the end of resetForRun so the run has already been reset
bool startRollback() {
  restoreRespawnLevel();

  auto diedOnLevel = gRollbackDiedOnLevel;
  gRollbackDiedOnLevel = 0;
  gRollingBack = false;
  gRollbackWarped = false;

  if (!gSeededModeState.rollback) {
    gRollbackHistory.clear();
    return false;
  }

  // Going back to the lobby resets the run too, but isn't a restart
  if (hddll::gGlobalState->screen_state == SCREEN_STATE_LOBBY) {
    gRollbackLastRestart = "full reset, went to the lobby";
    gRollbackHistory.clear();
    return false;
  }

  auto index = getRollbackTargetIndex(diedOnLevel);
  if (diedOnLevel == 0 || !index) {
    gRollbackLastRestart = diedOnLevel == 0 ? "full reset, no death seen"
                                            : "full reset, no levels saved";
    gRollbackHistory.clear();
    return false;
  }

  // Levels after the target are from a run that's been left behind
  gRollbackHistory.resize(*index + 1);
  gRollingBack = true;
  auto &target = gRollbackHistory.back();
  gRollbackLastRestart = std::format("went back to {}", target.name);

  // The run restarts on the level after respawn_level once this reset
  // returns, so point it at the target level until that level is seeded
  // level is left alone: going to the lobby also resets the run and builds the
  // lobby from it.
  gRollbackRespawnLevel = hddll::gGlobalState->respawn_level;
  for (size_t idx = 0; idx < std::size(SPECIAL_LEVEL_FLAGS); idx++) {
    gRollbackPrevSpecialLevelFlags[idx] =
        hddll::gGlobalState->*SPECIAL_LEVEL_FLAGS[idx];
  }
  hddll::gGlobalState->respawn_level = target.level - 1;
  setSpecialLevelFlags(target);
  return true;
}

// The reset turned out to be for going to the lobby, so undo the rollback
// before the lobby is built from the target level
static void cancelRollbackForLobby() {
  restoreRespawnLevel();
  for (size_t idx = 0; idx < std::size(SPECIAL_LEVEL_FLAGS); idx++) {
    hddll::gGlobalState->*SPECIAL_LEVEL_FLAGS[idx] =
        gRollbackPrevSpecialLevelFlags[idx];
  }
  gRollbackHistory.clear();
  gRollingBack = false;
  gRollbackWarped = false;
  gRollbackLastRestart = "full reset, went to the lobby";
}

static bool isRollbackLevel(uint32_t level) {
  return gSeededModeState.rollback && level >= 1 && level <= 20;
}

// The snapshot to restore for a level that's loading, if rolling back to it
static const RollbackSnapshot *getRollbackTarget(uint32_t level) {
  if (!gRollingBack || gRollbackHistory.back().level != level) {
    return nullptr;
  }
  return &gRollbackHistory.back();
}

static void restorePlayerState(const RollbackSnapshot &snapshot) {
  hddll::gGlobalState->player1_data = snapshot.playerData;
  hddll::gGlobalState->total_money = snapshot.money;
  std::memcpy(globalStateBytes(MONEY_CHECKPOINT), &snapshot.moneyCheckpoint,
              sizeof(snapshot.moneyCheckpoint));
  std::memcpy(globalStateBytes(MONEY_PICKED_UP), &snapshot.moneyPickedUp,
              sizeof(snapshot.moneyPickedUp));
  hddll::gGlobalState->total_favor = snapshot.favor;
  std::memcpy(globalStateBytes(RUN_TIMER_BEGIN), snapshot.runTimer.data(),
              snapshot.runTimer.size());
  // Hired hand textures are loaded during the level transition, which was
  // before the hired hands were put back
  loadHiredHandTextures();
}

void preSeedLevelRollback() {
  // The restart has read respawn_level by the time a level is seeded
  restoreRespawnLevel();

  // A restart reads the death before the next level is seeded, so one still
  // set here was seen while leaving the last level (e.g. through the exit)
  gRollbackDiedOnLevel = 0;

  auto level = hddll::gGlobalState->level;
  if (!isRollbackLevel(level)) {
    return;
  }

  if (gRollingBack) {
    if (auto target = getRollbackTarget(level)) {
      std::memcpy(globalStateBytes(RUN_FLAGS_BEGIN), target->runFlags.data(),
                  target->runFlags.size());
    }
    return;
  }

  // Levels skipped by Advance On Restart aren't part of the run
  if (level < gSeededModeState.advanceOnRestart) {
    return;
  }

  // A level seeded again before its rooms are placed replaces its snapshot
  if (gRollbackHistory.empty() || gRollbackHistory.back().level != level ||
      gRollbackHistory.back().hasPlayerState) {
    gRollbackHistory.emplace_back();
  }

  auto &snapshot = gRollbackHistory.back();
  snapshot = {};
  snapshot.level = level;
  snapshot.name = getRollbackLevelName();
  std::memcpy(snapshot.runFlags.data(), globalStateBytes(RUN_FLAGS_BEGIN),
              snapshot.runFlags.size());
  for (size_t idx = 0; idx < std::size(SPECIAL_LEVEL_FLAGS); idx++) {
    snapshot.specialLevelFlags[idx] =
        hddll::gGlobalState->*SPECIAL_LEVEL_FLAGS[idx];
  }
}

static void prePlaceRoomsRollback() {
  auto level = hddll::gGlobalState->level;
  if (!isRollbackLevel(level)) {
    return;
  }

  if (gRollingBack) {
    auto target = getRollbackTarget(level);
    if (target && target->hasPlayerState) {
      restorePlayerState(*target);
    }
    return;
  }

  // Only levels saved as they were seeded
  if (gRollbackHistory.empty() || gRollbackHistory.back().level != level ||
      gRollbackHistory.back().hasPlayerState) {
    return;
  }

  auto &snapshot = gRollbackHistory.back();
  snapshot.playerData = hddll::gGlobalState->player1_data;
  snapshot.money = hddll::gGlobalState->total_money;
  std::memcpy(&snapshot.moneyCheckpoint, globalStateBytes(MONEY_CHECKPOINT),
              sizeof(snapshot.moneyCheckpoint));
  std::memcpy(&snapshot.moneyPickedUp, globalStateBytes(MONEY_PICKED_UP),
              sizeof(snapshot.moneyPickedUp));
  snapshot.favor = hddll::gGlobalState->total_favor;
  std::memcpy(snapshot.runTimer.data(), globalStateBytes(RUN_TIMER_BEGIN),
              snapshot.runTimer.size());
  snapshot.hasPlayerState = true;
}

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

  prePlaceRoomsRollback();
}

void advanceLevel() {

  if (!gModsState.SeededMode) {
    return;
  }

  if (gRollingBack &&
      hddll::gGlobalState->screen_state == SCREEN_STATE_LOBBY) {
    cancelRollbackForLobby();
    return;
  }

  auto isDisabled = hddll::gGlobalState->screen_state != 0 ||
                    hddll::gGlobalState->play_state != 0;

  if (isDisabled)
    return;

  updateRollbackDeath();

  if (gRollingBack) {
    restoreRespawnLevel();
    auto &target = gRollbackHistory.back();
    if (hddll::gGlobalState->level != target.level && !gRollbackWarped) {
      // The restart didn't land on the target level
      gRollbackWarped = true;
      warpToLevel(target.level - 1);
      setSpecialLevelFlags(target);
    } else {
      gRollingBack = false;
    }
    return;
  }

  if (hddll::gGlobalState->level < gSeededModeState.advanceOnRestart) {
    warpToLevel(hddll::gGlobalState->level);
    return;
  }
}
