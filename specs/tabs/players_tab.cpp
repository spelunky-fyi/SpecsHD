
#include "players_tab.h"

#include "level_tab.h"

#include "../state.h"

PlayerState gPlayersState[4] = {{}, {}, {}, {}};

static void ensureLockedAmountsForPlayer(EntityPlayer *player, PlayerData &data,
                                         PlayerState *state) {
  if (state->LockHealth) {
    // If you ressurect a player it gets into a bad state.
    if (player->health > 0) {
      player->health = state->LockedHealthAmount;
    }
  }

  if (state->LockHeldItemId) {
    data.held_item_id = state->LockedHeldItemId;
  }
  if (state->LockHeldItemMetadata) {
    data.held_item_metadata = state->LockedHeldItemMetadata;
  }
  if (state->LockHiredHandCount) {
    data.hh_count = state->LockedHiredHandCount;
  }

  if (state->LockBombs) {
    data.bombs = state->LockedBombsAmount;
  }

  if (state->LockRopes) {
    data.ropes = state->LockedRopesAmount;
  }

  if (state->LockCompass) {
    data.has_compass = state->LockedCompassValue;
  }

  if (state->LockParachute) {
    data.has_parachute = state->LockedParachuteValue;
  }

  if (state->LockJetpack) {
    data.has_jetpack = state->LockedJetpackValue;
  }

  if (state->LockClimbingGloves) {
    data.has_climbing_gloves = state->LockedClimbingGlovesValue;
  }

  if (state->LockPitchersMitt) {
    data.has_pitchers_mitt = state->LockedPitchersMittValue;
  }

  if (state->LockSpringShoes) {
    data.has_spring_shoes = state->LockedSpringShoesValue;
  }

  if (state->LockSpikeShoes) {
    data.has_spike_shoes = state->LockedSpikeShoesValue;
  }

  if (state->LockSpectacles) {
    data.has_spectacles = state->LockedSpectaclesValue;
  }

  if (state->LockKapala) {
    data.has_kapala = state->LockedKapalaValue;
  }

  if (state->LockHedjet) {
    data.has_hedjet = state->LockedHedjetValue;
  }

  if (state->LockUdjatEye) {
    data.has_udjat = state->LockedUdjatEyeValue;
  }

  if (state->LockBookOfTheDead) {
    data.has_book_of_dead = state->LockedBookOfTheDeadValue;
  }

  if (state->LockAnkh) {
    data.has_ankh = state->LockedAnkhValue;
  }

  if (state->LockPaste) {
    data.has_paste = state->LockedPasteValue;
  }

  if (state->LockCape) {
    data.has_cape = state->LockedCapeValue;
  }

  if (state->LockVladsCape) {
    data.has_vlads_cape = state->LockedVladsCapeValue;
  }

  if (state->LockCrysknife) {
    data.has_crysknife = state->LockedCrysknifeValue;
  }

  if (state->LockVladsAmulet) {
    data.has_vlads_amulet = state->LockedVladsAmuletValue;
  }

  if (state->LockWhiteFlag) {
    data.has_white_flag = state->LockedWhiteFlagValue;
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

  ensureLockedLevelsState();
}

static void drawLockedPlayerDataCheckbox(std::string title, bool &val,
                                         bool &lockVar, bool &lockedVal) {
  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::Checkbox(std::format("##Lock{}", title).c_str(), &lockVar)) {
    if (lockVar) {
      lockedVal = val;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  if (ImGui::Checkbox(title.c_str(), &val)) {
    lockedVal = val;
  }
}

static void drawPlayerTab(EntityPlayer *player, PlayerData &data,
                           PlayerState *state) {
  ImGuiIO &io = ImGui::GetIO();

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
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::Text("Amount");

  if (ImGui::Checkbox("##LockHealth", &state->LockHealth)) {
    if (state->LockHealth) {
      state->LockedHealthAmount = player->health;
    }
  };
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
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
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
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
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Ropes", &data.ropes)) {
    data.ropes = std::clamp(data.ropes, 0, 99);
    state->LockedRopesAmount = data.ropes;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockHeldItemId", &state->LockHeldItemId)) {
    if (state->LockHeldItemId) {
      state->LockedHeldItemId = data.held_item_id;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Held Item", &data.held_item_id)) {
    state->LockedHeldItemId = data.held_item_id;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockHeldItemMetadata", &state->LockHeldItemMetadata)) {
    if (state->LockHeldItemMetadata) {
      state->LockedHeldItemMetadata = data.held_item_metadata;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Held Item Metadata", &data.held_item_metadata)) {
    state->LockedHeldItemMetadata = data.held_item_metadata;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockHiredHandCount", &state->LockHiredHandCount)) {
    if (state->LockHiredHandCount) {
      state->LockedHiredHandCount = data.hh_count;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  if (ImGui::InputInt("Hired Hand Count", &data.hh_count)) {
    state->LockedHiredHandCount = data.hh_count;
  }
  ImGui::PopItemWidth();

  ImGui::Separator();

  drawLockedPlayerDataCheckbox("Compass", data.has_compass, state->LockCompass,
                               state->LockedCompassValue);

  drawLockedPlayerDataCheckbox("Parachute", data.has_parachute,
                               state->LockParachute,
                               state->LockedParachuteValue);

  drawLockedPlayerDataCheckbox("Jetpack", data.has_jetpack, state->LockJetpack,
                               state->LockedJetpackValue);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Jetpack")) {
    gGlobalState->SpawnEntity(player->x, player->y, 522, true);
  }

  drawLockedPlayerDataCheckbox("Climbing Gloves", data.has_climbing_gloves,
                               state->LockClimbingGloves,
                               state->LockedClimbingGlovesValue);

  drawLockedPlayerDataCheckbox("Pitcher's Mitt", data.has_pitchers_mitt,
                               state->LockPitchersMitt,
                               state->LockedPitchersMittValue);

  drawLockedPlayerDataCheckbox("Spring Shoes", data.has_spring_shoes,
                               state->LockSpringShoes,
                               state->LockedSpringShoesValue);

  drawLockedPlayerDataCheckbox("Spike Shoes", data.has_spike_shoes,
                               state->LockSpikeShoes,
                               state->LockedSpikeShoesValue);

  drawLockedPlayerDataCheckbox("Spectacles", data.has_spectacles,
                               state->LockSpectacles,
                               state->LockedSpectaclesValue);

  drawLockedPlayerDataCheckbox("Kapala", data.has_kapala, state->LockKapala,
                               state->LockedKapalaValue);

  drawLockedPlayerDataCheckbox("Hedjet", data.has_hedjet, state->LockHedjet,
                               state->LockedHedjetValue);

  drawLockedPlayerDataCheckbox("Udjat Eye", data.has_udjat, state->LockUdjatEye,
                               state->LockedUdjatEyeValue);

  drawLockedPlayerDataCheckbox("Book of the Dead", data.has_book_of_dead,
                               state->LockBookOfTheDead,
                               state->LockedBookOfTheDeadValue);

  drawLockedPlayerDataCheckbox("Ankh", data.has_ankh, state->LockAnkh,
                               state->LockedAnkhValue);

  drawLockedPlayerDataCheckbox("Paste", data.has_paste, state->LockPaste,
                               state->LockedPasteValue);

  drawLockedPlayerDataCheckbox("Cape", data.has_cape, state->LockCape,
                               state->LockedCapeValue);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Cape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 521, true);
  }

  drawLockedPlayerDataCheckbox("Vlad's Cape", data.has_vlads_cape,
                               state->LockVladsCape,
                               state->LockedVladsCapeValue);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##VladsCape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 532, true);
  }

  drawLockedPlayerDataCheckbox("Crysknife", data.has_crysknife,
                               state->LockCrysknife,
                               state->LockedCrysknifeValue);

  drawLockedPlayerDataCheckbox("Vlad's Amulet", data.has_vlads_amulet,
                               state->LockVladsAmulet,
                               state->LockedVladsAmuletValue);

  drawLockedPlayerDataCheckbox("White Flag", data.has_white_flag,
                               state->LockWhiteFlag,
                               state->LockedWhiteFlagValue);
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
