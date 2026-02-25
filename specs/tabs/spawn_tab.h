#pragma once

#include <string>
#include <vector>

#include <imgui.h>

struct SpawnEntityConfig {
  int entityType = -1;
  bool activeEntity;
};

struct SpawnState {
  std::vector<SpawnEntityConfig> SpawnEntityInputs;
  std::string EntityListFilter;
  ImVec2 ClickedAt = {0, 0};
  bool ClickToSpawn = false;
  bool Clicking = false;
};

extern SpawnState gSpawnState;

void drawSpawnTab();
