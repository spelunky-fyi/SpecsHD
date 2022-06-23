
#include <Windows.h>
#include <algorithm>
#include <format>
#include <unordered_set>
#include <functional>

#include "3rdparty/imgui/imgui.h"

#include "hd.h"
#include "ui.h"

// Global States
DWORD gBaseAddress = NULL;
ImDrawList *gOverlayDrawList = NULL;

CameraState *gCameraState = NULL;
GlobalState *gGlobalState = NULL;
Entity* gSelectedEntity = NULL;

const size_t gNumAudioNames = 230;
const char *gAudioNames[gNumAudioNames] = {
    "alien_jump.wav",
    "alienexplosion.wav",
    "angry_kali.wav",
    "ani_squeak.wav",
    "ankhbreak.wav",
    "ankhflash.wav",
    "ankhflashback.wav",
    "anubisII_appear.wav",
    "anubisII_summons.wav",
    "armor_break.wav",
    "arrowhitwall.wav",
    "arrowshot.wav",
    "batflap.wav",
    "batoneflap.wav",
    "bee.wav",
    "bee2.wav",
    "bee3.wav",
    "blockfall.wav",
    "blocksmash.wav",
    "bomb_glue.wav",
    "bomb_timer.wav",
    "bone_shatter.wav",
    "boomerang_loop.wav",
    "bouldercoming.wav",
    "boulderhit.wav",
    "boulderhit2.wav",
    "boulderhit3.wav",
    "boulderhit4.wav",
    "bounce.wav",
    "bounce_light.wav",
    "camera.wav",
    "cape.wav",
    "catch_boomerang.wav",
    "chaching.wav",
    "char_unlock.wav",
    "chestopen.wav",
    "chime.wav",
    "chime3.wav",
    "chimp_attack.wav",
    "chimp_bounce.wav",
    "chute.wav",
    "coinsdrop.wav",
    "collect.wav",
    "crateopen.wav",
    "cricket.wav",
    "crushhit.wav",
    "crysknife.wav",
    "damsel_dog.wav",
    "damsel_female.wav",
    "damsel_male.wav",
    "damsel_sloth.wav",
    "damsel_water.wav",
    "demon.wav",
    "deposit.wav",
    "dm_go.wav",
    "dm_jump.wav",
    "dm_point.wav",
    "dm_ready.wav",
    "dm_winner.wav",
    "doorcrack.wav",
    "doorcrack2.wav",
    "doorglow.wav",
    "down.wav",
    "eggplant.wav",
    "end_chest.wav",
    "eruption.wav",
    "eyeblink.wav",
    "fadein.wav",
    "fadeout.wav",
    "fly_loop.wav",
    "forcefield.wav",
    "freezeray.wav",
    "frog1.wav",
    "frog2.wav",
    "frog3.wav",
    "frog_big_land.wav",
    "frog_bomb_charge.wav",
    "frog_mini.wav",
    "frozen.wav",
    "fwboom.wav",
    "fwshot.wav",
    "gem1.wav",
    "gem2.wav",
    "gem3.wav",
    "gem4.wav",
    "gem5.wav",
    "ghostloop.wav",
    "gold_poop.wav",
    "grab.wav",
    "grunt01.wav",
    "grunt02.wav",
    "grunt03.wav",
    "grunt04.wav",
    "grunt05.wav",
    "grunt06.wav",
    "heartbeat.wav",
    "hit.wav",
    "homing1.wav",
    "homing2.wav",
    "horsehead.wav",
    "ice_crack1.wav",
    "ice_crack2.wav",
    "ice_crack3.wav",
    "idol_get6.wav",
    "ihear_water.wav",
    "immortal_bounce.wav",
    "imp_flap.wav",
    "intodoor.wav",
    "item_drop.wav",
    "itemsplash.wav",
    "jelly_get.wav",
    "jetpack_loop.wav",
    "jump.wav",
    "kaboom.wav",
    "kaboombass.wav",
    "kiss.wav",
    "knifeattack.wav",
    "knifepickup.wav",
    "land.wav",
    "lasergun.wav",
    "lava_splash.wav",
    "lavastream.wav",
    "lick.wav",
    "lobbydrum.wav",
    "maindooropen.wav",
    "mantrapbite.wav",
    "match.wav",
    "menu_enter.wav",
    "menu_hor_l.wav",
    "menu_hor_r.wav",
    "menu_ret.wav",
    "menu_selection.wav",
    "menu_selection2.wav",
    "menu_swipe.wav",
    "menu_ver.wav",
    "metal_clank.wav",
    "mine_timer.wav",
    "mm_amb.wav",
    "mm_click.wav",
    "mm_door1.wav",
    "mm_door2.wav",
    "mm_door3.wav",
    "monkey_stealing.wav",
    "msgup.wav",
    "newshatter.wav",
    "oxface.wav",
    "pageget.wav",
    "pageturn.wav",
    "pause_in.wav",
    "pause_out.wav",
    "penguin.wav",
    "pickup.wav",
    "pushblock.wav",
    "queenblast.wav",
    "ropecatch.wav",
    "ropetoss.wav",
    "rubble.wav",
    "rubble2.wav",
    "rubble3.wav",
    "rubble_bone1.wav",
    "rubble_bone2.wav",
    "rubble_bone3.wav",
    "rubble_ice1.wav",
    "rubble_ice2.wav",
    "rubble_ice3.wav",
    "rubble_metal1.wav",
    "rubble_metal2.wav",
    "rubble_metal3.wav",
    "rubble_vase1.wav",
    "rubble_vase2.wav",
    "rubble_vase3.wav",
    "sacrifice.wav",
    "scarab_get.wav",
    "scorpion.wav",
    "scrollhit.wav",
    "secret.wav",
    "shatter.wav",
    "shop_bells.wav",
    "shopwheel.wav",
    "shotgun.wav",
    "shotgunpump.wav",
    "skeleton_arise.wav",
    "snail_bubble.wav",
    "snail_bubble_burst.wav",
    "snakebite.wav",
    "snowball.wav",
    "spider_jump.wav",
    "spidershot.wav",
    "spike_hit.wav",
    "splash.wav",
    "splat.wav",
    "spring.wav",
    "squish.wav",
    "sr_frogburp.wav",
    "succubus.wav",
    "talkbutton.wav",
    "tank.wav",
    "teleport.wav",
    "throw_item.wav",
    "tikifire.wav",
    "tikispike.wav",
    "torchgust.wav",
    "torchlight.wav",
    "torchlightshort.wav",
    "turretlaser.wav",
    "ufo_loop.wav",
    "ufo_shot.wav",
    "uhoh.wav",
    "up.wav",
    "vanish.wav",
    "volcanoshot.wav",
    "vomitflies.wav",
    "vsnake_sizzle.wav",
    "waterstream.wav",
    "webshot.wav",
    "whip.wav",
    "worm_block_destroy.wav",
    "worm_block_regen.wav",
    "worm_contact.wav",
    "worm_contact2.wav",
    "worm_contact3.wav",
    "worm_eats.wav",
    "worm_tounge_wiggle.wav",
    "yama_faceoff.wav",
    "yama_slam.wav",
    "yamaspew.wav",
    "yeti_roar.wav",
    "yeti_toss.wav",
    "zap.wav",
    "zombie_jump.wav",
};

