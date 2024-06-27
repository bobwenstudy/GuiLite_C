#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "api.h"
#include "display.h"

c_surface *c_display_alloc_surface(c_display *self, Z_ORDER_LEVEL max_zorder, c_rect layer_rect)
{
    GUILITE_ASSERT(max_zorder < Z_ORDER_LEVEL_MAX && self->m_surface_index < self->m_surface_cnt);
    if (c_rect_is_empty(&layer_rect))
    {
        c_rect tmp;
        c_rect_init(&tmp, 0, 0, self->m_width, self->m_height);
        c_surface_set_surface(self->m_surface_group[self->m_surface_index], max_zorder, tmp);
    }
    else
    {
        c_surface_set_surface(self->m_surface_group[self->m_surface_index], max_zorder, layer_rect);
    }
    return self->m_surface_group[self->m_surface_index++];
}

c_surface *c_display_alloc_surface_simple(c_display *self, Z_ORDER_LEVEL max_zorder)
{
    c_rect layer_rect;
    c_rect_init_empty(&layer_rect);
    return c_display_alloc_surface(self, max_zorder, layer_rect);
}

int c_display_swipe_surface(c_display *self, c_surface *s0, c_surface *s1, int x0, int x1, int y0, int y1, int offset)
{
    int surface_width = s0->m_width;
    int surface_height = s0->m_height;

    if (offset < 0 || offset > surface_width || y0 < 0 || y0 >= surface_height || y1 < 0 || y1 >= surface_height || x0 < 0 || x0 >= surface_width || x1 < 0 ||
        x1 >= surface_width)
    {
        GUILITE_ASSERT(false);
        return -1;
    }

    int width = (x1 - x0 + 1);
    if (width < 0 || width > surface_width || width < offset)
    {
        GUILITE_ASSERT(false);
        return -1;
    }

    x0 = (x0 >= self->m_width) ? (self->m_width - 1) : x0;
    x1 = (x1 >= self->m_width) ? (self->m_width - 1) : x1;
    y0 = (y0 >= self->m_height) ? (self->m_height - 1) : y0;
    y1 = (y1 >= self->m_height) ? (self->m_height - 1) : y1;

    if (self->m_phy_fb)
    {
        for (int y = y0; y <= y1; y++)
        {
            // Left surface
            char *addr_s = ((char *)(s0->m_fb) + (y * surface_width + x0 + offset) * self->m_color_bytes);
            char *addr_d = ((char *)(self->m_phy_fb) + (y * self->m_width + x0) * self->m_color_bytes);
            memcpy(addr_d, addr_s, (width - offset) * self->m_color_bytes);
            // Right surface
            addr_s = ((char *)(s1->m_fb) + (y * surface_width + x0) * self->m_color_bytes);
            addr_d = ((char *)(self->m_phy_fb) + (y * self->m_width + x0 + (width - offset)) * self->m_color_bytes);
            memcpy(addr_d, addr_s, offset * self->m_color_bytes);
        }
    }
    else if (self->m_color_bytes == 2)
    {
        void (*draw_pixel)(int x, int y, unsigned int rgb) = self->m_driver->draw_pixel;
        for (int y = y0; y <= y1; y++)
        {
            // Left surface
            for (int x = x0; x <= (x1 - offset); x++)
            {
                draw_pixel(x, y, GL_RGB_16_to_32(((unsigned short *)s0->m_fb)[y * self->m_width + x + offset]));
            }
            // Right surface
            for (int x = x1 - offset; x <= x1; x++)
            {
                draw_pixel(x, y, GL_RGB_16_to_32(((unsigned short *)s1->m_fb)[y * self->m_width + x + offset - x1 + x0]));
            }
        }
    }
    else // self->m_color_bytes == 3/4...
    {
        void (*draw_pixel)(int x, int y, unsigned int rgb) = self->m_driver->draw_pixel;
        for (int y = y0; y <= y1; y++)
        {
            // Left surface
            for (int x = x0; x <= (x1 - offset); x++)
            {
                draw_pixel(x, y, ((unsigned int *)s0->m_fb)[y * self->m_width + x + offset]);
            }
            // Right surface
            for (int x = x1 - offset; x <= x1; x++)
            {
                draw_pixel(x, y, ((unsigned int *)s1->m_fb)[y * self->m_width + x + offset - x1 + x0]);
            }
        }
    }

    self->m_phy_write_index++;
    return 0;
}

