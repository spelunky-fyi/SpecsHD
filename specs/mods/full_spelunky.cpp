
#include "full_spelunky.h"

#include "../drawing.h"
#include <hddll/memory.h>

FullSpelunkyState gFullSpelunkyState = {};

std::vector<hddll::Patch> gFullSpelunkyPatches = {
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

hddll::ForcePatch gDarkLevelForcePatch = {
    {0x6afbe, {0x1}, {0x0}},
    {0x6afae, {0x0}, {0x1}},
};

hddll::ForcePatch gSnakePitForcePatch = {
    {0xca167,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x90, 0x0, 0x0, 0x0}},
    {0xca166, {0x0}, {0x1}},
};

hddll::ForcePatch gSkinIsCrawlingForcePatch = {
    {0xcaa28,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x00, 0x02, 0x00, 0x00}},
    {0xcaa27, {0x0}, {0x1}},
};

hddll::ForcePatch gRushingWaterForcePatch = {
    {0xcad7c,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x84, 0xc1, 0x01, 0x00, 0x00}},
    {0xcad6c, {0x0}, {0x1}},
};

hddll::ForcePatch gDeadAreRestlessForcePatch = {
    {0xbdf38,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0xc9, 0x00, 0x00, 0x00}},
    {0xbdf28, {0x0}, {0x1}},
};

hddll::ForcePatch gBeesForcePatch = {
    {0xcafb2,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x80, 0x03, 0x00, 0x00}},
    {0xcafb1, {0x0}, {0x1}},
};

hddll::ForcePatch gTikiVillageForcePatch = {
    {0xcb3ca,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x90, 0x00, 0x00, 0x00}},
    {0xcb3c9, {0x0}, {0x1}},
};

hddll::ForcePatch gWetFurForcePatch = {
    {0xcb59c,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x84, 0xf5, 0x00, 0x00, 0x00}},
    {0xcb58c, {0x0}, {0x1}},
};

hddll::ForcePatch gPsychicPresenceForcePatch = {
    {0xcb704,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0xbf, 0x00, 0x00, 0x00}},
    {0xcb703, {0x0}, {0x1}},
};

hddll::ForcePatch gIcePoolsForcePatch = {
    {0xcb856,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x27, 0x01, 0x00, 0x00}},
    {0xcb855, {0x0}, {0x1}},
};

