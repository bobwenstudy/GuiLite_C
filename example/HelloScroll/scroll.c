#include "scroll.h"

void c_surface_offset_vir_draw_pixel(c_surface *self, int x, int y, unsigned int rgb, unsigned int z_order)
{
    c_surface_offset *surface = (c_surface_offset *)self;
    x -= surface->m_offset_x;
    y -= surface->m_offset_y;
    c_surface_vir_draw_pixel(self, x, y, rgb, z_order);
}

void c_surface_offset_vir_fill_rect(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order)
{
    c_surface_offset *surface = (c_surface_offset *)self;
    x0 -= surface->m_offset_x;
    x1 -= surface->m_offset_x;
    y0 -= surface->m_offset_y;
    y1 -= surface->m_offset_y;
    c_surface_vir_fill_rect(self, x0, y0, x1, y1, rgb, z_order);
}

int c_surface_offset_get_width(c_surface_offset *self)
{
    return self->base.m_width;
}
int c_surface_offset_get_height(c_surface_offset *self)
{
    return self->base.m_height;
}

static const c_surface_vir_api c_surface_offset_vir_api_table = {
        c_surface_offset_vir_draw_pixel,
        c_surface_offset_vir_fill_rect,
        c_surface_vir_fill_rect_low_level,
        c_surface_vir_draw_pixel_low_level,
};

void c_surface_offset_init(c_surface_offset *self, unsigned int width, unsigned int height, unsigned int color_bytes, int offset_x, int offset_y)
{
    c_surface_init_simple(&self->base, width, height, color_bytes);
    self->base.m_api = &c_surface_offset_vir_api_table;

    self->m_offset_x = offset_x;
    self->m_offset_y = offset_y;
}

int mem_display_get_width(mem_display *self)
{
    return self->base.m_width;
}

void mem_display_init(mem_display *self, void *phy_fb, int display_width, int display_height, c_surface *surface, DISPLAY_DRIVER *driver)
{
    c_display_init_with_surface(&self->base, phy_fb, display_width, display_height, surface, driver);
}

