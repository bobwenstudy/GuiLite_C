#pragma once

#include "display.h"

#define SURFACE_COLOR_BYTES 4

typedef struct c_surface_transparent c_surface_transparent;
struct c_surface_transparent
{
    c_surface base;
    int m_display_color_bytes;
};

void c_surface_transparent_correct_color_after_display(c_surface_transparent *self);
void c_surface_transparent_clear_layer(c_surface_transparent *self, unsigned int z_order);
void c_surface_transparent_vir_draw_pixel(c_surface *self, int x, int y, unsigned int rgb, unsigned int z_order);
void c_surface_transparent_vir_fill_rect(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order);
void c_surface_transparent_init(c_surface_transparent *self, unsigned int width, unsigned int height, unsigned int color_bytes, Z_ORDER_LEVEL max_zorder);
