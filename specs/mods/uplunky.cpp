
#include "uplunky.h"

std::vector<Patch> gUplunkyPatches = {
    // Change Exit Door to Entrance Door
    {0xd91ba, {0x02}, {0x03}},
    // Change Entrance Door to Exit Door
    {0xd929c, {0x03}, {0x02}},

    // Spawn player at exit instead of entrance
    {0x6651c, {0xa0}, {0x98}},
    {0x66532, {0x9c}, {0x94}},

    // Spawn shopkeeper at entrance instead of exit
    {0x6b10c, {0x98}, {0xa0}},
    {0x6b120, {0x94}, {0x9c}},

    // Open entrance when olmec dies
    {0x309fa, {0x94}, {0x9c}},
    {0x309ed, {0x98}, {0xa0}},
    {0xde7ef, {0x03}, {0x02}},
    {0xde966, {0x02}, {0x03}},
};

std::vector<RelativePatch> gUplunkyRelativePatches = {
    // // Swap entrance/exit on olmec
    // {0xde7d6, 0x13602c, 0x135b3c},
    // {0xde94d, 0x135b3c, 0x13602c},
};

void resetUplunkyState() {
  gGlobalState->player1_data.ropes = 64;
  gGlobalState->player1_data.bombs = 6;
}

void prePlaceRoomsUplunky() {
  if (!gModsState.DarkMode) {
    gGlobalState->dark_level = 0;
  }
}

void postPlaceRoomsUplunky() {
  // Commented out code from original - door swapping logic
}
