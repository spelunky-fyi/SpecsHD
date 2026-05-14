
#include "debug_tab.h"

#include "../drawing.h"
#include "../game_hooks.h"
#include "../sounds.h"
#include "../state.h"
#include <hddll/memory.h>
#include <hddll/utils.h>

EnabledEntities gAllEntities = {true, true, true, true, true,
                                true, true, true, -1,   {}};
DebugState gDebugState = {};
AIBotDebugState gAIBotDebugState = {};

static void drawToggleEntityTab(const char *preText,
                                EnabledEntities &enabledEnts) {
  ImGui::Checkbox(std::format("{} Active Entities", preText).c_str(),
                  &enabledEnts.activeEntities);
  ImGui::Checkbox(std::format("{} Light Emitting", preText).c_str(),
                  &enabledEnts.lightEmittingEntities);
  ImGui::Checkbox(
      std::format("{} Unknown 1400 (BG + Active?)", preText).c_str(),
      &enabledEnts.unknown1400);
  ImGui::Checkbox(std::format("{} Foreground Entities", preText).c_str(),
                  &enabledEnts.foregroundEntities);
  ImGui::Checkbox(std::format("{} Floor Entities", preText).c_str(),
                  &enabledEnts.floorEntities);
  ImGui::Checkbox(std::format("{} Floor Background Entities", preText).c_str(),
                  &enabledEnts.floorBgEntities);
  ImGui::Checkbox(std::format("{} Background Entities", preText).c_str(),
                  &enabledEnts.backgroundEntities);
  ImGui::Checkbox(std::format("{} 4c Struct Entities", preText).c_str(),
                  &enabledEnts._4cStructEntities);

  ImGui::InputInt(std::format("Exclude Entity {}", preText).c_str(),
                  &enabledEnts.excludeEntityInput);
  if (ImGui::Button(std::format("Exclude##Debug{}", preText).c_str())) {
    if (enabledEnts.excludeEntityInput >= 0) {
      enabledEnts.excluded.insert(enabledEnts.excludeEntityInput);
      enabledEnts.excludeEntityInput = 0;
    }
  }
  ImGui::Separator();
  int toErase = -1;
  for (auto ent_type : enabledEnts.excluded) {
    auto label = std::format("Remove {}##Debug{}", ent_type, preText);
    if (ImGui::Button(label.c_str())) {
      toErase = ent_type;
    }
  }
  if (toErase >= 0) {
    enabledEnts.excluded.erase(toErase);
  }
}

static void drawRawBytesTable(const char *str_id, char *start_addr,
                              size_t size) {
  if (ImGui::BeginTable(str_id, 6)) {

    ImGui::TableSetupColumn("Offset");
    ImGui::TableSetupColumn("Bytes");
    ImGui::TableSetupColumn("Signed");
    ImGui::TableSetupColumn("Unsigned");
    ImGui::TableSetupColumn("Hex");
    ImGui::TableSetupColumn("Float");
    ImGui::TableHeadersRow();

    for (size_t i = 0; i < size; i += 4) {

      ImGui::TableNextRow();

      char *addr = start_addr + i;
      ImGui::TableNextColumn();
      ImGui::Text("0x%X", i);
      {
        uint32_t a1, a2, a3, a4;
        a1 = (*(addr)) & (0xFF);
        a2 = (*(addr + 1)) & (0xFF);
        a3 = (*(addr + 2)) & (0xFF);
        a4 = (*(addr + 3)) & (0xFF);
        ImGui::TableNextColumn();
        ImGui::Text("%02X %02X %02X %02X", a1, a2, a3, a4);
      }
      ImGui::TableNextColumn();
      ImGui::Text("%d", *(int32_t *)addr);

      ImGui::TableNextColumn();
      ImGui::Text("%u", *(uint32_t *)addr);

      ImGui::TableNextColumn();
      ImGui::Text("0x%08X", *(uint32_t *)addr);

      ImGui::TableNextColumn();
      ImGui::Text("%f", *(float *)addr);
    }

    ImGui::EndTable();
  }
}

