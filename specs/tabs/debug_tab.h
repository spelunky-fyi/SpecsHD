#pragma once

#include <map>
#include <unordered_set>

#include "../state.h"

enum DataType_ {
  DataType_Byte,
  DataType_Dword_Unsigned,
  DataType_Dword_Signed,
  DataType_Dword_Hex,
  DataType_Float,

  // End of list
  DataType_COUNT,
};

struct EnabledEntities {
  bool activeEntities = false;
  bool floorEntities = false;
  bool floorBgEntities = false;
  bool backgroundEntities = false;
  bool unknown1400 = false;
  bool foregroundEntities = false;
  bool lightEmittingEntities = false;
  bool _4cStructEntities = false;

  int excludeEntityInput = -1;
  std::unordered_set<uint32_t> excluded = {171, 177};
};

struct DrawEntityOffsetsValueHashFunction {
  size_t operator()(const std::pair<DWORD, DataType_> &x) const {
    return x.first ^ x.second;
  }
};

struct DebugState {
  bool EnableTileBorders = false;
  bool EnableBinBorders = false;
  bool EnableRoomBorders = false;
  bool EnablePacifistOverlay = false;
  bool EnableSeededCrateOverlay = false;
  bool EnableSeededPotOverlay = false;
  bool EnableSeededKaliRewards = false;
  bool EnableSeededChestOverlay = false;
  bool DrawEnemyDetection = false;
  bool BlackMarketTrainer = false;
  bool IncludeHitboxOrigins = false;
  bool DrawHHFollowerLink = false;
  bool DrawHHFollowingLink = false;

  EnabledEntities Ids;
  EnabledEntities Hitboxes;
  EnabledEntities Selection;

  // (EntityKind, entity type): [(offset, datatype),...]
  std::map<std::pair<hddll::EntityKind, uint32_t>,
           std::unordered_set<std::pair<DWORD, DataType_>,
                              DrawEntityOffsetsValueHashFunction>>
      DrawEntityOffsets;

  bool DrawSelectedEntHitbox = false;
  bool DrawClosestEntHitbox = false;
  bool DrawClosestEntId = false;

  bool IncludeFloorDecos = false;

  bool DisableOlmecSpawns = false;
  bool DisableOlmecGaps = false;
  bool DisableOlmecCutscene = false;
  bool ShowOlmecCrushProbes = false;

  float MinZCutoff = 0.0f;
  float MaxZCutoff = 99.999f;
};

extern EnabledEntities gAllEntities;
extern DebugState gDebugState;

void drawDebugTab();
void onLevelStartOlmec();
