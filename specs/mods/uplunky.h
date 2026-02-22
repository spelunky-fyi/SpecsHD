#pragma once

#include "../memory.h"
#include "../state.h"

extern std::vector<Patch> gUplunkyPatches;
extern std::vector<RelativePatch> gUplunkyRelativePatches;

void resetUplunkyState();
void prePlaceRoomsUplunky();
void postPlaceRoomsUplunky();
