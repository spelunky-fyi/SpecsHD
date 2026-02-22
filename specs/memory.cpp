
#include "memory.h"

#include "state.h"

void patchReadOnlyCode(HANDLE process, DWORD addr, void *value, size_t size) {
  DWORD oldrights;
  VirtualProtectEx(process, (LPVOID)addr, size, PAGE_EXECUTE_READWRITE,
                   &oldrights);

  WriteProcessMemory(process, (LPVOID)addr, value, size, NULL);
  VirtualProtectEx(process, (LPVOID)addr, size, oldrights, &oldrights);
}

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

void applyRelativePatches(std::vector<RelativePatch> &patches, bool rollback) {

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

void applyPatches(std::vector<Patch> &patches, bool rollback) {

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

static std::vector<HookEntry> gInstalledHooks;

bool hook(void *toHook, void *ourFunc, int len) {
  if (len < 5) {
    return false;
  }

  // Save original bytes before overwriting
  HookEntry entry;
  entry.address = toHook;
  entry.length = len;
  entry.originalBytes.resize(len);
  memcpy(entry.originalBytes.data(), toHook, len);
  gInstalledHooks.push_back(entry);

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

bool unhook(void *toHook) {
  for (auto it = gInstalledHooks.begin(); it != gInstalledHooks.end(); ++it) {
    if (it->address == toHook) {
      DWORD curProtection;
      VirtualProtect(toHook, it->length, PAGE_EXECUTE_READWRITE,
                     &curProtection);
      memcpy(toHook, it->originalBytes.data(), it->length);
      DWORD tmp;
      VirtualProtect(toHook, it->length, curProtection, &tmp);
      gInstalledHooks.erase(it);
      return true;
    }
  }
  return false;
}

void cleanUpHooks() {
  while (!gInstalledHooks.empty()) {
    auto &entry = gInstalledHooks.back();
    DWORD curProtection;
    VirtualProtect(entry.address, entry.length, PAGE_EXECUTE_READWRITE,
                   &curProtection);
    memcpy(entry.address, entry.originalBytes.data(), entry.length);
    DWORD tmp;
    VirtualProtect(entry.address, entry.length, curProtection, &tmp);
    gInstalledHooks.pop_back();
  }
}
