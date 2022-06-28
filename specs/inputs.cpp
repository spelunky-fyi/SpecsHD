#include "3rdparty/imgui/imgui.h"

#include "inputs.h"

static const char *const _KeyFeatureNames[] = {
    "Hide",
    "Engine Pause",
    "Engine Frame Advance",
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
    "Left", "Right", "Middle", "4", "5",

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