
#include "selected_tab.h"

#include "debug_tab.h"

#include "../drawing.h"
#include "../state.h"

SelectedEntityState gSelectedEntityState = {};

static void drawRawBytesTableForSelected(const char *str_id, char *start_addr,
                                         size_t size) {
  static bool editMode = false;
  ImGui::Checkbox("Edit Mode", &editMode);

  if (ImGui::BeginTable(str_id, 6, ImGuiTableFlags_RowBg)) {

    ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Bytes", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Signed", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Unsigned", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Hex", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Float", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    for (size_t i = 0; i < size; i += 4) {

      ImGui::TableNextRow();
      ImGui::PushID((int)i);

      char *addr = start_addr + i;
      ImGui::TableNextColumn();
      ImGui::Text("0x%X", i);

      if (editMode) {
        float byteWidth = ImGui::CalcTextSize("FF").x +
                          ImGui::GetStyle().FramePadding.x * 2 + 4;

        ImGui::TableNextColumn();
        ImGui::PushItemWidth(byteWidth);
        ImGui::InputScalar("##b0", ImGuiDataType_U8, (uint8_t *)addr, NULL,
                           NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::SameLine();
        ImGui::InputScalar("##b1", ImGuiDataType_U8, (uint8_t *)(addr + 1),
                           NULL, NULL, "%02X",
                           ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::SameLine();
        ImGui::InputScalar("##b2", ImGuiDataType_U8, (uint8_t *)(addr + 2),
                           NULL, NULL, "%02X",
                           ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::SameLine();
        ImGui::InputScalar("##b3", ImGuiDataType_U8, (uint8_t *)(addr + 3),
                           NULL, NULL, "%02X",
                           ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::PopItemWidth();

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputScalar("##s", ImGuiDataType_S32, (int32_t *)addr);

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputScalar("##u", ImGuiDataType_U32, (uint32_t *)addr);

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputScalar("##h", ImGuiDataType_U32, (uint32_t *)addr, NULL,
                           NULL, "%08X", ImGuiInputTextFlags_CharsHexadecimal);

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputFloat("##f", (float *)addr, 0.0f, 0.0f, "%.6f");
      } else {
        std::pair<hddll::EntityKind, uint32_t> key = {
            gSelectedEntityState.Entity->entity_kind,
            gSelectedEntityState.Entity->entity_type};

        {
          uint32_t a1, a2, a3, a4;
          a1 = (*(addr)) & (0xFF);
          a2 = (*(addr + 1)) & (0xFF);
          a3 = (*(addr + 2)) & (0xFF);
          a4 = (*(addr + 3)) & (0xFF);

          ImGui::TableNextColumn();
          if (ImGui::Button(
                  std::format("{:02X}##SelectedEntityRaw-{}", a1, i)
                      .c_str())) {
            gDebugState.DrawEntityOffsets[key].insert({i, DataType_Byte});
          }
          ImGui::SameLine();

          if (ImGui::Button(
                  std::format("{:02X}##SelectedEntityRaw-{}", a2, i + 1)
                      .c_str())) {
            gDebugState.DrawEntityOffsets[key].insert({i + 1, DataType_Byte});
          }
          ImGui::SameLine();

          if (ImGui::Button(
                  std::format("{:02X}##SelectedEntityRaw-{}", a3, i + 2)
                      .c_str())) {
            gDebugState.DrawEntityOffsets[key].insert({i + 2, DataType_Byte});
          }
          ImGui::SameLine();

          if (ImGui::Button(
                  std::format("{:02X}##SelectedEntityRaw-{}", a4, i + 3)
                      .c_str())) {
            gDebugState.DrawEntityOffsets[key].insert({i + 3, DataType_Byte});
          }
        }

        ImGui::TableNextColumn();
        if (ImGui::Button(
                std::format("{:d}##SelectedEntityRaw-{}", *(int32_t *)addr, i)
                    .c_str(),
                {-1, 0})) {
          gDebugState.DrawEntityOffsets[key].insert(
              {i, DataType_Dword_Signed});
        }

        ImGui::TableNextColumn();
        if (ImGui::Button(
                std::format("{:d}##SelectedEntityRaw-{}", *(uint32_t *)addr, i)
                    .c_str(),
                {-1, 0})) {
          gDebugState.DrawEntityOffsets[key].insert(
              {i, DataType_Dword_Unsigned});
        }

        ImGui::TableNextColumn();
        if (ImGui::Button(std::format("0x{:08X}##SelectedEntityRaw-{}",
                                      *(uint32_t *)addr, i)
                              .c_str(),
                          {-1, 0})) {
          gDebugState.DrawEntityOffsets[key].insert({i, DataType_Dword_Hex});
        }

        ImGui::TableNextColumn();
        if (ImGui::Button(
                std::format("{:f}##SelectedEntityRaw-{}", *(float *)addr, i)
                    .c_str(),
                {-1, 0})) {
          gDebugState.DrawEntityOffsets[key].insert({i, DataType_Float});
        }
      }

      ImGui::PopID();
    }

    ImGui::EndTable();
  }
}

void drawSelectedEntityTab() {
  ImGuiIO &io = ImGui::GetIO();
  if (!gSelectedEntityState.Entity) {
    ImGui::Text("No selected entity");
    return;
  }
  ImGui::Text("Address: 0x%X", (uint32_t)gSelectedEntityState.Entity);
  ImGui::InputInt("Entity ID",
                  (int *)&gSelectedEntityState.Entity->entity_type);
  ImGui::InputInt("Entity kind",
                  (int *)&gSelectedEntityState.Entity->entity_kind);

  if (ImGui::Button(std::format("Follow With Camera##FollowEnt-{}",
                                (uint32_t)gSelectedEntityState.Entity)
                        .c_str())) {
    hddll::gCameraState->camera_following = gSelectedEntityState.Entity;
  }

  if (hddll::gCameraState->camera_following == gSelectedEntityState.Entity) {
    ImGui::SameLine();
    if (ImGui::Button("Stop Following##FollowEnt")) {
      hddll::gCameraState->camera_following = nullptr;
    }
  }

  if (ImGui::CollapsingHeader("Position, hitbox, etc.")) {
    ImGui::InputFloat("Entity x", &gSelectedEntityState.Entity->x, 0.0F, 0.0F,
                      "%.6f");
    ImGui::InputFloat("Entity y", &gSelectedEntityState.Entity->y, 0.0F, 0.0F,
                      "%.6f");
    ImGui::SliderFloat("width", &gSelectedEntityState.Entity->width, 0.0, 10.0);
    ImGui::SliderFloat("height", &gSelectedEntityState.Entity->height, 0.0,
                       10.0);
    ImGui::SliderFloat("current_z", &gSelectedEntityState.Entity->current_z,
                       0.0, 50.0, "%.4f");
    ImGui::SliderFloat("original_z", &gSelectedEntityState.Entity->original_z,
                       0.0, 50.0, "%.4f");
    ImGui::SliderFloat("alpha", &gSelectedEntityState.Entity->alpha, 0.0, 1.0);
    ImGui::SliderFloat("hitbox up", &gSelectedEntityState.Entity->hitbox_up,
                       0.0, 5.0);
    ImGui::SliderFloat("hitbox down", &gSelectedEntityState.Entity->hitbox_down,
                       0.0, 5.0);
    ImGui::SliderFloat("hitbox x", &gSelectedEntityState.Entity->hitbox_x, 0.0,
                       5.0);
    ImGui::SliderAngle("angle", &gSelectedEntityState.Entity->angle);
  }
  if (ImGui::CollapsingHeader("Flags1")) {
    drawCharBool("flag_deletion", gSelectedEntityState.Entity->flag_deletion);
    drawCharBool("flag_horizontal_flip",
                 gSelectedEntityState.Entity->flag_horizontal_flip);
    drawCharBool("flag_3", gSelectedEntityState.Entity->flag_3);
    drawCharBool("flag_4", gSelectedEntityState.Entity->flag_4);
    drawCharBool("flag_5", gSelectedEntityState.Entity->flag_5);
    drawCharBool("flag_6", gSelectedEntityState.Entity->flag_6);
    drawCharBool("flag_7", gSelectedEntityState.Entity->flag_7);
    drawCharBool("flag_8", gSelectedEntityState.Entity->flag_8);
    drawCharBool("flag_9", gSelectedEntityState.Entity->flag_9);
    drawCharBool("flag_10", gSelectedEntityState.Entity->flag_10);
    drawCharBool("flag_11", gSelectedEntityState.Entity->flag_11);
    drawCharBool("flag_12", gSelectedEntityState.Entity->flag_12);
    drawCharBool("flag_13", gSelectedEntityState.Entity->flag_13);
    drawCharBool("flag_14", gSelectedEntityState.Entity->flag_14);
    drawCharBool("flag_15", gSelectedEntityState.Entity->flag_15);
    drawCharBool("flag_16", gSelectedEntityState.Entity->flag_16);
    drawCharBool("flag_17", gSelectedEntityState.Entity->flag_17);
    drawCharBool("flag_18", gSelectedEntityState.Entity->flag_18);
    drawCharBool("flag_19", gSelectedEntityState.Entity->flag_19);
    drawCharBool("flag_20", gSelectedEntityState.Entity->flag_20);
    drawCharBool("flag_21", gSelectedEntityState.Entity->flag_21);
    drawCharBool("flag_22", gSelectedEntityState.Entity->flag_22);
    drawCharBool("flag_23", gSelectedEntityState.Entity->flag_23);
    drawCharBool("flag_24", gSelectedEntityState.Entity->flag_24);
  }
  if (gSelectedEntityState.Entity->entity_kind ==
          hddll::EntityKind::KIND_PLAYER &&
      ImGui::CollapsingHeader("EntityPlayer")) {
    auto entityPlayer =
        reinterpret_cast<hddll::EntityPlayer *>(gSelectedEntityState.Entity);
    ImGui::InputInt("Following", (int *)&entityPlayer->following);
    ImGui::InputInt("Follower", (int *)&entityPlayer->follower);
  }
  if (gSelectedEntityState.Entity->entity_kind ==
      hddll::EntityKind::KIND_PLAYER) {
    auto entityPlayer =
        reinterpret_cast<hddll::EntityPlayer *>(gSelectedEntityState.Entity);
    auto bot = entityPlayer->ai_bot;
    if (bot && ImGui::CollapsingHeader("AIBot")) {
      ImGui::Text("Address: 0x%X", (uint32_t)bot);
      ImGui::Text("Deathmatch mode: %s",
                  bot->bIs_deathmatch_mode ? "yes" : "no");
      ImGui::Text("Personality id: %u", bot->dwPersonality_id);

      // AiState
      {
        const char *states[] = {"HUNT",   "ITEM_PURSUIT",     "MOVE_TO_ANCHOR",
                                "WANDER", "ESCAPE_OFFSCREEN", "HOLD_POSITION"};
        int state = (int)bot->ai_state;
        if (state >= 0 && state < 6) {
          if (ImGui::Combo("AI State", &state, states, 6)) {
            bot->ai_state = (hddll::AiState)state;
          }
        } else {
          ImGui::Text("AI State: %d (raw)", state);
        }
      }
      // CombatAction (NONE == -1)
      {
        const char *actions[] = {"NONE",
                                 "BOMB_PRESS",
                                 "BOMB_HOLD",
                                 "THROW_HELD_UP",
                                 "LIVE_BOMB_DISPOSE",
                                 "JUMP_DIRECTIONAL",
                                 "THROW_ROPE",
                                 "DROP_AND_PURSUE"};
        int action = (int)bot->combat_action + 1;
        if (action >= 0 && action < 8) {
          if (ImGui::Combo("Combat Action", &action, actions, 8)) {
            bot->combat_action = (hddll::CombatAction)(action - 1);
          }
        } else {
          ImGui::Text("Combat Action: %d (raw)", (int)bot->combat_action);
        }
      }

      ImGui::InputInt("State Timer", &bot->state_timer);
      ImGui::InputInt("Repath Cooldown", &bot->repath_cooldown);
      ImGui::InputInt("Combat Cooldown", &bot->nCombat_cooldown);
      ImGui::InputInt("Prefer Ranged Stance", &bot->nPrefer_ranged_stance);

      drawCharBool("Path Dirty", bot->bPath_dirty);
      drawCharBool("Pathfind Found Path", bot->bPathfind_found_path);
      drawCharBool("Path Step Advanced", bot->bPath_step_advanced);
      ImGui::Text("Open / Closed nodes: %d / %d", bot->nPathfind_open_count,
                  bot->nPathfind_closed_count);

      // Live controller inputs the bot is feeding the engine this frame.
      ImGui::Separator();
      ImGui::TextDisabled("Live Inputs");
      if (entityPlayer->pPlayer_input) {
        auto in = entityPlayer->pPlayer_input;
        ImGui::Text("Move:  L/R %+d    U/D %+d", in->left_right, in->up_down);
        struct {
          const char *name;
          uint8_t value;
        } buttons[] = {
            {"jump(0)", in->action_0},  {"djump(1)", in->action_1},
            {"bomb(2)", in->action_2},  {"rope(3)", in->action_3},
            {"a4", in->action_4},       {"a5", in->action_5},
            {"a6", in->action_6},       {"run(7)", in->action_7},
            {"a8", in->action_8},       {"a9", in->action_9},
        };
        for (int i = 0; i < 10; i++) {
          if (i % 5 != 0) {
            ImGui::SameLine();
          }
          auto color = buttons[i].value ? ImVec4{0.2f, 1.0f, 0.3f, 1.0f}
                                        : ImVec4{0.4f, 0.4f, 0.4f, 1.0f};
          ImGui::TextColored(color, "%s", buttons[i].name);
        }
      } else {
        ImGui::TextDisabled("(no PlayerInput)");
      }

      // Entity-side flags that gate the bot's movement decisions.
      ImGui::Separator();
      ImGui::TextDisabled("AI Movement Gates (entity flags)");
      ImGui::Text("0x1ed collidable: %d    0x1f0 ai-suspended: %d",
                  (int)(uint8_t)entityPlayer->field49_0x1ed,
                  (int)(uint8_t)entityPlayer->field52_0x1f0);
      ImGui::Text("0x200 climb: %d    0x203 hang: %d    0x205 rope: %d",
                  (int)(uint8_t)entityPlayer->field68_0x200,
                  (int)(uint8_t)entityPlayer->field71_0x203,
                  (int)(uint8_t)entityPlayer->field73_0x205);
      ImGui::Separator();

      ImGui::InputFloat2("Target (x,y)", &bot->flTarget_x);
      ImGui::InputFloat2("Anchor (x,y)", &bot->flAnchor_x);
      ImGui::Text("Wander target: %d, %d", bot->nWander_target_x_int,
                  bot->nWander_target_y_int);
      ImGui::Text("Ledge walk timer: %d  state: %d", bot->nLedge_walk_timer,
                  bot->bLedge_walk_state);
      ImGui::Text("Tracked wins (p0-p3): %d / %d / %d / %d",
                  bot->nTracked_wins_p0, bot->nTracked_wins_p1,
                  bot->nTracked_wins_p2, bot->nTracked_wins_p3);

      // Current item / hunt target
      if (bot->pCurrent_item_target) {
        auto t = bot->pCurrent_item_target;
        ImGui::Text("Item target: 0x%X  %04d %s", (uint32_t)t, t->entity_type,
                    t->TypeName());
        ImGui::SameLine();
        if (ImGui::Button("Select##AIBotItemTarget")) {
          gSelectedEntityState.Entity = t;
        }
      } else {
        ImGui::Text("Item target: none");
      }

      // Perception list
      int perceptionCount = 0;
      for (int i = 0; i < 128; i++) {
        if (!bot->pPerception_list[i]) {
          break;
        }
        perceptionCount++;
      }
      if (ImGui::CollapsingHeader(
              std::format("Perception List ({})##AIBotPerception",
                          perceptionCount)
                  .c_str())) {
        ImGui::TextDisabled("entities the bot can currently 'see', nearest first-ish");
        for (int i = 0; i < perceptionCount; i++) {
          auto e = bot->pPerception_list[i];
          float dx = e->x - entityPlayer->x;
          float dy = e->y - entityPlayer->y;
          float distSq = dx * dx + dy * dy;
          if (ImGui::Button(
                  std::format("{}: {:04d} {} ({})  d^2={:.1f}##AIBotPerc-{}", i,
                              e->entity_type, e->TypeName(), e->KindName(),
                              distSq, i)
                      .c_str())) {
            gSelectedEntityState.Entity = e;
          }
        }
      }

      // Path nodes
      if (ImGui::CollapsingHeader("Path Nodes##AIBot")) {
        auto describe = [](const char *label, hddll::PathNode *n) {
          if (!n) {
            ImGui::Text("%s: null", label);
            return;
          }
          ImGui::Text(
              "%s: cell (%d,%d) world (%d,%d) tile=%d danger=%d blocked=%d",
              label, n->nCell_x, n->nCell_y, n->nWorld_x, n->nWorld_y,
              n->nTile_type, n->bDanger_flag, n->bBlocked_flag);
        };
        describe("Current", bot->pPath_current_node);
        describe("Active ", bot->pPath_active_node);
        describe("Goal   ", bot->pPath_start_node);
      }
    }
  }
  if ((uint32_t)gSelectedEntityState.Entity->entity_kind > 0 &&
      (uint32_t)gSelectedEntityState.Entity->entity_kind < 5 &&
      ImGui::CollapsingHeader("EntityActive")) {
    auto entityActive =
        reinterpret_cast<hddll::EntityActive *>(gSelectedEntityState.Entity);
    ImGui::InputInt("Health", &entityActive->health);
    ImGui::InputInt("Favor given", &entityActive->favor_given);
    ImGui::InputFloat("Velocity x", &entityActive->velocity_x);
    ImGui::InputFloat("Velocity y", &entityActive->velocity_y);
    if (ImGui::CollapsingHeader("Flags2")) {
      for (size_t i = 0x1f0; i <= 0x218; ++i) {
        char *addr = ((char *)gSelectedEntityState.Entity) + i;
        drawCharBool(std::format("Flag {:X}", i).c_str(), *addr);
      }
    }
  }
  if ((uint32_t)gSelectedEntityState.Entity->entity_kind == 0 &&
      ImGui::CollapsingHeader("EntityFloor")) {
    auto entityFloor =
        reinterpret_cast<hddll::EntityFloor *>(gSelectedEntityState.Entity);

    drawCharBool("Make Door onDestroy", entityFloor->field8_0x143);
  }

  if (ImGui::CollapsingHeader("Raw Entity Values")) {
    if (ImGui::Button("Copy to Clipboard")) {
      ImGui::LogToClipboard();
      for (size_t i = 0;
           i < sizeofEntityKind(gSelectedEntityState.Entity->entity_kind);
           i += 4) {
        char *addr = ((char *)gSelectedEntityState.Entity) + i;
        ImGui::LogText("0x%02X: %f 0x%02X\n", i, *(float *)addr,
                       *(uint32_t *)addr);
      }
      ImGui::LogFinish();
    }

    drawRawBytesTableForSelected(
        "Entity Raw Bytes", ((char *)gSelectedEntityState.Entity),
        sizeofEntityKind(gSelectedEntityState.Entity->entity_kind));
  }
  if (gSelectedEntityState.Entity->flag_deletion == 1) {
    gSelectedEntityState.Entity = NULL;
  }
}
