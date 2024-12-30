#pragma once

#include <map>

#define TOML11_PRESERVE_COMMENTS_BY_DEFAULT
#include "3rdparty/toml11/toml.hpp"

#include "inputs.h"

namespace Specs {

class Config {
public:
  std::map<Specs::KeyFeatures_, Specs::KeyConfig> keys = {
      {Specs::KeyFeatures_Hide, {ImGuiModFlags_None, ImGuiKey_Insert}},
      {Specs::KeyFeatures_Engine_Pause, {ImGuiModFlags_Ctrl, ImGuiKey_Space}},
      {Specs::KeyFeatures_Engine_Frame_Advance,
       {ImGuiModFlags_None, ImGuiKey_PageDown}},
      {Specs::KeyFeatures_Reset_Run, {ImGuiModFlags_None, ImGuiKey_F5}},
      {
          Specs::KeyFeatures_Next_Level,
          {ImGuiModFlags_Ctrl, ImGuiKey_Equal},

      },
  };

  std::map<Specs::MouseFeatures_, Specs::MouseConfig> buttons = {
      {Specs::MouseFeatures_SpawnEntity,
       {ImGuiModFlags_None, ImGuiMouseButton_Left}},
      {Specs::MouseFeatures_SelectEntity,
       {ImGuiModFlags_None, ImGuiMouseButton_Middle}},
      {Specs::MouseFeatures_Teleport,
       {ImGuiModFlags_None, ImGuiMouseButton_Right}},
  };

  static Config *load();
  void save();
};
} // namespace Specs