void c_scroll_view_vir_pre_create_wnd(c_wnd *self)
{
    c_scroll_view *sv = (c_scroll_view *)self;
    self->m_bg_color = GL_RGB(220, 220, 220);
}
void c_scroll_view_vir_on_init_children(c_wnd *self)
{
    c_scroll_view *sv = (c_scroll_view *)self;
    sv->m_parent_surface = self->m_surface;
    int width, height;
    width = height = 0;
    c_wnd *tmp = self->m_top_child;
    c_rect rect;
    while (tmp)
    {
        c_wnd_get_wnd_rect(tmp, &rect);
        if (width < rect.m_right)
        {
            width = rect.m_right + 1;
        }
        if (height < rect.m_bottom)
        {
            height = rect.m_bottom + 1;
        }
        tmp = c_wnd_get_next_sibling(tmp);
    }

    c_wnd_get_screen_rect(self, &rect);
    if (width < c_rect_width(&rect))
    {
        width = c_rect_width(&rect);
    }
    if (height < c_rect_height(&rect))
    {
        height = c_rect_height(&rect);
    }

    c_surface_offset_init(&sv->m_mem_surface, width, height, 2, rect.m_left, rect.m_top);
    mem_display_init(&sv->m_mem_display, guilite_calloc(width * height, 2), width, height, &sv->m_mem_surface.base, NULL);

    // change surface
    c_wnd_set_surface(self, &sv->m_mem_surface.base);
    tmp = self->m_top_child;
    while (tmp)
    {
        c_wnd_set_surface(tmp, &sv->m_mem_surface.base);
        tmp = c_wnd_get_next_sibling(tmp);
    }
}
void c_scroll_view_vir_on_paint(c_wnd *self)
{
    c_scroll_view *sv = (c_scroll_view *)self;
    sv->m_mem_surface.base.m_api->fill_rect(&sv->m_mem_surface.base, sv->m_mem_surface.m_offset_x, sv->m_mem_surface.m_offset_y,
                                            sv->m_mem_surface.m_offset_x + c_surface_offset_get_width(&sv->m_mem_surface) - 1,
                                            sv->m_mem_surface.m_offset_y + c_surface_offset_get_height(&sv->m_mem_surface) - 1, self->m_bg_color,
                                            self->m_z_order);
}
void c_scroll_view_vir_show_window(c_wnd *self)
{
    c_scroll_view *sv = (c_scroll_view *)self;
    c_wnd_vir_show_window(self);
    c_scroll_view_flush_mem_surface_simple(sv);
}
void c_scroll_view_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action)
{
    c_scroll_view *sv = (c_scroll_view *)self;
    c_rect rect;
    c_wnd_get_wnd_rect(self, &rect);
    bool force_update = false;

    // TOUCH_UP
    if (action == TOUCH_UP)
    {
        if (!sv->m_is_dragging)
        {
            c_wnd_vir_on_touch(self, x + sv->m_offset_x, y + sv->m_offset_y, TOUCH_DOWN);
            c_wnd_vir_on_touch(self, x + sv->m_offset_x, y + sv->m_offset_y, TOUCH_UP);
        }
        sv->m_is_dragging = false;
        goto EXIT;
    }

    // TOUCH_DOWN
    if (sv->m_touch_action == TOUCH_UP)
    {
        sv->m_down_x = x;
        sv->m_down_y = y;
        goto EXIT;
    }
    if (sv->m_is_dragging || abs(sv->m_down_y - y) > MOVE_THRESHOLD)
    {
        int offset_y = sv->m_offset_y;
        offset_y += (sv->m_down_y - y);
        if (offset_y > (c_surface_offset_get_height(&sv->m_mem_surface) - c_rect_height(&rect)))
        {
            offset_y = (c_surface_offset_get_height(&sv->m_mem_surface) - c_rect_height(&rect));
        }
        offset_y = (offset_y < 0) ? 0 : offset_y;
        if (offset_y != sv->m_offset_y)
        {
            force_update = true;
            sv->m_offset_y = offset_y;
        }
        sv->m_is_dragging = true;
    }
    if (sv->m_is_dragging || abs(sv->m_down_x - x) > MOVE_THRESHOLD)
    {
        int offset_x = sv->m_offset_x;
        offset_x += (sv->m_down_x - x);
        if (offset_x > (c_surface_offset_get_width(&sv->m_mem_surface) - c_rect_width(&rect)))
        {
            offset_x = (c_surface_offset_get_width(&sv->m_mem_surface) - c_rect_width(&rect));
        }
        offset_x = (offset_x < 0) ? 0 : offset_x;
        if (offset_x != sv->m_offset_x)
        {
            force_update = true;
            sv->m_offset_x = offset_x;
        }
        sv->m_is_dragging = true;
    }
EXIT:
    sv->m_touch_action = action;
    c_scroll_view_flush_mem_surface(sv, force_update);
}
void c_scroll_view_flush_mem_surface(c_scroll_view *self, bool force_update)
{
    c_rect rect;
    c_wnd_get_screen_rect(&self->base, &rect);
    GUILITE_ASSERT(self->m_offset_y + c_rect_height(&rect) <= c_surface_offset_get_height(&self->m_mem_surface));
    GUILITE_ASSERT(self->m_offset_x + c_rect_width(&rect) <= c_surface_offset_get_width(&self->m_mem_surface));
    unsigned short *fb = (unsigned short *)c_display_get_updated_fb(&self->m_mem_display.base, 0, 0, force_update);
    if (!fb)
    {
        return;
    }

    for (int y = rect.m_top; y <= rect.m_bottom; y++)
    {
        unsigned short *rgb_16 = fb + c_display_get_width(&self->m_mem_display.base) * (self->m_offset_y + y - rect.m_top) + self->m_offset_x;
        for (int x = rect.m_left; x <= rect.m_right; x++)
        {
            self->m_parent_surface->m_api->draw_pixel(self->m_parent_surface, x, y, GL_RGB_16_to_32(*rgb_16), self->base.m_z_order);
            rgb_16++;
        }
    }
}

void c_scroll_view_flush_mem_surface_simple(c_scroll_view *self)
{
    c_scroll_view_flush_mem_surface(self, false);
}

static const c_wnd_vir_api c_scroll_view_vir_api_table = {
        c_wnd_vir_connect,     c_scroll_view_vir_on_init_children, c_scroll_view_vir_on_paint, c_scroll_view_vir_show_window, c_scroll_view_vir_on_touch,
        c_wnd_vir_on_navigate, c_scroll_view_vir_pre_create_wnd,   c_wnd_vir_on_focus,         c_wnd_vir_on_kill_focus,
};

void c_scroll_view_init(c_scroll_view *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_scroll_view_vir_api_table;

    self->m_touch_action = TOUCH_UP;
    self->m_offset_x = 0;
    self->m_offset_y = 0;
    self->m_is_dragging = false;
}
