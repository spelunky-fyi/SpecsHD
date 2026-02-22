#pragma once

#include "state.h"

// JmpBack address variables
extern DWORD hookLoadCoffinTextureJmpBackAddr;
extern DWORD hookPrePlaceRoomsJmpBackAddr;
extern DWORD hookPreGenerateRoomJmpBackAddr;
extern DWORD hookPostPlaceRoomsJmpBackAddr;
extern DWORD hookPreSpawnTilesJmpBackAddr;
extern DWORD hookPreResetForRunJmpBackAddr;
extern DWORD hookEligibleBMsJmpBackAddr;
extern DWORD hookUnlockCoffinsJmpBackAddr;
extern DWORD hookWhipJmpBackAddr;
extern DWORD hookWhipSkipWhippingAddr;
extern DWORD hookSeedLevelJmpBackAddr;
extern DWORD hookPostSpawnEntityJmpBackAddr;

// Eligible BM floors (used by drawing.cpp)
extern int ELIGIBLE_FLOORS_FOR_BM[4692];
extern int ELIGIBLE_FLOORS_FOR_BM_COUNT;

// Last seed (used by drawing.cpp and seeded mode)
extern uint32_t lastSeed;

// Naked hook functions (naked attribute only on definitions in game_hooks.cpp)
void hookLoadCoffinTexture();
void hookPrePlaceRooms();
void hookPreGenerateRoom();
void hookPostPlaceRooms();
void hookPreSpawnTiles();
void hookPreResetForRun();
void hookEligibleBMs();
void hookUnlockCoffins();
void hookWhip();
void hookSeedLevel();
void hookPostSpawnEntity();

// Init hooks
void initHooks();
