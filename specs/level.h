#include "hd.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>


const std::map<std::string, ROOM_TYPE> roomMap = {{
    {"entrance", ROOM_TYPE::ENTRANCE},
    {"entrance_drop", ROOM_TYPE::ENTRANCE_DROP},
    {"exit", ROOM_TYPE::EXIT},
    {"exit_notop", ROOM_TYPE::EXIT_NOTOP},
    {"idol", ROOM_TYPE::IDOL},
    {"altar", ROOM_TYPE::ALTAR},
    {"path_drop_notop", ROOM_TYPE::PATH_DROP_NOTOP},
    {"shop", ROOM_TYPE::SHOP},
    {"shop_prize", ROOM_TYPE::SHOP_PRIZE},
    {"shop_ankh", ROOM_TYPE::SHOP_ANKH},

    {"side", ROOM_TYPE::SIDE},
    {"path", ROOM_TYPE::PATH},
    {"path_drop", ROOM_TYPE::PATH_DROP},
    {"path_notop", ROOM_TYPE::PATH_NOTOP},
    {"shop_right_facing", ROOM_TYPE::SHOP_RIGHT_FACING},
    {"shop_left_facing", ROOM_TYPE::SHOP_LEFT_FACING},
    {"snake_pit_top", ROOM_TYPE::SNAKE_PIT_TOP},
    {"snake_pit_middle", ROOM_TYPE::SNAKE_PIT_MIDDLE},
    {"snake_pit_bottom", ROOM_TYPE::SNAKE_PIT_BOTTOM},
    {"rushing_water_islands", ROOM_TYPE::RUSHING_WATER_ISLANDS},
    {"rushing_water_lake", ROOM_TYPE::RUSHING_WATER_LAKE},
    {"rushing_water_lake_bitey", ROOM_TYPE::RUSHING_WATER_LAKE_BITEY},
    {"psychic_presence_left", ROOM_TYPE::PSYCHIC_PRESENCE_LEFT},
    {"psychic_presence_center", ROOM_TYPE::PSYCHIC_PRESENCE_CENTER},
    {"psychic_presence_right", ROOM_TYPE::PSYCHIC_PRESENCE_RIGHT},
    {"moai", ROOM_TYPE::MOAI},
    {"kali_pit_top", ROOM_TYPE::KALI_PIT_TOP},
    {"kali_pit_middle", ROOM_TYPE::KALI_PIT_MIDDLE},
    {"kali_pit_bottom", ROOM_TYPE::KALI_PIT_BOTTOM},
    {"vlads_tower_top", ROOM_TYPE::VLADS_TOWER_TOP},
    {"vlads_tower_middle", ROOM_TYPE::VLADS_TOWER_MIDDLE},
    {"vlads_tower_bottom", ROOM_TYPE::VLADS_TOWER_BOTTOM},
    {"beehive_sides_open",
     ROOM_TYPE::BEEHIVE_SIDES_OPEN}, //{"beehive_left_right_open",
                                     //ROOM_TYPE::BEEHIVE_LEFT_RIGHT_OPEN},
    {"beehive_sides_down_open", ROOM_TYPE::BEEHIVE_SIDES_DOWN_OPEN},
    {"beehive_sides_up_open", ROOM_TYPE::BEEHIVE_SIDES_UP_OPEN},
    {"necronomicon_left", ROOM_TYPE::NECRONOMICON_LEFT},
    {"necronomicon_right", ROOM_TYPE::NECRONOMICON_RIGHT},
    {"mothership_entrance_top", ROOM_TYPE::MOTHERSHIP_ENTRANCE_TOP},
    {"mothership_entrance_bottom", ROOM_TYPE::MOTHERSHIP_ENTRANCE_BOTTOM},
    {"castle_1", ROOM_TYPE::CASTLE_1},
    {"castle_2", ROOM_TYPE::CASTLE_2},
    {"castle_3", ROOM_TYPE::CASTLE_3},
    {"castle_4", ROOM_TYPE::CASTLE_4},
    {"castle_5", ROOM_TYPE::CASTLE_5},
    {"castle_6", ROOM_TYPE::CASTLE_6},
    {"castle_7", ROOM_TYPE::CASTLE_7},
    {"castle_8", ROOM_TYPE::CASTLE_8},
    {"castle_9", ROOM_TYPE::CASTLE_9},
    {"castle_10", ROOM_TYPE::CASTLE_10},
    {"castle_11", ROOM_TYPE::CASTLE_11},
    {"crysknife_pit_left", ROOM_TYPE::CRYSKNIFE_PIT_LEFT},
    {"crysknife_pit_right", ROOM_TYPE::CRYSKNIFE_PIT_RIGHT},
    {"coffin", ROOM_TYPE::COFFIN},
    {"coffin_drop", ROOM_TYPE::COFFIN_DROP},
    {"coffin_notop", ROOM_TYPE::COFFIN_NOTOP},
    {"alien_queen", ROOM_TYPE::ALIEN_QUEEN},
    {"dar_castle_entrance", ROOM_TYPE::DAR_CASTLE_ENTRANCE},
    {"dar_crystal_idol", ROOM_TYPE::DAR_CRYSTAL_IDOL},
    {"yama_0_0", ROOM_TYPE::YAMA_0_0},
    {"yama_0_1", ROOM_TYPE::YAMA_0_1},
    {"yama_0_2", ROOM_TYPE::YAMA_0_2},
    {"yama_0_3", ROOM_TYPE::YAMA_0_3},
    {"yama_throne_top_left", ROOM_TYPE::YAMA_THRONE_TOP_LEFT},
    {"yama_throne_top_right", ROOM_TYPE::YAMA_THRONE_TOP_RIGHT},
    {"yama_middle_left", ROOM_TYPE::YAMA_MIDDLE_LEFT},
    {"yama_throne_bottom_left", ROOM_TYPE::YAMA_THRONE_BOTTOM_LEFT},
    {"yama_throne_bottom_right", ROOM_TYPE::YAMA_THRONE_BOTTOM_RIGHT},
    {"yama_middle_right", ROOM_TYPE::YAMA_MIDDLE_RIGHT},
    {"yama_3_0", ROOM_TYPE::YAMA_3_0},
    {"yama_3_1", ROOM_TYPE::YAMA_3_1},
    {"yama_3_2", ROOM_TYPE::YAMA_3_2},
    {"yama_3_3", ROOM_TYPE::YAMA_3_3},
    {"spider_lair_1", ROOM_TYPE::SPIDER_LAIR_1},
    {"spider_lair_2", ROOM_TYPE::SPIDER_LAIR_2},
    {"spider_lair_3", ROOM_TYPE::SPIDER_LAIR_3},
    {"spider_lair_4", ROOM_TYPE::SPIDER_LAIR_4},
    {"vault", ROOM_TYPE::VAULT},
    {"ice_caves_pool_single_room", ROOM_TYPE::ICE_CAVES_POOL_SINGLE_ROOM},
    {"ice_caves_pool_top", ROOM_TYPE::ICE_CAVES_POOL_TOP},
    {"ice_caves_pool_bottom", ROOM_TYPE::ICE_CAVES_POOL_BOTTOM},
    {"temple_altar", ROOM_TYPE::TEMPLE_ALTAR},
    {"coffin_exit_right", ROOM_TYPE::COFFIN_EXIT_RIGHT},
    {"coffin_exit_left", ROOM_TYPE::COFFIN_EXIT_LEFT},
    {"worm_regen_structure", ROOM_TYPE::WORM_REGEN_STRUCTURE},
    {"wet_fur_1", ROOM_TYPE::WET_FUR_1},
    {"wet_fur_2", ROOM_TYPE::WET_FUR_2},
    {"wet_fur_3", ROOM_TYPE::WET_FUR_3},
    {"shop_general_left", ROOM_TYPE::SHOP_GENERAL_LEFT},
    {"shop_specialty", ROOM_TYPE::SHOP_SPECIALTY},
    {"shop_clothing", ROOM_TYPE::SHOP_CLOTHING},
    {"shop_bombs", ROOM_TYPE::SHOP_BOMBS},
    {"shop_weapons", ROOM_TYPE::SHOP_WEAPONS},
    {"shop_kissing", ROOM_TYPE::SHOP_KISSING},
    {"shop_hiredhand", ROOM_TYPE::SHOP_HIREDHAND},
    {"shop_prize_or_ankh", ROOM_TYPE::SHOP_PRIZE_OR_ANKH},
    {"tutorial_0_0", ROOM_TYPE::TUTORIAL_0_0},
    {"tutorial_0_1", ROOM_TYPE::TUTORIAL_0_1},
    {"tutorial_0_2", ROOM_TYPE::TUTORIAL_0_2},
    {"tutorial_0_3", ROOM_TYPE::TUTORIAL_0_3},
    {"tutorial_1_0", ROOM_TYPE::TUTORIAL_1_0},
    {"tutorial_1_1", ROOM_TYPE::TUTORIAL_1_1},
    {"tutorial_1_2", ROOM_TYPE::TUTORIAL_1_2},
    {"tutorial_1_3", ROOM_TYPE::TUTORIAL_1_3},
    {"tutorial_2_0", ROOM_TYPE::TUTORIAL_2_0},
    {"tutorial_2_1", ROOM_TYPE::TUTORIAL_2_1},
    {"tutorial_2_2", ROOM_TYPE::TUTORIAL_2_2},
    {"tutorial_2_3", ROOM_TYPE::TUTORIAL_2_3},
    {"tutorial_3_0", ROOM_TYPE::TUTORIAL_3_0},
    {"tutorial_3_1", ROOM_TYPE::TUTORIAL_3_1},
    {"tutorial_3_2", ROOM_TYPE::TUTORIAL_3_2},
    {"tutorial_3_3", ROOM_TYPE::TUTORIAL_3_3},

    {"shop_general_flipped", ROOM_TYPE::SHOP_GENERAL_FLIPPED},
    {"shop_specialty_flipped", ROOM_TYPE::SHOP_SPECIALTY_FLIPPED},
    {"shop_clothing_flipped", ROOM_TYPE::SHOP_CLOTHING_FLIPPED},
    {"shop_bombs_flipped", ROOM_TYPE::SHOP_BOMBS_FLIPPED},
    {"shop_weapons_flipped", ROOM_TYPE::SHOP_WEAPONS_FLIPPED},
    {"shop_kissing_flipped", ROOM_TYPE::SHOP_KISSING_FLIPPED},
    {"shop_hiredhand_flipped", ROOM_TYPE::SHOP_HIREDHAND_FLIPPED},
    {"shop_prize_or_ankh_flipped", ROOM_TYPE::SHOP_PRIZE_OR_ANKH_FLIPPED},
}};

