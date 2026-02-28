
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
