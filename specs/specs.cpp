
#include <Windows.h>
#include <algorithm>
#include <format>
#include <functional>
#include <map>
#include <unordered_set>

#include "3rdparty/imgui/imgui.h"
#include "3rdparty/imgui/misc/cpp/imgui_stdlib.h"

#include "config.h"
#include "entities.h"
#include "hd.h"
#include "inputs.h"
#include "sounds.h"
#include "ui.h"

// Global States
DWORD gBaseAddress = NULL;
ImDrawList *gOverlayDrawList = NULL;

CameraState *gCameraState = NULL;
GlobalState *gGlobalState = NULL;

Specs::Config *gConfig = NULL;

bool gPaused = false;
int gPauseAt = 0;
int gFrame = 0;

int gWindowedMode = 0;
int gDisplayWidth = 0;
int gDisplayHeight = 0;

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

EnabledEntities gAllEntities = {true, true, true, true, true,
                                true, true, true, -1,   {}};

// Hash function
struct DrawEntityOffsetsValueHashFunction {
  size_t operator()(const std::pair<DWORD, DataType_> &x) const {
    return x.first ^ x.second;
  }
};

struct ModsState {
  bool TheFullSpelunky = false;
  bool Biglunky = false;
  bool DarkMode = false;
  bool Uplunky = false;
  bool TunnelMan = false;
  bool SeededMode = false;
};
ModsState gModsState = {};

struct FullSpelunkyState {
  std::vector<CharacterIndex> allCharacters = {
      CHARACTER_GUY,
      CHARACTER_RED,
      CHARACTER_GREEN,
      CHARACTER_BLUE,
      CHARACTER_MEATBOY,
      CHARACTER_YELLOW,
      CHARACTER_PURPLE,
      CHARACTER_VAN_HELSING,
      CHARACTER_CYAN,
      CHARACTER_LIME,
      CHARACTER_INUK,
      CHARACTER_ROUND_GIRL,
      CHARACTER_NINJA,
      CHARACTER_VIKING,
      CHARACTER_ROUND_BOY,
      CHARACTER_CARL,
      CHARACTER_ROBOT,
      CHARACTER_MONK,

      CHARACTER_JUNGLE_WARRIOR,
      CHARACTER_YANG,
  };

  std::vector<CharacterIndex> randoms = {
      CHARACTER_GUY,
      CHARACTER_RED,
      CHARACTER_GREEN,
      CHARACTER_BLUE,
      CHARACTER_YELLOW,
      CHARACTER_PURPLE,
      CHARACTER_CYAN,
      CHARACTER_LIME,
      CHARACTER_CARL,
      CHARACTER_ROUND_GIRL,
      CHARACTER_ROUND_BOY,
      CHARACTER_INUK,
      CHARACTER_JUNGLE_WARRIOR,
      CHARACTER_YANG,
  };

  bool showCharacterOverlay = false;
};
FullSpelunkyState gFullSpelunkyState = {};

struct SeededModeState {
  uint32_t seed = 1;
  std::vector<std::tuple<uint32_t, uint32_t>> levelSeeds = {};
  bool useDailySeeding = false;
  bool randomSeedOnRestart = false;
};

SeededModeState gSeededModeState = {};

const uint8_t MAX_SEED_OVERRIDES = 20;

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

std::string formatLevel(uint8_t levelNumber) {

  auto world = 0;
  auto level = 0;

  if (levelNumber <= 4) {
    world = 1;
    level = levelNumber;
  } else if (levelNumber <= 8) {
    world = 2;
    level = levelNumber - 4;
  } else if (levelNumber <= 12) {
    world = 3;
    level = levelNumber - 8;
  } else if (levelNumber <= 16) {
    world = 4;
    level = levelNumber - 12;
  } else if (levelNumber <= 20) {
    world = 5;
    level = levelNumber - 16;
  }

  return std::format("{}-{}", world, level);
}

struct DebugState {
  bool EnableTileBorders = false;
  bool EnableBinBorders = false;
  bool EnableRoomBorders = false;
  bool EnablePacifistOverlay = false;
  bool EnableSeededCrateOverlay = false;
  bool EnableSeededPotOverlay = false;
  bool EnableSeededKaliRewards = false;
  bool DrawEnemyDetection = false;
  bool BlackMarketTrainer = false;
  bool IncludeHitboxOrigins = false;
  bool DrawHHFollowerLink = false;
  bool DrawHHFollowingLink = false;

  EnabledEntities Ids;
  EnabledEntities Hitboxes;
  EnabledEntities Selection;

  // (EntityKind, entity type): [(offset, datatype),...]
  std::map<std::pair<EntityKind, uint32_t>,
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
DebugState gDebugState = {};

struct SpawnEntityConfig {
  int entityType = -1;
  bool activeEntity;
};

struct SpawnState {
  std::vector<SpawnEntityConfig> SpawnEntityInputs;
  std::string EntityListFilter;
  ImVec2 ClickedAt = {0, 0};
  bool ClickToSpawn = false;
  bool Clicking = false;
};
SpawnState gSpawnState = {};

struct SelectedEntityState {

  bool Clicking = false;
  Entity *Entity = NULL;
};
SelectedEntityState gSelectedEntityState = {};

void patchReadOnlyCode(HANDLE process, DWORD addr, void *value, size_t size) {
  DWORD oldrights;
  VirtualProtectEx(process, (LPVOID)addr, size, PAGE_EXECUTE_READWRITE,
                   &oldrights);

  WriteProcessMemory(process, (LPVOID)addr, value, size, NULL);
  VirtualProtectEx(process, (LPVOID)addr, size, oldrights, &oldrights);
}

enum FORCE_PATCH_TYPE : int32_t {
  FORCE_PATCH_TYPE_NORMAL = 0,
  FORCE_PATCH_TYPE_ALWAYS = 1,
  FORCE_PATCH_TYPE_NEVER = 2,
};

struct Patch {
  DWORD offset;
  std::vector<BYTE> patch;
  std::vector<BYTE> original;
};

struct RelativePatch {
  DWORD offset;
  DWORD patch;
  DWORD original;
};

struct ForcePatch {
  Patch always;
  Patch never;
};

void applyForcePatch(ForcePatch &patch, FORCE_PATCH_TYPE type) {
  auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                 PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                 PROCESS_CREATE_THREAD,
                             0, GetCurrentProcessId());
  if (type == FORCE_PATCH_TYPE_NORMAL) {
    patchReadOnlyCode(process, gBaseAddress + patch.always.offset,
                      &patch.always.original[0], patch.always.original.size());
    patchReadOnlyCode(process, gBaseAddress + patch.never.offset,
                      &patch.never.original[0], patch.never.original.size());
  } else if (type == FORCE_PATCH_TYPE_ALWAYS) {
    patchReadOnlyCode(process, gBaseAddress + patch.always.offset,
                      &patch.always.patch[0], patch.always.patch.size());
    patchReadOnlyCode(process, gBaseAddress + patch.never.offset,
                      &patch.never.original[0], patch.never.original.size());
  } else if (type == FORCE_PATCH_TYPE_NEVER) {
    patchReadOnlyCode(process, gBaseAddress + patch.always.offset,
                      &patch.always.original[0], patch.always.original.size());
    patchReadOnlyCode(process, gBaseAddress + patch.never.offset,
                      &patch.never.patch[0], patch.never.patch.size());
  }

  CloseHandle(process);
}

void applyRelativePatches(std::vector<RelativePatch> &patches,
                          bool rollback = false) {

  auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                 PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                 PROCESS_CREATE_THREAD,
                             0, GetCurrentProcessId());

  for (RelativePatch patch : patches) {
    if (rollback) {
      DWORD value = gBaseAddress + patch.original;
      patchReadOnlyCode(process, gBaseAddress + patch.offset, &value, 4);
    } else {
      DWORD value = gBaseAddress + patch.patch;
      patchReadOnlyCode(process, gBaseAddress + patch.offset, &value, 4);
    }
  }

  CloseHandle(process);
}

void applyPatches(std::vector<Patch> &patches, bool rollback = false) {

  auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                 PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                 PROCESS_CREATE_THREAD,
                             0, GetCurrentProcessId());

  for (Patch patch : patches) {
    if (rollback) {
      if (!patch.original.empty()) {
        patchReadOnlyCode(process, gBaseAddress + patch.offset,
                          &patch.original[0], patch.original.size());
      }
    } else {
      if (!patch.patch.empty()) {
        patchReadOnlyCode(process, gBaseAddress + patch.offset, &patch.patch[0],
                          patch.patch.size());
      }
    }
  }

  CloseHandle(process);
}

bool hook(void *toHook, void *ourFunc, int len) {
  if (len < 5) {
    return false;
  }

  DWORD curProtection;
  VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

  // Nop out original instructions
  memset(toHook, 0x90, len);

  DWORD relativeAddress = ((DWORD)ourFunc - (DWORD)toHook) - 5;

  *(BYTE *)toHook = 0xE9;
  *(DWORD *)((DWORD)toHook + 1) = relativeAddress;

  DWORD tmp;
  VirtualProtect(toHook, len, curProtection, &tmp);

  return true;
}

void prePlaceRoomsUplunky();
void postPlaceRoomsUplunky();
void prePlaceRoomsFullSpelunky();
void postPlaceRoomsFullSpelunky();
void resetFullSpelunkyState();
void unlockCoffinsFullSpelunky();
void preSpawnTilesFullSpelunky();
void preSpawnTilesBiglunky();
void preGenerateRoomBiglunky();
void prePlaceRoomsBiglunky();
void postPlaceRoomsBiglunky();
void resetTunnelManState();
void resetUplunkyState();
void chooseRandomSeed();
Entity *postSpawnEntityTunnelMan(Entity *);

TextureDefinition *getTextureById(int32_t texture_id) {
  TextureDefinition *texture_def;

  for (int texture_idx = 0; texture_idx < 256; texture_idx++) {
    texture_def = &gGlobalState->_34struct->texture_defs[texture_idx];
    if (texture_def->texture_id == texture_id) {
      return texture_def;
    }
  }
  return NULL;
}

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

DWORD hookLoadCoffinTextureJmpBackAddr = NULL;
void __declspec(naked) hookLoadCoffinTexture() {
  // 0xeef60 8b 15 6c
  //         44 35 00

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

DWORD hookPrePlaceRoomsJmpBackAddr = NULL;
void __declspec(naked) hookPrePlaceRooms() {

  // 0xbded9 (6 Bytes)
  // 8b 80 d4 05 44 00
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

DWORD hookPreGenerateRoomJmpBackAddr = NULL;
void __declspec(naked) hookPreGenerateRoom() {

  // 0xd66a6 8b 84 9d ec a5 00 00
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

DWORD hookPostPlaceRoomsJmpBackAddr = NULL;
void __declspec(naked) hookPostPlaceRooms() {
  // 002be845
  // 81 c4 68 49 00 00

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

DWORD hookPreSpawnTilesJmpBackAddr = NULL;
void __declspec(naked) hookPreSpawnTiles() {

  // 0xdd769 (6 Bytes)
  // 8b 85 5c 71 01 00
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

DWORD hookPreResetForRunJmpBackAddr = NULL;
void __declspec(naked) hookPreResetForRun() {
  // 0x64ed4 89 86 a0
  //        00 00 00

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

DWORD hookEligibleBMsJmpBackAddr = NULL;
int ELIGIBLE_FLOORS_FOR_BM[4692] = {0};
int ELIGIBLE_FLOORS_FOR_BM_COUNT = 0;

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

DWORD hookUnlockCoffinsJmpBackAddr = NULL;
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

DWORD hookWhipJmpBackAddr = NULL;
DWORD hookWhipSkipWhippingAddr = NULL;
void __declspec(naked) hookWhip() {

  //  0x569a5
  //  83 bf 34 01 00 00 1d

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
  for (auto [level_, seed_] : gSeededModeState.levelSeeds) {
    if (level_ == level) {
      seed = seed_;
      break;
    }
  }

  return seed;
}

DWORD hookSeedLevelJmpBackAddr = NULL;
void __declspec(naked) hookSeedLevel() {

  // 006ae01 52              PUSH       EDX
  // 006ae02 ff d6           CALL ESI=>KERNEL32.DLL::QueryPerformanceCounter
  // 006ae04 8b 44 24 18     MOV        EAX,dword ptr [ESP + local_10]

  int seed;

  __asm {
    ; Save all registers
    pushad
  }

  seed = getSeedForLevel(gGlobalState->level);

  __asm {
    ; Restore all registers
    popad

    mov eax, seed
    jmp[hookSeedLevelJmpBackAddr]
  }
}

DWORD hookPostSpawnEntityJmpBackAddr = NULL;
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

void specsOnDestroy() {
  auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                 PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                 PROCESS_CREATE_THREAD,
                             0, GetCurrentProcessId());

  BYTE patch[] = {0x89, 0x86, 0xa0, 0x00, 0x00, 0x00};
  patchReadOnlyCode(process, gBaseAddress + 0x64ed4, patch, 6);

  BYTE patch2[] = {0x8b, 0x80, 0xd4, 0x05, 0x44, 0x00};
  patchReadOnlyCode(process, gBaseAddress + 0xbded9, patch2, 6);

  BYTE patch3[] = {0x81, 0xc4, 0x68, 0x49, 0x00, 0x00};
  patchReadOnlyCode(process, gBaseAddress + 0xbe845, patch3, 6);

  BYTE patch4[] = {0x8b, 0x15, 0x6c, 0x44, 0x35, 0x00};
  patchReadOnlyCode(process, gBaseAddress + 0xeef60, patch4, 6);

  BYTE patch5[] = {0x8b, 0x85, 0x5c, 0x71, 0x01, 0x00};
  patchReadOnlyCode(process, gBaseAddress + 0xdd769, patch5, 6);

  BYTE patch6[] = {0x8b, 0x84, 0x9d, 0xec, 0x5, 0x00, 0x00};
  patchReadOnlyCode(process, gBaseAddress + 0xd66a6, patch6, 7);

  BYTE patch7[] = {0x8b, 0x4d, 0x30, 0x8b, 0xd6};
  patchReadOnlyCode(process, gBaseAddress + 0x7c387, patch7, 5);

  CloseHandle(process);
}

void specsOnInit() {
  srand((unsigned int)time(NULL));

  gConfig = Specs::Config::load();

  gBaseAddress = (size_t)GetModuleHandleA(NULL);
  setupOffsets(gBaseAddress);
  initHooks();

  gDebugState.Selection.activeEntities = true;
  gDebugState.Selection.floorEntities = true;

  auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                 PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                 PROCESS_CREATE_THREAD,
                             0, GetCurrentProcessId());

  BYTE patch[] = {0x4a};
  patchReadOnlyCode(process, gBaseAddress + 0x135B2A, patch, 1);

  BYTE patch2[] = {0xF0};
  patchReadOnlyCode(process, gBaseAddress + 0x1366C6, patch2, 1);
  CloseHandle(process);
}

struct PlayerState {
  bool LockHealth = false;
  int LockedHealthAmount = 0;

  bool LockBombs = false;
  int LockedBombsAmount = 0;

  bool LockRopes = false;
  int LockedRopesAmount = 0;

  bool LockHeldItemId = false;
  int LockedHeldItemId = 0;

  bool LockHeldItemMetadata = false;
  int LockedHeldItemMetadata = 0;

  bool LockHiredHandCount = false;
  int LockedHiredHandCount = 0;

  bool LockCompass = false;
  bool LockedCompassValue = false;

  bool LockParachute = false;
  bool LockedParachuteValue = false;

  bool LockJetpack = false;
  bool LockedJetpackValue = false;

  bool LockClimbingGloves = false;
  bool LockedClimbingGlovesValue = false;

  bool LockPitchersMitt = false;
  bool LockedPitchersMittValue = false;

  bool LockSpringShoes = false;
  bool LockedSpringShoesValue = false;

  bool LockSpikeShoes = false;
  bool LockedSpikeShoesValue = false;

  bool LockSpectacles = false;
  bool LockedSpectaclesValue = false;

  bool LockKapala = false;
  bool LockedKapalaValue = false;

  bool LockHedjet = false;
  bool LockedHedjetValue = false;

  bool LockUdjatEye = false;
  bool LockedUdjatEyeValue = false;

  bool LockBookOfTheDead = false;
  bool LockedBookOfTheDeadValue = false;

  bool LockAnkh = false;
  bool LockedAnkhValue = false;

  bool LockPaste = false;
  bool LockedPasteValue = false;

  bool LockCape = false;
  bool LockedCapeValue = false;

  bool LockVladsCape = false;
  bool LockedVladsCapeValue = false;

  bool LockCrysknife = false;
  bool LockedCrysknifeValue = false;

  bool LockVladsAmulet = false;
  bool LockedVladsAmuletValue = false;

  bool LockWhiteFlag = false;
  bool LockedWhiteFlagValue = false;
};
PlayerState gPlayersState[4] = {{}, {}, {}, {}};

struct LockableU8 {
  bool IsLocked = false;
  uint8_t LockedValue = 0;
};

struct LevelsState {
  LockableU8 FloodedMines = {};
  LockableU8 SkinIsCrawling = {};

  LockableU8 DeadAreRestless = {};
  LockableU8 RushingWater = {};
  LockableU8 HauntedCastle = {};
  LockableU8 TikiVillage = {};
  LockableU8 BlackMarket = {};

  LockableU8 WetFur = {};
  LockableU8 MotherShip = {};
  LockableU8 Worm = {};

  LockableU8 CityOfGold = {};

  LockableU8 AltarSpawned = {};
  LockableU8 IdolSpawned = {};
  LockableU8 DamselSpawned = {};
  LockableU8 GhostSpawned = {};
  LockableU8 VaultSpawnedInArea = {};
};

LevelsState gLevelsState = {};

// If we want to normalize the screen position to 0,0 at the top-left
// if (gWindowedMode == 2) {
//   RECT windowRect;
//   GetWindowRect(ui::window, &windowRect);
//   screen.x = screen.x + windowRect.left;
//   screen.y = screen.y + windowRect.top;
// }

ImVec2 screenToGame(ImVec2 screen) {

  auto x =
      (screen.x - ((float)gDisplayWidth / 2)) * (20 / (float)gDisplayWidth) +
      gCameraState->camera_x;
  auto y =
      (screen.y - ((float)gDisplayHeight / 2)) * -(20 / (float)gDisplayWidth) +
      gCameraState->camera_y;

  return {x, y};
}

ImVec2 gameToScreen(ImVec2 game) {
  auto x = (game.x - gCameraState->camera_x) / (20 / (float)gDisplayWidth) +
           ((float)gDisplayWidth / 2);
  auto y = (game.y - gCameraState->camera_y) / -(20 / (float)gDisplayWidth) +
           ((float)gDisplayHeight / 2);
  return {x, y};
}

void drawPointAtCoord(ImVec2 coord,
                      ImU32 color = ImGui::GetColorU32({255.f, 0.0f, 0.0f,
                                                        0.9f})) {
  auto topLeft = gameToScreen(coord);
  topLeft.x -= 1;
  topLeft.y += 1;

  ImVec2 topRight = {topLeft.x + 2, topLeft.y};
  ImVec2 bottomRight = {topLeft.x + 2, topLeft.y - 2};
  ImVec2 bottomLeft = {topLeft.x, topLeft.y - 2};

  gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft, color,
                            1.f);
}

void drawEntityCircle(Entity *ent, float radius,
                      ImU32 color = ImGui::GetColorU32({255.f, 0.0f, 238.0f,
                                                        0.7f}),
                      float x_offset = 0.0f, float y_offset = 0.0f,
                      bool filled = false) {
  auto screen = gameToScreen({ent->x + x_offset, ent->y + y_offset});
  auto screenRadius =
      gameToScreen({ent->x + x_offset + radius, 0}).x - screen.x;

  if (filled) {
    gOverlayDrawList->AddCircleFilled(screen, screenRadius, color, 0);
  } else {
    gOverlayDrawList->AddCircle(screen, screenRadius, color, 0, 1.f);
  }
}

void drawEntityHitbox(Entity *ent,
                      ImU32 color = ImGui::GetColorU32({255.f, 0.0f, 238.0f,
                                                        0.7f}),
                      bool filled = false) {

  ImVec2 topLeft =
      gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 topRight =
      gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 bottomRight =
      gameToScreen({ent->x + ent->hitbox_x, ent->y - ent->hitbox_down});
  ImVec2 bottomLeft =
      gameToScreen({ent->x - ent->hitbox_x, ent->y - ent->hitbox_down});

  if (filled) {
    gOverlayDrawList->AddQuadFilled(topLeft, topRight, bottomRight, bottomLeft,
                                    color);
  } else {
    gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft, color,
                              1.f);
  }

  if (gDebugState.IncludeHitboxOrigins) {
    auto ent_origin = gameToScreen({ent->x, ent->y});
    gOverlayDrawList->AddLine(topLeft, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(topRight, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(bottomLeft, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(bottomRight, ent_origin, color, 1.f);
    drawPointAtCoord({ent->x, ent->y}, color);
  }

  if (ent->flag_23 == 1 || ent->entity_kind == EntityKind::KIND_EXPLOSION) {
    drawEntityCircle(ent, ent->hitbox_down, color, 0.0f, 0.0f, filled);
  }
}

bool drawCharBool(const char *label, char &flag) {
  bool bflag = flag ? 1 : 0;
  bool res = ImGui::Checkbox(label, &bflag);
  bflag ? flag = 1 : flag = 0;
  return res;
}

bool drawCharBool(const char *label, uint8_t &flag) {
  bool bflag = flag ? 1 : 0;
  bool res = ImGui::Checkbox(label, &bflag);
  bflag ? flag = 1 : flag = 0;
  return res;
}

void drawEntityDetectionRay(Entity *ent, float len, ImU32 color) {

  auto factor = 1.001f;
  auto top = 0.25f;
  if (ent->flag_horizontal_flip) {
    gOverlayDrawList->AddQuad(
        gameToScreen({ent->x + ent->hitbox_x * factor - len, ent->y - top}),
        gameToScreen({ent->x + ent->hitbox_x * factor, ent->y - top}),

        gameToScreen({ent->x + ent->hitbox_x * factor,
                      ent->y - ent->hitbox_down * factor}),

        gameToScreen({ent->x + ent->hitbox_x * factor - len,
                      ent->y - ent->hitbox_down * factor}),
        color);
  } else {
    gOverlayDrawList->AddQuad(
        gameToScreen({ent->x - ent->hitbox_x * factor + len, ent->y - top}),
        gameToScreen({ent->x - ent->hitbox_x * factor, ent->y - top}),
        gameToScreen({ent->x - ent->hitbox_x * factor,
                      ent->y - ent->hitbox_down * factor}),
        gameToScreen({ent->x - ent->hitbox_x * factor + len,
                      ent->y - ent->hitbox_down * factor}),
        color);
  }
}
void drawEntityHitboxDefault(Entity *ent) { drawEntityHitbox(ent); }

void drawPacifistOverlay() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->owner == Ownership::Unowned) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", (int)ent->owner);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              out.c_str());
  }
}

uint32_t getKaliRewardForSeed(uint32_t seed) {
  mersenne_init_and_twist(seed);
  auto val = (mersenne_random() & 7) + 1;

  switch (val) {
  case 1:
    return 521; // Cape
  case 2:
    return 504; // Climbers
  case 3:
    return 503; // Specs
  case 4:
    return 505; // Pitchers
  case 5:
    return 506; // Spring Shoes
  case 6:
    return 507; // Spike Shoes
  case 7:
    return 508; // Paste
  case 8:
    return 509; // Compass
  }

  // should never happen
  return 0;
}

uint32_t getPotItemForSeed(uint32_t seed, bool sfx = true, bool rubble = true) {
  mersenne_init_and_twist(seed);

  // SFX
  if (sfx)
    mersenne_random();

  // Rubble
  if (rubble) {
    mersenne_random();
    mersenne_random();
    mersenne_random();
    mersenne_random();
    mersenne_random();

    mersenne_random();
    mersenne_random();
    mersenne_random();
    mersenne_random();
    mersenne_random();

    mersenne_random();
    mersenne_random();
    mersenne_random();
    mersenne_random();
    mersenne_random();
  }

  if (mersenne_random() % 5000 == 0)
    return 1026; // Alien

  if (mersenne_random() % 128 == 0)
    return 1029; // Scorpion

  if (mersenne_random() % 64 == 0)
    return 1036; // Cobra

  if (mersenne_random() % 12 == 0)
    return 1001; // Snake

  if (mersenne_random() % 8 == 0)
    return 1002; // Spider

  if (mersenne_random() % 16 == 0)
    return 106; // Large Ruby

  if (mersenne_random() % 12 == 0)
    return 105; // Large Saphire

  if (mersenne_random() % 8 == 0)
    return 104; // Large Emerald

  if (mersenne_random() % 8 == 0)
    return 118; // Large Gold Nugget

  if (mersenne_random() % 4 == 0)
    return 124; // Small Gold Nugget

  return 0;
}

uint32_t getCrateItemForSeed(uint32_t seed) {
  mersenne_init_and_twist(seed);

  if (mersenne_random() % 10000 == 0)
    return 517; // Plasma Cannon

  if (mersenne_random() % 500 == 0)
    return 522; // Jetpack

  if (mersenne_random() % 200 == 0)
    return 516; // Freeze Ray

  if (mersenne_random() % 200 == 0)
    return 521; // Cape

  if (mersenne_random() % 100 == 0)
    return 515; // Shotgun

  if (mersenne_random() % 100 == 0)
    return 510; // MATTOCK

  if (mersenne_random() % 100 == 0)
    return 519; // TELEPORTER

  if (mersenne_random() % 0x5a == 0)
    return 504; // CLIMBING_GLOVES

  if (mersenne_random() % 0x5a == 0)
    return 503; // SPECTACLE

  if (mersenne_random() % 0x50 == 0)
    return 514; // WEB_GUN

  if (mersenne_random() % 0x50 == 0)
    return 518; // CAMERA

  if (mersenne_random() % 0x50 == 0)
    return 505; // PITCHERS_MITT

  if (mersenne_random() % 0x3c == 0)
    return 508; // BOMB_PASTE

  if (mersenne_random() % 0x3c == 0)
    return 506; // SPRING_SHOES

  if (mersenne_random() % 0x3c == 0)
    return 507; // SPIKE_SHOES

  if (mersenne_random() % 0x3c == 0)
    return 511; // BOOMERANG

  if (mersenne_random() % 0x28 == 0)
    return 512; // MACHETE

  if (mersenne_random() % 0x28 == 0)
    return 502; // BOMB_BOX

  if (mersenne_random() % 0x14 == 0)
    return 509; // COMPASS

  if (mersenne_random() % 10 == 0)
    return 520; // PARACHUTE

  if ((mersenne_random() & 1) == 0)
    return 500; // Ropes

  return 501; // Bomb Bag
}

// 229001 - Min
// 229002
// ...
// 229999
// 230000 - Max

void drawSeededCrateOverlay() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type != 101) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto entity_type = getCrateItemForSeed(ent->z_depth_as_int);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              EntityTypeName(entity_type));
  }
}

