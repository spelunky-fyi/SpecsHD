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

DWORD gPlayMusic;

void GlobalState::PlayOlmecMusic(const char *audioName) {
  __asm {
        pushad

        mov ebx, dword ptr [ecx + 0x44]
        push 0x0
        push 0x1
        push 0x0
        push audioName
        xor eax,eax
        call gPlayMusic

        popad
  }
}

void setupOffsets(DWORD baseAddress) {
  gSpawnEntityOffset = baseAddress + 0x70AB0;

  gEntityPlaySoundOffset = baseAddress + 0x16A95;

  gPlayMusic = baseAddress + 0x9920;
}
