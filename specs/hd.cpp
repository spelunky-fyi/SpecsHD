#include "hd.h"

// GlobalState
DWORD gSpawnEntityOffset;

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

// Entity
DWORD gEntityPlaySoundOffset;

void Entity::PlaySound(const char *audioName) {

  using PlaySoundPtr =
      DWORD(__thiscall *)(DWORD thisPtr, const char *audioName);
  PlaySoundPtr PlaySound = (PlaySoundPtr)(gEntityPlaySoundOffset);
  PlaySound((DWORD)this, audioName);
}

void setupOffsets(DWORD baseAddress) {
  gSpawnEntityOffset = baseAddress + 0x70AB0;

  gEntityPlaySoundOffset = baseAddress + 0x16A95;
}