
#include "full_spelunky.h"

#include "../drawing.h"
#include "../memory.h"

FullSpelunkyState gFullSpelunkyState = {};

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
      // Place coffin in Worm
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

void onRunningFrameFullSpelunky() {
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

  for (int hh_idx = 0; hh_idx < gGlobalState->player1_data.hh_count; hh_idx++) {
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

void onLevelStartFullSpelunky() {
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
          ent->field5_0x140 = charIdToTextureId(gFullSpelunkyState.randoms[0]);
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
