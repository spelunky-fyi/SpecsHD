#pragma once

#include <stdint.h>

#include "Windows.h"

enum class Ownership : int32_t {
  Unowned = -99,
  HiredHand = -1,
  Player1 = 0,
  Player2 = 1,
  Player3 = 2,
  Player4 = 3
};

enum class EntityKind : int32_t {
  KIND_ENTITY = -99,
  KIND_FLOOR = 0,
  KIND_ACTIVE = 1,
  KIND_PLAYER = 2,
  KIND_MONSTER = 3,
  KIND_ITEM = 4,
  KIND_BACKGROUND = 5,
  KIND_EXPLOSION = 6
};

class Entity {
public:
  char pad_0000[8];              // 0x0000
  EntityKind N00000992;          // 0x0008
  uint32_t entity_type;          // 0x000C
  uint32_t bin_x;                // 0x0010
  uint32_t bin_y;                // 0x0014
  Ownership owner;               // 0x0018
  char pad_001C[4];              // 0x001C
  int32_t N00000997;             // 0x0020
  char pad_0024[8];              // 0x0024
  float N0000099B;               // 0x002C
  float x;                       // 0x0030
  float y;                       // 0x0034
  float width;                   // 0x0038
  float height;                  // 0x003C
  float current_z;               // 0x0040
  float original_z;              // 0x0044
  float N000009A2;               // 0x0048
  float hitbox_up;               // 0x004C
  float hitbox_down;             // 0x0050
  float hitbox_x;                // 0x0054
  float N000009A6;               // 0x0058
  float N000009A7;               // 0x005C
  float N000009A8;               // 0x0060
  float N000009A9;               // 0x0064
  float N000009AA;               // 0x0068
  float N000009AB;               // 0x006C
  float N000009AC;               // 0x0070
  float N000009AD;               // 0x0074
  float N000009AE;               // 0x0078
  float N000009AF;               // 0x007C
  float N000009B0;               // 0x0080
  float N000009B1;               // 0x0084
  float N000009B2;               // 0x0088
  float N000009B3;               // 0x008C
  float N000009B4;               // 0x0090
  float N00000DA9;               // 0x0094
  float N000009B7;               // 0x0098
  uint8_t deletion_flag;         // 0x009C
  uint8_t horizontal_flip;       // 0x009D
  uint8_t N00000DAD;             // 0x009E
  uint8_t N000009B7b;            // 0x009F
  uint8_t N000009B9;             // 0x00A0
  uint8_t N000017EF;             // 0x00A1
  uint8_t N000017FE;             // 0x00A2
  uint8_t N000017F0;             // 0x00A3
  uint8_t N000009BA;             // 0x00A4
  uint8_t N000017F2;             // 0x00A5
  uint8_t N00001801;             // 0x00A6
  uint8_t N000017F3;             // 0x00A7
  uint8_t N000009BB;             // 0x00A8
  uint8_t N000017F5;             // 0x00A9
  uint8_t N00001804;             // 0x00AA
  uint8_t N000017F6;             // 0x00AB
  uint8_t translucent;           // 0x00AC
  uint8_t invisible;             // 0x00AD
  uint8_t N00001807;             // 0x00AE
  uint8_t N000017F9;             // 0x00AF
  uint8_t brighten_maybe;        // 0x00B0
  uint8_t N000017FB;             // 0x00B1
  uint8_t eviscerate;            // 0x00B2
  uint8_t N000017FC;             // 0x00B3
  class Texture *texture_info;   // 0x00B4
  char pad_00B8[4];              // 0x00B8
  Entity *embedded_deco;         // 0x00BC
  Entity *top_deco;              // 0x00C0
  Entity *bottom_deco;           // 0x00C4
  Entity *left_deco;             // 0x00C8
  Entity *right_deco;            // 0x00CC
  char pad_00D0[100];            // 0x00D0
  uint32_t animation_type;       // 0x0134
  uint32_t animation_frame;      // 0x0138
  uint32_t N00000DB3;            // 0x013C
  int32_t health;                // 0x0140
  char pad_0144[8];              // 0x0144
  uint32_t favor_given;          // 0x014C
  char pad_0150[136];            // 0x0150
  float max_run_velocity_x;      // 0x01D8
  float N00000B24;               // 0x01DC
  float jump_multiplier;         // 0x01E0
  char pad_01E4[28];             // 0x01E4
  uint8_t N00000B2D;             // 0x0200
  uint8_t N0007A094;             // 0x0201
  uint8_t N0007A097;             // 0x0202
  uint8_t N0007A095;             // 0x0203
  uint8_t on_floor;              // 0x0204
  uint8_t N0007A02A;             // 0x0205
  uint8_t ducking;               // 0x0206
  uint8_t ledge_hanging;         // 0x0207
  char pad_0208[4];              // 0x0208
  uint8_t N00000B30;             // 0x020C
  uint8_t N0007A023;             // 0x020D
  uint8_t N0007A026;             // 0x020E
  uint8_t N0007A024;             // 0x020F
  uint8_t N00000B31;             // 0x0210
  uint8_t N0007A08F;             // 0x0211
  char pad_0212[30];             // 0x0212
  class Entity *holder_entity;   // 0x0230
  class Entity *holding_entity;  // 0x0234
  class Entity *backitem_entity; // 0x0238
  char pad_023C[8];              // 0x023C
  float velocity_x;              // 0x0244
  float velocity_y;              // 0x0248
  char pad_024C[72];             // 0x024C
  class N0008895B *N00000DD4;    // 0x0294
  char pad_0298[176];            // 0x0298
};

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
  uint32_t hh_count;            // 0x0020
  uint32_t N00008C20[8];        // 0x0024
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