struct DebugState {

  bool EnableTileBorders = false;
  bool EnableBinBorders = false;
  bool EnablePacifistOverlay = false;

  bool EnableActiveEntityIds = false;
  bool EnableFloorEntityIds = false;
  bool EnableFloorBgEntityIds = false;
  bool EnableBackgroundEntityIds = false;

  bool EnabledActiveHitboxes = false;
  bool Hide171Hitbox = true;
  bool Hide177Hitbox = true;
  bool EnabledFloorHitboxes = false;
  bool EnabledBackgroundHitboxes = false;
  bool EnabledForegroundHitboxes = false;

  bool EnabledUnknown1400 = false;
  bool EnabledForegroundEntities = false;
  bool EnabledEntitiesLightEmitting = false;

  int ExcludeEntityInput = -1;
  std::unordered_set<uint32_t> ExcludedEntities = {};
};
DebugState gDebugState = {};

struct SpawnState {
  int SpawnEntityInput = 0;
  bool ClickToSpawn = false;
};
SpawnState gSpawnState = {};

void specsOnInit() {
  gBaseAddress = (size_t)GetModuleHandleA(NULL);
  setupOffsets(gBaseAddress);
}

struct PlayerState {
  bool LockHealth = false;
  int LockedHealthAmount = 0;

