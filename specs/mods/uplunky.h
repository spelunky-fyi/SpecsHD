#pragma once

#include "../state.h"
#include <hddll/memory.h>

extern std::vector<hddll::Patch> gUplunkyPatches;
extern std::vector<hddll::RelativePatch> gUplunkyRelativePatches;

void resetUplunkyState();
void prePlaceRoomsUplunky();
void postPlaceRoomsUplunky();