int c_display_get_width(c_display *self)
{
    return self->m_width;
}
int c_display_get_height(c_display *self)
{
    return self->m_height;
}
void *c_display_get_phy_fb(c_display *self)
{
    return self->m_phy_fb;
}

void *c_display_get_updated_fb(c_display *self, int *width, int *height, bool force_update)
{
    if (width && height)
    {
        *width = self->m_width;
        *height = self->m_height;
    }
    if (force_update)
    {
        return self->m_phy_fb;
    }
    // Check if there is any update
    if (self->m_phy_read_index == self->m_phy_write_index)
    { // No update
        return 0;
    }
    self->m_phy_read_index = self->m_phy_write_index;
    return self->m_phy_fb;
}

int c_display_snap_shot(c_display *self, const char *file_name)
{
    // if (!self->m_phy_fb || (self->m_color_bytes !=2 && self->m_color_bytes != 4))
    // {
    // 	return -1;
    // }

    // //16 bits framebuffer
    // if (self->m_color_bytes == 2)
    // {
    // 	return build_bmp(file_name, self->m_width, self->m_height, (unsigned char*)self->m_phy_fb);
    // }

    // //32 bits framebuffer
    // unsigned short* p_bmp565_data = new unsigned short[self->m_width * self->m_height];
    // unsigned int* p_raw_data = (unsigned int*)self->m_phy_fb;

    // for (int i = 0; i < self->m_width * self->m_height; i++)
    // {
    // 	unsigned int rgb = *p_raw_data++;
    // 	p_bmp565_data[i] = GL_RGB_32_to_16(rgb);
    // }

    // int ret = build_bmp(file_name, self->m_width, self->m_height, (unsigned char*)p_bmp565_data);
    // delete[]p_bmp565_data;
    // return ret;

    return 0;
}

void c_display_vir_draw_pixel(c_display *self, int x, int y, unsigned int rgb)
{
    if ((x >= self->m_width) || (y >= self->m_height))
    {
        return;
    }

    if (self->m_driver && self->m_driver->draw_pixel)
    {
        return self->m_driver->draw_pixel(x, y, rgb);
    }

    if (self->m_color_bytes == 2)
    {
        ((unsigned short *)self->m_phy_fb)[y * self->m_width + x] = GL_RGB_32_to_16(rgb);
    }
    else
    {
        ((unsigned int *)self->m_phy_fb)[y * self->m_width + x] = rgb;
    }
}

void c_display_vir_fill_rect(c_display *self, int x0, int y0, int x1, int y1, unsigned int rgb)
{
    // use driver to fill rect if it's available
    if (self->m_driver && self->m_driver->fill_rect)
    {
        return self->m_driver->fill_rect(x0, y0, x1, y1, rgb);
    }

    // use driver to draw pixel if it's available
    if (self->m_driver && self->m_driver->draw_pixel)
    {
        for (int y = y0; y <= y1; y++)
        {
            for (int x = x0; x <= x1; x++)
            {
                self->m_driver->draw_pixel(x, y, rgb);
            }
        }
        return;
    }

    // fill rect in memory
    int _width = self->m_width;
    int _height = self->m_height;
    int x, y;
    if (self->m_color_bytes == 2)
    {
        unsigned short *phy_fb;
        unsigned int rgb_16 = GL_RGB_32_to_16(rgb);
        for (y = y0; y <= y1; y++)
        {
            phy_fb = &((unsigned short *)self->m_phy_fb)[y * _width + x0];
            for (x = x0; x <= x1; x++)
            {
                if ((x < _width) && (y < _height))
                {
                    *phy_fb++ = rgb_16;
                }
            }
        }
    }
    else
    {
        unsigned int *phy_fb;
        for (y = y0; y <= y1; y++)
        {
            phy_fb = &((unsigned int *)self->m_phy_fb)[y * _width + x0];
            for (x = x0; x <= x1; x++)
            {
                if ((x < _width) && (y < _height))
                {
                    *phy_fb++ = rgb;
                }
            }
        }
    }
}

