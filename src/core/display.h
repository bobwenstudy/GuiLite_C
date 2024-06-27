#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "api.h"

#define SURFACE_CNT_MAX 6 // root + pages

typedef enum
{
    Z_ORDER_LEVEL_0, // lowest graphic level
    Z_ORDER_LEVEL_1, // middle graphic level, call activate_layer before use it, draw everything inside the active rect.
    Z_ORDER_LEVEL_2, // highest graphic level, call activate_layer before use it, draw everything inside the active rect.
    Z_ORDER_LEVEL_MAX
} Z_ORDER_LEVEL;

typedef struct
{
    void (*draw_pixel)(int x, int y, unsigned int rgb);
    void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} DISPLAY_DRIVER;

typedef struct c_surface c_surface;

typedef struct c_display c_display;

typedef struct
{
    void *fb;    // framebuffer
    c_rect rect; // framebuffer area
    c_rect active_rect;
} c_layer;

typedef struct c_display_vir_api c_display_vir_api;
struct c_display_vir_api
{
    void (*draw_pixel)(c_display *wnd, int x, int y, unsigned int rgb);
    void (*fill_rect)(c_display *wnd, int x0, int y0, int x1, int y1, unsigned int rgb);
    int (*flush_screen)(c_display *wnd, int left, int top, int right, int bottom, void *fb, int fb_width);
};

struct c_display
{
    int m_width;              // in pixels
    int m_height;             // in pixels
    int m_color_bytes;        // 16/32 bits for default
    void *m_phy_fb;           // physical framebuffer for default
    DISPLAY_DRIVER *m_driver; // Rendering by external method without default physical framebuffer

    int m_phy_read_index;  // use for buffer update check
    int m_phy_write_index; // use for buffer update check
    c_surface *m_surface_group[SURFACE_CNT_MAX];
    int m_surface_cnt; // surface count
    int m_surface_index;

    const c_display_vir_api *m_api; // virtual api
};

typedef struct c_surface_vir_api c_surface_vir_api;
struct c_surface_vir_api
{
    void (*draw_pixel)(c_surface *self, int x, int y, unsigned int rgb, unsigned int z_order);
    void (*fill_rect)(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order);
    void (*fill_rect_low_level)(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb);
    void (*draw_pixel_low_level)(c_surface *self, int x, int y, unsigned int rgb);
};

struct c_surface
{
    int m_width;                         // in pixels
    int m_height;                        // in pixels
    int m_color_bytes;                   // 16 bits, 32 bits for default
    void *m_fb;                          // frame buffer you could see
    c_layer m_layers[Z_ORDER_LEVEL_MAX]; // all graphic layers
    bool m_is_active;                    // active flag
    Z_ORDER_LEVEL m_max_zorder;          // the highest graphic layer the surface will have
    Z_ORDER_LEVEL m_top_zorder;          // the current highest graphic layer the surface have
    int *m_phy_write_index;
    c_display *m_display;

    const c_surface_vir_api *m_api; // virtual api
};

void c_display_init(c_display *self, int width, int height, int color_bytes, void *phy_fb, DISPLAY_DRIVER *driver);
void c_display_init_with_surface(c_display *self, void *phy_fb, int display_width, int display_height, c_surface *surface, DISPLAY_DRIVER *driver);
void c_display_init_with_surface_param(c_display *self, void *phy_fb, int display_width, int display_height, int surface_width, int surface_height,
                                       unsigned int color_bytes, int surface_cnt, DISPLAY_DRIVER *driver);
c_surface *c_display_alloc_surface(c_display *self, Z_ORDER_LEVEL max_zorder, c_rect layer_rect);
c_surface *c_display_alloc_surface_simple(c_display *self, Z_ORDER_LEVEL max_zorder);
int c_display_swipe_surface(c_display *self, c_surface *s0, c_surface *s1, int x0, int x1, int y0, int y1, int offset);
int c_display_get_width(c_display *self);
int c_display_get_height(c_display *self);
void *c_display_get_phy_fb(c_display *self);
void *c_display_get_updated_fb(c_display *self, int *width, int *height, bool force_update);
int c_display_snap_shot(c_display *self, const char *file_name);
void c_display_vir_draw_pixel(c_display *self, int x, int y, unsigned int rgb);
void c_display_vir_fill_rect(c_display *self, int x0, int y0, int x1, int y1, unsigned int rgb);
int c_display_vir_flush_screen(c_display *self, int left, int top, int right, int bottom, void *fb, int fb_width);

Z_ORDER_LEVEL c_surface_get_max_z_order(c_surface *self);
void c_surface_set_surface(c_surface *self, Z_ORDER_LEVEL max_z_order, c_rect layer_rect);
void c_surface_init(c_surface *s, unsigned int width, unsigned int height, unsigned int color_bytes, Z_ORDER_LEVEL max_zorder, c_rect overlpa_rect);
void c_surface_init_with_order(c_surface *self, unsigned int width, unsigned int height, unsigned int color_bytes, Z_ORDER_LEVEL max_zorder);
void c_surface_init_simple(c_surface *self, unsigned int width, unsigned int height, unsigned int color_bytes);
unsigned int c_surface_get_pixel(c_surface *self, int x, int y, unsigned int z_order);
void c_surface_vir_draw_pixel(c_surface *self, int x, int y, unsigned int rgb, unsigned int z_order);
void c_surface_vir_fill_rect(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order);
void c_surface_draw_hline(c_surface *self, int x0, int x1, int y, unsigned int rgb, unsigned int z_order);
void c_surface_draw_vline(c_surface *self, int x, int y0, int y1, unsigned int rgb, unsigned int z_order);
void c_surface_draw_line(c_surface *self, int x1, int y1, int x2, int y2, unsigned int rgb, unsigned int z_order);
void c_surface_draw_rect(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order, unsigned int size);
void c_surface_draw_rect_simple(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order);
void c_surface_draw_rect_ex(c_surface *self, c_rect rect, unsigned int rgb, unsigned int size, unsigned int z_order);
void c_surface_fill_rect_ex(c_surface *self, c_rect rect, unsigned int rgb, unsigned int z_order);
int c_surface_flush_screen(c_surface *self, int left, int top, int right, int bottom);
bool c_surface_is_active(c_surface *self);
c_display *c_surface_get_display(c_surface *self);
void c_surface_activate_layer(c_surface *self, c_rect active_rect, unsigned int active_z_order);
void c_surface_set_active(c_surface *self, bool flag);
void c_surface_vir_fill_rect_low_level(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb);
void c_surface_vir_draw_pixel_low_level(c_surface *self, int x, int y, unsigned int rgb);
void c_surface_attach_display(c_surface *self, c_display *display);