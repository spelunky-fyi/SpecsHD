#pragma once

#include "../state.h"
#include <hddll/memory.h>

extern std::vector<hddll::Patch> gBiglunkyPatches;
extern std::vector<hddll::RelativePatch> gBiglunkyRelativePatches;

void prePlaceRoomsBiglunky();
void postPlaceRoomsBiglunky();
void preGenerateRoomBiglunky();
void preSpawnTilesBiglunky();
