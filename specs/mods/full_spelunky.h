#pragma once

#include "../state.h"
#include <hddll/memory.h>

struct FullSpelunkyState {
  std::vector<hddll::CharacterIndex> allCharacters = {
      hddll::CHARACTER_GUY,
      hddll::CHARACTER_RED,
      hddll::CHARACTER_GREEN,
      hddll::CHARACTER_BLUE,
      hddll::CHARACTER_MEATBOY,
      hddll::CHARACTER_YELLOW,
      hddll::CHARACTER_PURPLE,
      hddll::CHARACTER_VAN_HELSING,
      hddll::CHARACTER_CYAN,
      hddll::CHARACTER_LIME,
      hddll::CHARACTER_INUK,
      hddll::CHARACTER_ROUND_GIRL,
      hddll::CHARACTER_NINJA,
      hddll::CHARACTER_VIKING,
      hddll::CHARACTER_ROUND_BOY,
      hddll::CHARACTER_CARL,
      hddll::CHARACTER_ROBOT,
      hddll::CHARACTER_MONK,

      hddll::CHARACTER_JUNGLE_WARRIOR,
      hddll::CHARACTER_YANG,
  };

  std::vector<hddll::CharacterIndex> randoms = {
      hddll::CHARACTER_GUY,
      hddll::CHARACTER_RED,
      hddll::CHARACTER_GREEN,
      hddll::CHARACTER_BLUE,
      hddll::CHARACTER_YELLOW,
      hddll::CHARACTER_PURPLE,
      hddll::CHARACTER_CYAN,
      hddll::CHARACTER_LIME,
      hddll::CHARACTER_CARL,
      hddll::CHARACTER_ROUND_GIRL,
      hddll::CHARACTER_ROUND_BOY,
      hddll::CHARACTER_INUK,
      hddll::CHARACTER_JUNGLE_WARRIOR,
      hddll::CHARACTER_YANG,
  };

  bool showCharacterOverlay = false;
};

extern FullSpelunkyState gFullSpelunkyState;

extern std::vector<hddll::Patch> gFullSpelunkyPatches;

extern hddll::ForcePatch gDarkLevelForcePatch;
extern hddll::ForcePatch gSnakePitForcePatch;
extern hddll::ForcePatch gSkinIsCrawlingForcePatch;
extern hddll::ForcePatch gRushingWaterForcePatch;
extern hddll::ForcePatch gDeadAreRestlessForcePatch;
extern hddll::ForcePatch gBeesForcePatch;
extern hddll::ForcePatch gTikiVillageForcePatch;
extern hddll::ForcePatch gWetFurForcePatch;
extern hddll::ForcePatch gPsychicPresenceForcePatch;
extern hddll::ForcePatch gIcePoolsForcePatch;
extern hddll::ForcePatch gKaliPitForcePatch;

void resetFullSpelunkyState();
void prePlaceRoomsFullSpelunky();
void postPlaceRoomsFullSpelunky();
void preSpawnTilesFullSpelunky();
void unlockCoffinsFullSpelunky();
void onRunningFrameFullSpelunky();
void onLevelStartFullSpelunky();
void drawCharacterOverlay();
