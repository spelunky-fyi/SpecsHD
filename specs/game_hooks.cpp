
#include "game_hooks.h"

#include "memory.h"
#include "utils.h"
#include "mods/biglunky.h"
#include "mods/full_spelunky.h"
#include "mods/seeded_mode.h"
#include "mods/tunnel_man.h"
#include "mods/uplunky.h"

// JmpBack address variables
DWORD hookLoadCoffinTextureJmpBackAddr = NULL;
DWORD hookPrePlaceRoomsJmpBackAddr = NULL;
DWORD hookPreGenerateRoomJmpBackAddr = NULL;
DWORD hookPostPlaceRoomsJmpBackAddr = NULL;
DWORD hookPreSpawnTilesJmpBackAddr = NULL;
DWORD hookPreResetForRunJmpBackAddr = NULL;
DWORD hookEligibleBMsJmpBackAddr = NULL;
DWORD hookUnlockCoffinsJmpBackAddr = NULL;
DWORD hookWhipJmpBackAddr = NULL;
DWORD hookWhipSkipWhippingAddr = NULL;
DWORD hookSeedLevelJmpBackAddr = NULL;
DWORD hookPostSpawnEntityJmpBackAddr = NULL;

int ELIGIBLE_FLOORS_FOR_BM[4692] = {0};
int ELIGIBLE_FLOORS_FOR_BM_COUNT = 0;

uint32_t lastSeed = 0;

void loadHiredHandTextures() {
  TextureDefinition *texture_def;
  for (int hh_idx = 0; hh_idx < gGlobalState->player1_data.hh_count; hh_idx++) {
    texture_def =
        getTextureById(gGlobalState->player1_data.hh_texture_id[hh_idx]);
    if (texture_def && !texture_def->loaded) {
      LoadTexture(gGlobalState->_34struct, texture_def->name);
    }
  }
  if (!gFullSpelunkyState.randoms.empty()) {
    texture_def =
        getTextureById(charIdToTextureId(gFullSpelunkyState.randoms[0]));
    if (texture_def && !texture_def->loaded) {
      LoadTexture(gGlobalState->_34struct, texture_def->name);
    }
  }
}

void __declspec(naked) hookLoadCoffinTexture() {
  __asm {
    ; Stolen Bytes
    mov edx, dword ptr [gGlobalState]

    ; Save all registers
    pushad
  }

  loadHiredHandTextures();

  __asm {
    ; Restore all registers
    popad

    ; Jump back to previous location
    jmp [hookLoadCoffinTextureJmpBackAddr]
  }
}

void __declspec(naked) hookPrePlaceRooms() {
  __asm {
    ; Stolen Bytes
    mov eax,dword ptr [eax + 0x4405d4]

    ; Save all registers
    pushad
  }

  if (gModsState.TheFullSpelunky) {
    prePlaceRoomsFullSpelunky();
  }

  if (gModsState.Biglunky) {
    prePlaceRoomsBiglunky();
  }

  if (gModsState.Uplunky) {
    prePlaceRoomsUplunky();
  }

  __asm {
    ; Restore all registers
    popad

    ; Jump back to previous location
    jmp [hookPrePlaceRoomsJmpBackAddr]
  }
}

void __declspec(naked) hookPreGenerateRoom() {
  __asm {
    ; Stolen Bytes
    mov  eax,dword ptr [ebp + ebx*0x4 + 0xa5ec]

    ; Save all registers
    pushad
  }

  if (gModsState.Biglunky) {
    preGenerateRoomBiglunky();
  }

  __asm {
    ; Restore all registers
    popad

    ; Jump back to previous location
    jmp [hookPreGenerateRoomJmpBackAddr]
  }
}

void __declspec(naked) hookPostPlaceRooms() {
  __asm {
    ; Stolen Bytes
    add esp,0x4968

    ; Save all registers
    pushad
  }

  if (gModsState.TheFullSpelunky) {
    postPlaceRoomsFullSpelunky();
  }
  if (gModsState.Biglunky) {
    postPlaceRoomsBiglunky();
  }
  if (gModsState.Uplunky) {
    postPlaceRoomsUplunky();
  }

  __asm {
    ; Restore all registers
    popad

    ; Jump back to previous location
    jmp [hookPostPlaceRoomsJmpBackAddr]
  }
}

void __declspec(naked) hookPreSpawnTiles() {
  __asm {
    ; Stolen Bytes
    mov eax,dword ptr [ebp + 0x1715c]

    ; Save all registers
    pushad
  }

  if (gModsState.TheFullSpelunky) {
    preSpawnTilesFullSpelunky();
  }
  if (gModsState.Biglunky) {
    preSpawnTilesBiglunky();
  }

  __asm {
    ; Restore all registers
    popad

    ; Jump back to previous location
    jmp [hookPreSpawnTilesJmpBackAddr]
  }
}

void __declspec(naked) hookPreResetForRun() {
  __asm {
    ; Stolen Bytes
     MOV        dword ptr [ESI + 0xa0],EAX

    ; Save all registers
    pushad
  }

  if (gModsState.TheFullSpelunky) {
    resetFullSpelunkyState();
  }
  if (gModsState.TunnelMan) {
    resetTunnelManState();
  }
  if (gModsState.Uplunky) {
    resetUplunkyState();
  }
  if (gModsState.SeededMode && gSeededModeState.randomSeedOnRestart) {
    chooseRandomSeed();
  }

  __asm {
    ; Restore all registers
    popad

    ; Jump back to previous location
    jmp [hookPreResetForRunJmpBackAddr]
  }
}

