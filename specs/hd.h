#pragma once

#include <stdint.h>

#include "Windows.h"
#include "hd_entity.h"

class CameraState {
public:
  float camera_x;           // 0x0000
  float camera_y;           // 0x0004
  float N00000E4A;          // 0x0008
  float following_x;        // 0x000C
  float following_y;        // 0x0010
  float N00000E4D;          // 0x0014
  char pad_0018[8];         // 0x0018
  float camera_max_left;    // 0x0020
  float camera_max_right;   // 0x0024
  float camera_max_down;    // 0x0028
  float camera_max_up;      // 0x002C
  Entity *camera_following; // 0x0030
  float N00000E55;          // 0x0034
  float camera_speed;       // 0x0038
  char pad_003C[32];        // 0x003C
};

enum CharacterIndex : int32_t {
  CHARACTER_GUY = 0,
  CHARACTER_RED = 1,
  CHARACTER_GREEN = 2,
  CHARACTER_BLUE = 3,
  CHARACTER_YANG = 4,
  CHARACTER_MEATBOY = 5,
  CHARACTER_YELLOW = 6,
  CHARACTER_JUNGLE_WARRIOR = 7,
  CHARACTER_PURPLE = 8,
  CHARACTER_VAN_HELSING = 9,
  CHARACTER_CYAN = 10,
  CHARACTER_LIME = 11,
  CHARACTER_INUK = 12,
  CHARACTER_ROUND_GIRL = 13,
  CHARACTER_NINJA = 14,
  CHARACTER_VIKING = 15,
  CHARACTER_ROUND_BOY = 16,
  CHARACTER_CARL = 17,
  CHARACTER_ROBOT = 18,
  CHARACTER_MONK = 19,
};

enum TextureId : int32_t {
  TEXTURE_ID_GUY = 50,            // char_orange.png
  TEXTURE_ID_RED = 51,            // char_red.png
  TEXTURE_ID_GREEN = 52,          // char_green.png
  TEXTURE_ID_BLUE = 53,           // char_blue.png
  TEXTURE_ID_YANG = 54,           // char_white.png
  TEXTURE_ID_MEATBOY = 55,        // char_pink.png
  TEXTURE_ID_YELLOW = 56,         // char_yellow.png
  TEXTURE_ID_JUNGLE_WARRIOR = 57, // char_brown.png
  TEXTURE_ID_PURPLE = 58,         // char_purple.png
  TEXTURE_ID_VAN_HELSING = 59,    // char_black.png
  TEXTURE_ID_CYAN = 60,           // char_cyan.png
  TEXTURE_ID_LIME = 61,           // char_lime.png
  TEXTURE_ID_INUK = 1062,         // char_dlc1.png
  TEXTURE_ID_ROUND_GIRL = 1063,   // char_dlc2.png
  TEXTURE_ID_NINJA = 1064,        // char_dlc3.png
  TEXTURE_ID_VIKING = 1065,       // char_dlc4.png
  TEXTURE_ID_ROUND_BOY = 1066,    // char_dlc5.png
  TEXTURE_ID_CARL = 1067,         // char_dlc6.png
  TEXTURE_ID_ROBOT = 1068,        // char_dlc7.png
  TEXTURE_ID_MONK = 1069,         // char_dlc8.png
};

enum class DamselChoice : int32_t {
  Damsel = 0,
  Mansel = 1,
  Pug = 2,
  Random = 3
};

enum class Language : int32_t {
  English = 0,
  French = 1,
  Italian = 2,
  German = 3,
  Spanish = 4,
  Portuguese = 5,
  Japanese = 6,
  Russian = 7,
  SimplifiedChinese = 8,
  TraditionalChinese = 9,
  Korean = 10
};

class StringEntry {
public:
  wchar_t N00000779[64];   // 0x0000
  wchar_t N0000087A[1024]; // 0x0080
};                         // Size: 0x0880

class TextureDefinition {
public:
  int32_t texture_id;     // 0x0000
  char name[128];         // 0x0004
  char name_normal[128];  // 0x0084
  char pad_0104[8];       // 0x0104
  uint32_t sheet_width;   // 0x010C
  uint32_t sheet_height;  // 0x0110
  uint32_t sprite_width;  // 0x0114
  uint32_t sprite_height; // 0x0118
  uint32_t loaded;        // 0x011C
};                        // Size: 0x0120
static_assert(sizeof(TextureDefinition) == 0x120);