class LevelState {
public:
  Entity *entity_backgrounds[512];   // 0x0000
  uint32_t entity_backgrounds_count; // 0x0800
  uint32_t N0000256C;                // 0x0804
  Entity *entity_items[720];         // 0x0808
  uint32_t entity_items_count;       // 0x1348
  Entity *entity_floors[4692];       // 0x134C
  Entity *entity_floors_bg[4692];    // 0x5C9C
  uint32_t room_types[48];           // 0xA5EC
  char pad_A6AC[25528];              // 0xA6AC
  uint32_t room_types2[48];          // 0x10A64
  char pad_10B24[26252];             // 0x10B24
};                                   // Size: 0x171B0
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
  uint32_t N0007A2EB;                                     // 0x7814
  Entity *entities_active_by_bin[312][128];               // 0x7818
  uint32_t entities_active_by_bin_count[312];             // 0x2E818
  uint32_t entities_active_pushable_by_bin[312];          // 0x2ECF8
  Entity *entities_active_pushable_by_bin_count[312][24]; // 0x2F1D8
};                                                        // Size: 0x366D8
static_assert(sizeof(EntityStruct) == 0x366D8);

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
  void *textures;                                     // 0x0034
  void *_38struct;                                    // 0x0038
  LevelState *level_state;                            // 0x003C
  class Controls *controls;                           // 0x0040
  class FmodReverbProperties *fmod_reverb_properties; // 0x0044
  class Particles *particles;                         // 0x0048
  void *_4cstruct;                                    // 0x004C
  void *_50struct;                                    // 0x0050
  char pad_0054[4];                                   // 0x0054
  uint32_t screen_state;                              // 0x0058
  uint32_t play_state;                                // 0x005C
  char pad_0060[72];                                  // 0x0060
  uint32_t flag_player;                               // 0x00A8
  char pad_00AC[24];                                  // 0x00AC
  uint16_t N000001A7;                                 // 0x00C4
  wchar_t level_name[640];                            // 0x00C6
  class StringEntry string_table[2048];               // 0x05C6
  uint16_t padding;                                   // 0x4405C6
  uint32_t N0007A29D;                                 // 0x4405C8
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
  uint8_t N00000FB0;                                  // 0x440609
  uint8_t N00000FDD;                                  // 0x44060A
  uint8_t N00000FB1;                                  // 0x44060B
  uint8_t N000002D9;                                  // 0x44060C
  uint8_t N00000FB3;                                  // 0x44060D
  uint8_t N00000FE0;                                  // 0x44060E
  uint8_t N00000FB4;                                  // 0x44060F
  uint8_t N000002DA;                                  // 0x440610
  uint8_t N00000FB6;                                  // 0x440611
  uint8_t N00000FE3;                                  // 0x440612
  uint8_t N00000FB7;                                  // 0x440613
  uint8_t N000002DB;                                  // 0x440614
  uint8_t N00000FB9;                                  // 0x440615
  uint8_t N00000FE6;                                  // 0x440616
  uint8_t N00000FBA;                                  // 0x440617
  uint8_t N000002DC;                                  // 0x440618
  uint8_t N00000FBC;                                  // 0x440619
  uint8_t N00000FE9;                                  // 0x44061A
  uint8_t N00000FBD;                                  // 0x44061B
  uint8_t N000002DD;                                  // 0x44061C
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
  char pad_44062C[88];                                // 0x44062C
  Entity *player1;                                    // 0x440684
  Entity *player2;                                    // 0x440688
  Entity *player3;                                    // 0x44068C
  Entity *player4;                                    // 0x440690
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
  char pad_44602A[126];         // 0x44602A

  Entity *SpawnEntity(float x, float y, uint32_t entity_type, bool active);

}; // Size: 0x4460A8
static_assert(sizeof(GlobalState) == 0x4460A8);

void setupOffsets(DWORD baseAddress);