int c_display_vir_flush_screen(c_display *self, int left, int top, int right, int bottom, void *fb, int fb_width)
{
    if ((0 == self->m_phy_fb) || (0 == fb))
    {
        return -1;
    }

    int _width = self->m_width;
    int _height = self->m_height;

    left = (left >= _width) ? (_width - 1) : left;
    right = (right >= _width) ? (_width - 1) : right;
    top = (top >= _height) ? (_height - 1) : top;
    bottom = (bottom >= _height) ? (_height - 1) : bottom;

    for (int y = top; y < bottom; y++)
    {
        void *s_addr = (char *)fb + ((y * fb_width + left) * self->m_color_bytes);
        void *d_addr = (char *)self->m_phy_fb + ((y * _width + left) * self->m_color_bytes);
        memcpy(d_addr, s_addr, (right - left) * self->m_color_bytes);
    }
    return 0;
}

static const c_display_vir_api c_display_vir_api_table = {
        c_display_vir_draw_pixel,
        c_display_vir_fill_rect,
        c_display_vir_flush_screen,
};

void c_display_init(c_display *self, int width, int height, int color_bytes, void *phy_fb, DISPLAY_DRIVER *driver)
{
    self->m_width = width;
    self->m_height = height;
    self->m_color_bytes = color_bytes;
    self->m_phy_fb = phy_fb;
    self->m_driver = driver;
    self->m_phy_read_index = 0;
    self->m_phy_write_index = 1;
    self->m_surface_cnt = 0;
    self->m_surface_index = 0;

    self->m_api = &c_display_vir_api_table;
}

void c_display_init_with_surface(c_display *self, void *phy_fb, int display_width, int display_height, c_surface *surface, DISPLAY_DRIVER *driver)
{
    self->m_phy_fb = phy_fb;
    self->m_width = display_width;
    self->m_height = display_height;
    self->m_driver = driver;
    self->m_phy_read_index = 0;
    self->m_phy_write_index = 0;
    self->m_surface_cnt = 1;
    self->m_surface_index = 0;

    self->m_color_bytes = surface->m_color_bytes;
    surface->m_is_active = true;
    self->m_surface_group[0] = surface;

    self->m_api = &c_display_vir_api_table;
    c_surface_attach_display(surface, self);
}

c_surface c_display_surface_group[SURFACE_CNT_MAX];
void c_display_init_with_surface_param(c_display *self, void *phy_fb, int display_width, int display_height, int surface_width, int surface_height,
                                       unsigned int color_bytes, int surface_cnt, DISPLAY_DRIVER *driver)
{
    self->m_phy_fb = phy_fb;
    self->m_width = display_width;
    self->m_height = display_height;
    self->m_color_bytes = color_bytes;
    self->m_driver = driver;
    self->m_phy_read_index = 0;
    self->m_phy_write_index = 0;
    self->m_surface_cnt = surface_cnt;
    self->m_surface_index = 0;

    self->m_api = &c_display_vir_api_table;

    GUILITE_ASSERT(color_bytes == 2 || color_bytes == 4);
    GUILITE_ASSERT(self->m_surface_cnt <= SURFACE_CNT_MAX);
    memset(self->m_surface_group, 0, sizeof(self->m_surface_group));
    for (int i = 0; i < surface_cnt; i++)
    {
        self->m_surface_group[i] = &c_display_surface_group[i];
        c_surface_init_simple(self->m_surface_group[i], surface_width, surface_height, color_bytes);

        self->m_surface_group[i]->m_is_active = true;
        c_surface_attach_display(self->m_surface_group[i], self);
    }
}