class PlayerData {
public:
  int32_t health;               // 0x0000
  int32_t health2;              // 0x0004
  uint32_t N00008C19;           // 0x0008
  uint32_t N00008C1A;           // 0x000C
  int32_t bombs;                // 0x0010
  int32_t ropes;                // 0x0014
  uint32_t N00008C1D;           // 0x0018
  char pad_001C[4];             // 0x001C
  int32_t hh_count;             // 0x0020
  uint32_t hh_texture_id[8];    // 0x0024
  uint32_t N00008C28[8];        // 0x0044
  char pad_0064[8];             // 0x0064
  float classic_HUD_heart_size; // 0x006C
  bool has_compass;             // 0x0070
  bool has_parachute;           // 0x0071
  bool has_jetpack;             // 0x0072
  bool has_climbing_gloves;     // 0x0073
  bool has_pitchers_mitt;       // 0x0074
  bool has_spring_shoes;        // 0x0075
  bool has_spike_shoes;         // 0x0076
  bool has_spectacles;          // 0x0077
  bool has_kapala;              // 0x0078
  bool has_hedjet;              // 0x0079
  bool has_udjat;               // 0x007A
  bool has_book_of_dead;        // 0x007B
  bool has_ankh;                // 0x007C
  bool has_paste;               // 0x007D
  bool has_cape;                // 0x007E
  bool has_vlads_cape;          // 0x007F
  bool has_crysknife;           // 0x0080
  bool has_vlads_amulet;        // 0x0081
  bool has_white_flag;          // 0x0082
  int8_t padding;               // 0x0083
  int32_t kapala_charge;        // 0x0084
  int32_t held_item_id;         // 0x0088
  int32_t held_item_metadata;   // 0x008C
  int32_t total_kills;          // 0x0090
  int32_t total_damsel_saves;   // 0x0094
  int32_t kill_ids[256];        // 0x0098
  int32_t loot_ids[1024];       // 0x0498
  int32_t level_kills;          // 0x1498
  int32_t level_looted_count;   // 0x149C
  char pad_14A0[4];             // 0x14A0
};                              // Size: 0x14A4

class DeathmatchLevel {
public:
  char tiles[241];     // 0x0000
  uint8_t padding;     // 0x00F1
  wchar_t name[128];   // 0x00F2
  uint8_t padding2[2]; // 0x01F2
  uint32_t unknown1;   // 0x01F4
  uint32_t unknown2;   // 0x01F8
};                     // Size: 0x01FC
static_assert(sizeof(DeathmatchLevel) == 0x1FC);

class DeathmatchArea {
public:
  wchar_t name[128];         // 0x0000
  DeathmatchLevel levels[8]; // 0x0100
  uint32_t unlocked;         // 0x10E0
};                           // Size: 0x10E4
static_assert(sizeof(DeathmatchArea) == 0x10E4);

class LevelState {
public:
  EntityBackground *entity_backgrounds[512]; // 0x0000
  uint32_t entity_backgrounds_count;         // 0x0800
  uint32_t unknown;                          // 0x0804
  Entity *entity_items[720];                 // 0x0808
  uint32_t entity_items_count;               // 0x1348
  EntityFloor *entity_floors[4692];          // 0x134C
  EntityFloor *entity_floors_bg[4692];       // 0x5C9C
  uint32_t room_types[48];                   // 0xA5EC
  DeathmatchArea deathmatch_areas[9];        // 0xA6AC
  uint8_t unused[12972];                     // 0x13EB0
  class GlobalState *global_state;           // 0x1715C
  uint32_t shop_type;                        // 0x17160
  uint32_t entrance_room_x;                  // 0x17164
  uint32_t entrance_room_y;                  // 0x17168
  uint32_t exit_room_x;                      // 0x1716C
  uint32_t exit_room_y;                      // 0x17170
  float dm_spawn_p1_x;                       // 0x17174
  float dm_spawn_p1_y;                       // 0x17178
  float dm_spawn_p2_x;                       // 0x1717C
  float dm_spawn_p2_y;                       // 0x17180
  float dm_spawn_p3_x;                       // 0x17184
  float dm_spawn_p3_y;                       // 0x17188
  float dm_spawn_p4_x;                       // 0x1718C
  float dm_spawn_p4_y;                       // 0x17190
  float entrance_x;                          // 0x17194
  float entrance_y;                          // 0x17198
  float exit_x;                              // 0x1719C
  float exit_y;                              // 0x171A0
  float alt_exit_x;                          // 0x171A4
  float alt_exit_y;                          // 0x171A8
  uint32_t dm_num_crates_spawned;            // 0x171AC
};                                           // Size: 0x171B0
static_assert(sizeof(LevelState) == 0x171B0);

