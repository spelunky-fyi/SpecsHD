#pragma once

#include "../memory.h"
#include "../state.h"

extern std::vector<Patch> gTunnelManPatches;

void resetTunnelManState();
Entity *postSpawnEntityTunnelMan(Entity *ent);
void onRunningFrameTunnelMan();