const DWORD GETROOM_OFFSET = 0xd6690;
const DWORD GETROOM_GET_ROOM_OFF = GETROOM_OFFSET + 0x2d;
const DWORD GETROOM_SPAWN_ROOM_OFF = GETROOM_OFFSET + 0x174;
const DWORD SPAWN_LEVEL_TILES_OFF = 0xdd760;

using spawnLevelTilesFn = void(__stdcall *)(LevelState *levelState);
using RoomList = std::map<ROOM_TYPE, std::vector<std::string>>;

DWORD GetBaseAddress();

// Spawn bordertiles, does other stuff and then calls spawnRoom
// It's after the room types are decided and probably before spawning anything
// (spawn_rooms_bordertiles_etc in ghidra)
inline spawnLevelTilesFn spawnLevelTilesOriginal = nullptr;
inline spawnLevelTilesFn spawnLevelTilesPtr{
    (spawnLevelTilesFn)(GetBaseAddress() + SPAWN_LEVEL_TILES_OFF)};

// get room string and spawn tilecodes (tilecode_gen_and_spawning in ghidra)
inline void *spawnRoomOriginal = nullptr;
inline void *spawnRoomPtr{(void *)(GetBaseAddress() + GETROOM_OFFSET)};

void readFileLevel(std::string filename, RoomList &ret);
bool trySetRoom(ROOM_TYPE roomType, char *roomOut);
bool __stdcall customRoomGet(int doorRoomType, int roomIndex, char *roomOut,
                             LevelState *levelState);
bool __stdcall customRoomGet2(int doorRoomType, int roomIndex, char *roomOut,
                              LevelState *levelState);
void __stdcall preSpawnRoomsHook(LevelState *levelState);
void __declspec() spawnLevelTilesHook();