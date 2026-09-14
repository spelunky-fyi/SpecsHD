
#include "never_dark_mode.h"

void prePlaceRoomsNeverDarkMode() {
    if (gModsState.NeverDarkMode) {
        hddll::gGlobalState->dark_level = 0;
    }
}