  bool LockBombs = false;
  int LockedBombsAmount = 0;

  bool LockRopes = false;
  int LockedRopesAmount = 0;
};
PlayerState gPlayersState[4] = {{}, {}, {}, {}};

ImVec2 screenToGame(ImVec2 screen) {
  ImGuiIO &io = ImGui::GetIO();
  auto size = io.DisplaySize;

  auto x = (screen.x - (size.x / 2)) * (20 / size.x) + gCameraState->camera_x;
  auto y = (screen.y - (size.y / 2)) * -(20 / size.x) + gCameraState->camera_y;

  return {x, y};
}

ImVec2 gameToScreen(ImVec2 game) {
  ImGuiIO &io = ImGui::GetIO();
  auto size = io.DisplaySize;

  auto x = (game.x - gCameraState->camera_x) / (20 / size.x) + (size.x / 2);
  auto y = (game.y - gCameraState->camera_y) / -(20 / size.x) + (size.y / 2);

  return {x, y};
}

void drawEntityIds(Entity **entities, size_t count) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (gDebugState.ExcludedEntities.contains(ent->entity_type)) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", ent->entity_type);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              out.c_str());
  }
}

void drawEntityHitboxes(Entity **entities, size_t count) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (gDebugState.Hide171Hitbox && ent->entity_type == 171) {
      continue;
    }

    if (gDebugState.Hide177Hitbox && ent->entity_type == 177) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    ImVec2 topLeft =
        gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
    ImVec2 topRight =
        gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up});
    ImVec2 bottomRight =
        gameToScreen({ent->x + ent->hitbox_x, ent->y - ent->hitbox_down});
    ImVec2 bottomLeft =
        gameToScreen({ent->x - ent->hitbox_x, ent->y - ent->hitbox_down});

    gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft,
                              ImGui::GetColorU32({255.f, 0.0f, 238.0f, 0.7f}),
                              1.f);
  }
}

void drawPacifistOverlay() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->owner == Ownership::Unowned) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", (int)ent->owner);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              out.c_str());
  }
}

void drawBinBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 4.f) {
    gOverlayDrawList->AddLine(gameToScreen({x, -4.f}), gameToScreen({x, 120.f}),
                              IM_COL32_WHITE);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 4.f) {
    gOverlayDrawList->AddLine(gameToScreen({-4.f, y}), gameToScreen({50.f, y}),
                              IM_COL32_WHITE);
  }
}

void drawTileBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 1.f) {
    gOverlayDrawList->AddLine(gameToScreen({x + -.5f, -4.5f}),
                              gameToScreen({x + -.5f, 120.5f}), IM_COL32_WHITE,
                              0.5f);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 1.f) {
    gOverlayDrawList->AddLine(gameToScreen({-4.5f, y + 0.5f}),
                              gameToScreen({50.5f, y + 0.5f}), IM_COL32_WHITE,
                              0.5f);
  }
}

void drawEntityId(Entity* ent) {
    auto screen = gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", ent->entity_type);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              out.c_str());
}

using EntityCallback = std::function<void (Entity* e)>;
void forEntities(EntityCallback callback, Entity **entities, size_t count) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (gDebugState.ExcludedEntities.contains(ent->entity_type)) {
      continue;
    }

    callback(ent);
  }
}

