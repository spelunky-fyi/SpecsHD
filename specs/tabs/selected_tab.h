#pragma once

#include "../state.h"

struct SelectedEntityState {
  bool Clicking = false;
  Entity *Entity = NULL;
};

extern SelectedEntityState gSelectedEntityState;

void drawSelectedEntityTab();