Z_ORDER_LEVEL c_surface_get_max_z_order(c_surface *self)
{
    return self->m_max_zorder;
}

void c_surface_set_surface(c_surface *self, Z_ORDER_LEVEL max_z_order, c_rect layer_rect)
{
    self->m_max_zorder = max_z_order;
    if (self->m_display && (self->m_display->m_surface_cnt > 1))
    {
        self->m_fb = guilite_calloc(self->m_width * self->m_height, self->m_color_bytes);
    }

    for (int i = Z_ORDER_LEVEL_0; i < Z_ORDER_LEVEL_MAX; i++)
    {
        // self->m_layers[i].fb = 0;
        c_rect_init_empty(&self->m_layers[i].rect);
        c_rect_init_empty(&self->m_layers[i].active_rect);
    }

    for (int i = Z_ORDER_LEVEL_0; i < self->m_max_zorder; i++)
    { // Top layber fb always be 0
        GUILITE_ASSERT(self->m_layers[i].fb = guilite_calloc(c_rect_width(&layer_rect) * c_rect_height(&layer_rect), self->m_color_bytes));
        self->m_layers[i].rect = layer_rect;
    }

    self->m_layers[Z_ORDER_LEVEL_0].active_rect = layer_rect;
}

unsigned int c_surface_get_pixel(c_surface *self, int x, int y, unsigned int z_order)
{
    if (x >= self->m_width || y >= self->m_height || x < 0 || y < 0 || z_order >= Z_ORDER_LEVEL_MAX)
    {
        GUILITE_ASSERT(false);
        return 0;
    }
    if (self->m_layers[z_order].fb)
    {
        return (self->m_color_bytes == 2) ? GL_RGB_16_to_32(((unsigned short *)(self->m_layers[z_order].fb))[y * self->m_width + x])
                                          : ((unsigned int *)(self->m_layers[z_order].fb))[y * self->m_width + x];
    }
    else if (self->m_fb)
    {
        return (self->m_color_bytes == 2) ? GL_RGB_16_to_32(((unsigned short *)self->m_fb)[y * self->m_width + x])
                                          : ((unsigned int *)self->m_fb)[y * self->m_width + x];
    }
    else if (self->m_display->m_phy_fb)
    {
        return (self->m_color_bytes == 2) ? GL_RGB_16_to_32(((unsigned short *)self->m_display->m_phy_fb)[y * self->m_width + x])
                                          : ((unsigned int *)self->m_display->m_phy_fb)[y * self->m_width + x];
    }
    return 0;
}

void c_surface_vir_draw_pixel(c_surface *self, int x, int y, unsigned int rgb, unsigned int z_order)
{
    if (x >= self->m_width || y >= self->m_height || x < 0 || y < 0)
    {
        return;
    }

    if (z_order > (unsigned int)self->m_max_zorder)
    {
        GUILITE_ASSERT(false);
        return;
    }

    if (z_order > (unsigned int)self->m_top_zorder)
    {
        self->m_top_zorder = (Z_ORDER_LEVEL)z_order;
    }

    if (z_order == self->m_max_zorder)
    {
        return self->m_api->draw_pixel_low_level(self, x, y, rgb);
    }

    if (c_rect_pt_in_rect(&self->m_layers[z_order].rect, x, y))
    {
        c_rect layer_rect = self->m_layers[z_order].rect;
        if (self->m_color_bytes == 2)
        {
            ((unsigned short *)(self->m_layers[z_order].fb))[(x - layer_rect.m_left) + (y - layer_rect.m_top) * c_rect_width(&layer_rect)] =
                    GL_RGB_32_to_16(rgb);
        }
        else
        {
            ((unsigned int *)(self->m_layers[z_order].fb))[(x - layer_rect.m_left) + (y - layer_rect.m_top) * c_rect_width(&layer_rect)] = rgb;
        }
    }

    if (z_order == self->m_top_zorder)
    {
        return self->m_api->draw_pixel_low_level(self, x, y, rgb);
    }

    bool be_overlapped = false;
    for (unsigned int tmp_z_order = Z_ORDER_LEVEL_MAX - 1; tmp_z_order > z_order; tmp_z_order--)
    {
        if (c_rect_pt_in_rect(&self->m_layers[tmp_z_order].active_rect, x, y))
        {
            be_overlapped = true;
            break;
        }
    }

    if (!be_overlapped)
    {
        self->m_api->draw_pixel_low_level(self, x, y, rgb);
    }
}