class EntityStruct {
public:
  class Entity *entities_active[1280];                    // 0x0000
  class Entity *array_1400[3072];                         // 0x1400
  class Entity *entities_light_emitting[1280];            // 0x4400
  class Entity *entities_foreground[2048];                // 0x5800
  uint32_t array_entities_foreground_count;               // 0x7800
  uint32_t N0007A2E7;                                     // 0x7804
  uint32_t entities_light_emitting_count;                 // 0x7808
  uint32_t array_1400_count;                              // 0x780C
  uint32_t entities_active_count;                         // 0x7810
  uint32_t angered_shopkeeper_count;                      // 0x7814
  Entity *entities_active_by_bin[312][128];               // 0x7818
  uint32_t entities_active_by_bin_count[312];             // 0x2E818
  uint32_t entities_active_pushable_by_bin[312];          // 0x2ECF8
  Entity *entities_active_pushable_by_bin_count[312][24]; // 0x2F1D8
};                                                        // Size: 0x366D8
static_assert(sizeof(EntityStruct) == 0x366D8);

class _4cStruct {
public:
  Entity *entities[160];
};

class _34Struct {
public:
  TextureDefinition texture_defs[256]; // 0x0000
  char pad_12000[1060];                // 0x12000
  uint32_t char_id_to_texture_id[20];  // 0x12424
  char pad_12474[16];                  // 0x12474
  int32_t coffin_char;                 // 0x12484
};
static_assert(sizeof(_34Struct) == 0x12488);

