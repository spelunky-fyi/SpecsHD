#pragma once

#include "../memory.h"
#include "../state.h"

extern std::vector<Patch> gBiglunkyPatches;
extern std::vector<RelativePatch> gBiglunkyRelativePatches;

void prePlaceRoomsBiglunky();
void postPlaceRoomsBiglunky();
void preGenerateRoomBiglunky();
void preSpawnTilesBiglunky();
