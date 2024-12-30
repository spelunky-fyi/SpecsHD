#include "3rdparty/imgui/imgui.h"

#include <numeric>

#include "inputs.h"

static const char *const _KeyFeatureNames[] = {
    "Hide", "Engine Pause", "Engine Frame Advance", "Reset Run", "Next Level",
};
static_assert(Specs::KeyFeatures_COUNT ==
              (sizeof(_KeyFeatureNames) / sizeof(Specs::KeyFeatures_)));

const char *Specs::GetKeyFeatureName(Specs::KeyFeatures_ key) {

  if (key >= KeyFeatures_COUNT || key < 0) {
    return "Unknown";
  }
  return _KeyFeatureNames[key];
}

static const char *const _MouseFeatureNames[] = {
    "Spawn Entity",
    "Select Entities",
    "Teleport",
};
static_assert(Specs::MouseFeatures_COUNT ==
              (sizeof(_MouseFeatureNames) / sizeof(Specs::MouseFeatures_)));

const char *Specs::GetMouseFeatureName(Specs::MouseFeatures_ button) {

  if (button >= MouseFeatures_COUNT || button < 0) {
    return "Unknown";
  }
  return _MouseFeatureNames[button];
}

static const char *const _MouseButtons[] = {
    "Left Button", "Right Button", "Middle Button", "Button 4", "Button 5",

};
static_assert(ImGuiMouseButton_COUNT ==
              (sizeof(_MouseButtons) / sizeof(ImGuiMouseButton_)));

const char *Specs::GetMouseButtonName(ImGuiMouseButton button) {

  if (button >= ImGuiMouseButton_COUNT || button < 0) {
    return "Unknown";
  }

  return _MouseButtons[button];
}

bool expectedModifiersSet(ImGuiModFlags mods) {
  auto io = ImGui::GetIO();
  if (mods & ImGuiModFlags_Ctrl && !io.KeyCtrl) {
    return false;
  }
  if (mods & ImGuiModFlags_Alt && !io.KeyAlt) {
    return false;
  }
  if (mods & ImGuiModFlags_Super && !io.KeySuper) {
    return false;
  }
  if (mods & ImGuiModFlags_Shift && !io.KeyShift) {
    return false;
  }
  return true;
}

bool Specs::IsKeyPressed(Specs::KeyConfig keyConfig, bool repeat) {
  if (!expectedModifiersSet(keyConfig.KeyMods)) {
    return false;
  }
  return ImGui::IsKeyPressed(keyConfig.Key, repeat);
}

bool Specs::IsMouseClicked(Specs::MouseConfig mouseConfig, bool repeat) {
  if (!expectedModifiersSet(mouseConfig.KeyMods)) {
    return false;
  }
  return ImGui::IsMouseClicked(mouseConfig.Button, repeat);
}

std::vector<std::string> getConfiguredKeyMods(ImGuiModFlags mods) {

  std::vector<std::string> configuredMods = {};

  if (mods & ImGuiModFlags_Ctrl) {
    configuredMods.push_back("Ctrl");
  }
  if (mods & ImGuiModFlags_Alt) {
    configuredMods.push_back("Alt");
  }
  if (mods & ImGuiModFlags_Super) {
    configuredMods.push_back("Super");
  }
  if (mods & ImGuiModFlags_Shift) {
    configuredMods.push_back("Shift");
  }

  return configuredMods;
}

std::string Specs::MouseConfig::dbg() {
  auto out = getConfiguredKeyMods(this->KeyMods);
  out.push_back(GetMouseButtonName(this->Button));

  return std::accumulate(out.begin(), out.end(), std::string{},
                         [](const std::string &ss, const std::string &s) {
                           return ss.empty() ? s : ss + " + " + s;
                         });
}

std::string Specs::KeyConfig::dbg() {

  auto out = getConfiguredKeyMods(this->KeyMods);
  out.push_back(ImGui::GetKeyName(this->Key));
  return std::accumulate(out.begin(), out.end(), std::string{},
                         [](const std::string &ss, const std::string &s) {
                           return ss.empty() ? s : ss + " + " + s;
                         });
}