class GlobalState {
public:
  char pad_0000[4];             // 0x0000
  DamselChoice settings_damsel; // 0x0004
  DamselChoice N00000178;       // 0x0008
  Language lang1;               // 0x000C
  Language
      lang2; // 0x0010 Not sure why two languages. both updated as same time.
  char pad_0014[28];                                  // 0x0014
  EntityStruct *entities;                             // 0x0030
  _34Struct *_34struct;                               // 0x0034
  void *_38struct;                                    // 0x0038
  LevelState *level_state;                            // 0x003C
  class Controls *controls;                           // 0x0040
  class FmodReverbProperties *fmod_reverb_properties; // 0x0044
  class Particles *particles;                         // 0x0048
  _4cStruct *_4cstruct;                               // 0x004C
  void *_50struct;                                    // 0x0050
  char pad_0054[4];                                   // 0x0054
  uint32_t screen_state;                              // 0x0058
  uint32_t play_state;                                // 0x005C
  char pad_0060[8];                                   // 0x0060
  uint8_t pause_update;                               // 0x0068
  char pad_0069[63];                                  // 0x0069
  uint32_t flag_player;                               // 0x00A8
  char pad_00AC[24];                                  // 0x00AC
  uint16_t N000001A7;                                 // 0x00C4
  wchar_t level_name[640];                            // 0x00C6
  class StringEntry string_table[2048];               // 0x05C6
  uint16_t padding;                                   // 0x4405C6
  uint8_t N0007A29D;                                  // 0x4405C8
  uint8_t N0005953F;                                  // 0x4405C9
  uint8_t redraw_tiles;                               // 0x4405CA
  uint8_t N00059540;                                  // 0x4405CB
  uint32_t N0007A29F;                                 // 0x4405CC
  char pad_4405D0[4];                                 // 0x4405D0
  uint32_t level;                                     // 0x4405D4
  uint32_t level_track;                               // 0x4405D8
  uint32_t N0007A29A;                                 // 0x4405DC
  uint32_t N0007A29B;                                 // 0x4405E0
  uint8_t dark_level;                                 // 0x4405E4
  uint8_t altar_spawned;                              // 0x4405E5
  uint8_t idol_spawned;                               // 0x4405E6
  uint8_t damsel_spawned;                             // 0x4405E7
  uint8_t unknown_flag;                               // 0x4405E8
  uint8_t moai_unopened;                              // 0x4405E9
  uint8_t moai_broke_in;                              // 0x4405EA
  uint8_t ghost_spawned;                              // 0x4405EB
  uint8_t rescued_damsel;                             // 0x4405EC
  uint8_t shopkeeper_triggered;                       // 0x4405ED
  uint8_t area_had_dark_level;                        // 0x4405EE
  uint8_t level_has_udjat;                            // 0x4405EF
  uint8_t has_spawned_udjat;                          // 0x4405F0
  uint8_t unused_flag;                                // 0x4405F1
  uint8_t vault_spawned_in_area;                      // 0x4405F2
  uint8_t flooded_mines;                              // 0x4405F3
  uint8_t skin_is_crawling;                           // 0x4405F4
  uint8_t dead_are_restless;                          // 0x4405F5
  uint8_t rushing_water;                              // 0x4405F6
  uint8_t is_haunted_castle;                          // 0x4405F7
  uint8_t at_haunted_castle_exit;                     // 0x4405F8
  uint8_t tiki_village;                               // 0x4405F9
  uint8_t spawned_black_market_entrance;              // 0x4405FA
  uint8_t unused_flag2;                               // 0x4405FB
  uint8_t spawned_haunted_castle_entrance;            // 0x4405FC
  uint8_t mothership_spawned;                         // 0x4405FD
  uint8_t moai_spawned;                               // 0x4405FE
  uint8_t is_blackmarket;                             // 0x4405FF
  uint8_t at_blackmarket_exit;                        // 0x440600
  uint8_t is_wet_fur;                                 // 0x440601
  uint8_t is_mothership;                              // 0x440602
  uint8_t at_mothership_exit;                         // 0x440603
  uint8_t is_city_of_gold;                            // 0x440604
  uint8_t at_city_of_gold_exit;                       // 0x440605
  uint8_t is_worm;                                    // 0x440606
  uint8_t N00000886;                                  // 0x440607
  uint8_t N000002D8;                                  // 0x440608
  uint8_t placed_coffin;                              // 0x440609
  uint8_t is_snow_level;                              // 0x44060A
  uint8_t N00000FB1;                                  // 0x44060B
  uint32_t wanted_level;                              // 0x44060C
  uint8_t shopkeeper_music_triggered;                 // 0x440610
  uint8_t N00000FB6;                                  // 0x440611
  uint8_t N00000FE3;                                  // 0x440612
  uint8_t N00000FB7;                                  // 0x440613
  uint8_t N000002DB;                                  // 0x440614
  uint8_t N00000FB9;                                  // 0x440615
  uint8_t N00000FE6;                                  // 0x440616
  uint8_t N00000FBA;                                  // 0x440617
  uint32_t punishment_amount;                         // 0x440618
  uint8_t broke_altar_this_level;                     // 0x44061C
  uint8_t N00000FBF;                                  // 0x44061D
  uint8_t N00000FEC;                                  // 0x44061E
  uint8_t N00000FC0;                                  // 0x44061F
  uint8_t N000002DE;                                  // 0x440620
  uint8_t N00000FFE;                                  // 0x440621
  uint8_t N00001007;                                  // 0x440622
  uint8_t N00000FFF;                                  // 0x440623
  uint8_t N000002DF;                                  // 0x440624
  uint8_t N00001001;                                  // 0x440625
  uint8_t N0000100A;                                  // 0x440626
  uint8_t N00001002;                                  // 0x440627
  uint8_t N000002E0;                                  // 0x440628
  uint8_t N00001004;                                  // 0x440629
  uint8_t N0000100D;                                  // 0x44062A
  uint8_t N00001005;                                  // 0x44062B
  char pad_44062C[52];                                // 0x44062C
  float insertion_point;                              // 0x440660
  char pad_440664[32];                                // 0x440664
  EntityPlayer *player1;                              // 0x440684
  EntityPlayer *player2;                              // 0x440688
  EntityPlayer *player3;                              // 0x44068C
  EntityPlayer *player4;                              // 0x440690
  PlayerData player1_data;                            // 0x440694
  PlayerData player2_data;                            // 0x441B38
  PlayerData player3_data;                            // 0x442FDC
  PlayerData player4_data;                            // 0x444480
  char pad_445924[28];                                // 0x445924
  uint32_t total_minutes;                             // 0x445940
  uint32_t total_seconds;                             // 0x445944
  double total_ms;                                    // 0x445948
  uint32_t level_minutes;                             // 0x445950
  uint32_t level_seconds;                             // 0x445954
  double level_ms;                                    // 0x445958
  uint32_t prev_level_ms_total; // 0x445960 Actual ms trimmed
  uint32_t total_ms_snapshot;   // 0x445964
  char pad_445968[1728];        // 0x445968
  uint8_t N000020E4;            // 0x446028
  uint8_t N0007A565;            // 0x446029
  char pad_44602A[4898];        // 0x44602A
  uint32_t respawn_level;       // 0x44734C
  char pad_447350[504];         // 0x447350

  Entity *SpawnEntity(float x, float y, uint32_t entity_type, bool active);
  void PlayOlmecMusic(const char *audioName);

}; // Size: 0x447548
static_assert(sizeof(GlobalState) == 0x447548);

uint32_t GetRoomForPosition(float x, float y);
bool DestroyFloor(LevelState *level_state, EntityFloor *floor);

void LoadTexture(_34Struct *_34_struct, const char *texture_name);
void LoadCoffinTexture(_34Struct *_34_struct);

void setupOffsets(DWORD baseAddress);

TextureId charIdToTextureId(CharacterIndex);
ImU32 charIdToColor(CharacterIndex, float);
CharacterIndex TextureIdToCharId(TextureId);
void GenerateRoom(int32_t entrance_or_exit, LevelState *level_state,
                  int32_t x_start, int32_t y_start, int32_t room_idx);