void forAllEntities(EntityCallback callback) {
  
  // Active
  if (gDebugState.EnableActiveEntityIds) {
    forEntities(callback, gGlobalState->entities->entities_active,
                  gGlobalState->entities->entities_active_count);
  }

  // 1400
  if (gDebugState.EnabledUnknown1400) {
    forEntities(callback, gGlobalState->entities->array_1400,
                  gGlobalState->entities->array_1400_count);
  }
  // Foreground
  if (gDebugState.EnabledForegroundEntities) {
    forEntities(callback, gGlobalState->entities->entities_foreground,
                  gGlobalState->entities->array_entities_foreground_count);
  }

  // Light Emitting
  if (gDebugState.EnabledEntitiesLightEmitting) {
    forEntities(callback, gGlobalState->entities->entities_light_emitting,
                  gGlobalState->entities->entities_light_emitting_count);
  }

  // Floors
  if (gDebugState.EnableFloorEntityIds) {
    forEntities(callback, gGlobalState->level_state->entity_floors, 4692);
  }
  if (gDebugState.EnableFloorBgEntityIds) {
    forEntities(callback, gGlobalState->level_state->entity_floors_bg, 4692);
  }

  // Backgrounds
  if (gDebugState.EnableBackgroundEntityIds) {
    forEntities(callback, gGlobalState->level_state->entity_backgrounds,
                  gGlobalState->level_state->entity_backgrounds_count);
  }
}

float dist(ImVec2 pos1, ImVec2 pos2) {
  return sqrt(pow(pos2.x - pos1.x, 2.0f) + pow(pos2.y - pos1.y, 2.0f));
}

void drawOverlayWindow() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowSize(io.DisplaySize);
  ImGui::SetNextWindowPos({0, 0});
  ImGui::Begin(
      "Overlay", NULL,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
          ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
          ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
          ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs |
          ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  gOverlayDrawList = ImGui::GetWindowDrawList();

  if (ImGui::IsWindowHovered()) {
    if (io.MouseClicked[1]) {
      auto player = gGlobalState->player1;
      if (player) {
        auto pos = screenToGame(io.MousePos);
        player->x = pos.x;
        player->y = pos.y;
      }
    }

    if (gSpawnState.ClickToSpawn && io.MouseClicked[0]) {
      auto gamePos = screenToGame(io.MousePos);
      if (gSpawnState.SpawnEntityInput > 0) {
        gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                  gSpawnState.SpawnEntityInput, true);
      }
    }

    if (io.MouseClicked[2]) {
      auto gamePos = screenToGame(io.MousePos);
      float closestEntDist = 99999.0;
      Entity* closestEnt = 0x0;
      EntityCallback getClosestEnt = [&](Entity* e) mutable -> void {
        auto eDist = dist(gamePos, ImVec2(e->x, e->y));
        if (eDist < closestEntDist) {
          closestEnt = e;
          closestEntDist = eDist;
        }
      };
      forAllEntities(getClosestEnt);
      if (closestEnt != NULL) {
        gSelectedEntity = closestEnt;
      }
    }
  }

  gOverlayDrawList->AddText(
      ImGui::GetFont(), ImGui::GetFontSize() + 5, {148.f, 40.f},
      ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.05f}), "SpecsHD");

  if (gDebugState.EnableTileBorders) {
    drawTileBorders();
  }

  if (gDebugState.EnableBinBorders) {
    drawBinBorders();
  }

  if (gDebugState.EnablePacifistOverlay) {
    drawPacifistOverlay();
  }

  if (gDebugState.EnabledActiveHitboxes) {
    drawEntityHitboxes(gGlobalState->entities->entities_active,
                       gGlobalState->entities->entities_active_count);
  }

  if (gDebugState.EnabledFloorHitboxes) {
    drawEntityHitboxes(gGlobalState->level_state->entity_floors, 4692);
  }
  if (gDebugState.EnabledBackgroundHitboxes) {
    drawEntityHitboxes(gGlobalState->level_state->entity_backgrounds,
                       gGlobalState->level_state->entity_backgrounds_count);
  }
  if (gDebugState.EnabledForegroundHitboxes) {
    drawEntityHitboxes(gGlobalState->entities->entities_foreground,
                       gGlobalState->entities->array_entities_foreground_count);
  }

  forAllEntities(&drawEntityId);
  ImGui::End();
}

void drawSpawnTab() {
  ImGui::InputInt("Spawn Entity", &gSpawnState.SpawnEntityInput);
  ImGui::Checkbox("Click to spawn", &gSpawnState.ClickToSpawn);

  if (ImGui::Button("Spawn")) {
    if (gSpawnState.SpawnEntityInput > 0) {
      gGlobalState->SpawnEntity(gGlobalState->player1->x,
                                gGlobalState->player1->y,
                                gSpawnState.SpawnEntityInput, true);
    }
  }
}

