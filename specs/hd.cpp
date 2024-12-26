#include <format>
#include <string>

#include "3rdparty/imgui/imgui.h"

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

DWORD gSpawnHiredHandOffset;

EntityPlayer *GlobalState::SpawnHiredHand(float x, float y,
                                          uint32_t textureId) {

  using SpawnHiredHandPtr =
      DWORD(__stdcall *)(DWORD thisPtr, float x, float y, uint32_t textureId);
  SpawnHiredHandPtr SpawnHiredHand = (SpawnHiredHandPtr)(gSpawnHiredHandOffset);
  DWORD entityAddress = SpawnHiredHand((DWORD)this, x, y, textureId);
  if (entityAddress == NULL) {
    return NULL;
  }

  return reinterpret_cast<EntityPlayer *>(entityAddress);
}

DWORD gGenerateRoomOffset;
void GenerateRoom(int32_t entrance_or_exit, LevelState *level_state,
                  int32_t x_start, int32_t y_start, int32_t room_idx) {
  using GenerateRoomPtr = void(__fastcall *)(
      void *nothing, int32_t entrance_or_exit, LevelState *level_state,
      int32_t x_start, int32_t y_start, int32_t room_idx);
  GenerateRoomPtr GenerateRoom = (GenerateRoomPtr)(gGenerateRoomOffset);
  GenerateRoom(NULL, entrance_or_exit, level_state, x_start, y_start, room_idx);
}

// Entity
DWORD gEntityPlaySoundOffset;

const char *Entity::KindName() {
  switch (this->entity_kind) {
  case EntityKind::KIND_FLOOR:
    return "Floor";
  case EntityKind::KIND_ACTIVE:
    return "Active";
  case EntityKind::KIND_PLAYER:
    return "Player";
  case EntityKind::KIND_MONSTER:
    return "Monster";
  case EntityKind::KIND_ITEM:
    return "Item";
  case EntityKind::KIND_BACKGROUND:
    return "Background";
  case EntityKind::KIND_EXPLOSION:
    return "Explosion";
  case EntityKind::KIND_ENTITY:
    return "Entity";
  default:
    return "Unknown";
  }
}

const char *Entity::TypeName() { return EntityTypeName(this->entity_type); }

