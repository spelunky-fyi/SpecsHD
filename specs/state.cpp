
#include "state.h"

// Global state definitions

DWORD gBaseAddress = NULL;
ImDrawList *gOverlayDrawList = NULL;

CameraState *gCameraState = NULL;
GlobalState *gGlobalState = NULL;

Specs::Config *gConfig = NULL;

bool gPaused = false;
int gPauseAt = 0;
int gFrame = 0;

int gWindowedMode = 0;
int gDisplayWidth = 0;
int gDisplayHeight = 0;

ModsState gModsState = {};
uint32_t gScreenStatePrevious = 0;
