#include <stdio.h>
#include <assert.h>

#include "api.h"
#include "resource.h"
#include "word.h"
#include "image.h"

#define FONT_MAX  10
#define IMAGE_MAX 10
#define COLOR_MAX 10

static const void *s_font_map[FONT_MAX];
static const void *s_image_map[IMAGE_MAX];
static unsigned int s_color_map[COLOR_MAX];

c_lattice_font_op the_lattice_font_op;
c_bitmap_operator the_bitmap_op;

void c_theme_init(void)
{
    // load default font
    c_lattice_font_op_init(&the_lattice_font_op);
    c_word_init(&the_lattice_font_op.base);

    // load default image
    c_bitmap_operator_init(&the_bitmap_op);
    c_image_init(&the_bitmap_op.base);
}

int c_theme_add_font(int index, const void *font)
{
    if (index >= FONT_MAX)
    {
        GUILITE_ASSERT(0);
        return -1;
    }
    s_font_map[index] = font;
    return 0;
}

const void *c_theme_get_font(int index)
{
    if (index >= FONT_MAX)
    {
        GUILITE_ASSERT(0);
        return 0;
    }
    return s_font_map[index];
}

int c_theme_add_image(int index, const void *image_info)
{
    if (index >= IMAGE_MAX)
    {
        GUILITE_ASSERT(0);
        return -1;
    }
    s_image_map[index] = image_info;
    return 0;
}

const void *c_theme_get_image(int index)
{
    if (index >= IMAGE_MAX)
    {
        GUILITE_ASSERT(0);
        return 0;
    }
    return s_image_map[index];
}

int c_theme_add_color(int index, unsigned int color)
{
    if (index >= COLOR_MAX)
    {
        GUILITE_ASSERT(0);
        return -1;
    }
    s_color_map[index] = color;
    return 0;
}

unsigned int c_theme_get_color(int index)
{
    if (index >= COLOR_MAX)
    {
        GUILITE_ASSERT(0);
        return 0;
    }
    return s_color_map[index];
}