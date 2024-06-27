#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"

#define MOVE_THRESHOLD 10

typedef struct c_surface_offset c_surface_offset;
struct c_surface_offset
{
    c_surface base;
    int m_offset_x;
    int m_offset_y;
};

typedef struct mem_display mem_display;
struct mem_display
{
    c_display base;
};

typedef struct c_scroll_view c_scroll_view;
struct c_scroll_view
{
    c_wnd base;

    TOUCH_ACTION m_touch_action;
    mem_display m_mem_display;
    c_surface_offset m_mem_surface;
    c_surface *m_parent_surface;
    int m_offset_x;
    int m_offset_y;
    int m_down_x, m_down_y;
    bool m_is_dragging;
};

void c_surface_offset_vir_draw_pixel(c_surface *self, int x, int y, unsigned int rgb, unsigned int z_order);
void c_surface_offset_vir_fill_rect(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order);
int c_surface_offset_get_width(c_surface_offset *self);
int c_surface_offset_get_height(c_surface_offset *self);
void c_surface_offset_init(c_surface_offset *self, unsigned int width, unsigned int height, unsigned int color_bytes, int offset_x, int offset_y);
int mem_display_get_width(mem_display *self);
void mem_display_init(mem_display *self, void *phy_fb, int display_width, int display_height, c_surface *surface, DISPLAY_DRIVER *driver);

void c_scroll_view_vir_pre_create_wnd(c_wnd *self);
void c_scroll_view_vir_on_init_children(c_wnd *self);
void c_scroll_view_vir_on_paint(c_wnd *self);
void c_scroll_view_vir_show_window(c_wnd *self);
void c_scroll_view_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action);
void c_scroll_view_flush_mem_surface(c_scroll_view *self, bool force_update);
void c_scroll_view_flush_mem_surface_simple(c_scroll_view *self);
void c_scroll_view_init(c_scroll_view *self);