void warpToLevel(uint32_t level) {
  gGlobalState->level = level;
  gGlobalState->screen_state = 3;
}

void drawLevelTab() {

  auto isDisabled =
      gGlobalState->screen_state != 0 || gGlobalState->play_state != 0;

  if (isDisabled) {
    ImGui::BeginDisabled();
  }
  ImGui::Text("");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("Next Level")) {
    warpToLevel(gGlobalState->level);
  }

  ImGui::Text("Mines");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("1-1"))
    warpToLevel(0);
  ImGui::SameLine();
  if (ImGui::Button("1-2"))
    warpToLevel(1);
  ImGui::SameLine();
  if (ImGui::Button("1-3")) {
    warpToLevel(2);
  }
  ImGui::SameLine();
  if (ImGui::Button("1-4")) {
    warpToLevel(3);
  }

  ImGui::Text("Jungle");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("2-1"))
    warpToLevel(4);
  ImGui::SameLine();
  if (ImGui::Button("2-2"))
    warpToLevel(5);
  ImGui::SameLine();
  if (ImGui::Button("2-3")) {
    warpToLevel(6);
  }
  ImGui::SameLine();
  if (ImGui::Button("2-4")) {
    warpToLevel(7);
  }

  ImGui::Text("Ice Caves");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("3-1"))
    warpToLevel(8);
  ImGui::SameLine();
  if (ImGui::Button("3-2"))
    warpToLevel(9);
  ImGui::SameLine();
  if (ImGui::Button("3-3")) {
    warpToLevel(10);
  }
  ImGui::SameLine();
  if (ImGui::Button("3-4")) {
    warpToLevel(11);
  }

  ImGui::Text("Temple");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("4-1"))
    warpToLevel(12);
  ImGui::SameLine();
  if (ImGui::Button("4-2"))
    warpToLevel(13);
  ImGui::SameLine();
  if (ImGui::Button("4-3")) {
    warpToLevel(14);
  }
  ImGui::SameLine();
  if (ImGui::Button("Olmec")) {
    warpToLevel(15);
  }

  ImGui::Text("Hell");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("5-1"))
    warpToLevel(16);
  ImGui::SameLine();
  if (ImGui::Button("5-2"))
    warpToLevel(17);
  ImGui::SameLine();
  if (ImGui::Button("5-3")) {
    warpToLevel(18);
  }
  ImGui::SameLine();
  if (ImGui::Button("Yama")) {
    warpToLevel(19);
  }

  if (isDisabled) {
    ImGui::EndDisabled();
  }
}

void ensureLockedAmountsForPlayer(EntityPlayer *player, PlayerData &data,
                                  PlayerState *state) {
  if (state->LockHealth) {
    // If you ressurect a player it gets into a bad state.
    if (player->health > 0) {
      player->health = state->LockedHealthAmount;
    }
  }

  if (state->LockBombs) {
    data.bombs = state->LockedBombsAmount;
  }

  if (state->LockRopes) {
    data.ropes = state->LockedRopesAmount;
  }
}

void ensureLockedAmounts() {
  if (gGlobalState->player1) {
    ensureLockedAmountsForPlayer(gGlobalState->player1,
                                 gGlobalState->player1_data, &gPlayersState[0]);
  }

  if (gGlobalState->player2) {
    ensureLockedAmountsForPlayer(gGlobalState->player2,
                                 gGlobalState->player2_data, &gPlayersState[1]);
  }

  if (gGlobalState->player3) {
    ensureLockedAmountsForPlayer(gGlobalState->player3,
                                 gGlobalState->player3_data, &gPlayersState[2]);
  }

  if (gGlobalState->player4) {
    ensureLockedAmountsForPlayer(gGlobalState->player4,
                                 gGlobalState->player4_data, &gPlayersState[3]);
  }
}

