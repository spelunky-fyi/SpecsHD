#pragma once

#include <Windows.h>
#include <string>

#include "3rdparty/imgui/imgui.h"
#include "config.h"
#include "hd.h"
#include "hd_entity.h"

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

extern DWORD gBaseAddress;
extern ImDrawList *gOverlayDrawList;

extern CameraState *gCameraState;
extern GlobalState *gGlobalState;

extern Specs::Config *gConfig;

extern bool gPaused;
extern int gPauseAt;
extern int gFrame;

extern int gWindowedMode;
extern int gDisplayWidth;
extern int gDisplayHeight;

extern ModsState gModsState;
extern uint32_t gScreenStatePrevious;
