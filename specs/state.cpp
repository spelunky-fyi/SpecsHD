
#include "state.h"

// HDDLL global definitions
namespace hddll {
DWORD gBaseAddress = NULL;
CameraState *gCameraState = NULL;
GlobalState *gGlobalState = NULL;
} // namespace hddll

// SpecsHD global state definitions
ImDrawList *gOverlayDrawList = NULL;

Specs::Config *gConfig = NULL;

bool gPaused = false;
int gPauseAt = 0;
int gFrame = 0;

int gWindowedMode = 0;
int gDisplayWidth = 0;
int gDisplayHeight = 0;

ModsState gModsState = {};
uint32_t gScreenStatePrevious = 0;
