#pragma once

#include "../memory.h"
#include "../state.h"

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

extern FullSpelunkyState gFullSpelunkyState;

extern std::vector<Patch> gFullSpelunkyPatches;

extern ForcePatch gDarkLevelForcePatch;
extern ForcePatch gSnakePitForcePatch;
extern ForcePatch gSkinIsCrawlingForcePatch;
extern ForcePatch gRushingWaterForcePatch;
extern ForcePatch gDeadAreRestlessForcePatch;
extern ForcePatch gBeesForcePatch;
extern ForcePatch gTikiVillageForcePatch;
extern ForcePatch gWetFurForcePatch;
extern ForcePatch gPsychicPresenceForcePatch;
extern ForcePatch gIcePoolsForcePatch;
extern ForcePatch gKaliPitForcePatch;

void resetFullSpelunkyState();
void prePlaceRoomsFullSpelunky();
void postPlaceRoomsFullSpelunky();
void preSpawnTilesFullSpelunky();
void unlockCoffinsFullSpelunky();
void onRunningFrameFullSpelunky();
void onLevelStartFullSpelunky();
void drawCharacterOverlay();