void onLevelStartOlmec() {
  if (!gDebugState.DisableOlmecCutscene || hddll::gGlobalState->level != 16) {
    return;
  }

  auto player = hddll::gGlobalState->player1;
  if (!player)
    return;
  player->field1_0x130 = 1;
  player->field2_0x134 = 1;
  player->field3_0x138 = 1;
  player->field48_0x1ec = 1;
  player->field55_0x1f3 = 0;
  player->field7_0x271 = 0;

  // Destroy Floor
  EntityCallback cb = [&](hddll::Entity *e) {
    if (e->y == 75 && e->x >= 21 && e->x <= 25) {
      e->flag_deletion = 1;
    }
  };
  forEntities({}, cb, hddll::gGlobalState->entities->entities_active,
              hddll::gGlobalState->entities->entities_active_count);
  forEntities({}, cb,
              (hddll::Entity **)hddll::gGlobalState->level_state->entity_floors,
              hddll::ENTITY_FLOORS_COUNT);

  hddll::gCameraState->camera_speed = 0.2f;
  auto hawkman = (hddll::EntityMonster *)hddll::gGlobalState->SpawnEntity(
      18.019993f, 76.0f, 1011, true);
  hawkman->flag_horizontal_flip = 1;
  hawkman->field8_0x14c = 0xA;
  hawkman->field15_0x168 = 0x5A;
  hawkman->stun_timer = 0xFF;
  hawkman->field24_0x18c = 0xFF;
  hawkman->stunned = 1;

  // Spawn Olmec
  auto olmec = (hddll::EntityItem *)hddll::gGlobalState->SpawnEntity(
      23.109999f, 76.5f, 1055, true);
  olmec->flag_horizontal_flip = 1;

  olmec->field8_0x14c = 0;
  olmec->field9_0x150 = 0;
  olmec->field10_0x154 = 0;
  olmec->field11_0x158 = 2;
  olmec->field37_0x1c0 = 0.015000f;
  olmec->field48_0x1ec = 1;
  olmec->field68_0x200 = 1;
  olmec->field82_0x20e = 1;

  hddll::gGlobalState->PlayOlmecMusic("A04_boss.ogg");
  hddll::gGlobalState->total_seconds += 11;
  hddll::gGlobalState->total_ms += 33.50;
  hddll::gGlobalState->level_seconds = 11;
  hddll::gGlobalState->level_ms = 33.50;
}