void drawSeededPotOverlay() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type != 113) {
      continue;
    }

    auto screen = gameToScreen({ent->x - ent->hitbox_x, ent->y});
    auto fontSize = ImGui::GetFontSize() + 2.f;

    uint32_t entity_type = 0;
    entity_type = getPotItemForSeed(ent->z_depth_as_int);
    if (entity_type > 0) {
      gOverlayDrawList->AddText(ImGui::GetFont(), fontSize,
                                ImVec2{screen.x, screen.y - (fontSize + 2.f)},
                                IM_COL32_WHITE, EntityTypeName(entity_type));
    }

    entity_type = getPotItemForSeed(ent->z_depth_as_int, false, true);
    if (entity_type > 0) {
      gOverlayDrawList->AddText(
          ImGui::GetFont(), fontSize, ImVec2{screen.x, screen.y},
          IM_COL32_WHITE,
          std::format("Dupe: {}", EntityTypeName(entity_type)).c_str());
    }
  }
}

void drawSeededKaliRewardsOverlay() {
  for (auto idx = 0; idx < 4692; idx++) {
    auto ent = gGlobalState->level_state->entity_floors[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type != 35) {
      continue;
    }

    auto screen = gameToScreen({ent->x - (ent->hitbox_x / 2), ent->y});
    auto entity_type = getKaliRewardForSeed(ent->z_depth_as_int);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              EntityTypeName(entity_type));
  }
}

void drawBinBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 4.f) {
    gOverlayDrawList->AddLine(gameToScreen({x, -4.f}), gameToScreen({x, 120.f}),
                              IM_COL32_WHITE);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 4.f) {
    gOverlayDrawList->AddLine(gameToScreen({-4.f, y}), gameToScreen({50.f, y}),
                              IM_COL32_WHITE);
  }
}

void drawRoomBorders() {
  if (gGlobalState->screen_state == 0) {

    auto room_width = 10.f;
    auto room_height = 8.f;
    auto max_height = 99.5f;
    auto max_width = 42.5f;

    for (auto x = 2.5f; x < 50.f; x += room_width) {
      gOverlayDrawList->AddLine(gameToScreen({x, 3.5f}),
                                gameToScreen({x, max_height}), IM_COL32_WHITE,
                                2.0f);
    }
    for (auto y = 3.5f; y < 120.f; y += room_height) {
      gOverlayDrawList->AddLine(gameToScreen({2.5, y}),
                                gameToScreen({max_width, y}), IM_COL32_WHITE,
                                2.0f);
    }

    for (auto idx = 0; idx < 48; idx++) {
      auto column = idx % 4;
      auto row = idx / 4;
      auto type = gGlobalState->level_state->room_types[idx];

      auto y = max_height - (row * room_height);
      auto x = 2.5f + (column * room_width);

      auto out = std::format("Type: {}", type);
      auto screen = gameToScreen({x + 0.25f, y - 0.25f});
      gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 8,
                                ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                                out.c_str());
    }
  }
}

void drawTileBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 1.f) {
    gOverlayDrawList->AddLine(gameToScreen({x + -.5f, -4.5f}),
                              gameToScreen({x + -.5f, 120.5f}), IM_COL32_WHITE,
                              0.5f);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 1.f) {
    gOverlayDrawList->AddLine(gameToScreen({-4.5f, y + 0.5f}),
                              gameToScreen({50.5f, y + 0.5f}), IM_COL32_WHITE,
                              0.5f);
  }
}

void drawEntityId(Entity *ent) {
  auto screen = gameToScreen({ent->x, ent->y});
  auto out = std::format("{}", ent->entity_type);
  gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5,
                            ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                            out.c_str());
}

void drawEntityOffsetDebug(Entity *ent) {

  auto entry =
      gDebugState
          .DrawEntityOffsets[std::pair{ent->entity_kind, ent->entity_type}];
  if (entry.empty()) {
    return;
  }

  auto idx = 0;
  auto font = ImGui::GetFont();
  auto fontSize = ImGui::GetFontSize() + 5;
  auto screen = gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
  screen.y -= fontSize + 8;

  for (auto offset_pair : entry) {

    auto offset = offset_pair.first;
    auto dataType = offset_pair.second;

    char *addr = ((char *)ent) + offset;
    auto text = std::format("0x{:X}: ", offset);

    if (dataType == DataType_Byte) {
      text.append(std::format("{:02X}", *addr));
    } else if (dataType == DataType_Dword_Unsigned) {
      text.append(std::format("{:d}", *(uint32_t *)addr));
    } else if (dataType == DataType_Dword_Signed) {
      text.append(std::format("{:d}", *(int32_t *)addr));
    } else if (dataType == DataType_Dword_Hex) {
      text.append(std::format("0x{:08X}", *(uint32_t *)addr));
    } else if (dataType == DataType_Float) {
      text.append(std::format("{:f}", *(float *)addr));
    }

    gOverlayDrawList->AddText(
        font, fontSize, ImVec2{screen.x, screen.y - (idx * (fontSize + 2))},
        IM_COL32_WHITE, text.c_str());
    idx++;
  }
}

using EntityCallback = std::function<void(Entity *e)>;
void forEntities(std::unordered_set<uint32_t> excludedEntities,
                 EntityCallback callback, Entity **entities, size_t count,
                 bool decos = false) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (excludedEntities.contains(ent->entity_type)) {
      continue;
    }

    if (decos) {
      if (ent->deco_over) {
        callback(ent->deco_over);
      }
      if (ent->deco_top) {
        callback(ent->deco_top);
      }
      if (ent->deco_bottom) {
        callback(ent->deco_bottom);
      }
      if (ent->deco_left) {
        callback(ent->deco_left);
      }
      if (ent->deco_right) {
        callback(ent->deco_right);
      }
    }

    callback(ent);
  }
}

bool findEntityArray(Entity *searchEnt, Entity **entities, size_t count,
                     bool decos = false) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (decos) {
      if (ent->deco_over) {
        if (searchEnt == ent->deco_over) {
          return true;
        }
      }
      if (ent->deco_top) {
        if (searchEnt == ent->deco_top) {
          return true;
        }
      }
      if (ent->deco_bottom) {
        if (searchEnt == ent->deco_bottom) {
          return true;
        }
      }
      if (ent->deco_left) {
        if (searchEnt == ent->deco_left) {
          return true;
        }
      }
      if (ent->deco_right) {
        if (searchEnt == ent->deco_right) {
          return true;
        }
      }
    }

    if (searchEnt == ent)
      return true;
  }
  return false;
}

bool findEntity(Entity *searchEnt) {
  if (searchEnt == NULL) {
    return false;
  }
  return (
      findEntityArray(searchEnt, gGlobalState->entities->entities_active,
                      gGlobalState->entities->entities_active_count) ||
      findEntityArray(searchEnt, gGlobalState->entities->array_1400,
                      gGlobalState->entities->array_1400_count) ||
      findEntityArray(
          searchEnt, gGlobalState->entities->entities_foreground,
          gGlobalState->entities->array_entities_foreground_count) ||
      findEntityArray(
          searchEnt, gGlobalState->entities->entities_foreground,
          gGlobalState->entities->array_entities_foreground_count) ||
      findEntityArray(searchEnt,
                      gGlobalState->entities->entities_light_emitting,
                      gGlobalState->entities->entities_light_emitting_count) ||
      findEntityArray(searchEnt,
                      (Entity **)gGlobalState->level_state->entity_floors, 4692,
                      gDebugState.IncludeFloorDecos) ||

      findEntityArray(searchEnt,
                      (Entity **)gGlobalState->level_state->entity_floors_bg,
                      4692) ||

      findEntityArray(searchEnt,
                      (Entity **)gGlobalState->level_state->entity_backgrounds,
                      gGlobalState->level_state->entity_backgrounds_count) ||
      findEntityArray(searchEnt, gGlobalState->_4cstruct->entities, 160));
}

void forEnabledEntities(EnabledEntities &enabledEnts, EntityCallback callback) {

  // Active
  if (enabledEnts.activeEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_active,
                gGlobalState->entities->entities_active_count);
  }

  // 1400
  if (enabledEnts.unknown1400) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->array_1400,
                gGlobalState->entities->array_1400_count);
  }
  // Foreground
  if (enabledEnts.foregroundEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_foreground,
                gGlobalState->entities->array_entities_foreground_count);
  }

  // Light Emitting
  if (enabledEnts.lightEmittingEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_light_emitting,
                gGlobalState->entities->entities_light_emitting_count);
  }

  // Floors
  if (enabledEnts.floorEntities) {
    forEntities(enabledEnts.excluded, callback,
                (Entity **)gGlobalState->level_state->entity_floors, 4692,
                gDebugState.IncludeFloorDecos);
  }
  if (enabledEnts.floorBgEntities) {
    forEntities(enabledEnts.excluded, callback,
                (Entity **)gGlobalState->level_state->entity_floors_bg, 4692);
  }

  // Backgrounds
  if (enabledEnts.backgroundEntities) {
    forEntities(enabledEnts.excluded, callback,
                (Entity **)gGlobalState->level_state->entity_backgrounds,
                gGlobalState->level_state->entity_backgrounds_count);
  }

  if (enabledEnts._4cStructEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->_4cstruct->entities, 160);
  }
}

float dist(ImVec2 pos1, ImVec2 pos2) {
  return sqrt(pow(pos2.x - pos1.x, 2.0f) + pow(pos2.y - pos1.y, 2.0f));
}

bool entityCollidesWithCircle(Entity *targetEnt, float x_pos, float y_pos,
                              float radius) {
  auto target_edge_x_left = targetEnt->x - targetEnt->hitbox_x;
  auto target_edge_x_right = targetEnt->x + targetEnt->hitbox_x;
  auto target_x = target_edge_x_left;

  auto target_edge_y_top = targetEnt->y + targetEnt->hitbox_up;
  auto target_edge_y_bottom = targetEnt->y - targetEnt->hitbox_down;
  auto target_y = target_edge_y_top;

  auto source_x = x_pos;
  auto source_y = y_pos;

  if (target_x <= source_x) {
    target_x = target_edge_x_right;
    if (source_x <= target_edge_x_right) {
      target_x = source_x;
    }
  }

  if (source_y <= target_y) {
    target_y = target_edge_y_bottom;
    if (target_edge_y_bottom <= source_y) {
      target_y = source_y;
    }
  }

  auto distance =
      pow((target_x - source_x), 2.0) + pow((target_y - source_y), 2.0);

  return distance < radius;
}

bool collidesWithEntityCircle(Entity *sourceEnt, Entity *targetEnt,
                              float x_offset = 0.0f, float y_offset = 0.0f) {

  return entityCollidesWithCircle(targetEnt, sourceEnt->x + x_offset,
                                  sourceEnt->y + y_offset,
                                  sourceEnt->hitbox_down);
}

