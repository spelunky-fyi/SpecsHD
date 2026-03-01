#pragma once

#include <Windows.h>
#include <string>

#include "config.h"
#include <hddll/hd.h>
#include <hddll/hd_entity.h>
#include <hddll/hddll.h>
#include <imgui.h>

// Structs

struct ModsState {
  bool TheFullSpelunky = false;
  bool Biglunky = false;
  bool DarkMode = false;
  bool Uplunky = false;
  bool TunnelMan = false;
  bool SeededMode = false;
};

// Extern globals

extern ImDrawList *gOverlayDrawList;

extern Specs::Config *gConfig;

extern bool gPaused;
extern int gPauseAt;
extern int gFrame;

extern ModsState gModsState;
extern uint32_t gScreenStatePrevious;
