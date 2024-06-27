#pragma once

#include "GuiLite.h"

#define UI_WIDTH    1280
#define UI_HEIGHT   720
#define COLOR_BYTES 2

void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver);
void *uicode_get_current_ui(int *width, int *height, bool force_update);
void uicode_on_touch(int x, int y, bool is_down);
void uicode_on_navigate(unsigned int key);
