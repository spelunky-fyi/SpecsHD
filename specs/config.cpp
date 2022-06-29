#pragma once

#include "3rdparty/toml11/toml.hpp"

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
        auto key = (ImGuiKey)keyConfig[1].as_integer();
        auto keyMods = (ImGuiModFlags)keyConfig[2].as_integer();

        config->keys[featureIdx] = {keyMods, key};
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
        auto button = (ImGuiMouseButton)mouseConfig[1].as_integer();
        auto keyMods = (ImGuiModFlags)mouseConfig[2].as_integer();

        config->buttons[featureIdx] = {keyMods, button};
      }
    }
  } catch (std::exception &) {
    goto SAVE_AND_RETURN;
  }

  return config;

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
    auto keyMods = toml::integer(keyConfig.KeyMods);
    auto keyData = toml::value{{featureIdx, keyMods, key}};
    keys.push_back(keyData);
  }
  data["keys"] = keys;

  auto buttons = toml::array();
  for (size_t idx = 0; idx < MouseFeatures_COUNT; idx++) {
    auto mouseConfig = this->buttons[(MouseFeatures_)idx];
    auto featureIdx = toml::integer(idx);
    auto button = toml::integer(mouseConfig.Button);
    auto keyMods = toml::integer(mouseConfig.KeyMods);
    auto mouseData = toml::value{{featureIdx, keyMods, button}};
    buttons.push_back(mouseData);
  }
  data["mouse"] = buttons;

  std::ofstream out(gConfigFilename);
  out << std::setw(80) << data << std::endl;
  out.close();
}
