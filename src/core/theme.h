#pragma once

#include "../core/api.h"
#include "../core/resource.h"

// Rebuild gui library once you change this file
enum FONT_LIST
{
    FONT_NULL,
    FONT_DEFAULT,
    FONT_CUSTOM1,
    FONT_CUSTOM2,
    FONT_CUSTOM3,
    FONT_CUSTOM4,
    FONT_CUSTOM5,
    FONT_CUSTOM6,

    FONT_MAX
};

enum IMAGE_LIST
{
    IMAGE_CUSTOM1,
    IMAGE_CUSTOM2,
    IMAGE_CUSTOM3,
    IMAGE_CUSTOM4,
    IMAGE_CUSTOM5,
    IMAGE_CUSTOM6,

    IMAGE_MAX
};

enum COLOR_LIST
{
    COLOR_WND_FONT,
    COLOR_WND_NORMAL,
    COLOR_WND_PUSHED,
    COLOR_WND_FOCUS,
    COLOR_WND_BORDER,

    COLOR_CUSTOME1,
    COLOR_CUSTOME2,
    COLOR_CUSTOME3,
    COLOR_CUSTOME4,
    COLOR_CUSTOME5,
    COLOR_CUSTOME6,

    COLOR_MAX
};

int c_theme_add_font(int index, const void *font);
const void *c_theme_get_font(int index);
int c_theme_add_image(int index, const void *image_info);
const void *c_theme_get_image(int index);
int c_theme_add_color(int index, unsigned int color);
unsigned int c_theme_get_color(int index);

void c_theme_init(void);