void drawDebugTab() {
  ImGuiIO &io = ImGui::GetIO();

  auto gameMouse = hddll::screenToGame(io.MousePos);
  ImGui::Text("Mouse (Screen): %f %f", io.MousePos.x, io.MousePos.y);
  ImGui::Text("Mouse (Game): %f %f", gameMouse.x, gameMouse.y);
  if (hddll::gGlobalState->player1) {
    auto screenPlayer = hddll::gameToScreen(
        {hddll::gGlobalState->player1->x, hddll::gGlobalState->player1->y});
    ImGui::Text("Player (Screen): %f %f", screenPlayer.x, screenPlayer.y);
    ImGui::Text("Player (Game): %f %f", hddll::gGlobalState->player1->x,
                hddll::gGlobalState->player1->y);
  }
  ImGui::Checkbox("Draw Tile Borders", &gDebugState.EnableTileBorders);
  ImGui::Checkbox("Draw Bin Borders", &gDebugState.EnableBinBorders);
  ImGui::Checkbox("Draw Room Borders", &gDebugState.EnableRoomBorders);
  ImGui::Checkbox("Draw Owned Entities", &gDebugState.EnablePacifistOverlay);

  ImGui::Checkbox("Draw Detection Boxes", &gDebugState.DrawEnemyDetection);
  if (ImGui::Checkbox("Black Market Trainer",
                      &gDebugState.BlackMarketTrainer)) {
    if (!hookEligibleBMsJmpBackAddr) {
      // Hook Eligible BM Floors
      int hookLen = 7;
      DWORD hookAddr = hddll::gBaseAddress + 0xbe35e;
      hookEligibleBMsJmpBackAddr = hookAddr + hookLen;
      hddll::hook((void *)hookAddr, hookEligibleBMs, hookLen);
    }
  };
  ImGui::Checkbox("Include Hitbox Origins", &gDebugState.IncludeHitboxOrigins);
  ImGui::Checkbox("Include Floor Decorations", &gDebugState.IncludeFloorDecos);

  ImGui::Separator();
  ImGui::Checkbox("Draw HH Follower Link", &gDebugState.DrawHHFollowerLink);
  ImGui::Checkbox("Draw HH Following Link", &gDebugState.DrawHHFollowingLink);

  ImGui::Separator();
  ImGui::TextDisabled("AIBot (select a hired hand / CPU player)");
  ImGui::Checkbox("Draw AIBot Path", &gDebugState.DrawAIBotPath);
  ImGui::Checkbox("Draw AIBot Pathfinding Grid", &gDebugState.DrawAIBotGrid);
  ImGui::Checkbox("Draw AIBot Targets / Perception",
                  &gDebugState.DrawAIBotTargets);
  ImGui::Checkbox("Draw AIBot State Label", &gDebugState.DrawAIBotStateLabel);

  if (ImGui::CollapsingHeader("AIBot State Log")) {
    auto &log = gAIBotDebugState;
    if (!log.trackedBot) {
      ImGui::TextDisabled("Select a hired hand / CPU player to start logging.");
    } else {
      ImGui::Text("Tracking 0x%X  (%d transitions)", (uint32_t)log.trackedBot,
                  log.logCount);
      if (ImGui::Button("Clear Log")) {
        log.logCount = 0;
        log.logHead = 0;
      }
      if (ImGui::BeginChild("##AIBotStateLog", {-1, 200}, true)) {
        // newest first
        for (int i = 0; i < log.logCount; i++) {
          int idx = (log.logHead - 1 - i + AIBotDebugState::LogCapacity) %
                    AIBotDebugState::LogCapacity;
          auto &e = log.log[idx];
          ImGui::Text("f%-8d  %-16s  %s", e.frame, hddll::AiStateName(e.state),
                      hddll::CombatActionName(e.combatAction));
        }
      }
      ImGui::EndChild();
    }
  }

  ImGui::Separator();
  if (ImGui::Checkbox("Disable Olmec Spawns",
                      &gDebugState.DisableOlmecSpawns)) {
    auto process = GetCurrentProcess();
    if (gDebugState.DisableOlmecSpawns) {
      BYTE patch[] = {0x83, 0xfa, 0x08};
      hddll::patchReadOnlyCode(process, hddll::gBaseAddress + 0x3121c, patch,
                               3);
    } else {
      BYTE patch[] = {0x83, 0xfa, 0x01};
      hddll::patchReadOnlyCode(process, hddll::gBaseAddress + 0x3121c, patch,
                               3);
    }
  }
  if (ImGui::Checkbox("Disable Olmec Gaps", &gDebugState.DisableOlmecGaps)) {
    auto process = GetCurrentProcess();
    if (gDebugState.DisableOlmecGaps) {
      BYTE patch[] = {0xb9, 0x01, 0x00, 0x00, 0x00};
      hddll::patchReadOnlyCode(process, hddll::gBaseAddress + 0x0d5b71, patch,
                               5);
    } else {
      BYTE patch[] = {0xb9, 0x06, 0x00, 0x00, 0x00};
      hddll::patchReadOnlyCode(process, hddll::gBaseAddress + 0x0d5b71, patch,
                               5);
    }
  }
  if (ImGui::Checkbox("Disable Olmec Cutscene",
                      &gDebugState.DisableOlmecCutscene)) {
    auto process = GetCurrentProcess();
    if (gDebugState.DisableOlmecCutscene) {
      BYTE patch[] = {0x83, 0xb9, 0xd4, 0x05, 0x44, 0x00, 0x99};
      hddll::patchReadOnlyCode(process, hddll::gBaseAddress + 0x0be66d, patch,
                               7);
    } else {
      BYTE patch[] = {0x83, 0xb9, 0xd4, 0x05, 0x44, 0x00, 0x10};
      hddll::patchReadOnlyCode(process, hddll::gBaseAddress + 0x0be66d, patch,
                               7);
    }
  }
  ImGui::Checkbox("Show Olmec Crush Probes", &gDebugState.ShowOlmecCrushProbes);
  ImGui::Separator();

  ImGui::Checkbox("Draw Seeded Crate Contents",
                  &gDebugState.EnableSeededCrateOverlay);
  ImGui::Checkbox("Draw Seeded Pot Contents",
                  &gDebugState.EnableSeededPotOverlay);
  ImGui::Checkbox("Draw Seeded Chest Contents",
                  &gDebugState.EnableSeededChestOverlay);
  ImGui::Checkbox("Draw Seeded Kali Rewards",
                  &gDebugState.EnableSeededKaliRewards);
  ImGui::Separator();

  if (ImGui::CollapsingHeader("Draw Hitboxes")) {
    drawToggleEntityTab("Show", gDebugState.Hitboxes);
  }
  if (ImGui::CollapsingHeader("Draw Ids")) {
    drawToggleEntityTab("Draw", gDebugState.Ids);
  }
  if (ImGui::CollapsingHeader("Selectable Entities")) {
    ImGui::Checkbox("Draw Selected Entity Hitbox",
                    &gDebugState.DrawSelectedEntHitbox);
    ImGui::Checkbox("Draw Closest Entity Hitbox",
                    &gDebugState.DrawClosestEntHitbox);
    ImGui::Checkbox("Draw Closest Entity Id", &gDebugState.DrawClosestEntId);
    ImGui::Separator();
    drawToggleEntityTab("Enable", gDebugState.Selection);
  }

  if (ImGui::CollapsingHeader("Global State")) {

    ImGui::InputScalar("screen_state", ImGuiDataType_U32,
                       &hddll::gGlobalState->screen_state);
    ImGui::InputScalar("play_state", ImGuiDataType_U32,
                       &hddll::gGlobalState->play_state);
    ImGui::InputScalar("flag_player", ImGuiDataType_U32,
                       &hddll::gGlobalState->flag_player);
    ImGui::InputScalar("level", ImGuiDataType_U32, &hddll::gGlobalState->level);
    ImGui::InputScalar("level_track", ImGuiDataType_U32,
                       &hddll::gGlobalState->level_track);
    ImGui::InputFloat("insertion_point", &hddll::gGlobalState->insertion_point,
                      0.0001F, 0.0F, "%.4f");
    ImGui::Separator();
    ImGui::PushItemWidth(60.f);
    ImGui::InputScalar("Wanted Level", ImGuiDataType_S32,
                       &hddll::gGlobalState->wanted_level);
    ImGui::InputScalar(
        "Number of Angry Shopkeepers", ImGuiDataType_S32,
        &hddll::gGlobalState->entities->angered_shopkeeper_count);
    ImGui::PopItemWidth();
    ImGui::PushItemWidth(200.f);
    if (ImGui::InputInt("Kali Reward Level", &hddll::gGlobalState->reward_level,
                        1, 1)) {
      hddll::gGlobalState->reward_level =
          std::clamp(hddll::gGlobalState->reward_level, 0, 3);
    }
    if (ImGui::InputInt("Kali Punishment Level",
                        &hddll::gGlobalState->punishment_amount, 1, 1)) {
      hddll::gGlobalState->punishment_amount =
          std::clamp(hddll::gGlobalState->punishment_amount, -1, 3);
    }
    ImGui::InputInt("Total Favor", &hddll::gGlobalState->total_favor, 1, 1);
    ImGui::PopItemWidth();
    ImGui::Checkbox("Kali Broke Altar This Level",
                    &hddll::gGlobalState->broke_altar_this_level);

    drawCharBool("Shopkeeper Triggered",
                 hddll::gGlobalState->shopkeeper_triggered);
    drawCharBool("Shopkeeper Music Triggered",
                 hddll::gGlobalState->shopkeeper_music_triggered);

    if (ImGui::CollapsingHeader("GlobalState Flags")) {
      drawCharBool("dark_level", hddll::gGlobalState->dark_level);
      drawCharBool("altar_spawned", hddll::gGlobalState->altar_spawned);
      drawCharBool("idol_spawned", hddll::gGlobalState->idol_spawned);
      drawCharBool("damsel_spawned", hddll::gGlobalState->damsel_spawned);
      drawCharBool("unknown_flag", hddll::gGlobalState->unknown_flag);
      drawCharBool("moai_unopened", hddll::gGlobalState->moai_unopened);
      drawCharBool("moai_broke_in", hddll::gGlobalState->moai_broke_in);
      drawCharBool("ghost_spawned", hddll::gGlobalState->ghost_spawned);
      drawCharBool("rescued_damsel", hddll::gGlobalState->rescued_damsel);
      drawCharBool("shopkeeper_triggered",
                   hddll::gGlobalState->shopkeeper_triggered);
      drawCharBool("area_had_dark_level",
                   hddll::gGlobalState->area_had_dark_level);
      drawCharBool("level_has_udjat", hddll::gGlobalState->level_has_udjat);
      drawCharBool("has_spawned_udjat", hddll::gGlobalState->has_spawned_udjat);
      drawCharBool("unused_flag", hddll::gGlobalState->unused_flag);
      drawCharBool("vault_spawned_in_area",
                   hddll::gGlobalState->vault_spawned_in_area);
      drawCharBool("flooded_mines", hddll::gGlobalState->flooded_mines);
      drawCharBool("skin_is_crawling", hddll::gGlobalState->skin_is_crawling);
      drawCharBool("dead_are_restless", hddll::gGlobalState->dead_are_restless);
      drawCharBool("rushing_water", hddll::gGlobalState->rushing_water);
      drawCharBool("is_haunted_castle", hddll::gGlobalState->is_haunted_castle);
      drawCharBool("at_haunted_castle_exit",
                   hddll::gGlobalState->at_haunted_castle_exit);
      drawCharBool("tiki_village", hddll::gGlobalState->tiki_village);
      drawCharBool("spawned_black_market_entrance",
                   hddll::gGlobalState->spawned_black_market_entrance);
      drawCharBool("unused_flag2", hddll::gGlobalState->unused_flag2);
      drawCharBool("spawned_haunted_castle_entrance",
                   hddll::gGlobalState->spawned_haunted_castle_entrance);
      drawCharBool("mothership_spawned",
                   hddll::gGlobalState->mothership_spawned);
      drawCharBool("moai_spawned", hddll::gGlobalState->moai_spawned);
      drawCharBool("is_blackmarket", hddll::gGlobalState->is_blackmarket);
      drawCharBool("at_blackmarket_exit",
                   hddll::gGlobalState->at_blackmarket_exit);
      drawCharBool("is_wet_fur", hddll::gGlobalState->is_wet_fur);
      drawCharBool("is_mothership", hddll::gGlobalState->is_mothership);
      drawCharBool("at_mothership_exit",
                   hddll::gGlobalState->at_mothership_exit);
      drawCharBool("is_city_of_gold", hddll::gGlobalState->is_city_of_gold);
      drawCharBool("at_city_of_gold_exit",
                   hddll::gGlobalState->at_city_of_gold_exit);
      drawCharBool("is_worm", hddll::gGlobalState->is_worm);
    }
  }

  if (ImGui::CollapsingHeader("Level State")) {

    ImGui::InputScalar("shop_type", ImGuiDataType_S32,
                       &hddll::gGlobalState->level_state->shop_type);
    ImGui::InputScalar("entrance_room_x", ImGuiDataType_S32,
                       &hddll::gGlobalState->level_state->entrance_room_x);
    ImGui::InputScalar("entrance_room_y", ImGuiDataType_S32,
                       &hddll::gGlobalState->level_state->entrance_room_y);
    ImGui::InputScalar("exit_room_x", ImGuiDataType_S32,
                       &hddll::gGlobalState->level_state->exit_room_x);
    ImGui::InputScalar("exit_room_y", ImGuiDataType_S32,
                       &hddll::gGlobalState->level_state->exit_room_y);
    ImGui::InputScalar("entrance_x", ImGuiDataType_Float,
                       &hddll::gGlobalState->level_state->entrance_x);
    ImGui::InputScalar("entrance_y", ImGuiDataType_Float,
                       &hddll::gGlobalState->level_state->entrance_y);
    ImGui::InputScalar("exit_x", ImGuiDataType_Float,
                       &hddll::gGlobalState->level_state->exit_x);
    ImGui::InputScalar("exit_y", ImGuiDataType_Float,
                       &hddll::gGlobalState->level_state->exit_y);
    ImGui::InputScalar("alt_exit_x", ImGuiDataType_Float,
                       &hddll::gGlobalState->level_state->alt_exit_x);
    ImGui::InputScalar("alt_exit_y", ImGuiDataType_Float,
                       &hddll::gGlobalState->level_state->alt_exit_y);
  }

  if (ImGui::CollapsingHeader("Camera State")) {

    ImGui::InputScalar("camera_x", ImGuiDataType_Float,
                       &hddll::gCameraState->camera_x);
    ImGui::InputScalar("camera_y", ImGuiDataType_Float,
                       &hddll::gCameraState->camera_y);
    ImGui::InputScalar("following_x", ImGuiDataType_Float,
                       &hddll::gCameraState->following_x);
    ImGui::InputScalar("following_y", ImGuiDataType_Float,
                       &hddll::gCameraState->following_y);
    ImGui::InputScalar("camera_max_left", ImGuiDataType_Float,
                       &hddll::gCameraState->camera_max_left);
    ImGui::InputScalar("camera_max_right", ImGuiDataType_Float,
                       &hddll::gCameraState->camera_max_right);
    ImGui::InputScalar("camera_max_up", ImGuiDataType_Float,
                       &hddll::gCameraState->camera_max_up);
    ImGui::InputScalar("camera_max_down", ImGuiDataType_Float,
                       &hddll::gCameraState->camera_max_down);
    ImGui::InputScalar("camera_speed", ImGuiDataType_Float,
                       &hddll::gCameraState->camera_speed);

    if (ImGui::CollapsingHeader("Camera Raw")) {
      drawRawBytesTable("Raw Bytes##CameraState", (char *)hddll::gCameraState,
                        92);
    }
  }

  if (ImGui::CollapsingHeader("Render Dupe")) {
    ImGui::InputFloat("insertion_point", &hddll::gGlobalState->insertion_point,
                      0.0001F, 0.0F, "%.4f");
    ImGui::Text("Filter:");
    ImGui::InputFloat("  Minimum Z", &gDebugState.MinZCutoff, 0.0001F, 0.0F,
                      "%.4f");
    ImGui::InputFloat("  Maximum Z", &gDebugState.MaxZCutoff, 0.0001F, 0.0F,
                      "%.4f");
    if (ImGui::BeginTable("Entities##Render Dupe", 4, ImGuiTableFlags_RowBg)) {

      ImGui::TableSetupColumn("Kind", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("Current Z", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("Deleted", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      for (size_t idx = 0;
           idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
        auto ent = hddll::gGlobalState->entities->entities_active[idx];
        if (!ent) {
          continue;
        }

        if (ent->current_z < gDebugState.MinZCutoff) {
          continue;
        }

        if (ent->current_z > gDebugState.MaxZCutoff) {
          continue;
        }

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("%s", ent->KindName());

        ImGui::TableNextColumn();
        ImGui::Text("%04d: %s", ent->entity_type, ent->TypeName());

        ImGui::TableNextColumn();
        ImGui::Text("%.4f", ent->current_z);

        ImGui::TableNextColumn();
        ImGui::Text("%d", ent->flag_deletion);
      }
      ImGui::EndTable();
    }
  }

  if (ImGui::CollapsingHeader("Audio")) {
    auto player = hddll::gGlobalState->player1;
    if (!player) {
      ImGui::Text("Need Player Entity");
      return;
    }

    if (ImGui::BeginListBox("##", {-1, -1})) {
      for (size_t idx = 0; idx < gAudioNames.size(); idx++) {
        if (ImGui::Selectable(gAudioNames[idx], false)) {
          player->PlaySound(gAudioNames[idx]);
        }
      }
      ImGui::EndListBox();
    }
  }
}
