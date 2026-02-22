
#include "settings_tab.h"

#include "../state.h"

void drawSettingsTab() {

  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::CollapsingHeader("Mouse Controls")) {

    if (ImGui::BeginTable("Mouse Controls", 2)) {

      ImGui::TableSetupColumn("Function");
      ImGui::TableSetupColumn("Input");

      ImGui::TableHeadersRow();

      for (size_t idx = 0; idx < Specs::MouseFeatures_COUNT; idx++) {

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("%s",
                    Specs::GetMouseFeatureName((Specs::MouseFeatures_)idx));

        auto mouseConfig = gConfig->buttons[(Specs::MouseFeatures_)idx];
        ImGui::TableNextColumn();
        auto prettyName = mouseConfig.dbg();
        ImGui::Text("%s", prettyName.c_str());
      }

      ImGui::EndTable();
    }
  }
  if (ImGui::CollapsingHeader("Key Controls")) {

    if (ImGui::BeginTable("Key Controls", 2)) {

      ImGui::TableSetupColumn("Function");
      ImGui::TableSetupColumn("Input");

      ImGui::TableHeadersRow();

      for (size_t idx = 0; idx < Specs::KeyFeatures_COUNT; idx++) {

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("%s", Specs::GetKeyFeatureName((Specs::KeyFeatures_)idx));

        auto keyConfig = gConfig->keys[(Specs::KeyFeatures_)idx];
        ImGui::TableNextColumn();
        auto prettyName = keyConfig.dbg();
        ImGui::Text("%s", prettyName.c_str());
      }

      ImGui::EndTable();
    }
  }

  ImGui::DragFloat("Scale##StyleScale", &io.FontGlobalScale, 0.01f, 0.2f, 2.0f);
}