void drawOverlayWindow() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowSize(io.DisplaySize);
  ImGui::SetNextWindowPos({0, 0});
  ImGui::Begin(
      "Overlay", NULL,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
          ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
          ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
          ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs |
          ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  gOverlayDrawList = ImGui::GetWindowDrawList();

  // check if selected entity exists
  if (!findEntity(gSelectedEntityState.Entity)) {
    gSelectedEntityState.Entity = NULL;
  }

  Entity *closestEnt = NULL;
  if (ImGui::IsWindowHovered()) {

    // Teleport Player
    if (Specs::IsMouseClicked(
            gConfig->buttons[Specs::MouseFeatures_Teleport])) {
      auto player = gGlobalState->player1;
      if (player) {
        auto pos = screenToGame(io.MousePos);
        player->x = pos.x;
        player->y = pos.y;
      }
    }

    // Spawn Entity
    auto spawnMouseConfig = gConfig->buttons[Specs::MouseFeatures_SpawnEntity];
    if (gSpawnState.ClickToSpawn && Specs::IsMouseClicked(spawnMouseConfig)) {
      gSpawnState.ClickedAt = io.MousePos;
      gSpawnState.Clicking = true;
    }

    if (gSpawnState.ClickToSpawn && gSpawnState.Clicking &&
        io.MouseDown[spawnMouseConfig.Button]) {
      gOverlayDrawList->AddLine(gSpawnState.ClickedAt, io.MousePos,
                                ImGui::GetColorU32({0.0f, 1.0f, .5f, .9f}));
    }
    if (gSpawnState.ClickToSpawn && gSpawnState.Clicking &&
        io.MouseReleased[spawnMouseConfig.Button]) {
      gSpawnState.Clicking = false;

      for (auto const &spawnEntityConfig : gSpawnState.SpawnEntityInputs) {
        if (spawnEntityConfig.entityType >= 0) {
          if (io.MouseDownDurationPrev[spawnMouseConfig.Button] > 0.1f) {
            auto gamePos = screenToGame(gSpawnState.ClickedAt);
            Entity *ent;

            if (spawnEntityConfig.entityType == 0) {
              ent = gGlobalState->SpawnHiredHand(gamePos.x, gamePos.y, 90);
            } else {
              ent = gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                              spawnEntityConfig.entityType,
                                              spawnEntityConfig.activeEntity);
            }

            if ((uint32_t)ent->entity_kind > 0 &&
                (uint32_t)ent->entity_kind < 5) {

              auto activeEnt = (EntityActive *)ent;
              if (ent->entity_type != 108) {
                activeEnt->velocity_x =
                    (io.MousePos.x - gSpawnState.ClickedAt.x) * 0.01f;
              }
              activeEnt->velocity_y =
                  -((io.MousePos.y - gSpawnState.ClickedAt.y) * 0.01f);
            }
          } else {
            auto gamePos = screenToGame(io.MousePos);
            if (spawnEntityConfig.entityType == 0) {
              gGlobalState->SpawnHiredHand(gamePos.x, gamePos.y, 90);
            } else {
              gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                        spawnEntityConfig.entityType,
                                        spawnEntityConfig.activeEntity);
            }
          }
        }
      }
    }

    // Select Entity
    auto selectEntMouseConfig =
        gConfig->buttons[Specs::MouseFeatures_SelectEntity];
    auto selectEntClicked = Specs::IsMouseClicked(selectEntMouseConfig);

    if (gSelectedEntityState.Clicking &&
        io.MouseReleased[selectEntMouseConfig.Button]) {
      gSelectedEntityState.Clicking = false;
    }

    auto gamePos = screenToGame(io.MousePos);
    if (gDebugState.DrawClosestEntHitbox || gDebugState.DrawClosestEntId ||
        selectEntClicked) {
      float closestEntDist = 1;
      if (selectEntClicked) {
        gSelectedEntityState.Entity = NULL;
        gSelectedEntityState.Clicking = true;
      }
      EntityCallback getClosestEnt = [&](Entity *e) {
        auto eDist = dist(gamePos, ImVec2(e->x, e->y));
        if (eDist < closestEntDist) {
          closestEnt = e;
          closestEntDist = eDist;
        }
      };
      forEnabledEntities(gDebugState.Selection, getClosestEnt);
      if (closestEnt && selectEntClicked) {
        gSelectedEntityState.Entity = closestEnt;
      }
    }

    if (gSelectedEntityState.Entity != NULL && gSelectedEntityState.Clicking &&
        io.MouseDown[selectEntMouseConfig.Button] &&
        io.MouseDownDuration[selectEntMouseConfig.Button] > 0.2f) {
      gSelectedEntityState.Entity->x =
          std::lerp(gSelectedEntityState.Entity->x, gamePos.x, 1.f);
      gSelectedEntityState.Entity->y =
          std::lerp(gSelectedEntityState.Entity->y, gamePos.y, 1.f);
      if ((int)gSelectedEntityState.Entity->entity_kind > 0 &&
          (int)gSelectedEntityState.Entity->entity_kind < 5) {
        auto ent = (EntityActive *)gSelectedEntityState.Entity;
        ent->time_in_air = 0.f;
      }
    }
  }

  gOverlayDrawList->AddText(ImGui::GetFont(), 32.f, {148.f, 40.f},
                            ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.05f}),
                            "SpecsHD");

  if (gModsState.SeededMode) {
    auto levelSeed = getSeedForLevel(gGlobalState->level);
    auto out = std::format("Seed: {}", levelSeed, ImGui::GetFontSize());
    gOverlayDrawList->AddText(
        ImGui::GetFont(), 32.f, {io.DisplaySize.x - 348.f, 40.f},
        ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.8f}), out.c_str());
  }

  if (gModsState.TheFullSpelunky && gFullSpelunkyState.showCharacterOverlay) {

    auto size = 20.f;
    auto padding = 4.f;
    ImVec2 start = {
        io.DisplaySize.x - size - padding,
        (io.DisplaySize.y / 2) - ((size + padding) * 10),
    };

    for (auto idx = 0; idx < 20; idx++) {
      ImColor color = charIdToColor((CharacterIndex)idx, 0.7f);

      ImVec2 p0 = {start.x, start.y + idx * (size + padding)};
      ImVec2 p1 = {p0.x + size, p0.y + size};

      if (std::find(gFullSpelunkyState.allCharacters.begin(),
                    gFullSpelunkyState.allCharacters.end(),
                    idx) != gFullSpelunkyState.allCharacters.end()) {

        gOverlayDrawList->AddRectFilled(p0, p1, color);
        gOverlayDrawList->AddRect(p0, p1, ImColor({0.f, 0.f, 0.f, 1.f}), 0.f, 0,
                                  2.f);
      }
    }
  }

  if (gDebugState.EnableTileBorders) {
    drawTileBorders();
  }

  if (gDebugState.EnableBinBorders) {
    drawBinBorders();
  }

  if (gDebugState.EnableRoomBorders) {
    drawRoomBorders();
  }

  if (gDebugState.EnablePacifistOverlay) {
    drawPacifistOverlay();
  }

  if (gDebugState.EnableSeededCrateOverlay) {
    drawSeededCrateOverlay();
  }

  if (gDebugState.EnableSeededPotOverlay) {
    drawSeededPotOverlay();
  }

  if (gDebugState.EnableSeededKaliRewards) {
    drawSeededKaliRewardsOverlay();
  }

  forEnabledEntities(gDebugState.Hitboxes, &drawEntityHitboxDefault);

  forEnabledEntities(gDebugState.Ids, &drawEntityId);

  if (gDebugState.DrawEntityOffsets.size() > 0) {
    forEnabledEntities(gAllEntities, &drawEntityOffsetDebug);
  }

  if (gDebugState.ShowOlmecCrushProbes) {
    for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
         idx++) {
      auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

      if (!ent || ent->entity_type != 1055) {
        continue;
      }

      auto probe_idx = 0.0f;
      do {
        auto x = ent->x - ent->hitbox_x + probe_idx * 0.5f;
        auto y = ent->y - ent->hitbox_down - 0.5f;

        drawPointAtCoord({x, y});

        probe_idx++;
      } while (probe_idx < 8);
    }
  }

  if (gDebugState.BlackMarketTrainer && gGlobalState->screen_state == 0 &&
      gGlobalState->play_state == 0) {
    if (gGlobalState->level > 4 && gGlobalState->level < 9 &&
        gGlobalState->is_worm == 0 && gGlobalState->is_blackmarket == 0 &&
        gGlobalState->level_state->alt_exit_x > 0 &&
        gGlobalState->level_state->alt_exit_y > 0) {

      auto color = ImGui::GetColorU32({255.f, 0.0f, 0.0f, 0.25f});
      auto replace_color = ImGui::GetColorU32({255.0f, 255.0f, 0.0f, 0.25f});
      auto bm_color = ImGui::GetColorU32({0.f, 255.0f, 0.0f, 0.25f});

      if (gGlobalState->player1) {
        drawEntityCircle(gGlobalState->player1, 10.f, bm_color);
      }

      for (auto e_idx = 0; e_idx < ELIGIBLE_FLOORS_FOR_BM_COUNT; e_idx++) {

        auto idx = ELIGIBLE_FLOORS_FOR_BM[e_idx];
        auto floor = gGlobalState->level_state->entity_floors[idx];
        if (!floor) {
          continue;
        }
        auto screen = gameToScreen({floor->x - 0.3f, floor->y + 0.3f});

        // BM, draw green
        if (floor->x == gGlobalState->level_state->alt_exit_x &&
            floor->y == gGlobalState->level_state->alt_exit_y) {
          drawEntityHitbox(floor, bm_color, true);
          continue;
        }

        // Check for replaced entities
        if (floor->entity_type != 0x2389 && floor->entity_type != 0x2387) {
          drawEntityHitbox(floor, replace_color, true);
          continue;
        }

        drawEntityHitbox(floor, color, true);
      }
    }
  }

  if (gDebugState.DrawHHFollowerLink || gDebugState.DrawHHFollowingLink) {

    for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
         idx++) {
      auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

      if (!ent)
        continue;
      if (ent->entity_kind != EntityKind::KIND_PLAYER)
        continue;

      auto player = (EntityPlayer *)ent;

      if (gDebugState.DrawHHFollowerLink && player->follower) {
        auto follower_color = ImGui::GetColorU32({255.f, 0.0f, 0.0f, 0.9f});
        auto start = gameToScreen({player->x, player->y});
        auto end = gameToScreen({player->follower->x, player->follower->y});
        start.y += 4;
        end.y += 4;
        gOverlayDrawList->AddLine(start, end, follower_color, 1.f);
      }

      if (gDebugState.DrawHHFollowingLink && player->following) {
        auto following_color = ImGui::GetColorU32({0.f, 255.0f, 0.0f, 0.9f});
        auto start = gameToScreen({player->x, player->y});
        auto end = gameToScreen({player->following->x, player->following->y});
        start.y -= 4;
        end.y -= 4;
        gOverlayDrawList->AddLine(start, end, following_color, 1.f);
      }
    }
  }

  if (gDebugState.DrawEnemyDetection) {
    auto color = ImGui::GetColorU32({0.0f, 1.0f, .5f, .9f});
    auto wallColor = ImGui::GetColorU32({0.9f, 0.9f, 0.0f, .9f});
    auto waterColor = ImGui::GetColorU32({0.0f, 0.0f, 1.0f, .9f});
    auto bombColor = ImGui::GetColorU32({255.f, 0.0f, 0.0f, 0.25f});

    if (gGlobalState->player1) {
      drawPointAtCoord({gGlobalState->player1->x, gGlobalState->player1->y});
    }
    for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
         idx++) {
      auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

      if (ent) {
        auto entityRoom = gGlobalState->level_state
                              ->room_types[GetRoomForPosition(ent->x, ent->y)];
        if (ent->entity_type == 1002) { // Spider
          gOverlayDrawList->AddQuad(
              gameToScreen({ent->x - 0.4f, ent->y}),
              gameToScreen({ent->x + 0.4f, ent->y}),
              gameToScreen({ent->x + 0.4f, ent->y - 7.0f}),
              gameToScreen({ent->x - 0.4f, ent->y - 7.0f}), color);
        } else if (ent->entity_type == 1003) { // Bat
          gOverlayDrawList->PathArcToFast(gameToScreen({ent->x, ent->y - 0.2f}),
                                          gameToScreen({ent->x + 6.0f, 0.f}).x -
                                              gameToScreen({ent->x, 0.f}).x,
                                          0, 6);
          gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
        } else if ( // Caveman / Hawk Man / Croc Man / Green Knight /
                    // Scorpion / Tiki
            ent->entity_type == 1004 || ent->entity_type == 1011 ||
            ent->entity_type == 1044 || ent->entity_type == 1045 ||
            ent->entity_type == 1029 || ent->entity_type == 1041) {
          drawEntityDetectionRay(ent, 6.0f, wallColor);
        } else if (ent->entity_type == 1042) { // Scorpion Fly
          drawEntityDetectionRay(ent, 6.0f, wallColor);
          drawEntityCircle(ent, 6.f, color);
        } else if (ent->entity_type == 1006) { // Shopkeeper

          // 0 - Chilling
          // 1 - Following (suspicious while in shop)
          // 5 - Aggro (Hoping around shooting like a maniac)
          // 6 - Unknown - Immediately transitions to aggro
          // 7 - Patrol (Standing)
          // 8 - Patrol (Walking)
          // 9 - Patrol (No break, Vault Keeper). No proximity Aggro

          // Holding Item, Follow
          if (ent->field8_0x14c == 0 || ent->field8_0x14c == 1) {
            drawEntityCircle(ent, 2.f, color);
            drawEntityCircle(ent, 1.f, color);
          }

          // Patrolling
          if (ent->field8_0x14c == 7 || ent->field8_0x14c == 8) {
            drawEntityCircle(ent, 6.f, color);
          }

          drawEntityCircle(ent, 20.f, color);
          if (ent->field8_0x14c == 5) {
            drawEntityCircle(ent, 2.f, color);
            drawEntityCircle(ent, 12.f, color);
            if (ent->flag_horizontal_flip) {

              gOverlayDrawList->AddQuad(
                  gameToScreen({ent->x - 5.0f, ent->y + 12.0f}),
                  gameToScreen({ent->x, ent->y + 12.0f}),
                  gameToScreen({ent->x, ent->y - 12.0f}),
                  gameToScreen({ent->x - 5.0f, ent->y - 12.0f}), color);
            } else {
              gOverlayDrawList->AddQuad(
                  gameToScreen({ent->x, ent->y + 12.0f}),
                  gameToScreen({ent->x + 5.0f, ent->y + 12.0f}),
                  gameToScreen({ent->x + 5.0f, ent->y - 12.0f}),
                  gameToScreen({ent->x, ent->y - 12.0f}), color);
            }
          }
        } else if (ent->entity_type == 1007 ||
                   ent->entity_type == 1021) { // Blue / Orange Frog
          drawEntityCircle(ent, 8.f, color);
        } else if (ent->entity_type == 1010) { // UFO

          gOverlayDrawList->AddLine(gameToScreen({ent->x - 8.0f, ent->y}),
                                    gameToScreen({ent->x + 8.0f, ent->y}),
                                    color);
          gOverlayDrawList->AddLine(
              gameToScreen({ent->x - 5.25f, ent->y - 6.0f}),
              gameToScreen({ent->x + 5.25f, ent->y - 6.0f}), color);
          drawEntityCircle(ent, 8.f, color);
          gOverlayDrawList->AddQuad(
              gameToScreen({ent->x - 0.5f, ent->y}),
              gameToScreen({ent->x + 0.5f, ent->y}),
              gameToScreen({ent->x + 0.5f, ent->y - 8.0f}),
              gameToScreen({ent->x - 0.5f, ent->y - 8.0f}), color);
        } else if (ent->entity_type == 1012) { // Skeleton
          gOverlayDrawList->PathArcToFast(gameToScreen({ent->x, ent->y}),
                                          gameToScreen({ent->x + 4.0f, 0.f}).x -
                                              gameToScreen({ent->x, 0.f}).x,
                                          0, 6);
          gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
        } else if (ent->entity_type == 1013) { // Piranha
          drawEntityCircle(ent, 6.f, waterColor);
        } else if (ent->entity_type == 1023) { // Old Bitey
          drawEntityCircle(ent, 5.f, waterColor);
        } else if (ent->entity_type == 1014) { // Mummy
          gOverlayDrawList->AddLine(
              gameToScreen({ent->x - 5.2f, ent->y - 3.0f}),
              gameToScreen({ent->x + 5.2f, ent->y - 3.0f}), color);
          gOverlayDrawList->AddLine(
              gameToScreen({ent->x - 5.2f, ent->y + 3.0f}),
              gameToScreen({ent->x + 5.2f, ent->y + 3.0f}), color);
          drawEntityCircle(ent, 6.f, color);
        } else if (ent->entity_type == 1015) { // Monkey
          // On vine
          if (ent->field71_0x203 != 0) {
            gOverlayDrawList->PathArcToFast(
                gameToScreen({ent->x, ent->y}),
                gameToScreen({ent->x + 4.0f, 0.f}).x -
                    gameToScreen({ent->x, 0.f}).x,
                0, 6);
            gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
          } else {
            drawEntityCircle(ent, 4.f, color);
          }
        } else if (ent->entity_type == 1020 ||
                   ent->entity_type == 1028) { // Vampire or Vlad
          if (entityRoom < 0x13 || entityRoom > 0x15) {
            gOverlayDrawList->PathArcToFast(
                gameToScreen({ent->x, ent->y - 0.2f}),
                gameToScreen({ent->x + 6.0f, 0.f}).x -
                    gameToScreen({ent->x, 0.f}).x,
                0, 6);
            gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
          } else {
            gOverlayDrawList->AddQuad(
                gameToScreen({ent->x - 1.0f, ent->y - 0.2f}),
                gameToScreen({ent->x + 1.0f, ent->y - 0.2f}),
                gameToScreen({ent->x + 1.0f, ent->y - 6.0f}),
                gameToScreen({ent->x - 1.0f, ent->y - 6.0f}), color);
          }
        } else if (ent->entity_type == 1018) { // Giant Spider
          gOverlayDrawList->AddQuad(
              gameToScreen({ent->x - 1.0f, ent->y}),
              gameToScreen({ent->x + 1.0f, ent->y}),
              gameToScreen({ent->x + 1.0f, ent->y - 8.0f}),
              gameToScreen({ent->x - 1.0f, ent->y - 8.0f}), color);
        } else if (ent->entity_type == 1019) { // Jiang Shi
          drawEntityCircle(ent, 8.f, color);
        } else if (ent->entity_type == 1024) { // Scarab
          drawEntityCircle(ent, 6.f, color);
          gOverlayDrawList->AddLine(
              gameToScreen({ent->x - 5.95f, ent->y - 0.2f}),
              gameToScreen({ent->x + 5.95f, ent->y - 0.2f}), color);
        } else if (ent->entity_type == 1046) { // Worm Egg
          drawEntityCircle(ent, 6.f, color);
          gOverlayDrawList->AddLine(gameToScreen({ent->x - 5.6f, ent->y + 2.f}),
                                    gameToScreen({ent->x + 5.6f, ent->y + 2.f}),
                                    color);
        } else if (ent->entity_type == 1025) { // Yeti King
          drawEntityCircle(ent, 6.f, color);
        } else if (ent->entity_type == 1030) { // Imp
          gOverlayDrawList->AddQuad(
              gameToScreen({ent->x - 1.0f, ent->y}),
              gameToScreen({ent->x + 1.0f, ent->y}),
              gameToScreen({ent->x + 1.0f, ent->y - 8.0f}),
              gameToScreen({ent->x - 1.0f, ent->y - 8.0f}), color);
        } else if (/*ent->entity_type == 1032 || ent->entity_type == 1034 ||*/
                   ent->entity_type == 1038) { // Bee, Queen Bee, Giant Frog
          drawEntityCircle(ent, 6.f, color);
        } else if (ent->entity_type == 1031) { // Blue Devil
          drawEntityDetectionRay(ent, 6.0f, wallColor);
          gOverlayDrawList->AddQuad(
              gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up}),
              gameToScreen(
                  {ent->x - ent->hitbox_x, ent->y + ent->hitbox_up + 5.0f}),
              gameToScreen(
                  {ent->x + ent->hitbox_x, ent->y + ent->hitbox_up + 5.0f}),
              gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up}),
              wallColor);
        } else if (ent->entity_type == 1033) { // Anubis
          // Engage
          drawEntityCircle(ent, 11.f, color);
          // Shoot
          drawEntityCircle(ent, 8.f, color);
          // Retreat
          drawEntityCircle(ent, 4.f, color);
        } else if (ent->entity_type == 1040) { // Alien Tank
          drawEntityCircle(ent, 6.f, color);

          gOverlayDrawList->AddLine(gameToScreen({ent->x, ent->y - 2.0f}),
                                    gameToScreen({ent->x, ent->y + 6.0f}),
                                    color);

          if (ent->flag_horizontal_flip) {
            gOverlayDrawList->AddLine(
                gameToScreen({ent->x - 5.6f, ent->y - 2.0f}),
                gameToScreen({ent->x, ent->y - 2.0f}), color);
          } else {
            gOverlayDrawList->AddLine(
                gameToScreen({ent->x, ent->y - 2.0f}),
                gameToScreen({ent->x + 5.6f, ent->y - 2.0f}), color);
          }
        } else if (ent->entity_type == 1016) { // Alien Lord
          drawEntityCircle(ent, 8.f, color);
        } else if (ent->entity_type == 1048) { // Alien Queen
          drawEntityCircle(ent, 12.f, color);
        } else if (ent->entity_type == 1051) { // Succubus
          drawEntityCircle(ent, 4.f, color);
          drawEntityDetectionRay(ent, 6.0f, wallColor);
        } else if (ent->entity_type == 1052 ||
                   ent->entity_type == 1053) { // Horse Head / Ox Face
          drawEntityCircle(ent, 4.f, color);
        } else if (ent->entity_type == 1054) { // Anubis 2
          drawEntityCircle(ent, 8.f, color);
        } else if (ent->entity_type == 1055) { // Olmec
          drawEntityCircle(ent, 10.f, color);
        } else if (ent->entity_type == 1056) { // Yama Head
          if (ent->field75_0x207 == 0) {

            drawEntityCircle(ent, 6.f, color);
          } else {

            // Above Yama Slam
            gOverlayDrawList->AddQuad(gameToScreen({ent->x - 1.0f, 200.0}),
                                      gameToScreen({ent->x + 1.0f, 200.0}),
                                      gameToScreen({ent->x + 1.0f, ent->y}),
                                      gameToScreen({ent->x - 1.0f, ent->y}),
                                      color, 1.f);

            // Pursue Zone
            gOverlayDrawList->AddQuad(gameToScreen({14.0, 110.0}),
                                      gameToScreen({31.0, 110.0}),
                                      gameToScreen({31.0, 88.0}),
                                      gameToScreen({14.0, 88.0}), color, 1.f);

            // Slam Zones
            gOverlayDrawList->AddQuad(gameToScreen({-10.0, 110.0}),
                                      gameToScreen({14.0, 110.0}),
                                      gameToScreen({14.0, 93.0}),
                                      gameToScreen({-10.0, 93.0}), color, 1.f);
            gOverlayDrawList->AddQuad(gameToScreen({31.0, 110.0}),
                                      gameToScreen({60.0, 110.0}),
                                      gameToScreen({60.0, 93.0}),
                                      gameToScreen({31.0, 93.0}), color, 1.f);
          }
        } else if (ent->entity_type == 1057) { // Yama Hand
          drawEntityCircle(ent, 6.f, color);

          // Slam Line
          gOverlayDrawList->AddLine(gameToScreen({-10.0f, ent->y - 2.0f}),
                                    gameToScreen({60.0f, ent->y - 2.0f}),
                                    color);
        } else if (ent->entity_type == 1058) { // Turret
          drawEntityCircle(ent, 6.f, color);
          gOverlayDrawList->AddLine(gameToScreen({ent->x - 6.0f, ent->y}),
                                    gameToScreen({ent->x + 6.0f, ent->y}),
                                    color);
        } else if (ent->entity_type == 107 ||
                   ent->entity_type == 92) { // Bomb / Landmine
          drawEntityCircle(ent, sqrt(3.75f), color);
          drawEntityCircle(ent, sqrt(1.6f), color);

          // Find all floor collisions
          auto x_pos = (int)(ent->x + 0.5);
          auto y_pos = (int)(ent->y + 0.5);
          auto x_offset = -3;
          do {
            auto y_count = 7;
            auto y_offset = (99 - y_pos) * 0x2e;
            do {
              auto floor_idx = x_offset + x_pos + y_offset;
              if (floor_idx >= 0 && floor_idx < 4692) {
                auto floor =
                    gGlobalState->level_state->entity_floors[floor_idx];

                if (floor && (floor->flag_6 != 0 || floor->flag_9 != 0) &&
                    floor->flag_4 == 0) {
                  auto x_distance = abs(ent->x - floor->x);
                  auto y_distance = abs(ent->y - floor->y);
                  auto distance =
                      x_distance * x_distance + y_distance * y_distance;

                  if (distance < 3.75) {
                    drawEntityHitbox(floor, bombColor, true);
                  }
                }
              }

              y_count--;
              y_offset += 0x2e;
            } while (y_count > 0);
            x_offset++;
          } while (x_offset < 4);

          // Check all active entities for bomb collision
          for (size_t idx = 0;
               idx < gGlobalState->entities->entities_active_count; idx++) {
            auto collision_ent =
                (EntityActive *)gGlobalState->entities->entities_active[idx];

            if (collision_ent && collision_ent != ent &&
                collision_ent->field49_0x1ed != 0 &&
                (collision_ent->field50_0x1ee != 0 ||
                 collision_ent->field63_0x1fb != 0) &&
                collision_ent->entity_type != 137) {
              auto x_distance = abs(ent->x - collision_ent->x);
              auto y_distance = abs(ent->y - collision_ent->y);
              auto distance = x_distance * x_distance + y_distance * y_distance;

              if (distance <= 16.0f &&
                  entityCollidesWithCircle(collision_ent, ent->x, ent->y,
                                           1.6f)) {
                drawEntityHitbox(collision_ent, bombColor, true);
              }
            }
          }
        }
      }
    }
  }

  if (gSelectedEntityState.Entity != NULL &&
      gDebugState.DrawSelectedEntHitbox) {
    drawEntityHitbox(gSelectedEntityState.Entity,
                     ImGui::GetColorU32({1.0f, 1.0f, 1.0f, .9f}));
  }
  if (closestEnt) {
    if (gDebugState.DrawClosestEntHitbox)
      drawEntityHitbox(closestEnt, ImGui::GetColorU32({0.0f, 1.0f, .5f, .9f}));
    if (gDebugState.DrawClosestEntId) {
      drawEntityId(closestEnt);
    }
  }
  ImGui::End();
}

