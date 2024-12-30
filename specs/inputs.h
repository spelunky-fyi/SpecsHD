#pragma once

#include <string>
#include <vector>

#include "3rdparty/imgui/imgui.h"

namespace Specs {

enum KeyFeatures_ {
  KeyFeatures_Hide,
  KeyFeatures_Engine_Pause,
  KeyFeatures_Engine_Frame_Advance,
  KeyFeatures_Reset_Run,
  KeyFeatures_Next_Level,

  // End of list
  KeyFeatures_COUNT,
};

enum MouseFeatures_ {
  MouseFeatures_SpawnEntity,
  MouseFeatures_SelectEntity,
  MouseFeatures_Teleport,

  // End of list
  MouseFeatures_COUNT,
};

struct KeyConfig {
  ImGuiModFlags KeyMods;
  ImGuiKey Key;

  std::string dbg();
};

struct MouseConfig {
  ImGuiModFlags KeyMods;
  ImGuiMouseButton Button;

  std::string dbg();
};

const char *GetKeyFeatureName(KeyFeatures_ key);
const char *GetMouseFeatureName(MouseFeatures_ key);
const char *GetMouseButtonName(ImGuiMouseButton button);

bool IsKeyPressed(KeyConfig keyConfig, bool repeat = false);
bool IsMouseClicked(Specs::MouseConfig mouseConfig, bool repeat = false);

} // namespace Specs