void drawPlayerTab(EntityPlayer *player, PlayerData &data, PlayerState *state) {
  if (!player) {
    ImGui::Text("No Player Entity");
    return;
  }

  if (ImGui::Button("Max Health/Bombs/Ropes")) {
    player->health = 99;
    state->LockedHealthAmount = player->health;
    data.bombs = 99;
    state->LockedBombsAmount = data.bombs;
    data.ropes = 99;
    state->LockedRopesAmount = data.ropes;
  }

  ImGui::Text("Locked?");
  ImGui::SameLine(80.0f);
  ImGui::Text("Amount");

  if (ImGui::Checkbox("##LockHealth", &state->LockHealth)) {
    if (state->LockHealth) {
      state->LockedHealthAmount = player->health;
    }
  };
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Health", &player->health)) {
    player->health = std::clamp(player->health, 0, 99);
    state->LockedHealthAmount = player->health;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockBombs", &state->LockBombs)) {
    if (state->LockBombs) {
      state->LockedBombsAmount = data.bombs;
    }
  }
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Bombs", &data.bombs)) {
    data.bombs = std::clamp(data.bombs, 0, 99);
    state->LockedBombsAmount = data.bombs;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockRopes", &state->LockRopes)) {
    if (state->LockRopes) {
      state->LockedRopesAmount = data.ropes;
    }
  }
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Ropes", &data.ropes)) {
    data.ropes = std::clamp(data.ropes, 0, 99);
    state->LockedRopesAmount = data.ropes;
  }
  ImGui::PopItemWidth();
  ImGui::Separator();

  ImGui::Checkbox("Compass", &data.has_compass);
  ImGui::Checkbox("Parachute", &data.has_parachute);

  ImGui::Checkbox("Jetpack", &data.has_jetpack);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Jetpack")) {
    gGlobalState->SpawnEntity(player->x, player->y, 522, true);
  }

  ImGui::Checkbox("Climbing Gloves", &data.has_climbing_gloves);
  ImGui::Checkbox("Pitcher's Mitt", &data.has_pitchers_mitt);
  ImGui::Checkbox("Spring Shoes", &data.has_spring_shoes);
  ImGui::Checkbox("Spike Shoes", &data.has_spike_shoes);
  ImGui::Checkbox("Spectacles", &data.has_spectacles);
  ImGui::Checkbox("Kapala", &data.has_kapala);
  ImGui::Checkbox("Hedjet", &data.has_hedjet);
  ImGui::Checkbox("Udjat Eye", &data.has_udjat);
  ImGui::Checkbox("Book of the Dead", &data.has_book_of_dead);
  ImGui::Checkbox("Ankh", &data.has_ankh);
  ImGui::Checkbox("Paste", &data.has_paste);

  ImGui::Checkbox("Cape", &data.has_cape);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Cape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 521, true);
  }

  ImGui::Checkbox("Vlad's Cape", &data.has_vlads_cape);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##VladsCape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 532, true);
  }

  ImGui::Checkbox("Crysknife", &data.has_crysknife);
  ImGui::Checkbox("Vlad's Amulet", &data.has_vlads_amulet);
  ImGui::Checkbox("White Flag", &data.has_white_flag);
}

