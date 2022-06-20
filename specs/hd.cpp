#include "hd.h"

DWORD gSpawnEntityOffset;

void setupOffsets(DWORD baseAddress) {
  gSpawnEntityOffset = baseAddress + 0x70AB0;
}

Entity *GlobalState::SpawnEntity(float x, float y, uint32_t entityType,
                                 bool active) {

  using SpawnEntityPtr = DWORD(__thiscall *)(DWORD thisPtr, float x, float y,
                                             uint32_t entityType, bool active);
  SpawnEntityPtr SpawnEntity = (SpawnEntityPtr)(gSpawnEntityOffset);
  DWORD entityAddress = SpawnEntity((DWORD)this, x, y, entityType, true);
  if (entityAddress == NULL) {
    return NULL;
  }

  return reinterpret_cast<Entity *>(entityAddress);
}