void drawSpawnTab() {
  ImGuiIO &io = ImGui::GetIO();

  auto scrollLock = true;

  if (gSpawnState.SpawnEntityInputs.empty()) {
    gSpawnState.SpawnEntityInputs.push_back(SpawnEntityConfig{-1, true});
  }

  for (auto idx = 0; auto &spawnEntityConfig : gSpawnState.SpawnEntityInputs) {

    ImGui::PushItemWidth(200 * io.FontGlobalScale);

    if (idx > 0) {
      if (ImGui::Button(std::format("-##SpawnEntityInput-{}", idx).c_str())) {
        gSpawnState.SpawnEntityInputs.erase(
            gSpawnState.SpawnEntityInputs.begin() + idx);
      }
    } else {
      auto size = ImGui::CalcTextSize("-");
      size.x += 8.0f;
      ImGui::InvisibleButton(std::format("-##SpawnEntityInput-{}", idx).c_str(),
                             size);
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::InputInt(
            std::format("Entity ID##SpawnEntityInputId-{}", idx).c_str(),
            &spawnEntityConfig.entityType)) {
      scrollLock = false;
    };
    ImGui::SameLine();
    ImGui::Checkbox(std::format("Active##SpawnEntityInputId-{}", idx).c_str(),
                    &spawnEntityConfig.activeEntity);

    idx++;
  }

  if (ImGui::Button("Add Additional Entity")) {
    gSpawnState.SpawnEntityInputs.push_back(SpawnEntityConfig{-1, true});
  }

  ImGui::Separator();

  if (ImGui::Button("Spawn")) {
    for (auto const &spawnEntityConfig : gSpawnState.SpawnEntityInputs) {

      if (spawnEntityConfig.entityType >= 0) {
        if (spawnEntityConfig.entityType == 0) {
          gGlobalState->SpawnHiredHand(gGlobalState->player1->x,
                                       gGlobalState->player1->y, 90);
        } else {
          gGlobalState->SpawnEntity(
              gGlobalState->player1->x, gGlobalState->player1->y,
              spawnEntityConfig.entityType, spawnEntityConfig.activeEntity);
        }
      }
    }
  }
  ImGui::SameLine();
  ImGui::Checkbox("Click to spawn", &gSpawnState.ClickToSpawn);

  ImGui::Separator();
  if (ImGui::InputText("Filter", &gSpawnState.EntityListFilter)) {
    std::transform(gSpawnState.EntityListFilter.begin(),
                   gSpawnState.EntityListFilter.end(),
                   gSpawnState.EntityListFilter.begin(), ::tolower);
  }

  if (ImGui::BeginListBox("##", {-1, -1})) {
    auto spawnEntityConfig = &gSpawnState.SpawnEntityInputs.back();
    for (auto const &[name, entity_type] : gEntities) {

      std::string lowerName = name;
      std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                     ::tolower);
      if (!gSpawnState.EntityListFilter.empty() &&
          lowerName.find(gSpawnState.EntityListFilter) == std::string::npos) {
        continue;
      }
      auto label = std::format("{:4}: {}", entity_type, name);
      const bool is_selected = (entity_type == spawnEntityConfig->entityType);
      if (ImGui::Selectable(label.c_str(), is_selected)) {
        spawnEntityConfig->entityType = entity_type;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
        if (!scrollLock) {
          ImGui::SetScrollHereY();
        }
      }
    }
    ImGui::EndListBox();
  }
}

void warpToLevel(uint32_t level) {
  gGlobalState->level_minutes = 0;
  gGlobalState->level_seconds = 0;
  gGlobalState->level_ms = 0;
  gGlobalState->level = level;
  gGlobalState->screen_state = 3;
}

void RectFilled(ImVec2 &size, ImU32 col = IM_COL32_WHITE, float rounding = 0.f,
                ImDrawFlags flags = 0) {

  ImGui::Dummy(size);

  if (!ImGui::IsItemVisible()) {
    return;
  }

  auto p0 = ImGui::GetItemRectMin();
  auto p1 = ImGui::GetItemRectMax();
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(p0, p1, true);
  draw_list->AddRectFilled(p0, p1, col, rounding, flags);
  draw_list->PopClipRect();
}

void Rect(const char *label, ImVec2 &size, ImU32 col = IM_COL32_WHITE,
          float rounding = 0.f, ImDrawFlags flags = 0, float thickness = 1.0f) {

  ImGui::Dummy(size);

  if (!ImGui::IsItemVisible()) {
    return;
  }

  auto p0 = ImGui::GetItemRectMin();
  auto p1 = ImGui::GetItemRectMax();
  auto textSize = ImGui::CalcTextSize(label);
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(p0, p1, true);
  draw_list->AddRect(p0, p1, col, rounding, flags, thickness);
  draw_list->AddText({p0.x + ((p1.x - p0.x) / 2) - (textSize.x / 2),
                      p0.y + ((p1.y - p0.y) / 2) - (textSize.y / 2)},
                     col, label);
  draw_list->PopClipRect();
}

void drawLockedLevelFlag(std::string title, uint8_t &val,
                         LockableU8 *lockable) {
  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::Checkbox(std::format("##LevelLock{}", title).c_str(),
                      &lockable->IsLocked)) {
    if (lockable->IsLocked) {
      lockable->LockedValue = val;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  if (drawCharBool(title.c_str(), val)) {
    lockable->LockedValue = val;
  }
}

void drawLevelTab() {

  ImGuiIO &io = ImGui::GetIO();
  auto isDisabled =
      gGlobalState->screen_state != 0 || gGlobalState->play_state != 0;

  if (isDisabled) {
    ImGui::BeginDisabled();
  }
  ImGui::Text("");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("Next Level")) {
    warpToLevel(gGlobalState->level);
  }

  ImGui::Text("Mines");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("1-1"))
    warpToLevel(0);
  ImGui::SameLine();
  if (ImGui::Button("1-2"))
    warpToLevel(1);
  ImGui::SameLine();
  if (ImGui::Button("1-3")) {
    warpToLevel(2);
  }
  ImGui::SameLine();
  if (ImGui::Button("1-4")) {
    warpToLevel(3);
  }

  ImGui::Text("Jungle");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("2-1"))
    warpToLevel(4);
  ImGui::SameLine();
  if (ImGui::Button("2-2"))
    warpToLevel(5);
  ImGui::SameLine();
  if (ImGui::Button("2-3")) {
    warpToLevel(6);
  }
  ImGui::SameLine();
  if (ImGui::Button("2-4")) {
    warpToLevel(7);
  }

  ImGui::Text("Ice Caves");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("3-1"))
    warpToLevel(8);
  ImGui::SameLine();
  if (ImGui::Button("3-2"))
    warpToLevel(9);
  ImGui::SameLine();
  if (ImGui::Button("3-3")) {
    warpToLevel(10);
  }
  ImGui::SameLine();
  if (ImGui::Button("3-4")) {
    warpToLevel(11);
  }

  ImGui::Text("Temple");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("4-1"))
    warpToLevel(12);
  ImGui::SameLine();
  if (ImGui::Button("4-2"))
    warpToLevel(13);
  ImGui::SameLine();
  if (ImGui::Button("4-3")) {
    warpToLevel(14);
  }
  ImGui::SameLine();
  if (ImGui::Button("Olmec")) {
    warpToLevel(15);
  }

  ImGui::Text("Hell");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("5-1"))
    warpToLevel(16);
  ImGui::SameLine();
  if (ImGui::Button("5-2"))
    warpToLevel(17);
  ImGui::SameLine();
  if (ImGui::Button("5-3")) {
    warpToLevel(18);
  }
  ImGui::SameLine();
  if (ImGui::Button("Yama")) {
    warpToLevel(19);
  }

  if (isDisabled) {
    ImGui::EndDisabled();
  }

  ImGui::Separator();
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  ImGui::InputInt("Respawn Level Skip", (int *)&gGlobalState->respawn_level);
  ImGui::SameLine();
  if (ImGui::Button("Set Current Level")) {
    gGlobalState->respawn_level =
        std::clamp((int)gGlobalState->level - 1, 0, (int)gGlobalState->level);
  }

  if (ImGui::CollapsingHeader("Level Flags")) {
    ImGui::Text("Locked?");
    ImGui::SameLine(80.0f * io.FontGlobalScale);
    ImGui::Text("Value");
    ImGui::Separator();

    ImGui::Separator();

    drawLockedLevelFlag("Flooded Mines", gGlobalState->flooded_mines,
                        &gLevelsState.FloodedMines);
    drawLockedLevelFlag("Skin is Crawling", gGlobalState->skin_is_crawling,
                        &gLevelsState.SkinIsCrawling);

    ImGui::Separator();
    drawLockedLevelFlag("Dead are Restless", gGlobalState->dead_are_restless,
                        &gLevelsState.DeadAreRestless);
    drawLockedLevelFlag("Rushing Water", gGlobalState->rushing_water,
                        &gLevelsState.RushingWater);
    drawLockedLevelFlag("Haunted Castle", gGlobalState->is_haunted_castle,
                        &gLevelsState.HauntedCastle);
    drawLockedLevelFlag("Tiki Village", gGlobalState->tiki_village,
                        &gLevelsState.TikiVillage);
    drawLockedLevelFlag("Black Market", gGlobalState->is_blackmarket,
                        &gLevelsState.BlackMarket);

    ImGui::Separator();
    drawLockedLevelFlag("Wet Fur", gGlobalState->is_wet_fur,
                        &gLevelsState.WetFur);
    drawLockedLevelFlag("Mothership", gGlobalState->is_mothership,
                        &gLevelsState.MotherShip);
    drawLockedLevelFlag("Worm", gGlobalState->is_worm, &gLevelsState.Worm);

    ImGui::Separator();
    drawLockedLevelFlag("City of Gold", gGlobalState->is_city_of_gold,
                        &gLevelsState.CityOfGold);

    ImGui::Separator();
    drawLockedLevelFlag("Altar Spawned", gGlobalState->altar_spawned,
                        &gLevelsState.AltarSpawned);
    drawLockedLevelFlag("Idol Spawned", gGlobalState->idol_spawned,
                        &gLevelsState.IdolSpawned);
    drawLockedLevelFlag("Damsel Spawned", gGlobalState->damsel_spawned,
                        &gLevelsState.DamselSpawned);
    drawLockedLevelFlag("Ghost Spawned", gGlobalState->ghost_spawned,
                        &gLevelsState.GhostSpawned);
    drawLockedLevelFlag("Vault Spawned in Area",
                        gGlobalState->vault_spawned_in_area,
                        &gLevelsState.VaultSpawnedInArea);
  }

  ImVec2 size = {5.f, 5.f};
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Level Map")) {
    for (auto idx = 0; idx < 4692; idx++) {
      auto ent = gGlobalState->level_state->entity_floors[idx];

      auto col = IM_COL32(183, 183, 183, 255);
      // Empty
      if (ent == NULL) {
        col = IM_COL32(0, 0, 0, 0);
      } else if (ent->entity_type == 3 || ent->entity_type == 2) {
        // Doors
        col = IM_COL32(59, 196, 0, 255);
      } else if (ent->entity_type == 4 || ent->entity_type == 5) {
        // Ladders
        col = IM_COL32(133, 133, 133, 100);
      } else if (ent->entity_type == 25) {
        // Bedrock
        col = IM_COL32(90, 90, 90, 255);
      } else if (ent->entity_type == 27) {
        // Water
        col = IM_COL32(43, 114, 214, 100);
      } else if (ent->entity_type == 36) {
        // Lava
        col = IM_COL32(214, 54, 43, 100);
      } else if (ent->entity_type == 91) {
        // Acid
        col = IM_COL32(43, 214, 77, 100);
      }

      if (idx % 46 > 0) {

        ImGui::SameLine(0.f * io.FontGlobalScale, 4.f * io.FontGlobalScale);
      }
      RectFilled(size, col);
    }
  }

  ImVec2 roomTypeSize = {40.f, 40.f};
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Room Types")) {
    for (auto idx = 0; idx < 48; idx++) {
      auto column = idx % 4;
      auto type = gGlobalState->level_state->room_types[idx];
      auto col = IM_COL32(183, 183, 183, 255);
      if (type >= 1 && type <= 3) {
        // Path
        col = IM_COL32(59, 196, 0, 255);
      }
      if (column > 0) {

        ImGui::SameLine(0.f * io.FontGlobalScale, 4.f * io.FontGlobalScale);
      }
      auto label = std::format("{}", type);
      Rect(label.c_str(), roomTypeSize, col);
    }
  }
}

void ensureLockedAmountsForPlayer(EntityPlayer *player, PlayerData &data,
                                  PlayerState *state) {
  if (state->LockHealth) {
    // If you ressurect a player it gets into a bad state.
    if (player->health > 0) {
      player->health = state->LockedHealthAmount;
    }
  }

  if (state->LockHeldItemId) {
    data.held_item_id = state->LockedHeldItemId;
  }
  if (state->LockHeldItemMetadata) {
    data.held_item_metadata = state->LockedHeldItemMetadata;
  }
  if (state->LockHiredHandCount) {
    data.hh_count = state->LockedHiredHandCount;
  }

  if (state->LockBombs) {
    data.bombs = state->LockedBombsAmount;
  }

  if (state->LockRopes) {
    data.ropes = state->LockedRopesAmount;
  }

  if (state->LockCompass) {
    data.has_compass = state->LockedCompassValue;
  }

  if (state->LockParachute) {
    data.has_parachute = state->LockedParachuteValue;
  }

  if (state->LockJetpack) {
    data.has_jetpack = state->LockedJetpackValue;
  }

  if (state->LockClimbingGloves) {
    data.has_climbing_gloves = state->LockedClimbingGlovesValue;
  }

  if (state->LockPitchersMitt) {
    data.has_pitchers_mitt = state->LockedPitchersMittValue;
  }

  if (state->LockSpringShoes) {
    data.has_spring_shoes = state->LockedSpringShoesValue;
  }

  if (state->LockSpikeShoes) {
    data.has_spike_shoes = state->LockedSpikeShoesValue;
  }

  if (state->LockSpectacles) {
    data.has_spectacles = state->LockedSpectaclesValue;
  }

  if (state->LockKapala) {
    data.has_kapala = state->LockedKapalaValue;
  }

  if (state->LockHedjet) {
    data.has_hedjet = state->LockedHedjetValue;
  }

  if (state->LockUdjatEye) {
    data.has_udjat = state->LockedUdjatEyeValue;
  }

  if (state->LockBookOfTheDead) {
    data.has_book_of_dead = state->LockedBookOfTheDeadValue;
  }

  if (state->LockAnkh) {
    data.has_ankh = state->LockedAnkhValue;
  }

  if (state->LockPaste) {
    data.has_paste = state->LockedPasteValue;
  }

  if (state->LockCape) {
    data.has_cape = state->LockedCapeValue;
  }

  if (state->LockVladsCape) {
    data.has_vlads_cape = state->LockedVladsCapeValue;
  }

  if (state->LockCrysknife) {
    data.has_crysknife = state->LockedCrysknifeValue;
  }

  if (state->LockVladsAmulet) {
    data.has_vlads_amulet = state->LockedVladsAmuletValue;
  }

  if (state->LockWhiteFlag) {
    data.has_white_flag = state->LockedWhiteFlagValue;
  }
}

void ensureLockedU8(uint8_t &val, LockableU8 *lockable) {
  if (lockable->IsLocked) {
    val = lockable->LockedValue;
  }
}

void ensureLockedLevelsState() {
  ensureLockedU8(gGlobalState->flooded_mines, &gLevelsState.FloodedMines);
  ensureLockedU8(gGlobalState->skin_is_crawling, &gLevelsState.SkinIsCrawling);

  ensureLockedU8(gGlobalState->dead_are_restless,
                 &gLevelsState.DeadAreRestless);
  ensureLockedU8(gGlobalState->rushing_water, &gLevelsState.RushingWater);
  ensureLockedU8(gGlobalState->is_haunted_castle, &gLevelsState.HauntedCastle);
  ensureLockedU8(gGlobalState->tiki_village, &gLevelsState.TikiVillage);
  ensureLockedU8(gGlobalState->is_blackmarket, &gLevelsState.BlackMarket);

  ensureLockedU8(gGlobalState->is_wet_fur, &gLevelsState.WetFur);
  ensureLockedU8(gGlobalState->is_mothership, &gLevelsState.MotherShip);
  ensureLockedU8(gGlobalState->is_worm, &gLevelsState.Worm);

  ensureLockedU8(gGlobalState->is_city_of_gold, &gLevelsState.CityOfGold);

  ensureLockedU8(gGlobalState->altar_spawned, &gLevelsState.AltarSpawned);
  ensureLockedU8(gGlobalState->idol_spawned, &gLevelsState.IdolSpawned);
  ensureLockedU8(gGlobalState->damsel_spawned, &gLevelsState.DamselSpawned);
  ensureLockedU8(gGlobalState->ghost_spawned, &gLevelsState.GhostSpawned);
  ensureLockedU8(gGlobalState->vault_spawned_in_area,
                 &gLevelsState.VaultSpawnedInArea);
}

void ensureLockedAmounts() {
  if (gGlobalState->player1) {
    ensureLockedAmountsForPlayer(gGlobalState->player1,
                                 gGlobalState->player1_data, &gPlayersState[0]);
  }

  if (gGlobalState->player2) {
    ensureLockedAmountsForPlayer(gGlobalState->player2,
                                 gGlobalState->player2_data, &gPlayersState[1]);
  }

  if (gGlobalState->player3) {
    ensureLockedAmountsForPlayer(gGlobalState->player3,
                                 gGlobalState->player3_data, &gPlayersState[2]);
  }

  if (gGlobalState->player4) {
    ensureLockedAmountsForPlayer(gGlobalState->player4,
                                 gGlobalState->player4_data, &gPlayersState[3]);
  }

  ensureLockedLevelsState();
}