void c_surface_vir_fill_rect(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order)
{
    x0 = (x0 < 0) ? 0 : x0;
    y0 = (y0 < 0) ? 0 : y0;
    x1 = (x1 > (self->m_width - 1)) ? (self->m_width - 1) : x1;
    y1 = (y1 > (self->m_height - 1)) ? (self->m_height - 1) : y1;

    // guilite_log("c_surface_vir_fill_rect, x0:%d, y0:%d, x1:%d, y1:%d, z_order:%d max_zorder:%d top_zorder:%d\n", x0, y0, x1, y1, z_order, self->m_max_zorder,
    // self->m_top_zorder);
    if (z_order == self->m_max_zorder)
    {
        return self->m_api->fill_rect_low_level(self, x0, y0, x1, y1, rgb);
    }

    if (z_order == self->m_top_zorder)
    {
        int width = c_rect_width(&self->m_layers[z_order].rect);
        c_rect layer_rect = self->m_layers[z_order].rect;
        unsigned int rgb_16 = GL_RGB_32_to_16(rgb);
        for (int y = y0; y <= y1; y++)
        {
            for (int x = x0; x <= x1; x++)
            {
                if (c_rect_pt_in_rect(&layer_rect, x, y))
                {
                    if (self->m_color_bytes == 2)
                    {
                        ((unsigned short *)self->m_layers[z_order].fb)[(y - layer_rect.m_top) * width + (x - layer_rect.m_left)] = rgb_16;
                    }
                    else
                    {
                        ((unsigned int *)self->m_layers[z_order].fb)[(y - layer_rect.m_top) * width + (x - layer_rect.m_left)] = rgb;
                    }
                }
            }
        }
        return self->m_api->fill_rect_low_level(self, x0, y0, x1, y1, rgb);
    }

    for (; y0 <= y1; y0++)
    {
        c_surface_draw_hline(self, x0, x1, y0, rgb, z_order);
    }
}

void c_surface_draw_hline(c_surface *self, int x0, int x1, int y, unsigned int rgb, unsigned int z_order)
{
    for (; x0 <= x1; x0++)
    {
        self->m_api->draw_pixel(self, x0, y, rgb, z_order);
    }
}

void c_surface_draw_vline(c_surface *self, int x, int y0, int y1, unsigned int rgb, unsigned int z_order)
{
    for (; y0 <= y1; y0++)
    {
        self->m_api->draw_pixel(self, x, y0, rgb, z_order);
    }
}

void c_surface_draw_line(c_surface *self, int x1, int y1, int x2, int y2, unsigned int rgb, unsigned int z_order)
{
    int dx, dy, x, y, e;

    (x1 > x2) ? (dx = x1 - x2) : (dx = x2 - x1);
    (y1 > y2) ? (dy = y1 - y2) : (dy = y2 - y1);

    if (((dx > dy) && (x1 > x2)) || ((dx <= dy) && (y1 > y2)))
    {
        x = x2;
        y = y2;
        x2 = x1;
        y2 = y1;
        x1 = x;
        y1 = y;
    }
    x = x1;
    y = y1;

    if (dx > dy)
    {
        e = dy - dx / 2;
        for (; x1 <= x2; ++x1, e += dy)
        {
            self->m_api->draw_pixel(self, x1, y1, rgb, z_order);
            if (e > 0)
            {
                e -= dx;
                (y > y2) ? --y1 : ++y1;
            }
        }
    }
    else
    {
        e = dx - dy / 2;
        for (; y1 <= y2; ++y1, e += dx)
        {
            self->m_api->draw_pixel(self, x1, y1, rgb, z_order);
            if (e > 0)
            {
                e -= dy;
                (x > x2) ? --x1 : ++x1;
            }
        }
    }
}