const char *EntityTypeName(uint32_t entity_type) {
  switch (entity_type) {
  case 0:
    return "Player/HH";
  case 11:
    return "Rib Cage";
  case 42:
    return "Ceiling Trap";
  case 43:
    return "Trap Door";
  case 44:
    return "Spring Trap";
  case 45:
    return "Crush Trap";
  case 92:
    return "Landmine";
  case 100:
    return "Chest";
  case 101:
    return "Crate";
  case 102:
    return "Gold Bar";
  case 103:
    return "Gold Pyramid";
  case 104:
    return "Emerald Large";
  case 105:
    return "Sapphire Large";
  case 106:
    return "Ruby Large";
  case 107:
    return "Live Bomb";
  case 108:
    return "Deploy Rope";
  case 109:
    return "Whip";
  case 110:
    return "Blood";
  case 111:
    return "Dirt Break";
  case 112:
    return "Rock";
  case 113:
    return "Pot";
  case 114:
    return "Skull";
  case 115:
    return "Cobweb";
  case 116:
    return "Sticky Honey";
  case 117:
    return "Bullet";
  case 118:
    return "Gold Nugget Large";
  case 120:
    return "Boulder";
  case 121:
    return "Push Block";
  case 122:
    return "Arrow";
  case 124:
    return "Gold Nugget Small";
  case 125:
    return "Emerald Small";
  case 126:
    return "Sapphire Small";
  case 127:
    return "Ruby Small";
  case 137:
    return "Rope Segment";
  case 142:
    return "Cobweb Projectile";
  case 153:
    return "Udjat Chest";
  case 154:
    return "Golden Key";
  case 156:
    return "Used Parachute";
  case 157:
    return "Tiki Spikes";
  case 158:
    return "Static Swing Attack Projectile";
  case 159:
    return "Psychic Attack Bubbling";
  case 160:
    return "UFO Projectile";
  case 161:
    return "Blue Falling Platform";
  case 162:
    return "Lantern";
  case 163:
    return "Flare";
  case 164:
    return "Snowball";
  case 165:
    return "Vomit Fly";
  case 171:
    return "White Flag";
  case 172:
    return "Piranha Skeleton";
  case 173:
    return "Diamond";
  case 174:
    return "Worm Tongue";
  case 176:
    return "Magma Cauldron";
  case 177:
    return "Wide Light Emitter";
  case 178:
    return "Spike Ball Detached";
  case 179:
    return "Breaking Chain Projectile";
  case 180:
    return "Tutorial Journal";
  case 181:
    return "Journal Page";
  case 182:
    return "Worm Regen Block";
  case 183:
    return "Cracking Ice Platform";
  case 184:
    return "Leaf";
  case 187:
    return "Decoy Chest";
  case 188:
    return "Prize Wheel";
  case 189:
    return "Prize Wheel Pin";
  case 190:
    return "Prize Wheel Barricade";
  case 192:
    return "Snail Bubble";
  case 193:
    return "Cobra Venom Projectile";
  case 194:
    return "Falling Icicle Projectile";
  case 195:
    return "Broken Ice Projectiles";
  case 196:
    return "Splashing Water Projectile";
  case 197:
    return "Forcefield Ground Laser";
  case 198:
    return "Forcefield Laser";
  case 203:
    return "Freeze Ray Projectile";
  case 204:
    return "Plasma Cannon Projectile/Shot";
  case 210:
    return "Mattock Head";
  case 211:
    return "Coffin";
  case 213:
    return "Turret Projectile";
  case 214:
    return "Mothership Platform";
  case 215:
    return "Mothership Elevator";
  case 216:
    return "Arrow Shaft";
  case 217:
    return "Olmec Enemy Spawn Projectile";
  case 218:
    return "Splashing Water";
  case 220:
    return "Ball & Chain without Chain";
  case 221:
    return "Smoke Poof";
  case 224:
    return "Ending Cutscene Camel";
  case 225:
    return "Kill Target";
  case 226:
    return "Activated Kill Target Laser";
  case 227:
    return "Mothership Lights";
  case 228:
    return "Broken Web Pouch";
  case 232:
    return "Breaking Animation";
  case 233:
    return "Magma Flame Animation";
  case 234:
    return "Anubis II Spawner";
  case 235:
    return "TNT";
  case 236:
    return "Spinner Spider Thread";
  case 237:
    return "Destroyed Cobweb";
  case 239:
    return "Decoy Yang";
  case 240:
    return "Zero-Value Gold Nugget";
  case 243:
    return "Lava/Water Spout";
  case 245:
    return "Mounted Lightable Torch Holder";
  case 246:
    return "Unlit Torch";
  case 247:
    return "Purple Target";
  case 248:
    return "Mystery Box";
  case 249:
    return "Alien Queen Corpse";
  case 250:
    return "Crowned Skull";
  case 252:
    return "Eggplant";
  case 301:
    return "Exploding Animation";
  case 302:
    return "Laser Effect / Jetpack Flame";
  case 303:
    return "Small Light";
  case 304:
    return "Spring Rings";
  case 305:
    return "Teleport Effect";
  case 306:
    return "Wall Torch Flame";
  case 307:
    return "Extinguished Torch Animation";
  case 500:
    return "Rope Pile";
  case 501:
    return "Bomb Bag";
  case 502:
    return "Bomb Box";
  case 503:
    return "Spectacles";
  case 504:
    return "Climbing Gloves";
  case 505:
    return "Pitcher's Mitt";
  case 506:
    return "Spring Shoes";
  case 507:
    return "Spike Shoes";
  case 508:
    return "Bomb Paste";
  case 509:
    return "Compass";
  case 510:
    return "Mattock";
  case 511:
    return "Boomerang";
  case 512:
    return "Machete";
  case 513:
    return "Crysknife";
  case 514:
    return "Web Gun";
  case 515:
    return "Shotgun";
  case 516:
    return "Freeze Ray";
  case 517:
    return "Plasma Cannon";
  case 518:
    return "Camera";
  case 519:
    return "Teleporter";
  case 520:
    return "Parachute";
  case 521:
    return "Cape";
  case 522:
    return "Jetpack";
  case 523:
    return "Shield";
  case 524:
    return "Queen Bee Royal Jelly";
  case 525:
    return "Idol";
  case 526:
    return "Kapala";
  case 527:
    return "Udjat Eye";
  case 528:
    return "Ankh";
  case 529:
    return "Hedjet";
  case 530:
    return "Scepter";
  case 531:
    return "Book of the Dead";
  case 532:
    return "Vlad's Cape";
  case 533:
    return "Vlad's Amulet";
  case 1001:
    return "Snake";
  case 1002:
    return "Spider";
  case 1003:
    return "Bat";
  case 1004:
    return "Caveman";
  case 1005:
    return "Damsel";
  case 1006:
    return "Shopkeeper";
  case 1007:
    return "Blue Frog";
  case 1008:
    return "Mantrap";
  case 1009:
    return "Yeti";
  case 1010:
    return "UFO";
  case 1011:
    return "Hawk Man";
  case 1012:
    return "Skeleton";
  case 1013:
    return "Piranha";
  case 1014:
    return "Mummy";
  case 1015:
    return "Monkey";
  case 1016:
    return "Alien Lord";
  case 1017:
    return "Ghost";
  case 1018:
    return "Giant Spider";
  case 1019:
    return "Jiang Shi";
  case 1020:
    return "Vampire";
  case 1021:
    return "Orange Frog";
  case 1022:
    return "Tunnel Man";
  case 1023:
    return "Old Bitey";
  case 1024:
    return "Golden Scarab";
  case 1025:
    return "Yeti King";
  case 1026:
    return "Little Alien";
  case 1027:
    return "Magma Man";
  case 1028:
    return "Vlad";
  case 1029:
    return "Scorpion";
  case 1030:
    return "Imp";
  case 1031:
    return "Blue Devil";
  case 1032:
    return "Bee";
  case 1033:
    return "Anubis";
  case 1034:
    return "Queen Bee";
  case 1035:
    return "Bacterium";
  case 1036:
    return "Cobra";
  case 1037:
    return "Spinner Spider";
  case 1038:
    return "Big Frog";
  case 1039:
    return "Mammoth";
  case 1040:
    return "Alien Tank";
  case 1041:
    return "Tiki Man";
  case 1042:
    return "Scorpion Fly";
  case 1043:
    return "Snail";
  case 1044:
    return "Croc Man";
  case 1045:
    return "Green Knight";
  case 1046:
    return "Worm Egg";
  case 1047:
    return "Worm Baby";
  case 1048:
    return "Alien Queen";
  case 1049:
    return "Black Knight";
  case 1050:
    return "Golden Monkey";
  case 1051:
    return "Succubus";
  case 1052:
    return "Horse Head";
  case 1053:
    return "Ox Face";
  case 1055:
    return "Olmec";
  case 1056:
    return "King Yama Head";
  case 1057:
    return "King Yama Fist";
  case 1058:
    return "Turret";
  case 1059:
    return "Blue Frog Critter";
  case 1060:
    return "Alien Queen Eye";
  case 1061:
    return "Spiderling Critter";
  case 1062:
    return "Fish Critter";
  case 1063:
    return "Rat Critter";
  case 1064:
    return "Penguin Critter";
  case 1065:
    return "Little Alien Horizontally Moving";
  case 1067:
    return "Locust Critter";
  case 1068:
    return "Maggot Critter";
  default:
    return "Unknown";
  }
}

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