void drawLockedPlayerDataCheckbox(std::string title, bool &val, bool &lockVar,
                                  bool &lockedVal) {
  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::Checkbox(std::format("##Lock{}", title).c_str(), &lockVar)) {
    if (lockVar) {
      lockedVal = val;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  if (ImGui::Checkbox(title.c_str(), &val)) {
    lockedVal = val;
  }
}

void drawPlayerTab(EntityPlayer *player, PlayerData &data, PlayerState *state) {
  ImGuiIO &io = ImGui::GetIO();

  if (!player) {
    ImGui::Text("No Player Entity");
    return;
  }

  if (ImGui::Button("Max Health/Bombs/Ropes")) {
    player->health = 99;
    state->LockedHealthAmount = player->health;
    data.bombs = 99;
    state->LockedBombsAmount = data.bombs;
    data.ropes = 99;
    state->LockedRopesAmount = data.ropes;
  }

  ImGui::Text("Locked?");
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::Text("Amount");

  if (ImGui::Checkbox("##LockHealth", &state->LockHealth)) {
    if (state->LockHealth) {
      state->LockedHealthAmount = player->health;
    }
  };
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Health", &player->health)) {
    player->health = std::clamp(player->health, 0, 99);
    state->LockedHealthAmount = player->health;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockBombs", &state->LockBombs)) {
    if (state->LockBombs) {
      state->LockedBombsAmount = data.bombs;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Bombs", &data.bombs)) {
    data.bombs = std::clamp(data.bombs, 0, 99);
    state->LockedBombsAmount = data.bombs;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockRopes", &state->LockRopes)) {
    if (state->LockRopes) {
      state->LockedRopesAmount = data.ropes;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Ropes", &data.ropes)) {
    data.ropes = std::clamp(data.ropes, 0, 99);
    state->LockedRopesAmount = data.ropes;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockHeldItemId", &state->LockHeldItemId)) {
    if (state->LockHeldItemId) {
      state->LockedHeldItemId = data.held_item_id;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Held Item", &data.held_item_id)) {
    state->LockedHeldItemId = data.held_item_id;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockHeldItemMetadata", &state->LockHeldItemMetadata)) {
    if (state->LockHeldItemMetadata) {
      state->LockedHeldItemMetadata = data.held_item_metadata;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Held Item Metadata", &data.held_item_metadata)) {
    state->LockedHeldItemMetadata = data.held_item_metadata;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockHiredHandCount", &state->LockHiredHandCount)) {
    if (state->LockHiredHandCount) {
      state->LockedHiredHandCount = data.hh_count;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Hired Hand Count", &data.hh_count)) {
    state->LockedHiredHandCount = data.hh_count;
  }
  ImGui::PopItemWidth();

  ImGui::Separator();

  drawLockedPlayerDataCheckbox("Compass", data.has_compass, state->LockCompass,
                               state->LockedCompassValue);

  drawLockedPlayerDataCheckbox("Parachute", data.has_parachute,
                               state->LockParachute,
                               state->LockedParachuteValue);

  drawLockedPlayerDataCheckbox("Jetpack", data.has_jetpack, state->LockJetpack,
                               state->LockedJetpackValue);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Jetpack")) {
    gGlobalState->SpawnEntity(player->x, player->y, 522, true);
  }

  drawLockedPlayerDataCheckbox("Climbing Gloves", data.has_climbing_gloves,
                               state->LockClimbingGloves,
                               state->LockedClimbingGlovesValue);

  drawLockedPlayerDataCheckbox("Pitcher's Mitt", data.has_pitchers_mitt,
                               state->LockPitchersMitt,
                               state->LockedPitchersMittValue);

  drawLockedPlayerDataCheckbox("Spring Shoes", data.has_spring_shoes,
                               state->LockSpringShoes,
                               state->LockedSpringShoesValue);

  drawLockedPlayerDataCheckbox("Spike Shoes", data.has_spike_shoes,
                               state->LockSpikeShoes,
                               state->LockedSpikeShoesValue);

  drawLockedPlayerDataCheckbox("Spectacles", data.has_spectacles,
                               state->LockSpectacles,
                               state->LockedSpectaclesValue);

  drawLockedPlayerDataCheckbox("Kapala", data.has_kapala, state->LockKapala,
                               state->LockedKapalaValue);

  drawLockedPlayerDataCheckbox("Hedjet", data.has_hedjet, state->LockHedjet,
                               state->LockedHedjetValue);

  drawLockedPlayerDataCheckbox("Udjat Eye", data.has_udjat, state->LockUdjatEye,
                               state->LockedUdjatEyeValue);

  drawLockedPlayerDataCheckbox("Book of the Dead", data.has_book_of_dead,
                               state->LockBookOfTheDead,
                               state->LockedBookOfTheDeadValue);

  drawLockedPlayerDataCheckbox("Ankh", data.has_ankh, state->LockAnkh,
                               state->LockedAnkhValue);

  drawLockedPlayerDataCheckbox("Paste", data.has_paste, state->LockPaste,
                               state->LockedPasteValue);

  drawLockedPlayerDataCheckbox("Cape", data.has_cape, state->LockCape,
                               state->LockedCapeValue);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Cape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 521, true);
  }

  drawLockedPlayerDataCheckbox("Vlad's Cape", data.has_vlads_cape,
                               state->LockVladsCape,
                               state->LockedVladsCapeValue);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##VladsCape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 532, true);
  }

  drawLockedPlayerDataCheckbox("Crysknife", data.has_crysknife,
                               state->LockCrysknife,
                               state->LockedCrysknifeValue);

  drawLockedPlayerDataCheckbox("Vlad's Amulet", data.has_vlads_amulet,
                               state->LockVladsAmulet,
                               state->LockedVladsAmuletValue);

  drawLockedPlayerDataCheckbox("White Flag", data.has_white_flag,
                               state->LockWhiteFlag,
                               state->LockedWhiteFlagValue);
}

void drawPlayersTab() {

  if (ImGui::BeginTabBar("Players")) {
    if (ImGui::BeginTabItem("Player 1")) {
      drawPlayerTab(gGlobalState->player1, gGlobalState->player1_data,
                    &gPlayersState[0]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 2")) {
      drawPlayerTab(gGlobalState->player2, gGlobalState->player2_data,
                    &gPlayersState[1]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 3")) {
      drawPlayerTab(gGlobalState->player3, gGlobalState->player3_data,
                    &gPlayersState[2]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 4")) {
      drawPlayerTab(gGlobalState->player4, gGlobalState->player4_data,
                    &gPlayersState[3]);
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
}

std::vector<Patch> gDarkModePatches = {
    {0x6afbe, {0x1}, {0x0}},
};

std::vector<Patch> gUplunkyPatches = {
    // Change Exit Door to Entrance Door
    {0xd91ba, {0x02}, {0x03}},
    // Change Entrance Door to Exit Door
    {0xd929c, {0x03}, {0x02}},

    // Spawn player at exit instead of entrance
    {0x6651c, {0xa0}, {0x98}},
    {0x66532, {0x9c}, {0x94}},

    // Spawn shopkeeper at entrance instead of exit
    {0x6b10c, {0x98}, {0xa0}},
    {0x6b120, {0x94}, {0x9c}},

    // Open entrance when olmec dies
    {0x309fa, {0x94}, {0x9c}},
    {0x309ed, {0x98}, {0xa0}},
    {0xde7ef, {0x03}, {0x02}},
    {0xde966, {0x02}, {0x03}},
};

std::vector<RelativePatch> gUplunkyRelativePatches = {
    // // Swap entrance/exit on olmec
    // {0xde7d6, 0x13602c, 0x135b3c},
    // {0xde94d, 0x135b3c, 0x13602c},
};

void resetUplunkyState() {
  gGlobalState->player1_data.ropes = 64;
  gGlobalState->player1_data.bombs = 6;
}

void prePlaceRoomsUplunky() {
  if (!gModsState.DarkMode) {
    gGlobalState->dark_level = 0;
  }
}

// Get bytes for float
//  ["{:02x}".format(i) for i in bytearray(struct.pack("f", 0.0))]
//
std::vector<Patch> gBiglunkyPatches = {
    // Worm Max Right Camera Bounds: 15.5 -> 35.5
    {0x135d34, {0x00, 0x00, 0x0e, 0x42}, {0x00, 0x00, 0x78, 0x41}},

    // Non-Worm Max Down Camera Bounds:
    // 70.125 -> 6.125
    {0x135d30, {0x00, 0x00, 0xc4, 0x40}, {0x00, 0x40, 0x8c, 0x42}},
    // 54.125 -> 6.125
    {0x135d24, {0x00, 0x00, 0xc4, 0x40}, {0x00, 0x80, 0x58, 0x42}},
    // 62.125 -> 6.125
    {0x135d28, {0x00, 0x00, 0xc4, 0x40}, {0x00, 0x80, 0x78, 0x42}},

    // Force levels to be 12 High
    {0xdd7b5, {0x90, 0x90}, {0x74, 0x16}},

    // Force levels to be 4 wide
    {
        0xdd84f,
        {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
         0x90},
        {0xc7, 0x44, 0x24, 0x14, 0x17, 0x00, 0x00, 0x00, 0x89, 0x5c, 0x24,
         0x2c},
    },

    // Death Depth: 46 -> 0
    {0x1367a4, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x38, 0x42}},

    {0xe8230, {0x90, 0x90}, {0x74, 0x05}},

    // Exit Room Y 11
    {0xc9d49, {0xb}, {0x3}},

    // 12 Rows
    {0xca820, {0xc}, {0x4}},
    {0xca806, {0xb}, {0x3}},

    // Spawn Bounds
    {0xbe7e5, {0x2d}, {0x19}}, // Worm Width Common
    {0xbe7f2, {0x65}, {0x2d}}, // Normal Height Common
    {0xbe7fc, {0x65}, {0x25}}, // Wet Fur Height Common
    {0xbe828, {0x65}, {0x2e}}, // Rushing Water Enemies
    {0xbe82f, {0x65}, {0x26}}, // Normal Enemies

    // Spawn Arrow Traps
    {0xe0795, {0x82, 0x12}, {0x78, 0x06}},

    // Push Blocks
    {0xe0351, {0x54, 0x12}, {0x4a, 0x06}},

    // Decorations
    {0xdd736, {0x82, 0x12}, {0xd4, 0x06}},
    {0xbe49c, {0x63}, {0x23}},

    // Move Yama Entrance
    {0xca07e, {0xb}, {0x3}},

    // Make worm 1 level
    {0x6aa37, {0xeb}, {0x74}},

    // Wet Fur Path
    {0xd34f6, {0x30}, {0x10}},

    // Mothership Path
    {0xd2b14, {0x30}, {0x10}},

    // Don't spawn Moship BG. it doesn't do a bounds check and crashes the
    // game...
    {0xc9a4d, {0x90, 0xe9}, {0x0f, 0x84}},

    // Move Olmec Lava down.
    {0xdedd4, {0x57}, {0x27}},
    {0xdeddc, {0xa5, 0x0f}, {0x05, 0x07}},
    {0xdee17, {0xb9, 0x10}, {0x19, 0x08}},
    {0xdee5e, {0x15, 0x11}, {0x75, 0x08}},

    // Move Olmec Exit Door
    {0xde980, {0x90, 0x4a}, {0xa0, 0x26}},
    {0xde989, {0x90, 0x4a}, {0xa0, 0x26}},
    {0xde996, {0x90, 0x4a}, {0xa0, 0x26}},
    {0xde9a3, {0x90, 0x4a}, {0xa0, 0x26}},

    // Udjat Blink Faster
    {0x530d6, {0x90}, {0x40}},
    {0x530d7, {0x90, 0x90, 0x90}, {0x8d, 0x04, 0x80}},

    // More Alien Lords
    {0xd2c72, {0x06}, {0x0a}},

    // Remove Rushing Water Rooms (Placed manually).
    {0xcad87,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x89, 0x85, 0x1c, 0xa6, 0x00, 0x00}},
    {0xcad8d,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x89, 0x85, 0x20, 0xa6, 0x00, 0x00}},
    {0xcad93,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x89, 0x85, 0x24, 0xa6, 0x00, 0x00}},
    {0xcad99,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x89, 0x85, 0x28, 0xa6, 0x00, 0x00}},
    {0xcada4,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x89, 0x85, 0x2c, 0xa6, 0x00, 0x00}},
    {0xcadaa,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x89, 0x85, 0x30, 0xa6, 0x00, 0x00}},
    {0xcadb0,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x89, 0x85, 0x34, 0xa6, 0x00, 0x00}},
    {0xcadb6,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x89, 0x85, 0x38, 0xa6, 0x00, 0x00}},
    {0xcae84,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0xc7, 0x84, 0x85, 0x2c, 0xa6, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00}},

    // Rushing Water Limit
    {0x1365c0, {0x00, 0x00, 0xc0, 0x41}, {0x00, 0x00, 0x90, 0x42}},

    // Longer Tiki Village
    {0xceb97, {0x27}, {0x7}},
    {0xce9b3, {0x27}, {0x7}},

    // Spawn Tiki's further
    {0xbe46b, {0x64}, {0x24}},

    // Spawn Crush Traps further
    {0xe15cb, {0xca, 0x11}, {0x4a, 0x06}},

    // Spawn Spikeballs further
    {0xe1ac3, {0xf8, 0x11}, {0x78, 0x06}},

    // CoG tiles overflow this because they only allow 1024 items
    // but this bounds check is for 2048
    {0x8917c, {0x04}, {0x08}},

    // Fix Teleporting below normal borders
    {0x15b21, {0x90, 0x90}, {0x7c, 0x7c}},
    {0x15b26, {0xeb}, {0x7e}},
    {0x349bc, {0xeb}, {0x75}},

    // Vlad's Tower
    {0xca4bb, {0xc}, {0x4}},
    {0xca4a2, {0xb}, {0x3}},
    {0xca4a8, {0x9c, 0xa6}, {0x1c, 0xa6}},

    {0xca8f5, {0x2}, {0x3}},
    {0xca8fa, {0x2}, {0x1}},
};

std::vector<RelativePatch> gBiglunkyRelativePatches = {
    // Move Hell Door
    {0xdeb55, 0x136044, 0x135cdc},
    {0xdec45, 0x136044, 0x135cdc},

    // Move Olmec Exit Door
    {0xde961, 0x136284, 0x1367fc},

    // Udjat Range from 100.0 to 500.0
    {0x530c0, 0x1364e0, 0x135aa4},

    // Alien Queen Range from 144.0 to 1200.0
    {0x437b4, 0x136698, 0x365bc},

    // HC Background from 32.0 -> 96.0
    {0xc9fc3, 0x135d0c, 0x135bdc},
    {0xc9f77, 0x135e98, 0x136050}, // Move BG down to center it

    // // Vlad's Background from 32.0 -> 96.0
    {0xca516, 0x135d0c, 0x135bdc},
    {0xca4c0, 0x135e98, 0x136670}, // Move BG down to center it
};

std::vector<Patch> gFullSpelunkyPatches = {
    // Allow coffins on level 1
    {0xbe12a, {0x0}, {0x1}},
    {0x6ab16, {0x0}, {0x1}},

    // Don't set the flag that you've placed a coffin
    {0xd9abe, {0x0}, {0x1}},

    // Put back stolen bytes
    {0xe8860, {}, {0x8b, 0x83, 0x5c, 0x71, 0x01, 0x00}},

    // Allow coffins in Hell
    {0x6ab5f, {0x14}, {0x11}},
    {0xbe12f, {0x14}, {0x10}},

    // Overwrite coffins in hell gen to use coop coffins
    {0xd5338, {0x49}, {0x2b}},
    {0xd5347, {0x49}, {0x2d}},

    // Assign Coffin on Skin is Crawling
    {0xcaa43,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x39, 0xb9, 0x84, 0x24, 0x01, 0x00, 0x75, 0x10}},

    // Assign Coffin on Rushing Water
    {0xcaf0c,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x39, 0xb9, 0x84, 0x24, 0x01, 0x00, 0x75, 0x10}},

    // Assign Coffin on Tiki Village
    {0xcb3e5,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x39, 0xb9, 0x84, 0x24, 0x01, 0x00, 0x75, 0x10}},

    // Assign Coffin on Wet Fur
    {0xcb5fa,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x39, 0xb9, 0x84, 0x24, 0x01, 0x00, 0x75, 0x10}},

    // Allow Unlock Coffin in Rushing Water
    {0xbe119,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x80, 0xb8, 0xf6, 0x05, 0x44, 0x00, 0x00, 0x75, 0x20}},

    // Allow Unlock Coffin in Tiki Village
    {0xbe110,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x80, 0xb8, 0xf9, 0x05, 0x44, 0x00, 0x00, 0x75, 0x29}},

    // Prevent Old Bitey From Spawning over Coffin
    // Need to have 2 options on the left as the game won't spawn Old Bitey
    // under the exit and will keep rerolling.
    {0xcae14, {0x1}, {0x3}},
    {0xcae75, {0x1}, {0x3}},

    // Force BM to be on second level of Jungle
    {0xbe1cc, {0x6}, {0x5}},
    {0xbe1d5, {0x7}, {0x8}},
    {0xbe1ea, {0x7}, {0x8}},

    // Always Spawn Carl in BM
    {0xcc1f6,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
      0x90, 0x90, 0x90, 0x90, 0x90},
     {0x83, 0xba, 0x84, 0x24, 0x01, 0x00, 0xff, 0x75, 0x6d, 0x39, 0xb0, 0x20,
      0x64, 0x44, 0x00, 0x75, 0x65}},

    // Psychic Presence always on third row
    {0xcb721, {0x0}, {0x1}},
    {0xcb72d, {0x7f, 0x29, 0x0, 0x0}, {0x7b, 0x29, 0x0, 0x0}},

    // Allow Psychic Presence on Moai
    {0xcb6c5,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0xf3, 0x00, 0x00, 0x00}},

    // Moai always on second row
    {0xcb4c2, {0x0}, {0x1}},
    {0xcb539, {0x0}, {0x1}},

};

std::vector<Patch> gSeededModePatches = {
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

std::vector<Patch> gSeededModeDailySeedingPatches = {
    // Remove Roulette Wheel
    {0xdeef8,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x84, 0x78, 0x00, 0x00, 0x00}},
};

std::vector<Patch> gTunnelManPatches = {
    // Put back stolen bytes
    {0x569a5, {}, {0x83, 0xbf, 0x34, 0x01, 0x00, 0x00, 0x1d}},
};

void resetTunnelManState() {
  gGlobalState->player1_data.health = 2;
  gGlobalState->player1_data.health2 = 2;
  gGlobalState->player1_data.bombs = 0;
  gGlobalState->player1_data.ropes = 0;
  if (gGlobalState->player1) {
    gGlobalState->player1->health = 2;
  }
}

void chooseRandomSeed() { gSeededModeState.seed = mersenne_random(); }

void resetFullSpelunkyState() {
  gFullSpelunkyState.allCharacters = {
      CHARACTER_GUY,
      CHARACTER_RED,
      CHARACTER_GREEN,
      CHARACTER_BLUE,
      CHARACTER_MEATBOY,
      CHARACTER_YELLOW,
      CHARACTER_PURPLE,
      CHARACTER_VAN_HELSING,
      CHARACTER_CYAN,
      CHARACTER_LIME,
      CHARACTER_INUK,
      CHARACTER_ROUND_GIRL,
      CHARACTER_NINJA,
      CHARACTER_VIKING,
      CHARACTER_ROUND_BOY,
      CHARACTER_CARL,
      CHARACTER_ROBOT,
      CHARACTER_MONK,

      CHARACTER_JUNGLE_WARRIOR,
      CHARACTER_YANG,
  };

  gFullSpelunkyState.randoms = {
      CHARACTER_GUY,
      CHARACTER_RED,
      CHARACTER_GREEN,
      CHARACTER_BLUE,
      CHARACTER_YELLOW,
      CHARACTER_PURPLE,
      CHARACTER_CYAN,
      CHARACTER_LIME,
      CHARACTER_CARL,
      CHARACTER_ROUND_GIRL,
      CHARACTER_ROUND_BOY,
      CHARACTER_INUK,
      CHARACTER_JUNGLE_WARRIOR,
      CHARACTER_YANG,
  };
}

ForcePatch gDarkLevelForcePatch = {
    {0x6afbe, {0x1}, {0x0}},
    {0x6afae, {0x0}, {0x1}},
};

ForcePatch gSnakePitForcePatch = {
    {0xca167,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x90, 0x0, 0x0, 0x0}},
    {0xca166, {0x0}, {0x1}},
};

ForcePatch gSkinIsCrawlingForcePatch = {
    {0xcaa28,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x00, 0x02, 0x00, 0x00}},
    {0xcaa27, {0x0}, {0x1}},
};

ForcePatch gRushingWaterForcePatch = {
    {0xcad7c,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x84, 0xc1, 0x01, 0x00, 0x00}},
    {0xcad6c, {0x0}, {0x1}},
};

ForcePatch gDeadAreRestlessForcePatch = {
    {0xbdf38,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0xc9, 0x00, 0x00, 0x00}},
    {0xbdf28, {0x0}, {0x1}},
};

ForcePatch gBeesForcePatch = {
    {0xcafb2,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x80, 0x03, 0x00, 0x00}},
    {0xcafb1, {0x0}, {0x1}},
};

ForcePatch gTikiVillageForcePatch = {
    {0xcb3ca,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x90, 0x00, 0x00, 0x00}},
    {0xcb3c9, {0x0}, {0x1}},
};

ForcePatch gWetFurForcePatch = {
    {0xcb59c,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x84, 0xf5, 0x00, 0x00, 0x00}},
    {0xcb58c, {0x0}, {0x1}},
};

ForcePatch gPsychicPresenceForcePatch = {
    {0xcb704,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0xbf, 0x00, 0x00, 0x00}},
    {0xcb703, {0x0}, {0x1}},
};

ForcePatch gIcePoolsForcePatch = {
    {0xcb856,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x27, 0x01, 0x00, 0x00}},
    {0xcb855, {0x0}, {0x1}},
};

ForcePatch gKaliPitForcePatch = {
    {0xca3b8,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x9f, 0x00, 0x00, 0x00}},
    {0xca3b7, {0x0}, {0x1}},
};

void prePlaceRoomsFullSpelunky() {
  // Mines
  if (gGlobalState->level >= 1 && gGlobalState->level <= 4) {
    if (gGlobalState->level == 3) {
      applyForcePatch(gSnakePitForcePatch, FORCE_PATCH_TYPE_ALWAYS);
      applyForcePatch(gSkinIsCrawlingForcePatch, FORCE_PATCH_TYPE_NEVER);
    } else if (gGlobalState->level == 4) {
      applyForcePatch(gSnakePitForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gSkinIsCrawlingForcePatch, FORCE_PATCH_TYPE_ALWAYS);
    } else {
      applyForcePatch(gSnakePitForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gSkinIsCrawlingForcePatch, FORCE_PATCH_TYPE_NEVER);
    }
    // Jungle
  } else if (gGlobalState->level >= 5 && gGlobalState->level <= 8) {
    if (gGlobalState->level == 5) {
      applyForcePatch(gRushingWaterForcePatch, FORCE_PATCH_TYPE_ALWAYS);
      applyForcePatch(gDeadAreRestlessForcePatch, FORCE_PATCH_TYPE_ALWAYS);
      applyForcePatch(gBeesForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gTikiVillageForcePatch, FORCE_PATCH_TYPE_NEVER);
    } else if (gGlobalState->level == 8) {
      applyForcePatch(gRushingWaterForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gDeadAreRestlessForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gBeesForcePatch, FORCE_PATCH_TYPE_ALWAYS);
      applyForcePatch(gTikiVillageForcePatch, FORCE_PATCH_TYPE_ALWAYS);
    } else {
      applyForcePatch(gDeadAreRestlessForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gRushingWaterForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gBeesForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gTikiVillageForcePatch, FORCE_PATCH_TYPE_NEVER);
    }
    // Ice Caves
  } else if (gGlobalState->level >= 9 && gGlobalState->level <= 12) {
    if (gGlobalState->level == 9) {
      applyForcePatch(gWetFurForcePatch, FORCE_PATCH_TYPE_ALWAYS);
      applyForcePatch(gPsychicPresenceForcePatch, FORCE_PATCH_TYPE_NEVER);
    } else if (gGlobalState->level == 11 and
               gGlobalState->mothership_spawned == 0) {
      applyForcePatch(gPsychicPresenceForcePatch, FORCE_PATCH_TYPE_ALWAYS);
      applyForcePatch(gWetFurForcePatch, FORCE_PATCH_TYPE_NEVER);
    } else if (gGlobalState->level == 12 and
               gGlobalState->mothership_spawned == 0) {
      applyForcePatch(gPsychicPresenceForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gWetFurForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gIcePoolsForcePatch, FORCE_PATCH_TYPE_ALWAYS);
    } else if (gGlobalState->level == 12 and
               gGlobalState->mothership_spawned == 1) {
      applyForcePatch(gPsychicPresenceForcePatch, FORCE_PATCH_TYPE_ALWAYS);
      applyForcePatch(gWetFurForcePatch, FORCE_PATCH_TYPE_NEVER);
    } else {
      applyForcePatch(gWetFurForcePatch, FORCE_PATCH_TYPE_NEVER);
      applyForcePatch(gPsychicPresenceForcePatch, FORCE_PATCH_TYPE_NEVER);
    }
    // Temple
  } else if (gGlobalState->level >= 13 && gGlobalState->level <= 15) {
    if (gGlobalState->level == 14) {
      applyForcePatch(gKaliPitForcePatch, FORCE_PATCH_TYPE_ALWAYS);
    } else {
      applyForcePatch(gKaliPitForcePatch, FORCE_PATCH_TYPE_NEVER);
    }
  }

  if (!gModsState.DarkMode) {
    if (gGlobalState->level == 12 and gGlobalState->mothership_spawned == 0) {
      gGlobalState->dark_level = 1;
    } else {
      gGlobalState->dark_level = 0;
    }
  }
}

Entity *postSpawnEntityTunnelMan(Entity *ent) {
  if (gGlobalState && gGlobalState->screen_state == 0 &&
      gGlobalState->play_state == 0) {
    if (ent && ent->entity_type == 109) {
      ent->alpha = 0.0;
      ent->flag_deletion = 1;
      auto new_ent = gGlobalState->SpawnEntity(ent->x, ent->y, 510, true);

      return new_ent;
    }
  }
  return ent;
}

void prePlaceRoomsBiglunky() {
  if (!gModsState.DarkMode && !gModsState.TheFullSpelunky) {
    gGlobalState->dark_level = 0;
  }
}

void postPlaceRoomsBiglunky() {
  if (gModsState.Biglunky) {
    if (gGlobalState->is_city_of_gold) {
      auto num_flag_21 = 0;
      for (auto idx = 0; idx < 4692; idx++) {
        auto ent = gGlobalState->level_state->entity_floors[idx];
        if (ent && ent->flag_21 == 1) {
          if (num_flag_21 > 512) {
            ent->flag_21 = 0;
          }
          num_flag_21++;
        }
      }
    } else if (gGlobalState->rushing_water) {
      for (auto idx = 0; idx < 4692; idx++) {
        auto ent = gGlobalState->level_state->entity_floors[idx];
        if (ent && ent->y == 12 && ent->x >= 3 && ent->x < 43) {
          DestroyFloor(gGlobalState->level_state, ent);
          auto bg = gGlobalState->level_state->entity_floors_bg[idx];
          if (bg) {
            bg->flag_deletion = 1;
          }
        }
      }
    }
  }
}