void c_surface_draw_rect(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order, unsigned int size)
{
    for (unsigned int offset = 0; offset < size; offset++)
    {
        c_surface_draw_hline(self, x0 + offset, x1 - offset, y0 + offset, rgb, z_order);
        c_surface_draw_hline(self, x0 + offset, x1 - offset, y1 - offset, rgb, z_order);
        c_surface_draw_vline(self, x0 + offset, y0 + offset, y1 - offset, rgb, z_order);
        c_surface_draw_vline(self, x1 - offset, y0 + offset, y1 - offset, rgb, z_order);
    }
}

void c_surface_draw_rect_simple(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order)
{
    c_surface_draw_rect(self, x0, y0, x1, y1, rgb, z_order, 1);
}

void c_surface_draw_rect_ex(c_surface *self, c_rect rect, unsigned int rgb, unsigned int size, unsigned int z_order)
{
    c_surface_draw_rect(self, rect.m_left, rect.m_top, rect.m_right, rect.m_bottom, rgb, z_order, size);
}

void c_surface_fill_rect_ex(c_surface *self, c_rect rect, unsigned int rgb, unsigned int z_order)
{
    self->m_api->fill_rect(self, rect.m_left, rect.m_top, rect.m_right, rect.m_bottom, rgb, z_order);
}

int c_surface_flush_screen(c_surface *self, int left, int top, int right, int bottom)
{
    if (!self->m_is_active)
    {
        return -1;
    }

    if (left < 0 || left >= self->m_width || right < 0 || right >= self->m_width || top < 0 || top >= self->m_height || bottom < 0 || bottom >= self->m_height)
    {
        GUILITE_ASSERT(false);
    }

    self->m_display->m_api->flush_screen(self->m_display, left, top, right, bottom, self->m_fb, self->m_width);
    *self->m_phy_write_index = *self->m_phy_write_index + 1;
    return 0;
}

bool c_surface_is_active(c_surface *self)
{
    return self->m_is_active;
}
c_display *c_surface_get_display(c_surface *self)
{
    return self->m_display;
}

void c_surface_activate_layer(c_surface *self, c_rect active_rect, unsigned int active_z_order) // empty active rect means inactivating the layer
{
    GUILITE_ASSERT(active_z_order > Z_ORDER_LEVEL_0 && active_z_order <= Z_ORDER_LEVEL_MAX);

    // Show the layers below the current active rect.
    c_rect current_active_rect = self->m_layers[active_z_order].active_rect;
    for (int low_z_order = Z_ORDER_LEVEL_0; low_z_order < active_z_order; low_z_order++)
    {
        c_rect low_layer_rect = self->m_layers[low_z_order].rect;
        c_rect low_active_rect = self->m_layers[low_z_order].active_rect;
        void *fb = self->m_layers[low_z_order].fb;
        int width = c_rect_width(&low_layer_rect);
        for (int y = current_active_rect.m_top; y <= current_active_rect.m_bottom; y++)
        {
            for (int x = current_active_rect.m_left; x <= current_active_rect.m_right; x++)
            {
                if (c_rect_pt_in_rect(&low_active_rect, x, y) && c_rect_pt_in_rect(&low_layer_rect, x, y)) // active rect maybe is bigger than layer rect
                {
                    unsigned int rgb = (self->m_color_bytes == 2)
                                               ? GL_RGB_16_to_32(((unsigned short *)fb)[(x - low_layer_rect.m_left) + (y - low_layer_rect.m_top) * width])
                                               : ((unsigned int *)fb)[(x - low_layer_rect.m_left) + (y - low_layer_rect.m_top) * width];
                    self->m_api->draw_pixel_low_level(self, x, y, rgb);
                }
            }
        }
    }
    self->m_layers[active_z_order].active_rect = active_rect; // set the new acitve rect.
}

