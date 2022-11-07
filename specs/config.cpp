#pragma once

#include "config.h"

const std::string gConfigFilename = "specs-hd.toml";

Specs::Config *Specs::Config::load() {
  auto config = new Config();
  toml::value data;

  try {
    data = toml::parse(gConfigFilename);
  } catch (std::exception &) {
    goto SAVE_AND_RETURN;
  }

  try {
    if (data.contains("keys")) {
      auto keys = data["keys"].as_array();
      for (size_t idx = 0; idx < keys.size(); idx++) {
        auto keyConfig = keys[idx].as_array();
        if (keyConfig.size() != 3) {
          continue;
        }

        if (!keyConfig[0].is_integer() || !keyConfig[1].is_integer() ||
            !keyConfig[2].is_integer()) {
          continue;
        }

        auto featureIdx = (KeyFeatures_)keyConfig[0].as_integer();
        auto keyMods = (ImGuiModFlags)keyConfig[1].as_integer();
        auto key = (ImGuiKey)keyConfig[2].as_integer();

        auto actualKeyMods = 0;
        if (keyMods & 1)
          actualKeyMods |= ImGuiMod_Ctrl;
        if (keyMods & 2)
          actualKeyMods |= ImGuiMod_Shift;
        if (keyMods & 4)
          actualKeyMods |= ImGuiMod_Alt;
        if (keyMods & 8)
          actualKeyMods |= ImGuiMod_Super;

        config->keys[featureIdx] = {actualKeyMods, key};
      }
    }
  } catch (std::exception &) {
    goto SAVE_AND_RETURN;
  }

  try {
    if (data.contains("mouse")) {
      auto mouse = data["mouse"].as_array();
      for (size_t idx = 0; idx < mouse.size(); idx++) {
        auto mouseConfig = mouse[idx].as_array();
        if (mouseConfig.size() != 3) {
          continue;
        }

        if (!mouseConfig[0].is_integer() || !mouseConfig[1].is_integer() ||
            !mouseConfig[2].is_integer()) {
          continue;
        }

        auto featureIdx = (MouseFeatures_)mouseConfig[0].as_integer();
        auto keyMods = (ImGuiModFlags)mouseConfig[1].as_integer();
        auto button = (ImGuiMouseButton)mouseConfig[2].as_integer();

        auto actualKeyMods = 0;
        if (keyMods & 1)
          actualKeyMods |= ImGuiMod_Ctrl;
        if (keyMods & 2)
          actualKeyMods |= ImGuiMod_Shift;
        if (keyMods & 4)
          actualKeyMods |= ImGuiMod_Alt;
        if (keyMods & 8)
          actualKeyMods |= ImGuiMod_Super;

        config->buttons[featureIdx] = {actualKeyMods, button};
      }
    }
  } catch (std::exception &) {
    goto SAVE_AND_RETURN;
  }

SAVE_AND_RETURN:
  config->save();
  return config;
}

void Specs::Config::save() {
  toml::value data;
  auto keys = toml::array();
  for (size_t idx = 0; idx < KeyFeatures_COUNT; idx++) {
    auto keyConfig = this->keys[(KeyFeatures_)idx];
    auto featureIdx = toml::integer(idx);
    auto key = toml::integer(keyConfig.Key);

    auto savedKeyMods = 0;
    if (keyConfig.KeyMods & ImGuiMod_Ctrl)
      savedKeyMods |= 1;
    if (keyConfig.KeyMods & ImGuiMod_Shift)
      savedKeyMods |= 2;
    if (keyConfig.KeyMods & ImGuiMod_Alt)
      savedKeyMods |= 4;
    if (keyConfig.KeyMods & ImGuiMod_Super)
      savedKeyMods |= 8;

    auto keyMods = toml::integer(savedKeyMods);
    auto keyData = toml::value{{featureIdx, keyMods, key}};
    keyData.comments().push_back(GetKeyFeatureName((KeyFeatures_)idx));

    keys.push_back(keyData);
  }
  data["keys"] = keys;

  auto buttons = toml::array();
  for (size_t idx = 0; idx < MouseFeatures_COUNT; idx++) {
    auto mouseConfig = this->buttons[(MouseFeatures_)idx];
    auto featureIdx = toml::integer(idx);
    auto button = toml::integer(mouseConfig.Button);

    auto savedKeyMods = 0;
    if (mouseConfig.KeyMods & ImGuiMod_Ctrl)
      savedKeyMods |= 1;
    if (mouseConfig.KeyMods & ImGuiMod_Shift)
      savedKeyMods |= 2;
    if (mouseConfig.KeyMods & ImGuiMod_Alt)
      savedKeyMods |= 4;
    if (mouseConfig.KeyMods & ImGuiMod_Super)
      savedKeyMods |= 8;

    auto keyMods = toml::integer(savedKeyMods);
    auto mouseData = toml::value{{featureIdx, keyMods, button}};
    mouseData.comments().push_back(GetMouseFeatureName((MouseFeatures_)idx));
    buttons.push_back(mouseData);
  }
  data["mouse"] = buttons;

  std::ofstream out(gConfigFilename);
  out << std::setw(80) << data << std::endl;
  out.close();
}