void postPlaceRoomsUplunky() {

  // auto entrance_x = gGlobalState->level_state->entrance_x;
  // auto entrance_y = gGlobalState->level_state->entrance_y;
  // auto exit_x = gGlobalState->level_state->exit_x;
  // auto exit_y = gGlobalState->level_state->exit_y;

  // auto entrance_room_x = gGlobalState->level_state->entrance_room_x;
  // auto entrance_room_y = gGlobalState->level_state->entrance_room_y;
  // auto exit_room_x = gGlobalState->level_state->exit_room_x;
  // auto exit_room_y = gGlobalState->level_state->exit_room_y;

  // // auto entrance_idx = (int)entrance_y * 42 + (int)entrance_x;
  // // auto exit_idx = (int)exit_y * 42 + (int)exit_x;

  // // auto entrance_door =
  // // gGlobalState->level_state->entity_floors[entrance_idx]; auto exit_door =
  // // gGlobalState->level_state->entity_floors[exit_idx];

  // EntityFloor *entrance_door = nullptr;
  // EntityFloor *exit_door = nullptr;

  // for (auto idx = 0; idx < 4692; idx++) {
  //   auto ent = gGlobalState->level_state->entity_floors[idx];
  //   if (ent && ent->entity_kind == EntityKind::KIND_FLOOR) {
  //     if (ent->entity_type == 0x2) {
  //       entrance_door = ent;
  //     }
  //     if (ent->entity_type == 0x3) {
  //       exit_door = ent;
  //     }
  //   }
  // }

  // auto entrance_entity_idx = entrance_door->entity_index;
  // auto entrance_door_x = entrance_door->x;
  // auto entrance_door_y = entrance_door->y;
  // entrance_door->entity_index = exit_door->entity_index;
  // entrance_door->x = exit_door->x;
  // entrance_door->y = exit_door->y;
  // exit_door->entity_index = entrance_entity_idx;
  // exit_door->x = entrance_door_x;
  // exit_door->y = entrance_door_x;

  // gGlobalState->level_state->entity_floors[exit_door->entity_index] =
  // exit_door;
  // gGlobalState->level_state->entity_floors[entrance_door->entity_index] =
  //     entrance_door;

  // gGlobalState->level_state->entrance_x = exit_x;
  // gGlobalState->level_state->entrance_y = exit_y;
  // gGlobalState->level_state->exit_x = entrance_x;
  // gGlobalState->level_state->exit_y = entrance_y;

  // gGlobalState->level_state->entrance_room_x = exit_room_x;
  // gGlobalState->level_state->entrance_room_y = exit_room_y;
  // gGlobalState->level_state->exit_room_x = entrance_room_x;
  // gGlobalState->level_state->exit_room_y = entrance_room_y;
}

void postPlaceRoomsFullSpelunky() {
  // Mines
  if (gGlobalState->level >= 1 && gGlobalState->level <= 4) {
    applyForcePatch(gSnakePitForcePatch, FORCE_PATCH_TYPE_NORMAL);
    applyForcePatch(gSkinIsCrawlingForcePatch, FORCE_PATCH_TYPE_NORMAL);
  } else if (gGlobalState->level >= 5 && gGlobalState->level <= 8) {
    applyForcePatch(gRushingWaterForcePatch, FORCE_PATCH_TYPE_NORMAL);
    applyForcePatch(gDeadAreRestlessForcePatch, FORCE_PATCH_TYPE_NORMAL);
    applyForcePatch(gBeesForcePatch, FORCE_PATCH_TYPE_NORMAL);
    applyForcePatch(gTikiVillageForcePatch, FORCE_PATCH_TYPE_NORMAL);
  } else if (gGlobalState->level >= 9 && gGlobalState->level <= 12) {
    applyForcePatch(gWetFurForcePatch, FORCE_PATCH_TYPE_NORMAL);
    applyForcePatch(gPsychicPresenceForcePatch, FORCE_PATCH_TYPE_NORMAL);
    applyForcePatch(gIcePoolsForcePatch, FORCE_PATCH_TYPE_NORMAL);
    // Temple
  } else if (gGlobalState->level >= 13 && gGlobalState->level <= 15) {
    applyForcePatch(gKaliPitForcePatch, FORCE_PATCH_TYPE_NORMAL);
  }
}

void preGenerateRoomBiglunky() {
  if (gModsState.Biglunky) {
    if (gGlobalState->is_mothership && std::rand() % 6 == 0) {
      // Set altar spawned to 0 to force an alien lord
      gGlobalState->altar_spawned = 0;
    }
  }
}

void preSpawnTilesBiglunky() {

  if (gModsState.Biglunky) {

    if (gGlobalState->is_blackmarket == 1) {
      gGlobalState->level_state->exit_room_y = 11;
      gGlobalState->level_state->entrance_room_x = 1;

      gGlobalState->level_state->room_types[0] = 1;
      gGlobalState->level_state->room_types[1] = 2;
      gGlobalState->level_state->room_types[2] = 2;
      gGlobalState->level_state->room_types[3] = 1;

      gGlobalState->level_state->room_types[4] = 4;
      gGlobalState->level_state->room_types[5] = 2;
      gGlobalState->level_state->room_types[6] = 2;
      gGlobalState->level_state->room_types[7] = 2;

      gGlobalState->level_state->room_types[8] = 1;
      gGlobalState->level_state->room_types[9] = 3;
      gGlobalState->level_state->room_types[10] = 2;
      gGlobalState->level_state->room_types[11] = 4;

      gGlobalState->level_state->room_types[12] = 4;
      gGlobalState->level_state->room_types[13] = 2;
      gGlobalState->level_state->room_types[14] = 3;
      gGlobalState->level_state->room_types[15] = 1;

      gGlobalState->level_state->room_types[16] = 1;
      gGlobalState->level_state->room_types[17] = 3;
      gGlobalState->level_state->room_types[18] = 2;
      gGlobalState->level_state->room_types[19] = 5;

      gGlobalState->level_state->room_types[20] = 4;
      gGlobalState->level_state->room_types[21] = 2;
      gGlobalState->level_state->room_types[22] = 3;
      gGlobalState->level_state->room_types[23] = 1;

      gGlobalState->level_state->room_types[24] = 1;
      gGlobalState->level_state->room_types[25] = 3;
      gGlobalState->level_state->room_types[26] = 2;
      gGlobalState->level_state->room_types[27] = 4;

      gGlobalState->level_state->room_types[28] = 4;
      gGlobalState->level_state->room_types[29] = 2;
      gGlobalState->level_state->room_types[30] = 3;
      gGlobalState->level_state->room_types[31] = 1;

      gGlobalState->level_state->room_types[32] = 2;
      gGlobalState->level_state->room_types[33] = 3;
      gGlobalState->level_state->room_types[34] = 2;
      gGlobalState->level_state->room_types[35] = 2;

    } else if (gGlobalState->rushing_water == 1) {
      gGlobalState->level_state->exit_room_y =
          gGlobalState->level_state->exit_room_y - 2;

      // Lake Islands
      gGlobalState->level_state->room_types[36] = 9;
      gGlobalState->level_state->room_types[37] = 9;
      gGlobalState->level_state->room_types[38] = 9;
      gGlobalState->level_state->room_types[39] = 9;

      // Middle Lake
      gGlobalState->level_state->room_types[40] = 10;
      gGlobalState->level_state->room_types[41] = 11;
      gGlobalState->level_state->room_types[42] = 11;
      gGlobalState->level_state->room_types[43] = 10;

      // Bottom Lake
      gGlobalState->level_state->room_types[44] = 10;
      gGlobalState->level_state->room_types[45] = 10;
      gGlobalState->level_state->room_types[46] = 10;
      gGlobalState->level_state->room_types[47] = 10;
    } else if (gGlobalState->is_haunted_castle == 1) {

      // Copy normal bottom floors to new bottom

      gGlobalState->level_state->room_types[40] =
          gGlobalState->level_state->room_types[8];
      gGlobalState->level_state->room_types[41] =
          gGlobalState->level_state->room_types[9];
      gGlobalState->level_state->room_types[42] =
          gGlobalState->level_state->room_types[10];
      gGlobalState->level_state->room_types[43] =
          gGlobalState->level_state->room_types[11];
      gGlobalState->level_state->room_types[44] =
          gGlobalState->level_state->room_types[12];
      gGlobalState->level_state->room_types[45] =
          gGlobalState->level_state->room_types[13];
      gGlobalState->level_state->room_types[46] =
          gGlobalState->level_state->room_types[14];
      gGlobalState->level_state->room_types[47] =
          gGlobalState->level_state->room_types[15];

      // Extend Castle
      gGlobalState->level_state->room_types[8] =
          gGlobalState->level_state->room_types[5];
      gGlobalState->level_state->room_types[9] =
          gGlobalState->level_state->room_types[4];
      gGlobalState->level_state->room_types[10] =
          gGlobalState->level_state->room_types[6];
      gGlobalState->level_state->room_types[11] =
          gGlobalState->level_state->room_types[7];

      gGlobalState->level_state->room_types[12] =
          gGlobalState->level_state->room_types[4];
      gGlobalState->level_state->room_types[13] =
          gGlobalState->level_state->room_types[5];
      gGlobalState->level_state->room_types[14] =
          gGlobalState->level_state->room_types[6];
      gGlobalState->level_state->room_types[15] =
          gGlobalState->level_state->room_types[7];

      gGlobalState->level_state->room_types[16] =
          gGlobalState->level_state->room_types[5];
      gGlobalState->level_state->room_types[17] =
          gGlobalState->level_state->room_types[4];
      gGlobalState->level_state->room_types[18] =
          gGlobalState->level_state->room_types[6];
      gGlobalState->level_state->room_types[19] =
          gGlobalState->level_state->room_types[7];

      gGlobalState->level_state->room_types[20] =
          gGlobalState->level_state->room_types[4];
      gGlobalState->level_state->room_types[21] =
          gGlobalState->level_state->room_types[5];
      gGlobalState->level_state->room_types[22] =
          gGlobalState->level_state->room_types[6];
      gGlobalState->level_state->room_types[23] =
          gGlobalState->level_state->room_types[7];

      gGlobalState->level_state->room_types[24] =
          gGlobalState->level_state->room_types[5];
      gGlobalState->level_state->room_types[25] =
          gGlobalState->level_state->room_types[4];
      gGlobalState->level_state->room_types[26] =
          gGlobalState->level_state->room_types[6];
      gGlobalState->level_state->room_types[27] =
          gGlobalState->level_state->room_types[7];

      gGlobalState->level_state->room_types[28] =
          gGlobalState->level_state->room_types[4];
      gGlobalState->level_state->room_types[29] =
          gGlobalState->level_state->room_types[5];
      gGlobalState->level_state->room_types[30] =
          gGlobalState->level_state->room_types[6];
      gGlobalState->level_state->room_types[31] =
          gGlobalState->level_state->room_types[7];

      gGlobalState->level_state->room_types[32] =
          gGlobalState->level_state->room_types[5];
      gGlobalState->level_state->room_types[33] =
          gGlobalState->level_state->room_types[4];
      gGlobalState->level_state->room_types[34] =
          gGlobalState->level_state->room_types[6];
      gGlobalState->level_state->room_types[35] =
          gGlobalState->level_state->room_types[7];

      gGlobalState->level_state->room_types[36] =
          gGlobalState->level_state->room_types[4];
      gGlobalState->level_state->room_types[37] =
          gGlobalState->level_state->room_types[5];
      gGlobalState->level_state->room_types[38] =
          gGlobalState->level_state->room_types[6];
      gGlobalState->level_state->room_types[39] =
          gGlobalState->level_state->room_types[7];

      gGlobalState->level_state->exit_room_y = 11;

    } else if (gGlobalState->level == 16) {
      GenerateRoom(0, gGlobalState->level_state, 3, 45, 36);
      GenerateRoom(0, gGlobalState->level_state, 13, 45, 37);
      GenerateRoom(0, gGlobalState->level_state, 23, 45, 38);
      GenerateRoom(0, gGlobalState->level_state, 33, 45, 39);

      GenerateRoom(0, gGlobalState->level_state, 3, 61, 36);
      GenerateRoom(0, gGlobalState->level_state, 13, 61, 37);
      GenerateRoom(0, gGlobalState->level_state, 23, 61, 38);
      GenerateRoom(0, gGlobalState->level_state, 33, 61, 39);

      GenerateRoom(0, gGlobalState->level_state, 3, 77, 36);
      GenerateRoom(0, gGlobalState->level_state, 13, 77, 37);
      GenerateRoom(0, gGlobalState->level_state, 23, 77, 38);
      GenerateRoom(0, gGlobalState->level_state, 33, 77, 39);
    } else if (gGlobalState->level == 20) {
      for (auto idx = 12; idx < 44; idx++) {
        if (idx % 4 < 2) {
          gGlobalState->level_state->room_types[idx] = 55;
        } else {
          gGlobalState->level_state->room_types[idx] = 58;
        }
      }

      // Bottom Row
      gGlobalState->level_state->room_types[44] = 59;
      gGlobalState->level_state->room_types[45] = 60;
      gGlobalState->level_state->room_types[46] = 61;
      gGlobalState->level_state->room_types[47] = 62;
    }
  }
}

void preSpawnTilesFullSpelunky() {
  if (gModsState.TheFullSpelunky) {
    if (gGlobalState->level == 5) {
      bool found_hc_entrance = false;
      bool placed_hc_entrance = false;
      for (auto idx = 0; idx < 48; idx++) {
        if (gGlobalState->level_state->room_types[idx] == 47) {
          found_hc_entrance = true;
          break;
        }
      }
      if (!found_hc_entrance) {
        for (auto idx = 4; idx < 48; idx++) {
          if (gGlobalState->level_state->room_types[idx] == 0) {
            gGlobalState->level_state->room_types[idx] = 47;
            placed_hc_entrance = true;
            break;
          }
        }

        // If you didn't find an open side room shove it on the path
        if (!placed_hc_entrance) {
          for (auto idx = 4; idx < 48; idx++) {
            auto roomType = gGlobalState->level_state->room_types[idx];
            if (roomType == 2 || roomType == 3) {
              gGlobalState->level_state->room_types[idx] = 47;
              placed_hc_entrance = true;
              break;
            }
          }
        }
      }
    } else if (gGlobalState->level == 9 && gGlobalState->is_wet_fur == 1) {
      // Place coffin in Wet Fur
      for (auto idx = 4; idx < 12; idx++) {
        if (gGlobalState->level_state->room_types[idx] == 0) {
          gGlobalState->level_state->room_types[idx] = 43;
          break;
        }
      }
    } else if (gGlobalState->level == 11 && gGlobalState->is_mothership == 1) {
      // Place coffin in Mothership
      for (auto idx = 4; idx < 12; idx++) {
        if (gGlobalState->level_state->room_types[idx] == 0) {
          gGlobalState->level_state->room_types[idx] = 43;
          break;
        }
      }
    } else if (gGlobalState->is_worm == 1) {
      // Place coffin in Mothership
      for (auto idx = 8; idx < 48; idx++) {
        if (((idx % 4) < 2) &&
            gGlobalState->level_state->room_types[idx] == 1) {
          gGlobalState->level_state->room_types[idx] = 44;
          break;
        }
      }
    } else if (gGlobalState->level == 15 && gGlobalState->is_city_of_gold) {
      bool found_coffin = false;
      for (auto idx = 0; idx < 48; idx++) {
        if (gGlobalState->level_state->room_types[idx] == 44 && idx < 4 &&
            gGlobalState->level_state->entrance_room_x != idx) {
          found_coffin = true;
          break;
        }
      }
      if (!found_coffin) {
        for (auto idx = 4; idx < 48; idx++) {
          if (gGlobalState->level_state->room_types[idx] == 2) {
            gGlobalState->level_state->room_types[idx] = 44;
            break;
          }
        }
      }
      // Hell
    } else if (gGlobalState->level > 16) {
      if (gGlobalState->_34struct->coffin_char > -1) {
        bool found_coffin = false;
        for (auto idx = 0; idx < 48; idx++) {
          auto levelType = gGlobalState->level_state->room_types[idx];
          if (levelType == 73) {
            found_coffin = true;
            break;
          }
        }
        if (!found_coffin) {
          for (auto idx = 4; idx < 48; idx++) {
            if (gGlobalState->level_state->room_types[idx] == 0) {
              gGlobalState->level_state->room_types[idx] = 73;
              break;
            }
          }
        }
      }
    }
  }
}

void unlockCoffinsFullSpelunky() {
  if (gFullSpelunkyState.allCharacters.empty() ||
      gFullSpelunkyState.randoms.empty()) {
    gGlobalState->_34struct->coffin_char = -1;
    return;
  }

  if (gGlobalState->is_haunted_castle != 0) {
    gGlobalState->_34struct->coffin_char = CHARACTER_VAN_HELSING;
  } else if (gGlobalState->is_worm != 0) {
    gGlobalState->_34struct->coffin_char = CHARACTER_MEATBOY;
  } else if (gGlobalState->skin_is_crawling != 0) {
    gGlobalState->_34struct->coffin_char = CHARACTER_ROUND_GIRL;
  } else if (gGlobalState->rushing_water != 0) {
    gGlobalState->_34struct->coffin_char = CHARACTER_VIKING;
  } else if (gGlobalState->tiki_village != 0) {
    gGlobalState->_34struct->coffin_char = CHARACTER_ROUND_BOY;
  } else if (gGlobalState->is_wet_fur != 0) {
    gGlobalState->_34struct->coffin_char = CHARACTER_INUK;
  } else if (gGlobalState->is_mothership != 0) {
    gGlobalState->_34struct->coffin_char = CHARACTER_ROBOT;
  } else if (gGlobalState->is_city_of_gold != 0) {
    gGlobalState->_34struct->coffin_char = CHARACTER_MONK;
  } else if (gGlobalState->level == 16) {
    gGlobalState->_34struct->coffin_char = CHARACTER_NINJA;
  } else {
    gGlobalState->_34struct->coffin_char = gFullSpelunkyState.randoms[0];
  }

  if (gGlobalState->_34struct->coffin_char == CHARACTER_YANG &&
      gFullSpelunkyState.allCharacters.size() > 1) {
    gGlobalState->_34struct->coffin_char = -1;
  }

  // Only allow Jungle Warrior by Temple
  if (gGlobalState->_34struct->coffin_char == CHARACTER_JUNGLE_WARRIOR &&
      gGlobalState->level < 13) {
    gGlobalState->_34struct->coffin_char = -1;
  }

  // Only allow Yang in Hell
  if (gGlobalState->_34struct->coffin_char == CHARACTER_YANG &&
      gGlobalState->level < 17) {
    gGlobalState->_34struct->coffin_char = -1;
  }
}

void onRunningFrame() {
  if (gModsState.TunnelMan && gGlobalState->player1) {
    if (gGlobalState->player1->holding_entity &&
        gGlobalState->player1->holding_entity->entity_type == 510) {

      if (gGlobalState->player1->field27_0x198 < 1) {
        auto mattock = gGlobalState->player1->holding_entity;
        mattock->flag_deletion = 1;
        mattock->holder_entity = NULL;
        gGlobalState->player1->holding_entity = NULL;
        gGlobalState->player1->player_data->held_item_id = 0;
      }
    }
  }

  if (gModsState.TheFullSpelunky) {
    if (gGlobalState->player1) {
      if (gGlobalState->dark_level) {
        if (gGlobalState->player1_data.has_udjat ||
            gGlobalState->player1_data.has_spectacles) {
          gGlobalState->player1->brightness = 20.0;
        } else {
          gGlobalState->player1->brightness = 15.0;
        }
      } else {
        gGlobalState->player1->brightness = 5.0;
      }
    }

    for (int hh_idx = 0; hh_idx < gGlobalState->player1_data.hh_count;
         hh_idx++) {
      auto texture_id = gGlobalState->player1_data.hh_texture_id[hh_idx];
      auto char_id = TextureIdToCharId((TextureId)texture_id);

      std::vector<CharacterIndex>::iterator position;
      position = std::find(gFullSpelunkyState.randoms.begin(),
                           gFullSpelunkyState.randoms.end(), char_id);
      if (position != gFullSpelunkyState.randoms.end()) {
        gFullSpelunkyState.randoms.erase(position);
      }

      position = std::find(gFullSpelunkyState.allCharacters.begin(),
                           gFullSpelunkyState.allCharacters.end(), char_id);
      if (position != gFullSpelunkyState.allCharacters.end()) {
        gFullSpelunkyState.allCharacters.erase(position);
      }
    }

    for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
         idx++) {

      auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

      if (!ent) {
        continue;
      }

      if (ent->entity_kind != EntityKind::KIND_PLAYER) {
        continue;
      }

      EntityPlayer *player = (EntityPlayer *)ent;
      if (!player->ai_bot) {
        continue;
      }

      auto texture_id = player->texture_definition->texture_id;
      auto char_id = TextureIdToCharId((TextureId)texture_id);

      std::vector<CharacterIndex>::iterator position;
      position = std::find(gFullSpelunkyState.randoms.begin(),
                           gFullSpelunkyState.randoms.end(), char_id);
      if (position != gFullSpelunkyState.randoms.end()) {
        gFullSpelunkyState.randoms.erase(position);
      }

      position = std::find(gFullSpelunkyState.allCharacters.begin(),
                           gFullSpelunkyState.allCharacters.end(), char_id);
      if (position != gFullSpelunkyState.allCharacters.end()) {
        gFullSpelunkyState.allCharacters.erase(position);
      }
    }
  }
}

const char *levelItems[] = {
    "1-1", "1-2", "1-3", "1-4", "2-1", "2-2", "2-3", "2-4", "3-1", "3-2",
    "3-3", "3-4", "4-1", "4-2", "4-3", "4-4", "5-1", "5-2", "5-3", "5-4",
};

