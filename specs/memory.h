#pragma once

#include <Windows.h>
#include <cstdint>
#include <vector>

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

struct HookEntry {
  void *address;
  int length;
  std::vector<BYTE> originalBytes;
};

void patchReadOnlyCode(HANDLE process, DWORD addr, void *value, size_t size);
void applyForcePatch(ForcePatch &patch, FORCE_PATCH_TYPE type);
void applyRelativePatches(std::vector<RelativePatch> &patches,
                          bool rollback = false);
void applyPatches(std::vector<Patch> &patches, bool rollback = false);
bool hook(void *toHook, void *ourFunc, int len);
bool unhook(void *toHook);
void cleanUpHooks();
