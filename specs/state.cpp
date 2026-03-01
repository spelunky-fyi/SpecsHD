
#include "state.h"

// SpecsHD global state definitions
ImDrawList *gOverlayDrawList = NULL;

Specs::Config *gConfig = NULL;

bool gPaused = false;
int gPauseAt = 0;
int gFrame = 0;

ModsState gModsState = {};
uint32_t gScreenStatePrevious = 0;