DWORD gDestroyFloor;
bool DestroyFloor(LevelState *level_state, EntityFloor *floor) {

  bool val;
  __asm {
        pushad

        push level_state
        mov eax, floor
        call gDestroyFloor
        mov val, al

        popad
  }

  return val;
}

DWORD gResetForRun;
void resetForRun(GlobalState *global_state, int isDeathmatch) {

  __asm {
        pushad

        push isDeathmatch
        mov eax, global_state
        call gResetForRun

        popad
  }
}

DWORD gResetForLevel;
void resetForLevel(GlobalState *global_state) {

  __asm {
        pushad

        mov esi, global_state
        call gResetForLevel

        popad
  }
}

DWORD gLoadCoffinTexture;
void LoadCoffinTexture(_34Struct *_34_struct) {
  __asm {
        pushad

        mov esi, _34_struct
        call gLoadCoffinTexture

        popad
  }
}

DWORD gGetRoomForPosition;
uint32_t GetRoomForPosition(float x, float y) {
  using GetRoomForPositionPtr = uint32_t(__stdcall *)(float x, float y);
  GetRoomForPositionPtr GetRoomForPosition =
      (GetRoomForPositionPtr)(gGetRoomForPosition);
  return GetRoomForPosition(x, y);
}

DWORD gLoadTexture;
void LoadTexture(_34Struct *_34_struct, const char *texture_name) {
  using LoadTexturePtr =
      void(__stdcall *)(_34Struct * _34_struct, const char *texture_name);
  LoadTexturePtr LoadTexture = (LoadTexturePtr)(gLoadTexture);
  return LoadTexture(_34_struct, texture_name);
}

