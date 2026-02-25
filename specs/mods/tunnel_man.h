#pragma once

#include "../state.h"
#include <hddll/memory.h>

extern std::vector<hddll::Patch> gTunnelManPatches;

void resetTunnelManState();
hddll::Entity *postSpawnEntityTunnelMan(hddll::Entity *ent);
void onRunningFrameTunnelMan();