void __declspec(naked) hookEligibleBMs() {
  int *eligible;

  __asm {
    ; Stolen Bytes
    mov eax, dword ptr [ebp + ecx*0x4 + 0x134c]

    ; Save all registers
    pushad

    ; Get the total amount of eligable floors
    mov ELIGIBLE_FLOORS_FOR_BM_COUNT, ebx
    ; Get the address of the array on the stack.
    ; Actually at esp+0x30 but add 0x20 for pushad
    mov ecx, esp
    add ecx, 0x50
    mov eligible, ecx
  }

  for (int i = 0; i < ELIGIBLE_FLOORS_FOR_BM_COUNT; i++) {
    ELIGIBLE_FLOORS_FOR_BM[i] = eligible[i];
  }

  __asm {
    ; Restore all registers
    popad

    ; Jump back to previous location
    jmp [hookEligibleBMsJmpBackAddr]
  }
}

void __declspec(naked) hookUnlockCoffins() {
  __asm {
    ; Stolen Bytes
    mov eax,dword ptr [EBX + 0x1715c]

    ; Save all registers
    pushad
  }

  unlockCoffinsFullSpelunky();

  __asm {
    ; Restore all registers
    popad

    ; Jump back to previous location
    jmp [hookUnlockCoffinsJmpBackAddr]
  }
}

void __declspec(naked) hookWhip() {
  __asm {
    ; Save all registers
    pushad
  }

  // Player off ground
  if (gGlobalState->player1 && gGlobalState->player1_data.has_crysknife == 0 &&
      gGlobalState->player1->field68_0x200 == 0) {
    __asm {
      ; Restore all registers
      popad

      ; Jump back to previous location
      jmp [hookWhipSkipWhippingAddr]
    }
  }
  else {
    __asm {
      ; Restore all registers
      popad

      CMP dword ptr [EDI + 0x134],0x1d

      ; Jump back to previous location
      jmp [hookWhipJmpBackAddr]
    }
  }
}

uint32_t getSeedForLevel(int level) {
  int seed = gSeededModeState.seed;

  auto searchLevel = level;
  if (level == 11 && gGlobalState->is_mothership) {
    searchLevel = 21;
  } else if (level == 12 && gGlobalState->mothership_spawned) {
    searchLevel = 22;
  }

  for (auto [level_, seed_] : gSeededModeState.levelSeeds) {
    if (level_ == searchLevel) {
      seed = seed_;
      break;
    }
  }

  return seed;
}

void __declspec(naked) hookSeedLevel() {
  int seed;

  __asm {
    ; Save all registers
    pushad
  }

  lastSeed = getSeedForLevel(gGlobalState->level);
  seed = lastSeed * gGlobalState->level;

  __asm {
    ; Restore all registers
    popad

    mov eax, seed
    jmp[hookSeedLevelJmpBackAddr]
  }
}

void __declspec(naked) hookPostSpawnEntity() {

  Entity *spawned_entity;

  __asm {

    ; Stolen Bytes
    MOV        ECX,dword ptr [EBP + 0x30]
    MOV        EDX,ESI

    ; Save all registers
    pushad

    mov spawned_entity, esi
  }

  if (gModsState.TunnelMan) {
    spawned_entity = postSpawnEntityTunnelMan(spawned_entity);
  }

  __asm {
    ; Restore all registers
    popad

    mov esi, spawned_entity
    mov spawned_entity, 0

    ; Jump back to previous location
    jmp [hookPostSpawnEntityJmpBackAddr]
  }
}

void initHooks() {
  int hookLen;
  DWORD hookAddr;

  // Hook Reset For Run
  hookLen = 6;
  hookAddr = gBaseAddress + 0x64ed4;
  hookPreResetForRunJmpBackAddr = hookAddr + hookLen;
  hook((void *)hookAddr, hookPreResetForRun, hookLen);

  // Hook Pre Place Rooms
  hookLen = 6;
  hookAddr = gBaseAddress + 0xbded9;
  hookPrePlaceRoomsJmpBackAddr = hookAddr + hookLen;
  hook((void *)hookAddr, hookPrePlaceRooms, hookLen);

  // Hook Post Place Rooms
  hookLen = 6;
  hookAddr = gBaseAddress + 0xbe845;
  hookPostPlaceRoomsJmpBackAddr = hookAddr + hookLen;
  hook((void *)hookAddr, hookPostPlaceRooms, hookLen);

  hookLen = 6;
  hookAddr = gBaseAddress + 0xeef60;
  hookLoadCoffinTextureJmpBackAddr = hookAddr + hookLen;
  hook((void *)hookAddr, hookLoadCoffinTexture, hookLen);

  hookLen = 6;
  hookAddr = gBaseAddress + 0xdd769;
  hookPreSpawnTilesJmpBackAddr = hookAddr + hookLen;
  hook((void *)hookAddr, hookPreSpawnTiles, hookLen);

  hookLen = 7;
  hookAddr = gBaseAddress + 0xd66a6;
  hookPreGenerateRoomJmpBackAddr = hookAddr + hookLen;
  hook((void *)hookAddr, hookPreGenerateRoom, hookLen);

  hookLen = 5;
  hookAddr = gBaseAddress + 0x7c387;
  hookPostSpawnEntityJmpBackAddr = hookAddr + hookLen;
  hook((void *)hookAddr, hookPostSpawnEntity, hookLen);
}