DWORD gMersenneInitAndTwist;
void mersenne_init_and_twist(uint32_t seed) {
  __asm {
      pushad

      mov eax, seed
      call gMersenneInitAndTwist

      popad
  }
}

DWORD gMersenneRandom;
uint32_t mersenne_random() {
  uint32_t val;

  __asm {
      pushad

      call gMersenneRandom
      mov val, eax

      popad
  }

  return val;
};

void setupOffsets(DWORD baseAddress) {
  gSpawnEntityOffset = baseAddress + 0x70AB0;
  gSpawnHiredHandOffset = baseAddress + 0x65330;

  gEntityPlaySoundOffset = baseAddress + 0x16A95;

  gPlayMusic = baseAddress + 0x9920;

  gDestroyFloor = baseAddress + 0xdf2c0;

  gGetRoomForPosition = baseAddress + 0x0CC760;

  gLoadTexture = baseAddress + 0xef350;
  gLoadCoffinTexture = baseAddress + 0xeef60;

  gGenerateRoomOffset = baseAddress + 0xd6690;

  gMersenneInitAndTwist = baseAddress + 0x11420;
  gMersenneRandom = baseAddress + 0x07d10;

  gResetForLevel = baseAddress + 0x64ee0;
  gResetForRun = baseAddress + 0x649e0;
}

TextureId charIdToTextureId(CharacterIndex id) {
  int32_t new_id = id + 50;
  if (new_id >= 62) {
    new_id = new_id + 1000;
  }
  return (TextureId)new_id;
}

ImU32 charIdToColor(CharacterIndex id, float alpha = 1.f) {
  switch (id) {
  case CHARACTER_GUY:
    return ImGui::GetColorU32({1.00f, 0.58f, 0.03f, alpha});
  case CHARACTER_RED:
    return ImGui::GetColorU32({1.00f, 0.07f, 0.07f, alpha});
  case CHARACTER_GREEN:
    return ImGui::GetColorU32({0.34f, 0.92f, 0.12f, alpha});
  case CHARACTER_BLUE:
    return ImGui::GetColorU32({0.12f, 0.40f, 0.93f, alpha});
  case CHARACTER_YANG:
    return ImGui::GetColorU32({1.00f, 1.00f, 1.00f, alpha});
  case CHARACTER_MEATBOY:
    return ImGui::GetColorU32({1.00f, 0.46f, 0.45f, alpha});
  case CHARACTER_YELLOW:
    return ImGui::GetColorU32({0.94f, 0.96f, 0.22f, alpha});
  case CHARACTER_JUNGLE_WARRIOR:
    return ImGui::GetColorU32({0.51f, 0.31f, 0.14f, alpha});
  case CHARACTER_PURPLE:
    return ImGui::GetColorU32({0.71f, 0.23f, 0.90f, alpha});
  case CHARACTER_VAN_HELSING:
    return ImGui::GetColorU32({0.20f, 0.20f, 0.20f, alpha});
  case CHARACTER_CYAN:
    return ImGui::GetColorU32({0.47f, 1.00f, 1.00f, alpha});
  case CHARACTER_LIME:
    return ImGui::GetColorU32({0.73f, 0.88f, 0.00f, alpha});
  case CHARACTER_INUK:
    return ImGui::GetColorU32({0.92f, 0.16f, 0.40f, alpha});
  case CHARACTER_ROUND_GIRL:
    return ImGui::GetColorU32({0.27f, 0.06f, 0.43f, alpha});
  case CHARACTER_NINJA:
    return ImGui::GetColorU32({0.23f, 0.38f, 0.00f, alpha});
  case CHARACTER_VIKING:
    return ImGui::GetColorU32({0.67f, 0.67f, 0.67f, alpha});
  case CHARACTER_ROUND_BOY:
    return ImGui::GetColorU32({0.24f, 0.83f, 0.62f, alpha});
  case CHARACTER_CARL:
    return ImGui::GetColorU32({1.00f, 0.29f, 0.05f, alpha});
  case CHARACTER_ROBOT:
    return ImGui::GetColorU32({0.27f, 0.27f, 0.27f, alpha});
  case CHARACTER_MONK:
    return ImGui::GetColorU32({1.00f, 0.90f, 0.22f, alpha});
  }
  return ImGui::GetColorU32({1.f, 1.f, 1.f, alpha});
}

CharacterIndex TextureIdToCharId(TextureId id) {
  int32_t new_id = id - 50;
  if (new_id > 1000) {
    new_id = new_id - 1000;
  }
  return (CharacterIndex)new_id;
}