hddll::ForcePatch gKaliPitForcePatch = {
    {0xca3b8,
     {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
     {0x0f, 0x85, 0x9f, 0x00, 0x00, 0x00}},
    {0xca3b7, {0x0}, {0x1}},
};

void resetFullSpelunkyState() {
  gFullSpelunkyState.allCharacters = {
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

  gFullSpelunkyState.randoms = {
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
}

void prePlaceRoomsFullSpelunky() {
  // Mines
  if (hddll::gGlobalState->level >= 1 && hddll::gGlobalState->level <= 4) {
    if (hddll::gGlobalState->level == 3) {
      hddll::applyForcePatch(gSnakePitForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
      hddll::applyForcePatch(gSkinIsCrawlingForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
    } else if (hddll::gGlobalState->level == 4) {
      hddll::applyForcePatch(gSnakePitForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gSkinIsCrawlingForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
    } else {
      hddll::applyForcePatch(gSnakePitForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gSkinIsCrawlingForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
    }
    // Jungle
  } else if (hddll::gGlobalState->level >= 5 &&
             hddll::gGlobalState->level <= 8) {
    if (hddll::gGlobalState->level == 5) {
      hddll::applyForcePatch(gRushingWaterForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
      hddll::applyForcePatch(gDeadAreRestlessForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
      hddll::applyForcePatch(gBeesForcePatch, hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gTikiVillageForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
    } else if (hddll::gGlobalState->level == 8) {
      hddll::applyForcePatch(gRushingWaterForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gDeadAreRestlessForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gBeesForcePatch, hddll::FORCE_PATCH_TYPE_ALWAYS);
      hddll::applyForcePatch(gTikiVillageForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
    } else {
      hddll::applyForcePatch(gDeadAreRestlessForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gRushingWaterForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gBeesForcePatch, hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gTikiVillageForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
    }
    // Ice Caves
  } else if (hddll::gGlobalState->level >= 9 &&
             hddll::gGlobalState->level <= 12) {
    if (hddll::gGlobalState->level == 9) {
      hddll::applyForcePatch(gWetFurForcePatch, hddll::FORCE_PATCH_TYPE_ALWAYS);
      hddll::applyForcePatch(gPsychicPresenceForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
    } else if (hddll::gGlobalState->level == 11 and
               hddll::gGlobalState->mothership_spawned == 0) {
      hddll::applyForcePatch(gPsychicPresenceForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
      hddll::applyForcePatch(gWetFurForcePatch, hddll::FORCE_PATCH_TYPE_NEVER);
    } else if (hddll::gGlobalState->level == 12 and
               hddll::gGlobalState->mothership_spawned == 0) {
      hddll::applyForcePatch(gPsychicPresenceForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gWetFurForcePatch, hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gIcePoolsForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
    } else if (hddll::gGlobalState->level == 12 and
               hddll::gGlobalState->mothership_spawned == 1) {
      hddll::applyForcePatch(gPsychicPresenceForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
      hddll::applyForcePatch(gWetFurForcePatch, hddll::FORCE_PATCH_TYPE_NEVER);
    } else {
      hddll::applyForcePatch(gWetFurForcePatch, hddll::FORCE_PATCH_TYPE_NEVER);
      hddll::applyForcePatch(gPsychicPresenceForcePatch,
                             hddll::FORCE_PATCH_TYPE_NEVER);
    }
    // Temple
  } else if (hddll::gGlobalState->level >= 13 &&
             hddll::gGlobalState->level <= 15) {
    if (hddll::gGlobalState->level == 14) {
      hddll::applyForcePatch(gKaliPitForcePatch,
                             hddll::FORCE_PATCH_TYPE_ALWAYS);
    } else {
      hddll::applyForcePatch(gKaliPitForcePatch, hddll::FORCE_PATCH_TYPE_NEVER);
    }
  }

  if (!gModsState.DarkMode) {
    if (hddll::gGlobalState->level == 12 and
        hddll::gGlobalState->mothership_spawned == 0) {
      hddll::gGlobalState->dark_level = 1;
    } else {
      hddll::gGlobalState->dark_level = 0;
    }
  }
}

void postPlaceRoomsFullSpelunky() {
  // Mines
  if (hddll::gGlobalState->level >= 1 && hddll::gGlobalState->level <= 4) {
    hddll::applyForcePatch(gSnakePitForcePatch, hddll::FORCE_PATCH_TYPE_NORMAL);
    hddll::applyForcePatch(gSkinIsCrawlingForcePatch,
                           hddll::FORCE_PATCH_TYPE_NORMAL);
  } else if (hddll::gGlobalState->level >= 5 &&
             hddll::gGlobalState->level <= 8) {
    hddll::applyForcePatch(gRushingWaterForcePatch,
                           hddll::FORCE_PATCH_TYPE_NORMAL);
    hddll::applyForcePatch(gDeadAreRestlessForcePatch,
                           hddll::FORCE_PATCH_TYPE_NORMAL);
    hddll::applyForcePatch(gBeesForcePatch, hddll::FORCE_PATCH_TYPE_NORMAL);
    hddll::applyForcePatch(gTikiVillageForcePatch,
                           hddll::FORCE_PATCH_TYPE_NORMAL);
  } else if (hddll::gGlobalState->level >= 9 &&
             hddll::gGlobalState->level <= 12) {
    hddll::applyForcePatch(gWetFurForcePatch, hddll::FORCE_PATCH_TYPE_NORMAL);
    hddll::applyForcePatch(gPsychicPresenceForcePatch,
                           hddll::FORCE_PATCH_TYPE_NORMAL);
    hddll::applyForcePatch(gIcePoolsForcePatch, hddll::FORCE_PATCH_TYPE_NORMAL);
    // Temple
  } else if (hddll::gGlobalState->level >= 13 &&
             hddll::gGlobalState->level <= 15) {
    hddll::applyForcePatch(gKaliPitForcePatch, hddll::FORCE_PATCH_TYPE_NORMAL);
  }
}

void preSpawnTilesFullSpelunky() {
  if (gModsState.TheFullSpelunky) {
    if (hddll::gGlobalState->level == 5) {
      bool found_hc_entrance = false;
      bool placed_hc_entrance = false;
      for (auto idx = 0; idx < 48; idx++) {
        if (hddll::gGlobalState->level_state->room_types[idx] == 47) {
          found_hc_entrance = true;
          break;
        }
      }
      if (!found_hc_entrance) {
        for (auto idx = 4; idx < 48; idx++) {
          if (hddll::gGlobalState->level_state->room_types[idx] == 0) {
            hddll::gGlobalState->level_state->room_types[idx] = 47;
            placed_hc_entrance = true;
            break;
          }
        }

        // If you didn't find an open side room shove it on the path
        if (!placed_hc_entrance) {
          for (auto idx = 4; idx < 48; idx++) {
            auto roomType = hddll::gGlobalState->level_state->room_types[idx];
            if (roomType == 2 || roomType == 3) {
              hddll::gGlobalState->level_state->room_types[idx] = 47;
              placed_hc_entrance = true;
              break;
            }
          }
        }
      }
    } else if (hddll::gGlobalState->level == 9 &&
               hddll::gGlobalState->is_wet_fur == 1) {
      // Place coffin in Wet Fur
      for (auto idx = 4; idx < 12; idx++) {
        if (hddll::gGlobalState->level_state->room_types[idx] == 0) {
          hddll::gGlobalState->level_state->room_types[idx] = 43;
          break;
        }
      }
    } else if (hddll::gGlobalState->level == 11 &&
               hddll::gGlobalState->is_mothership == 1) {
      // Place coffin in Mothership
      for (auto idx = 4; idx < 12; idx++) {
        if (hddll::gGlobalState->level_state->room_types[idx] == 0) {
          hddll::gGlobalState->level_state->room_types[idx] = 43;
          break;
        }
      }
    } else if (hddll::gGlobalState->is_worm == 1) {
      // Place coffin in Worm
      for (auto idx = 8; idx < 48; idx++) {
        if (((idx % 4) < 2) &&
            hddll::gGlobalState->level_state->room_types[idx] == 1) {
          hddll::gGlobalState->level_state->room_types[idx] = 44;
          break;
        }
      }
    } else if (hddll::gGlobalState->level == 15 &&
               hddll::gGlobalState->is_city_of_gold) {
      bool found_coffin = false;
      for (auto idx = 0; idx < 48; idx++) {
        if (hddll::gGlobalState->level_state->room_types[idx] == 44 &&
            idx < 4 &&
            hddll::gGlobalState->level_state->entrance_room_x != idx) {
          found_coffin = true;
          break;
        }
      }
      if (!found_coffin) {
        for (auto idx = 4; idx < 48; idx++) {
          if (hddll::gGlobalState->level_state->room_types[idx] == 2) {
            hddll::gGlobalState->level_state->room_types[idx] = 44;
            break;
          }
        }
      }
      // Hell
    } else if (hddll::gGlobalState->level > 16) {
      if (hddll::gGlobalState->_34struct->coffin_char > -1) {
        bool found_coffin = false;
        for (auto idx = 0; idx < 48; idx++) {
          auto levelType = hddll::gGlobalState->level_state->room_types[idx];
          if (levelType == 73) {
            found_coffin = true;
            break;
          }
        }
        if (!found_coffin) {
          for (auto idx = 4; idx < 48; idx++) {
            if (hddll::gGlobalState->level_state->room_types[idx] == 0) {
              hddll::gGlobalState->level_state->room_types[idx] = 73;
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
    hddll::gGlobalState->_34struct->coffin_char = -1;
    return;
  }

  if (hddll::gGlobalState->is_haunted_castle != 0) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_VAN_HELSING;
  } else if (hddll::gGlobalState->is_worm != 0) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_MEATBOY;
  } else if (hddll::gGlobalState->skin_is_crawling != 0) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_ROUND_GIRL;
  } else if (hddll::gGlobalState->rushing_water != 0) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_VIKING;
  } else if (hddll::gGlobalState->tiki_village != 0) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_ROUND_BOY;
  } else if (hddll::gGlobalState->is_wet_fur != 0) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_INUK;
  } else if (hddll::gGlobalState->is_mothership != 0) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_ROBOT;
  } else if (hddll::gGlobalState->is_city_of_gold != 0) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_MONK;
  } else if (hddll::gGlobalState->level == 16) {
    hddll::gGlobalState->_34struct->coffin_char = hddll::CHARACTER_NINJA;
  } else {
    hddll::gGlobalState->_34struct->coffin_char = gFullSpelunkyState.randoms[0];
  }

  if (hddll::gGlobalState->_34struct->coffin_char == hddll::CHARACTER_YANG &&
      gFullSpelunkyState.allCharacters.size() > 1) {
    hddll::gGlobalState->_34struct->coffin_char = -1;
  }

  // Only allow Jungle Warrior by Temple
  if (hddll::gGlobalState->_34struct->coffin_char ==
          hddll::CHARACTER_JUNGLE_WARRIOR &&
      hddll::gGlobalState->level < 13) {
    hddll::gGlobalState->_34struct->coffin_char = -1;
  }

  // Only allow Yang in Hell
  if (hddll::gGlobalState->_34struct->coffin_char == hddll::CHARACTER_YANG &&
      hddll::gGlobalState->level < 17) {
    hddll::gGlobalState->_34struct->coffin_char = -1;
  }
}

void onRunningFrameFullSpelunky() {
  if (hddll::gGlobalState->player1) {
    if (hddll::gGlobalState->dark_level) {
      if (hddll::gGlobalState->player1_data.has_udjat ||
          hddll::gGlobalState->player1_data.has_spectacles) {
        hddll::gGlobalState->player1->brightness = 20.0;
      } else {
        hddll::gGlobalState->player1->brightness = 15.0;
      }
    } else {
      hddll::gGlobalState->player1->brightness = 5.0;
    }
  }

  for (int hh_idx = 0; hh_idx < hddll::gGlobalState->player1_data.hh_count;
       hh_idx++) {
    auto texture_id = hddll::gGlobalState->player1_data.hh_texture_id[hh_idx];
    auto char_id = hddll::TextureIdToCharId((hddll::TextureId)texture_id);

    std::vector<hddll::CharacterIndex>::iterator position;
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

  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {

    auto ent = (hddll::EntityActive *)
                   hddll::gGlobalState->entities->entities_active[idx];

    if (!ent) {
      continue;
    }

    if (ent->entity_kind != hddll::EntityKind::KIND_PLAYER) {
      continue;
    }

    hddll::EntityPlayer *player = (hddll::EntityPlayer *)ent;
    if (!player->ai_bot) {
      continue;
    }

    auto texture_id = player->texture_definition->texture_id;
    auto char_id = hddll::TextureIdToCharId((hddll::TextureId)texture_id);

    std::vector<hddll::CharacterIndex>::iterator position;
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

void onLevelStartFullSpelunky() {
  if (hddll::gGlobalState->level == 5 || hddll::gGlobalState->level == 8) {

    for (size_t idx = 0;
         idx < hddll::gGlobalState->entities->entities_active_count; idx++) {

      auto ent = (hddll::EntityActive *)
                     hddll::gGlobalState->entities->entities_active[idx];

      if (!ent) {
        continue;
      }

      if (ent->entity_type != 211) {
        continue;
      }

      auto room = hddll::GetRoomForPosition(ent->x, ent->y);
      auto roomType = hddll::gGlobalState->level_state->room_types[room];
      if (roomType != 73 && roomType != 74) {
        continue;
      }

      if (gFullSpelunkyState.randoms.size() > 2) {
        ent->field5_0x140 =
            hddll::charIdToTextureId(gFullSpelunkyState.randoms[0]);
      }
      break;
    }
  } else if ((hddll::gGlobalState->level == 9 &&
              hddll::gGlobalState->is_wet_fur) ||
             (hddll::gGlobalState->level == 11 &&
              hddll::gGlobalState->is_mothership) ||
             hddll::gGlobalState->is_worm) {
    for (size_t idx = 0;
         idx < hddll::gGlobalState->entities->entities_active_count; idx++) {

      auto ent = (hddll::EntityActive *)
                     hddll::gGlobalState->entities->entities_active[idx];

      if (!ent) {
        continue;
      }

      if (ent->entity_type != 211) {
        continue;
      }

      auto room = hddll::GetRoomForPosition(ent->x, ent->y);
      auto roomType = hddll::gGlobalState->level_state->room_types[room];
      if (roomType == 43 || roomType == 44 || roomType == 45) {
        if (gFullSpelunkyState.randoms.size() > 2) {
          ent->field5_0x140 =
              hddll::charIdToTextureId(gFullSpelunkyState.randoms[0]);
        }
        break;
      }
    }
  }
}

void drawCharacterOverlay() {
  ImGuiIO &io = ImGui::GetIO();

  auto size = 20.f;
  auto padding = 4.f;
  ImVec2 start = {
      io.DisplaySize.x - size - padding,
      (io.DisplaySize.y / 2) - ((size + padding) * 10),
  };

  for (auto idx = 0; idx < 20; idx++) {
    ImColor color = hddll::charIdToColor((hddll::CharacterIndex)idx, 0.7f);

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