void drawPlayersTab() {

  if (ImGui::BeginTabBar("Players")) {
    if (ImGui::BeginTabItem("Player 1")) {
      drawPlayerTab(gGlobalState->player1, gGlobalState->player1_data,
                    &gPlayersState[0]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 2")) {
      drawPlayerTab(gGlobalState->player2, gGlobalState->player2_data,
                    &gPlayersState[1]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 3")) {
      drawPlayerTab(gGlobalState->player3, gGlobalState->player3_data,
                    &gPlayersState[2]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 4")) {
      drawPlayerTab(gGlobalState->player4, gGlobalState->player4_data,
                    &gPlayersState[3]);
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
}

void drawAudioTab() {

  // auto player = *(&((*gGlobalState).player1) + gGlobalState->flag_player);
  auto player = gGlobalState->player1;
  if (!player) {
    ImGui::Text("Need Player Entity");
    return;
  }

  for (auto idx = 0; idx < gNumAudioNames; idx++) {
    if (ImGui::Button(gAudioNames[idx])) {
      player->PlaySound(gAudioNames[idx]);
    }
  }
}

void drawDebugTab() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::Text("Mouse: %f %f", io.MousePos.x, io.MousePos.y);
  ImGui::Checkbox("Draw Tile Borders", &gDebugState.EnableTileBorders);
  ImGui::Checkbox("Draw Bin Borders", &gDebugState.EnableBinBorders);
  ImGui::Checkbox("Draw Owned Entities", &gDebugState.EnablePacifistOverlay);

  ImGui::Separator();
  ImGui::Checkbox("Draw Active Hitboxes", &gDebugState.EnabledActiveHitboxes);
  ImGui::SameLine();
  ImGui::Checkbox("Hide 171", &gDebugState.Hide171Hitbox);
  ImGui::SameLine();
  ImGui::Checkbox("Hide 177", &gDebugState.Hide177Hitbox);

  ImGui::Checkbox("Draw Floor Hitboxes", &gDebugState.EnabledFloorHitboxes);
  ImGui::Checkbox("Draw Background Hitboxes",
                  &gDebugState.EnabledBackgroundHitboxes);
  ImGui::Checkbox("Draw Foreground Hitboxes",
                  &gDebugState.EnabledForegroundHitboxes);

  ImGui::Separator();
  ImGui::Checkbox("Draw Active Entity IDs", &gDebugState.EnableActiveEntityIds);
  ImGui::Checkbox("Draw Light Emitting IDs",
                  &gDebugState.EnabledEntitiesLightEmitting);
  ImGui::Checkbox("Draw Unknown 1400 IDs (BG + Active?)",
                  &gDebugState.EnabledUnknown1400);
  ImGui::Checkbox("Draw Foreground Entity IDs",
                  &gDebugState.EnabledForegroundEntities);
  ImGui::Checkbox("Draw Floor Entity IDs", &gDebugState.EnableFloorEntityIds);
  ImGui::Checkbox("Draw Floor Background Entity IDs",
                  &gDebugState.EnableFloorBgEntityIds);
  ImGui::Checkbox("Draw Background Entity IDs",
                  &gDebugState.EnableBackgroundEntityIds);

  ImGui::Separator();
  ImGui::InputInt("Exclude Entity", &gDebugState.ExcludeEntityInput);
  if (ImGui::Button("Exclude")) {
    if (gDebugState.ExcludeEntityInput >= 0) {
      gDebugState.ExcludedEntities.insert(gDebugState.ExcludeEntityInput);
      gDebugState.ExcludeEntityInput = 0;
    }
  }

  ImGui::Separator();
  for (auto ent_type : gDebugState.ExcludedEntities) {
    auto label = std::format("Remove {}", ent_type);
    if (ImGui::Button(label.c_str())) {
      gDebugState.ExcludedEntities.erase(ent_type);
    }
  }
}

void drawSelectedEntityTab() {
  ImGuiIO &io = ImGui::GetIO();
  if (!gSelectedEntity) {
    ImGui::Text("No selected entity");
    return;
  }
  ImGui::Text("Entity ID: %d", gSelectedEntity->entity_type);
  ImGui::InputFloat("ent_x", &gSelectedEntity->x);
  ImGui::InputFloat("ent_y", &gSelectedEntity->y);
}

void drawToolWindow() {
  if (!ui::open) {
    return;
  }
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2{0.f, 0.f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2{400.f, 500.f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Specs HD");

  if (ImGui::IsWindowHovered()) {
    io.MouseDrawCursor = true;
    io.WantCaptureMouse = true;
  } else {
    io.MouseDrawCursor = false;
    io.WantCaptureMouse = false;
  }

  auto mouse_game = screenToGame(io.MousePos);

  if (ImGui::BeginTabBar("Specs HD")) {
    if (ImGui::BeginTabItem("Spawn")) {
      drawSpawnTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Level")) {
      drawLevelTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Players")) {
      drawPlayersTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Audio")) {
      drawAudioTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Debug")) {
      drawDebugTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Selected")) {
      drawSelectedEntityTab();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
}

void specsOnFrame() {

  gCameraState =
      reinterpret_cast<CameraState *>(*((DWORD *)(gBaseAddress + 0x154510)));
  gGlobalState =
      reinterpret_cast<GlobalState *>(*((DWORD *)(gBaseAddress + 0x15446C)));

  gGlobalState->N00001004 = 0; // 440629

  ensureLockedAmounts();

  drawOverlayWindow();
  drawToolWindow();
}