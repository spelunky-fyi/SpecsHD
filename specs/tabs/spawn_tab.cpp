
#include "spawn_tab.h"

#include "../state.h"
#include <hddll/entities.h>
#include <misc/cpp/imgui_stdlib.h>

SpawnState gSpawnState = {};

void drawSpawnTab() {
  ImGuiIO &io = ImGui::GetIO();

  auto scrollLock = true;

  if (gSpawnState.SpawnEntityInputs.empty()) {
    gSpawnState.SpawnEntityInputs.push_back(SpawnEntityConfig{-1, true});
  }

  int eraseIdx = -1;
  for (size_t idx = 0; idx < gSpawnState.SpawnEntityInputs.size(); idx++) {
    auto &spawnEntityConfig = gSpawnState.SpawnEntityInputs[idx];

    ImGui::PushItemWidth(200 * io.FontGlobalScale);

    if (idx > 0) {
      if (ImGui::Button(std::format("-##SpawnEntityInput-{}", idx).c_str())) {
        eraseIdx = static_cast<int>(idx);
      }
    } else {
      auto size = ImGui::CalcTextSize("-");
      size.x += 8.0f;
      ImGui::InvisibleButton(std::format("-##SpawnEntityInput-{}", idx).c_str(),
                             size);
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::InputInt(
            std::format("Entity ID##SpawnEntityInputId-{}", idx).c_str(),
            &spawnEntityConfig.entityType)) {
      scrollLock = false;
    };
    ImGui::SameLine();
    ImGui::Checkbox(std::format("Active##SpawnEntityInputId-{}", idx).c_str(),
                    &spawnEntityConfig.activeEntity);
  }
  if (eraseIdx >= 0) {
    gSpawnState.SpawnEntityInputs.erase(gSpawnState.SpawnEntityInputs.begin() +
                                        eraseIdx);
  }

  if (ImGui::Button("Add Additional Entity")) {
    gSpawnState.SpawnEntityInputs.push_back(SpawnEntityConfig{-1, true});
  }

  ImGui::Separator();

  if (ImGui::Button("Spawn") && hddll::gGlobalState->player1) {
    for (auto const &spawnEntityConfig : gSpawnState.SpawnEntityInputs) {

      if (spawnEntityConfig.entityType >= 0) {
        if (spawnEntityConfig.entityType == 0) {
          hddll::gGlobalState->SpawnHiredHand(hddll::gGlobalState->player1->x,
                                              hddll::gGlobalState->player1->y,
                                              90);
        } else {
          hddll::gGlobalState->SpawnEntity(
              hddll::gGlobalState->player1->x, hddll::gGlobalState->player1->y,
              spawnEntityConfig.entityType, spawnEntityConfig.activeEntity);
        }
      }
    }
  }
  ImGui::SameLine();
  ImGui::Checkbox("Click to spawn", &gSpawnState.ClickToSpawn);

  ImGui::Separator();
  if (ImGui::InputText("Filter", &gSpawnState.EntityListFilter)) {
    std::transform(gSpawnState.EntityListFilter.begin(),
                   gSpawnState.EntityListFilter.end(),
                   gSpawnState.EntityListFilter.begin(), ::tolower);
  }

  if (ImGui::BeginListBox("##", {-1, -1})) {
    auto spawnEntityConfig = &gSpawnState.SpawnEntityInputs.back();
    for (auto const &[name, entity_type] : hddll::gEntities) {

      std::string lowerName = name;
      std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                     ::tolower);
      if (!gSpawnState.EntityListFilter.empty() &&
          lowerName.find(gSpawnState.EntityListFilter) == std::string::npos) {
        continue;
      }
      auto label = std::format("{:4}: {}", entity_type, name);
      const bool is_selected = (entity_type == spawnEntityConfig->entityType);
      if (ImGui::Selectable(label.c_str(), is_selected)) {
        spawnEntityConfig->entityType = entity_type;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
        if (!scrollLock) {
          ImGui::SetScrollHereY();
        }
      }
    }
    ImGui::EndListBox();
  }
}