void drawModsTab() {
  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::Checkbox("Dark Mode", &gModsState.DarkMode)) {
    applyPatches(gDarkModePatches, !gModsState.DarkMode);
  };

  ImGui::Separator();
  if (ImGui::Checkbox("The Full Spelunky", &gModsState.TheFullSpelunky)) {
    if (gModsState.TheFullSpelunky && !hookUnlockCoffinsJmpBackAddr) {
      // Hook Coffin Assignments
      int hookLen = 6;
      DWORD hookAddr = gBaseAddress + 0xe8860;
      hookUnlockCoffinsJmpBackAddr = hookAddr + hookLen;
      hook((void *)hookAddr, hookUnlockCoffins, hookLen);
      applyPatches(gFullSpelunkyPatches);
    }
    if (!gModsState.TheFullSpelunky && hookUnlockCoffinsJmpBackAddr) {
      applyPatches(gFullSpelunkyPatches, true);
      hookUnlockCoffinsJmpBackAddr = NULL;
    }
  };
  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  ImGui::Checkbox("Show Character Overlay##The Full Spelunky",
                  &gFullSpelunkyState.showCharacterOverlay);

  ImGui::Separator();
  if (ImGui::Checkbox("Biglunky", &gModsState.Biglunky)) {
    if (gModsState.Biglunky) {
      applyPatches(gBiglunkyPatches);
      applyRelativePatches(gBiglunkyRelativePatches);
    } else {
      applyPatches(gBiglunkyPatches, true);
      applyRelativePatches(gBiglunkyRelativePatches, true);
    }
  };

  ImGui::Separator();
  if (ImGui::Checkbox("Uplunky (Beta)", &gModsState.Uplunky)) {
    applyPatches(gUplunkyPatches, !gModsState.Uplunky);
    applyRelativePatches(gUplunkyRelativePatches, !gModsState.Uplunky);
    if (gModsState.Uplunky) {
      resetUplunkyState();
    }
  };

  ImGui::Separator();
  if (ImGui::Checkbox("Tunnel Man", &gModsState.TunnelMan)) {
    if (gModsState.TunnelMan) {
      resetTunnelManState();

      if (!hookWhipJmpBackAddr) {

        int hookLen = 7;
        DWORD hookAddr = gBaseAddress + 0x569a5;
        hookWhipJmpBackAddr = hookAddr + hookLen;
        hookWhipSkipWhippingAddr = gBaseAddress + 0x56a80;
        hook((void *)hookAddr, hookWhip, hookLen);
      }
    } else {
      if (hookWhipJmpBackAddr) {
        applyPatches(gTunnelManPatches, true);
        hookWhipJmpBackAddr = NULL;
        hookWhipSkipWhippingAddr = NULL;
      }
    }
  };

  ImGui::Separator();
  if (ImGui::Checkbox("Seeded", &gModsState.SeededMode)) {
    if (gModsState.SeededMode) {
      if (!hookSeedLevelJmpBackAddr) {
        int hookLen = 7;
        DWORD hookAddr = gBaseAddress + 0x6ae01;
        hookSeedLevelJmpBackAddr = hookAddr + hookLen;
        hook((void *)hookAddr, hookSeedLevel, hookLen);
        applyPatches(gSeededModePatches);
        if (gSeededModeState.useDailySeeding) {
          applyPatches(gSeededModeDailySeedingPatches);
        }
      }
    } else {
      if (hookSeedLevelJmpBackAddr) {
        applyPatches(gSeededModePatches, true);
        applyPatches(gSeededModeDailySeedingPatches, true);
        hookSeedLevelJmpBackAddr = NULL;
      }
    }
  }
  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::InputScalar("Seed##SeededMode", ImGuiDataType_U32,
                         &gSeededModeState.seed)) {
    gSeededModeState.seed =
        std::clamp(gSeededModeState.seed, (uint32_t)0, UINT32_MAX);
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::Checkbox("Use Daily Seeding##SeededMode",
                      &gSeededModeState.useDailySeeding)) {
    if (gModsState.SeededMode && gSeededModeState.useDailySeeding) {
      applyPatches(gSeededModeDailySeedingPatches);
    } else {
      applyPatches(gSeededModeDailySeedingPatches, true);
    }
  }

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  ImGui::Checkbox("Random Seed On Restart##SeededMode",
                  &gSeededModeState.randomSeedOnRestart);

  ImGui::Text("");
  ImGui::SameLine(20.0f * io.FontGlobalScale);
  if (ImGui::Button("Add Level Seed##SeededMode")) {
    auto nextLevel = getNextAvailableLevelForSeed();
    if (nextLevel > 0 && nextLevel <= 21) {
      gSeededModeState.levelSeeds.push_back({nextLevel, 1});
    }
  }

  if (gSeededModeState.levelSeeds.size() > 0) {

    if (ImGui::BeginTable("##LevelSeedTable", 3)) {

      ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 1.0f);
      ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthStretch,
                              2.0f);
      ImGui::TableSetupColumn("Seed", ImGuiTableColumnFlags_WidthStretch, 4.0f);
      ImGui::TableHeadersRow();

      auto usedLevels = getUsedLevelsForSeed();

      for (size_t i = 0; i < gSeededModeState.levelSeeds.size(); i++) {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        if (ImGui::Button(std::format("X##RemoveLevelSeed{}", i).c_str())) {
          gSeededModeState.levelSeeds.erase(
              gSeededModeState.levelSeeds.begin() + i);
          continue;
        }

        uint8_t currentLevel = std::get<0>(gSeededModeState.levelSeeds[i]);
        int itemSelectedIdx = currentLevel - 1;
        const char *comboPreviewValue = levelItems[itemSelectedIdx];

        ImGui::TableNextColumn();
        // ImGui::Text("%s", formatLevel(level).c_str());

        if (ImGui::BeginCombo(std::format("##LevelLevelSeed{}", i).c_str(),
                              comboPreviewValue)) {

          for (int n = 0; n < IM_ARRAYSIZE(levelItems); n++) {
            const bool is_selected = (itemSelectedIdx == n);
            auto flags = ImGuiSelectableFlags_None;
            if (!is_selected && usedLevels.contains(n + 1)) {
              flags = ImGuiSelectableFlags_Disabled;
            }
            if (ImGui::Selectable(levelItems[n], is_selected, flags)) {
              std::get<0>(gSeededModeState.levelSeeds[i]) = n + 1;
            }
            if (is_selected)
              ImGui::SetItemDefaultFocus();
          }
          ImGui::EndCombo();
        }

        ImGui::TableNextColumn();
        uint32_t &levelSeed = std::get<1>(gSeededModeState.levelSeeds[i]);
        if (ImGui::InputScalar(std::format("##SeedLevelSeed{}", i).c_str(),
                               ImGuiDataType_U32, &levelSeed)) {
          levelSeed = std::clamp(levelSeed, (uint32_t)0, UINT32_MAX);
        }
      }

      ImGui::EndTable();
    }
  }
}

void drawToggleEntityTab(const char *preText, EnabledEntities &enabledEnts) {
  ImGui::Checkbox(std::format("{} Active Entities", preText).c_str(),
                  &enabledEnts.activeEntities);
  ImGui::Checkbox(std::format("{} Light Emitting", preText).c_str(),
                  &enabledEnts.lightEmittingEntities);
  ImGui::Checkbox(
      std::format("{} Unknown 1400 (BG + Active?)", preText).c_str(),
      &enabledEnts.unknown1400);
  ImGui::Checkbox(std::format("{} Foreground Entities", preText).c_str(),
                  &enabledEnts.foregroundEntities);
  ImGui::Checkbox(std::format("{} Floor Entities", preText).c_str(),
                  &enabledEnts.floorEntities);
  ImGui::Checkbox(std::format("{} Floor Background Entities", preText).c_str(),
                  &enabledEnts.floorBgEntities);
  ImGui::Checkbox(std::format("{} Background Entities", preText).c_str(),
                  &enabledEnts.backgroundEntities);
  ImGui::Checkbox(std::format("{} 4c Struct Entities", preText).c_str(),
                  &enabledEnts._4cStructEntities);

  ImGui::InputInt(std::format("Exclude Entity {}", preText).c_str(),
                  &enabledEnts.excludeEntityInput);
  if (ImGui::Button(std::format("Exclude##Debug{}", preText).c_str())) {
    if (enabledEnts.excludeEntityInput >= 0) {
      enabledEnts.excluded.insert(enabledEnts.excludeEntityInput);
      enabledEnts.excludeEntityInput = 0;
    }
  }
  ImGui::Separator();
  for (auto ent_type : enabledEnts.excluded) {
    auto label = std::format("Remove {}##Debug{}", ent_type, preText);
    if (ImGui::Button(label.c_str())) {
      enabledEnts.excluded.erase(ent_type);
    }
  }
}

void drawRawBytesTable(const char *str_id, char *start_addr, size_t size) {
  if (ImGui::BeginTable(str_id, 6)) {

    ImGui::TableSetupColumn("Offset");
    ImGui::TableSetupColumn("Bytes");
    ImGui::TableSetupColumn("Signed");
    ImGui::TableSetupColumn("Unsigned");
    ImGui::TableSetupColumn("Hex");
    ImGui::TableSetupColumn("Float");
    ImGui::TableHeadersRow();

    for (size_t i = 0; i < size; i += 4) {

      ImGui::TableNextRow();

      char *addr = start_addr + i;
      ImGui::TableNextColumn();
      ImGui::Text("0x%X", i);
      {
        uint32_t a1, a2, a3, a4;
        a1 = (*(addr)) & (0xFF);
        a2 = (*(addr + 1)) & (0xFF);
        a3 = (*(addr + 2)) & (0xFF);
        a4 = (*(addr + 3)) & (0xFF);
        ImGui::TableNextColumn();
        ImGui::Text("%02X %02X %02X %02X", a1, a2, a3, a4);
      }
      ImGui::TableNextColumn();
      ImGui::Text("%d", *(int32_t *)addr);

      ImGui::TableNextColumn();
      ImGui::Text("%u", *(uint32_t *)addr);

      ImGui::TableNextColumn();
      ImGui::Text("0x%08X", *(uint32_t *)addr);

      ImGui::TableNextColumn();
      ImGui::Text("%f", *(float *)addr);
    }

    ImGui::EndTable();
  }
}

void drawRawBytesTableForSelected(const char *str_id, char *start_addr,
                                  size_t size) {
  if (ImGui::BeginTable(str_id, 6, ImGuiTableFlags_RowBg)) {

    ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Bytes", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Signed", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Unsigned", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Hex", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Float", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    for (size_t i = 0; i < size; i += 4) {

      ImGui::TableNextRow();

      char *addr = start_addr + i;
      ImGui::TableNextColumn();
      ImGui::Text("0x%X", i);

      std::pair<EntityKind, uint32_t> key = {
          gSelectedEntityState.Entity->entity_kind,
          gSelectedEntityState.Entity->entity_type};

      {
        uint32_t a1, a2, a3, a4;
        a1 = (*(addr)) & (0xFF);
        a2 = (*(addr + 1)) & (0xFF);
        a3 = (*(addr + 2)) & (0xFF);
        a4 = (*(addr + 3)) & (0xFF);

        ImGui::TableNextColumn();
        if (ImGui::Button(
                std::format("{:02X}##SelectedEntityRaw-{}", a1, i).c_str())) {
          gDebugState.DrawEntityOffsets[key].insert({i, DataType_Byte});
        }
        ImGui::SameLine();

        if (ImGui::Button(std::format("{:02X}##SelectedEntityRaw-{}", a2, i + 1)
                              .c_str())) {
          gDebugState.DrawEntityOffsets[key].insert({i + 1, DataType_Byte});
        }
        ImGui::SameLine();

        if (ImGui::Button(std::format("{:02X}##SelectedEntityRaw-{}", a3, i + 2)
                              .c_str())) {
          gDebugState.DrawEntityOffsets[key].insert({i + 2, DataType_Byte});
        }
        ImGui::SameLine();

        if (ImGui::Button(std::format("{:02X}##SelectedEntityRaw-{}", a4, i + 3)
                              .c_str())) {
          gDebugState.DrawEntityOffsets[key].insert({i + 3, DataType_Byte});
        }
      }

      ImGui::TableNextColumn();
      if (ImGui::Button(
              std::format("{:d}##SelectedEntityRaw-{}", *(int32_t *)addr, i)
                  .c_str(),
              {-1, 0})) {
        gDebugState.DrawEntityOffsets[key].insert({i, DataType_Dword_Signed});
      }

      ImGui::TableNextColumn();
      if (ImGui::Button(
              std::format("{:d}##SelectedEntityRaw-{}", *(uint32_t *)addr, i)
                  .c_str(),
              {-1, 0})) {
        gDebugState.DrawEntityOffsets[key].insert({i, DataType_Dword_Unsigned});
      }

      ImGui::TableNextColumn();
      if (ImGui::Button(std::format("0x{:08X}##SelectedEntityRaw-{}",
                                    *(uint32_t *)addr, i)
                            .c_str(),
                        {-1, 0})) {
        gDebugState.DrawEntityOffsets[key].insert({i, DataType_Dword_Hex});
      }

      ImGui::TableNextColumn();
      if (ImGui::Button(
              std::format("{:f}##SelectedEntityRaw-{}", *(float *)addr, i)
                  .c_str(),
              {-1, 0})) {
        gDebugState.DrawEntityOffsets[key].insert({i, DataType_Float});
      }
    }

    ImGui::EndTable();
  }
}

