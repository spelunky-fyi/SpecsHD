
#include "biglunky.h"

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

void prePlaceRoomsBiglunky() {
  if (!gModsState.DarkMode && !gModsState.TheFullSpelunky) {
    gGlobalState->dark_level = 0;
  }
}

void postPlaceRoomsBiglunky() {
  if (gModsState.Biglunky) {
    if (gGlobalState->is_city_of_gold) {
      auto num_flag_21 = 0;
      for (auto idx = 0; idx < ENTITY_FLOORS_COUNT; idx++) {
        auto ent = gGlobalState->level_state->entity_floors[idx];
        if (ent && ent->flag_21 == 1) {
          if (num_flag_21 > 512) {
            ent->flag_21 = 0;
          }
          num_flag_21++;
        }
      }
    } else if (gGlobalState->rushing_water) {
      for (auto idx = 0; idx < ENTITY_FLOORS_COUNT; idx++) {
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