void c_surface_set_active(c_surface *self, bool flag)
{
    self->m_is_active = flag;
}

void c_surface_vir_fill_rect_low_level(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb)
{ // fill rect on framebuffer of surface
    int x, y;
    if (self->m_color_bytes == 2)
    {
        unsigned short *fb;
        unsigned int rgb_16 = GL_RGB_32_to_16(rgb);
        for (y = y0; y <= y1; y++)
        {
            fb = self->m_fb ? &((unsigned short *)self->m_fb)[y * self->m_width + x0] : 0;
            if (!fb)
            {
                break;
            }
            for (x = x0; x <= x1; x++)
            {
                *fb++ = rgb_16;
            }
        }
    }
    else
    {
        unsigned int *fb;
        for (y = y0; y <= y1; y++)
        {
            fb = self->m_fb ? &((unsigned int *)self->m_fb)[y * self->m_width + x0] : 0;
            if (!fb)
            {
                break;
            }
            for (x = x0; x <= x1; x++)
            {
                *fb++ = rgb;
            }
        }
    }

    if (!self->m_is_active)
    {
        return;
    }
    self->m_display->m_api->fill_rect(self->m_display, x0, y0, x1, y1, rgb);
    *self->m_phy_write_index = *self->m_phy_write_index + 1;
}

void c_surface_vir_draw_pixel_low_level(c_surface *self, int x, int y, unsigned int rgb)
{
    if (self->m_fb)
    { // draw pixel on framebuffer of surface
        if (self->m_color_bytes == 2)
        {
            ((unsigned short *)self->m_fb)[y * self->m_width + x] = GL_RGB_32_to_16(rgb);
        }
        else
        {
            ((unsigned int *)self->m_fb)[y * self->m_width + x] = rgb;
        }
    }
    if (!self->m_is_active)
    {
        return;
    }
    self->m_display->m_api->draw_pixel(self->m_display, x, y, rgb);
    *self->m_phy_write_index = *self->m_phy_write_index + 1;
}

void c_surface_attach_display(c_surface *self, c_display *display)
{
    GUILITE_ASSERT(display);
    self->m_display = display;
    self->m_phy_write_index = &display->m_phy_write_index;
}

static const c_surface_vir_api c_surface_vir_api_table = {
        c_surface_vir_draw_pixel,
        c_surface_vir_fill_rect,
        c_surface_vir_fill_rect_low_level,
        c_surface_vir_draw_pixel_low_level,
};

void c_surface_init(c_surface *self, unsigned int width, unsigned int height, unsigned int color_bytes, Z_ORDER_LEVEL max_zorder, c_rect overlpa_rect)
{
    self->m_width = width;
    self->m_height = height;
    self->m_color_bytes = color_bytes;
    self->m_fb = 0;
    self->m_is_active = false;
    self->m_top_zorder = Z_ORDER_LEVEL_0;
    self->m_phy_write_index = 0;
    self->m_display = NULL;

    self->m_api = &c_surface_vir_api_table;

    if (c_rect_is_empty(&overlpa_rect))
    {
        c_rect tmp;
        c_rect_init(&tmp, 0, 0, width, height);
        c_surface_set_surface(self, max_zorder, tmp);
    }
    else
    {
        c_surface_set_surface(self, max_zorder, overlpa_rect);
    }
}

void c_surface_init_with_order(c_surface *self, unsigned int width, unsigned int height, unsigned int color_bytes, Z_ORDER_LEVEL max_zorder)
{
    c_rect tmp;
    c_rect_init_empty(&tmp);

    c_surface_init(self, width, height, color_bytes, max_zorder, tmp);
}

void c_surface_init_simple(c_surface *self, unsigned int width, unsigned int height, unsigned int color_bytes)
{
    c_surface_init_with_order(self, width, height, color_bytes, Z_ORDER_LEVEL_0);
}