void drawDebugTab() {
  ImGuiIO &io = ImGui::GetIO();

  auto gameMouse = screenToGame(io.MousePos);
  ImGui::Text("Mouse (Screen): %f %f", io.MousePos.x, io.MousePos.y);
  ImGui::Text("Mouse (Game): %f %f", gameMouse.x, gameMouse.y);
  if (gGlobalState->player1) {
    auto screenPlayer =
        gameToScreen({gGlobalState->player1->x, gGlobalState->player1->y});
    ImGui::Text("Player (Screen): %f %f", screenPlayer.x, screenPlayer.y);
    ImGui::Text("Player (Game): %f %f", gGlobalState->player1->x,
                gGlobalState->player1->y);
  }
  ImGui::Checkbox("Draw Tile Borders", &gDebugState.EnableTileBorders);
  ImGui::Checkbox("Draw Bin Borders", &gDebugState.EnableBinBorders);
  ImGui::Checkbox("Draw Room Borders", &gDebugState.EnableRoomBorders);
  ImGui::Checkbox("Draw Owned Entities", &gDebugState.EnablePacifistOverlay);

  ImGui::Checkbox("Draw Detection Boxes", &gDebugState.DrawEnemyDetection);
  if (ImGui::Checkbox("Black Market Trainer",
                      &gDebugState.BlackMarketTrainer)) {
    if (!hookEligibleBMsJmpBackAddr) {
      // Hook Eligible BM Floors
      int hookLen = 7;
      DWORD hookAddr = gBaseAddress + 0xbe35e;
      hookEligibleBMsJmpBackAddr = hookAddr + hookLen;
      hook((void *)hookAddr, hookEligibleBMs, hookLen);
    }
  };
  ImGui::Checkbox("Include Hitbox Origins", &gDebugState.IncludeHitboxOrigins);
  ImGui::Checkbox("Include Floor Decorations", &gDebugState.IncludeFloorDecos);

  ImGui::Separator();
  ImGui::Checkbox("Draw HH Follower Link", &gDebugState.DrawHHFollowerLink);
  ImGui::Checkbox("Draw HH Following Link", &gDebugState.DrawHHFollowingLink);

  ImGui::Separator();
  if (ImGui::Checkbox("Disable Olmec Spawns",
                      &gDebugState.DisableOlmecSpawns)) {
    auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                   PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                   PROCESS_CREATE_THREAD,
                               0, GetCurrentProcessId());
    if (gDebugState.DisableOlmecSpawns) {
      BYTE patch[] = {0x83, 0xfa, 0x08};
      patchReadOnlyCode(process, gBaseAddress + 0x3121c, patch, 3);
    } else {
      BYTE patch[] = {0x83, 0xfa, 0x01};
      patchReadOnlyCode(process, gBaseAddress + 0x3121c, patch, 3);
    }
    CloseHandle(process);
  }
  if (ImGui::Checkbox("Disable Olmec Gaps", &gDebugState.DisableOlmecGaps)) {
    auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                   PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                   PROCESS_CREATE_THREAD,
                               0, GetCurrentProcessId());
    if (gDebugState.DisableOlmecGaps) {
      BYTE patch[] = {0xb9, 0x01, 0x00, 0x00, 0x00};
      patchReadOnlyCode(process, gBaseAddress + 0x0d5b71, patch, 5);
    } else {
      BYTE patch[] = {0xb9, 0x06, 0x00, 0x00, 0x00};
      patchReadOnlyCode(process, gBaseAddress + 0x0d5b71, patch, 5);
    }
    CloseHandle(process);
  }
  if (ImGui::Checkbox("Disable Olmec Cutscene",
                      &gDebugState.DisableOlmecCutscene)) {
    auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                   PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                   PROCESS_CREATE_THREAD,
                               0, GetCurrentProcessId());
    if (gDebugState.DisableOlmecCutscene) {

      BYTE patch[] = {0x83, 0xb9, 0xd4, 0x05, 0x44, 0x00, 0x99};
      patchReadOnlyCode(process, gBaseAddress + 0x0be66d, patch, 7);
    } else {

      BYTE patch[] = {0x83, 0xb9, 0xd4, 0x05, 0x44, 0x00, 0x10};
      patchReadOnlyCode(process, gBaseAddress + 0x0be66d, patch, 7);
    }
    CloseHandle(process);
  }
  ImGui::Checkbox("Show Olmec Crush Probes", &gDebugState.ShowOlmecCrushProbes);
  ImGui::Separator();

  ImGui::Checkbox("Draw Seeded Crate Contents",
                  &gDebugState.EnableSeededCrateOverlay);
  ImGui::Checkbox("Draw Seeded Pot Contents",
                  &gDebugState.EnableSeededPotOverlay);
  ImGui::Checkbox("Draw Seeded Kali Rewards",
                  &gDebugState.EnableSeededKaliRewards);
  ImGui::Separator();

  // if (ImGui::Button("Copy to Clipboard")) {
  //   ImGui::LogToClipboard();
  //   gGlobalState->insertion_point = 0.000;
  //   for (size_t i = 2200000; i > 2190000; i = i - 10) {
  //     auto insertion = gGlobalState->insertion_point;

  //     auto ent = gGlobalState->SpawnEntity(0.0, 0.0, 113, 0);
  //     auto result = getPotItemForSeed(ent->z_depth_as_int);
  //     auto name = "";
  //     if (result > 0) {
  //       name = EntityTypeName(result);
  //     }
  //     auto result2 = getPotItemForSeed(ent->z_depth_as_int, false);
  //     auto name2 = "";
  //     if (result2 > 0) {
  //       name2 = EntityTypeName(result2);
  //     }
  //     ent->flag_deletion = 1;

  //     ImGui::LogText("%d, %d, %06f, %s, %s\n", i, ent->z_depth_as_int,
  //                    insertion, name, name2);
  //   }
  //   ImGui::LogFinish();
  // }

  if (ImGui::CollapsingHeader("Draw Hitboxes")) {
    drawToggleEntityTab("Show", gDebugState.Hitboxes);
  }
  if (ImGui::CollapsingHeader("Draw Ids")) {
    drawToggleEntityTab("Draw", gDebugState.Ids);
  }
  if (ImGui::CollapsingHeader("Selectable Entities")) {
    ImGui::Checkbox("Draw Selected Entity Hitbox",
                    &gDebugState.DrawSelectedEntHitbox);
    ImGui::Checkbox("Draw Closest Entity Hitbox",
                    &gDebugState.DrawClosestEntHitbox);
    ImGui::Checkbox("Draw Closest Entity Id", &gDebugState.DrawClosestEntId);
    ImGui::Separator();
    drawToggleEntityTab("Enable", gDebugState.Selection);
  }

  if (ImGui::CollapsingHeader("Global State")) {

    ImGui::InputScalar("screen_state", ImGuiDataType_U32,
                       &gGlobalState->screen_state);
    ImGui::InputScalar("play_state", ImGuiDataType_U32,
                       &gGlobalState->play_state);
    ImGui::InputScalar("flag_player", ImGuiDataType_U32,
                       &gGlobalState->flag_player);
    ImGui::InputScalar("level", ImGuiDataType_U32, &gGlobalState->level);
    ImGui::InputScalar("level_track", ImGuiDataType_U32,
                       &gGlobalState->level_track);
    ImGui::InputFloat("insertion_point", &gGlobalState->insertion_point,
                      0.0001F, 0.0F, "%.4f");
    ImGui::Separator();
    ImGui::PushItemWidth(60.f);
    ImGui::InputScalar("Wanted Level", ImGuiDataType_S32,
                       &gGlobalState->wanted_level);
    ImGui::InputScalar("Number of Angry Shopkeepers", ImGuiDataType_S32,
                       &gGlobalState->entities->angered_shopkeeper_count);
    ImGui::PopItemWidth();
    drawCharBool("Shopkeeper Triggered", gGlobalState->shopkeeper_triggered);
    drawCharBool("Shopkeeper Music Triggered",
                 gGlobalState->shopkeeper_music_triggered);

    if (ImGui::CollapsingHeader("GlobalState Flags")) {
      drawCharBool("dark_level", gGlobalState->dark_level);
      drawCharBool("altar_spawned", gGlobalState->altar_spawned);
      drawCharBool("idol_spawned", gGlobalState->idol_spawned);
      drawCharBool("damsel_spawned", gGlobalState->damsel_spawned);
      drawCharBool("unknown_flag", gGlobalState->unknown_flag);
      drawCharBool("moai_unopened", gGlobalState->moai_unopened);
      drawCharBool("moai_broke_in", gGlobalState->moai_broke_in);
      drawCharBool("ghost_spawned", gGlobalState->ghost_spawned);
      drawCharBool("rescued_damsel", gGlobalState->rescued_damsel);
      drawCharBool("shopkeeper_triggered", gGlobalState->shopkeeper_triggered);
      drawCharBool("area_had_dark_level", gGlobalState->area_had_dark_level);
      drawCharBool("level_has_udjat", gGlobalState->level_has_udjat);
      drawCharBool("has_spawned_udjat", gGlobalState->has_spawned_udjat);
      drawCharBool("unused_flag", gGlobalState->unused_flag);
      drawCharBool("vault_spawned_in_area",
                   gGlobalState->vault_spawned_in_area);
      drawCharBool("flooded_mines", gGlobalState->flooded_mines);
      drawCharBool("skin_is_crawling", gGlobalState->skin_is_crawling);
      drawCharBool("dead_are_restless", gGlobalState->dead_are_restless);
      drawCharBool("rushing_water", gGlobalState->rushing_water);
      drawCharBool("is_haunted_castle", gGlobalState->is_haunted_castle);
      drawCharBool("at_haunted_castle_exit",
                   gGlobalState->at_haunted_castle_exit);
      drawCharBool("tiki_village", gGlobalState->tiki_village);
      drawCharBool("spawned_black_market_entrance",
                   gGlobalState->spawned_black_market_entrance);
      drawCharBool("unused_flag2", gGlobalState->unused_flag2);
      drawCharBool("spawned_haunted_castle_entrance",
                   gGlobalState->spawned_haunted_castle_entrance);
      drawCharBool("mothership_spawned", gGlobalState->mothership_spawned);
      drawCharBool("moai_spawned", gGlobalState->moai_spawned);
      drawCharBool("is_blackmarket", gGlobalState->is_blackmarket);
      drawCharBool("at_blackmarket_exit", gGlobalState->at_blackmarket_exit);
      drawCharBool("is_wet_fur", gGlobalState->is_wet_fur);
      drawCharBool("is_mothership", gGlobalState->is_mothership);
      drawCharBool("at_mothership_exit", gGlobalState->at_mothership_exit);
      drawCharBool("is_city_of_gold", gGlobalState->is_city_of_gold);
      drawCharBool("at_city_of_gold_exit", gGlobalState->at_city_of_gold_exit);
      drawCharBool("is_worm", gGlobalState->is_worm);
    }
  }

  if (ImGui::CollapsingHeader("Level State")) {

    ImGui::InputScalar("shop_type", ImGuiDataType_S32,
                       &gGlobalState->level_state->shop_type);
    ImGui::InputScalar("entrance_room_x", ImGuiDataType_S32,
                       &gGlobalState->level_state->entrance_room_x);
    ImGui::InputScalar("entrance_room_y", ImGuiDataType_S32,
                       &gGlobalState->level_state->entrance_room_y);
    ImGui::InputScalar("exit_room_x", ImGuiDataType_S32,
                       &gGlobalState->level_state->exit_room_x);
    ImGui::InputScalar("exit_room_y", ImGuiDataType_S32,
                       &gGlobalState->level_state->exit_room_y);
    ImGui::InputScalar("entrance_x", ImGuiDataType_Float,
                       &gGlobalState->level_state->entrance_x);
    ImGui::InputScalar("entrance_y", ImGuiDataType_Float,
                       &gGlobalState->level_state->entrance_y);
    ImGui::InputScalar("exit_x", ImGuiDataType_Float,
                       &gGlobalState->level_state->exit_x);
    ImGui::InputScalar("exit_y", ImGuiDataType_Float,
                       &gGlobalState->level_state->exit_y);
    ImGui::InputScalar("alt_exit_x", ImGuiDataType_Float,
                       &gGlobalState->level_state->alt_exit_x);
    ImGui::InputScalar("alt_exit_y", ImGuiDataType_Float,
                       &gGlobalState->level_state->alt_exit_y);
  }

  if (ImGui::CollapsingHeader("Camera State")) {

    ImGui::InputScalar("camera_x", ImGuiDataType_Float,
                       &gCameraState->camera_x);
    ImGui::InputScalar("camera_y", ImGuiDataType_Float,
                       &gCameraState->camera_y);
    ImGui::InputScalar("following_x", ImGuiDataType_Float,
                       &gCameraState->following_x);
    ImGui::InputScalar("following_y", ImGuiDataType_Float,
                       &gCameraState->following_y);
    ImGui::InputScalar("camera_max_left", ImGuiDataType_Float,
                       &gCameraState->camera_max_left);
    ImGui::InputScalar("camera_max_right", ImGuiDataType_Float,
                       &gCameraState->camera_max_right);
    ImGui::InputScalar("camera_max_up", ImGuiDataType_Float,
                       &gCameraState->camera_max_up);
    ImGui::InputScalar("camera_max_down", ImGuiDataType_Float,
                       &gCameraState->camera_max_down);
    ImGui::InputScalar("camera_speed", ImGuiDataType_Float,
                       &gCameraState->camera_speed);

    if (ImGui::CollapsingHeader("Camera Raw")) {
      drawRawBytesTable("Raw Bytes##CameraState", (char *)gCameraState, 92);
    }
  }

  if (ImGui::CollapsingHeader("Render Dupe")) {
    ImGui::InputFloat("insertion_point", &gGlobalState->insertion_point,
                      0.0001F, 0.0F, "%.4f");
    ImGui::Text("Filter:");
    ImGui::InputFloat("  Minimum Z", &gDebugState.MinZCutoff, 0.0001F, 0.0F,
                      "%.4f");
    ImGui::InputFloat("  Maximum Z", &gDebugState.MaxZCutoff, 0.0001F, 0.0F,
                      "%.4f");
    // TODO: Min/Max Z Filters
    if (ImGui::BeginTable("Entities##Render Dupe", 4, ImGuiTableFlags_RowBg)) {

      ImGui::TableSetupColumn("Kind", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("Current Z", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("Deleted", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
           idx++) {
        auto ent = gGlobalState->entities->entities_active[idx];
        if (!ent) {
          continue;
        }

        if (ent->current_z < gDebugState.MinZCutoff) {
          continue;
        }

        if (ent->current_z > gDebugState.MaxZCutoff) {
          continue;
        }

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("%s", ent->KindName());

        ImGui::TableNextColumn();
        ImGui::Text("%04d: %s", ent->entity_type, ent->TypeName());

        ImGui::TableNextColumn();
        ImGui::Text("%.4f", ent->current_z);

        ImGui::TableNextColumn();
        ImGui::Text("%d", ent->flag_deletion);
      }
      ImGui::EndTable();
    }
  }

  if (ImGui::CollapsingHeader("Audio")) {
    // auto player = *(&((*gGlobalState).player1) +
    // gGlobalState->flag_player);
    auto player = gGlobalState->player1;
    if (!player) {
      ImGui::Text("Need Player Entity");
      return;
    }

    if (ImGui::BeginListBox("##", {-1, -1})) {
      for (size_t idx = 0; idx < gAudioNames.size(); idx++) {
        if (ImGui::Selectable(gAudioNames[idx], false)) {
          player->PlaySound(gAudioNames[idx]);
        }
      }
      ImGui::EndListBox();
    }
  }
}

size_t sizeofEntityKind(EntityKind entityKind) {
  switch (entityKind) {
  case EntityKind::KIND_FLOOR:
    return sizeof(EntityFloor);
  case EntityKind::KIND_ACTIVE:
    return sizeof(EntityActive);
  case EntityKind::KIND_PLAYER:
    return sizeof(EntityPlayer);
  case EntityKind::KIND_MONSTER:
    return sizeof(EntityMonster);
  case EntityKind::KIND_ITEM:
    return sizeof(EntityItem);
  case EntityKind::KIND_BACKGROUND:
    return sizeof(EntityBackground);
  case EntityKind::KIND_EXPLOSION:
    return sizeof(EntityExplosion);
  case EntityKind::KIND_ENTITY:
    return sizeof(Entity);
  default:
    return 0;
  }
}

void drawSelectedEntityTab() {
  ImGuiIO &io = ImGui::GetIO();
  if (!gSelectedEntityState.Entity) {
    ImGui::Text("No selected entity");
    return;
  }
  ImGui::Text("Address: 0x%X", (uint32_t)gSelectedEntityState.Entity);
  ImGui::InputInt("Entity ID",
                  (int *)&gSelectedEntityState.Entity->entity_type);
  ImGui::InputInt("Entity kind",
                  (int *)&gSelectedEntityState.Entity->entity_kind);

  if (ImGui::Button(std::format("Follow With Camera##FollowEnt-{}",
                                (uint32_t)gSelectedEntityState.Entity)
                        .c_str())) {
    gCameraState->camera_following = gSelectedEntityState.Entity;
  }

  if (gCameraState->camera_following == gSelectedEntityState.Entity) {
    ImGui::SameLine();
    if (ImGui::Button("Stop Following##FollowEnt")) {
      gCameraState->camera_following = nullptr;
    }
  }

  if (ImGui::CollapsingHeader("Position, hitbox, etc.")) {
    ImGui::InputFloat("Entity x", &gSelectedEntityState.Entity->x, 0.0F, 0.0F,
                      "%.6f");
    ImGui::InputFloat("Entity y", &gSelectedEntityState.Entity->y, 0.0F, 0.0F,
                      "%.6f");
    ImGui::SliderFloat("width", &gSelectedEntityState.Entity->width, 0.0, 10.0);
    ImGui::SliderFloat("height", &gSelectedEntityState.Entity->height, 0.0,
                       10.0);
    ImGui::SliderFloat("current_z", &gSelectedEntityState.Entity->current_z,
                       0.0, 50.0, "%.4f");
    ImGui::SliderFloat("original_z", &gSelectedEntityState.Entity->original_z,
                       0.0, 50.0, "%.4f");
    ImGui::SliderFloat("alpha", &gSelectedEntityState.Entity->alpha, 0.0, 1.0);
    ImGui::SliderFloat("hitbox up", &gSelectedEntityState.Entity->hitbox_up,
                       0.0, 5.0);
    ImGui::SliderFloat("hitbox down", &gSelectedEntityState.Entity->hitbox_down,
                       0.0, 5.0);
    ImGui::SliderFloat("hitbox x", &gSelectedEntityState.Entity->hitbox_x, 0.0,
                       5.0);
    ImGui::SliderAngle("angle", &gSelectedEntityState.Entity->angle);
  }
  if (ImGui::CollapsingHeader("Flags1")) {
    drawCharBool("flag_deletion", gSelectedEntityState.Entity->flag_deletion);
    drawCharBool("flag_horizontal_flip",
                 gSelectedEntityState.Entity->flag_horizontal_flip);
    drawCharBool("flag_3", gSelectedEntityState.Entity->flag_3);
    drawCharBool("flag_4", gSelectedEntityState.Entity->flag_4);
    drawCharBool("flag_5", gSelectedEntityState.Entity->flag_5);
    drawCharBool("flag_6", gSelectedEntityState.Entity->flag_6);
    drawCharBool("flag_7", gSelectedEntityState.Entity->flag_7);
    drawCharBool("flag_8", gSelectedEntityState.Entity->flag_8);
    drawCharBool("flag_9", gSelectedEntityState.Entity->flag_9);
    drawCharBool("flag_10", gSelectedEntityState.Entity->flag_10);
    drawCharBool("flag_11", gSelectedEntityState.Entity->flag_11);
    drawCharBool("flag_12", gSelectedEntityState.Entity->flag_12);
    drawCharBool("flag_13", gSelectedEntityState.Entity->flag_13);
    drawCharBool("flag_14", gSelectedEntityState.Entity->flag_14);
    drawCharBool("flag_15", gSelectedEntityState.Entity->flag_15);
    drawCharBool("flag_16", gSelectedEntityState.Entity->flag_16);
    drawCharBool("flag_17", gSelectedEntityState.Entity->flag_17);
    drawCharBool("flag_18", gSelectedEntityState.Entity->flag_18);
    drawCharBool("flag_19", gSelectedEntityState.Entity->flag_19);
    drawCharBool("flag_20", gSelectedEntityState.Entity->flag_20);
    drawCharBool("flag_21", gSelectedEntityState.Entity->flag_21);
    drawCharBool("flag_22", gSelectedEntityState.Entity->flag_22);
    drawCharBool("flag_23", gSelectedEntityState.Entity->flag_23);
    drawCharBool("flag_24", gSelectedEntityState.Entity->flag_24);
  }
  if (gSelectedEntityState.Entity->entity_kind == EntityKind::KIND_PLAYER &&
      ImGui::CollapsingHeader("EntityPlayer")) {
    auto entityPlayer =
        reinterpret_cast<EntityPlayer *>(gSelectedEntityState.Entity);
    ImGui::InputInt("Following", (int *)&entityPlayer->following);
    ImGui::InputInt("Follower", (int *)&entityPlayer->follower);
  }
  if ((uint32_t)gSelectedEntityState.Entity->entity_kind > 0 &&
      (uint32_t)gSelectedEntityState.Entity->entity_kind < 5 &&
      ImGui::CollapsingHeader("EntityActive")) {
    auto entityActive =
        reinterpret_cast<EntityActive *>(gSelectedEntityState.Entity);
    ImGui::InputInt("Health", &entityActive->health);
    ImGui::InputInt("Favor given", &entityActive->favor_given);
    ImGui::InputFloat("Velocity x", &entityActive->velocity_x);
    ImGui::InputFloat("Velocity y", &entityActive->velocity_y);
    if (ImGui::CollapsingHeader("Flags2")) {
      for (size_t i = 0x1f0; i <= 0x218; ++i) {
        char *addr = ((char *)gSelectedEntityState.Entity) + i;
        drawCharBool(std::format("Flag {:X}", i).c_str(), *addr);
      }
    }
  }
  if ((uint32_t)gSelectedEntityState.Entity->entity_kind == 0 &&
      ImGui::CollapsingHeader("EntityFloor")) {
    auto entityFloor =
        reinterpret_cast<EntityFloor *>(gSelectedEntityState.Entity);

    drawCharBool("Make Door onDestroy", entityFloor->field8_0x143);
  }

  if (ImGui::CollapsingHeader("Raw Entity Values")) {
    if (ImGui::Button("Copy to Clipboard")) {
      ImGui::LogToClipboard();
      for (size_t i = 0;
           i < sizeofEntityKind(gSelectedEntityState.Entity->entity_kind);
           i += 4) {
        char *addr = ((char *)gSelectedEntityState.Entity) + i;
        ImGui::LogText("0x%02X: %f 0x%02X\n", i, *(float *)addr,
                       *(uint32_t *)addr);
      }
      ImGui::LogFinish();
    }

    drawRawBytesTableForSelected(
        "Entity Raw Bytes", ((char *)gSelectedEntityState.Entity),
        sizeofEntityKind(gSelectedEntityState.Entity->entity_kind));
  }
  if (gSelectedEntityState.Entity->flag_deletion == 1) {
    gSelectedEntityState.Entity = NULL;
  }
}

void drawSettingsTab() {

  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::CollapsingHeader("Mouse Controls")) {

    if (ImGui::BeginTable("Mouse Controls", 2)) {

      ImGui::TableSetupColumn("Function");
      ImGui::TableSetupColumn("Input");

      ImGui::TableHeadersRow();

      for (size_t idx = 0; idx < Specs::MouseFeatures_COUNT; idx++) {

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("%s",
                    Specs::GetMouseFeatureName((Specs::MouseFeatures_)idx));

        auto mouseConfig = gConfig->buttons[(Specs::MouseFeatures_)idx];
        ImGui::TableNextColumn();
        auto prettyName = mouseConfig.dbg();
        ImGui::Text("%s", prettyName.c_str());
      }

      ImGui::EndTable();
    }
  }
  if (ImGui::CollapsingHeader("Key Controls")) {

    if (ImGui::BeginTable("Key Controls", 2)) {

      ImGui::TableSetupColumn("Function");
      ImGui::TableSetupColumn("Input");

      ImGui::TableHeadersRow();

      for (size_t idx = 0; idx < Specs::KeyFeatures_COUNT; idx++) {

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("%s", Specs::GetKeyFeatureName((Specs::KeyFeatures_)idx));

        auto keyConfig = gConfig->keys[(Specs::KeyFeatures_)idx];
        ImGui::TableNextColumn();
        auto prettyName = keyConfig.dbg();
        ImGui::Text("%s", prettyName.c_str());
      }

      ImGui::EndTable();
    }
  }

  ImGui::DragFloat("Scale##StyleScale", &io.FontGlobalScale, 0.01f, 0.2f, 2.0f);
}

void drawToolWindow() {
  if (!ui::open) {
    return;
  }
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2{0.f, 0.f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2{400.f, 500.f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Specs HD");

  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
    io.MouseDrawCursor = true;
    io.WantCaptureMouse = true;
  } else {
    io.MouseDrawCursor = false;
    io.WantCaptureMouse = false;
  }

  auto mouse_game = screenToGame(io.MousePos);

  if (ImGui::BeginTabBar("Specs HD")) {
    if (ImGui::BeginTabItem("Spawn")) {
      drawSpawnTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Level")) {
      drawLevelTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Players")) {
      drawPlayersTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Mods")) {
      drawModsTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Debug")) {
      drawDebugTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Selected")) {
      drawSelectedEntityTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Settings")) {
      drawSettingsTab();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
}

void handleKeyInput() {
  auto keys = gConfig->keys;

  if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Hide])) {
    ui::open = !ui::open;
  }

  if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Engine_Pause])) {
    gPaused = !gPaused;
    gPauseAt = gFrame;
  }

  if (gPaused && gFrame > gPauseAt) {
    if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Engine_Frame_Advance],
                            true)) {
      gGlobalState->pause_update = 0;
      gPauseAt = gFrame + 1;
    } else {
      gGlobalState->pause_update = 1;
    }
  } else {
    gGlobalState->pause_update = 0;
  }
}

void onLevelStart() {
  // Olmec
  if (gGlobalState->level == 16) {
    if (gDebugState.DisableOlmecCutscene) {

      auto player = gGlobalState->player1;
      player->field1_0x130 = 1;
      player->field2_0x134 = 1;
      player->field3_0x138 = 1;
      player->field48_0x1ec = 1;
      player->field55_0x1f3 = 0;
      player->field7_0x271 = 0;

      // Destroy Floor
      EntityCallback cb = [&](Entity *e) {
        if (e->y == 75 && e->x >= 21 && e->x <= 25) {
          e->flag_deletion = 1;
        }
      };
      forEntities({}, cb, gGlobalState->entities->entities_active,
                  gGlobalState->entities->entities_active_count);
      forEntities({}, cb, (Entity **)gGlobalState->level_state->entity_floors,
                  4692);

      gCameraState->camera_speed = 0.2f;
      auto hawkman = (EntityMonster *)gGlobalState->SpawnEntity(
          18.019993f, 76.0f, 1011, true);
      hawkman->flag_horizontal_flip = 1;
      hawkman->field8_0x14c = 0xA;
      hawkman->field15_0x168 = 0x5A;
      hawkman->field21_0x180 = 0xFF;
      hawkman->field24_0x18c = 0xFF;
      hawkman->field81_0x20d = 1;

      // Spawn Olmec
      auto olmec = (EntityItem *)gGlobalState->SpawnEntity(23.109999f, 76.5f,
                                                           1055, true);
      olmec->flag_horizontal_flip = 1;

      olmec->field8_0x14c = 0;
      olmec->field9_0x150 = 0;
      olmec->field10_0x154 = 0;
      olmec->field11_0x158 = 2;
      // stomping
      // olmec->field8_0x14c = 0;
      olmec->field37_0x1c0 = 0.015000f;
      olmec->field48_0x1ec = 1;
      olmec->field68_0x200 = 1;
      olmec->field82_0x20e = 1;

      gGlobalState->PlayOlmecMusic("A04_boss.ogg");
      gGlobalState->total_seconds += 11;
      gGlobalState->total_ms += 33.50;
      gGlobalState->level_seconds = 11;
      gGlobalState->level_ms = 33.50;
    }
  }

  if (gModsState.TheFullSpelunky) {
    if (gGlobalState->level == 5 || gGlobalState->level == 8) {

      for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
           idx++) {

        auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

        if (!ent) {
          continue;
        }

        if (ent->entity_type != 211) {
          continue;
        }

        auto room = GetRoomForPosition(ent->x, ent->y);
        auto roomType = gGlobalState->level_state->room_types[room];
        if (roomType != 73 && roomType != 74) {
          continue;
        }

        if (gFullSpelunkyState.randoms.size() > 2) {
          ent->field5_0x140 = charIdToTextureId(gFullSpelunkyState.randoms[0]);
        }
        break;
      }
    } else if (gGlobalState->level == 9 && gGlobalState->is_wet_fur ||
               gGlobalState->level == 11 && gGlobalState->is_mothership ||
               gGlobalState->is_worm) {
      for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
           idx++) {

        auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

        if (!ent) {
          continue;
        }

        if (ent->entity_type != 211) {
          continue;
        }

        auto room = GetRoomForPosition(ent->x, ent->y);
        auto roomType = gGlobalState->level_state->room_types[room];
        if (roomType == 43 || roomType == 44 || roomType == 45) {
          if (gFullSpelunkyState.randoms.size() > 2) {
            ent->field5_0x140 =
                charIdToTextureId(gFullSpelunkyState.randoms[0]);
          }
          break;
        }
      }
    }
  }
}

uint32_t gScreenStatePrevious = 0;
void specsOnFrame() {

  gCameraState =
      reinterpret_cast<CameraState *>(*((DWORD *)(gBaseAddress + 0x154510)));
  gGlobalState =
      reinterpret_cast<GlobalState *>(*((DWORD *)(gBaseAddress + 0x15446C)));

  gWindowedMode = static_cast<int>(*((DWORD *)(gBaseAddress + 0x15a52c)));
  gDisplayWidth = static_cast<int>(*((DWORD *)(gBaseAddress + 0x140a8c)));
  gDisplayHeight = static_cast<int>(*((DWORD *)(gBaseAddress + 0x140a90)));

  gGlobalState->N00001004 = 0; // 440629
  gFrame++;

  if (gGlobalState->screen_state == 0 && gScreenStatePrevious == 2) {
    onLevelStart();
  }

  handleKeyInput();
  ensureLockedAmounts();
  drawOverlayWindow();
  drawToolWindow();

  if (gGlobalState->screen_state == 0 && gGlobalState->play_state == 0) {
    onRunningFrame();
  }

  gScreenStatePrevious = gGlobalState->screen_state;
}