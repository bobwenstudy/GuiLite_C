#include "surface_transparent.h"

void c_surface_transparent_correct_color_after_display(c_surface_transparent *self)
{
    self->m_display_color_bytes = self->base.m_color_bytes;
    if (self->base.m_color_bytes != SURFACE_COLOR_BYTES)
    {
        self->base.m_color_bytes = SURFACE_COLOR_BYTES;
    }
}

void c_surface_transparent_clear_layer(c_surface_transparent *self, unsigned int z_order)
{
    memset(self->base.m_layers[z_order].fb, 0,
           c_rect_width(&self->base.m_layers[z_order].rect) * c_rect_height(&self->base.m_layers[z_order].rect) * SURFACE_COLOR_BYTES);
}

void c_surface_transparent_vir_draw_pixel(c_surface *self, int x, int y, unsigned int rgb, unsigned int z_order)
{
    c_surface_transparent *transparent = (c_surface_transparent *)self;
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

    ((unsigned int *)(self->m_layers[z_order].fb))[x + y * self->m_width] = rgb;
    *self->m_phy_write_index = *self->m_phy_write_index + 1;

    unsigned int a = GL_ARGB_A(rgb);
    unsigned int r = GL_RGB_R(rgb);
    unsigned int g = GL_RGB_G(rgb);
    unsigned int b = GL_RGB_B(rgb);
    if (z_order + 1 <= self->m_top_zorder)
    { // meet up layer
        unsigned int rgb_ = c_surface_get_pixel(self, x, y, (z_order + 1));
        unsigned int a_ = GL_ARGB_A(rgb_);
        unsigned int r_ = GL_RGB_R(rgb_);
        unsigned int g_ = GL_RGB_G(rgb_);
        unsigned int b_ = GL_RGB_B(rgb_);

        unsigned int alpha_r = (r_ * a_ + r * (255 - a_)) / 255;
        unsigned int alpha_g = (g_ * a_ + g * (255 - a_)) / 255;
        unsigned int alpha_b = (b_ * a_ + b * (255 - a_)) / 255;
        rgb = GL_RGB(alpha_r, alpha_g, alpha_b);
    }

    void *phy_fb = c_display_get_phy_fb(self->m_display);
    if (transparent->m_display_color_bytes == 4)
    {
        ((unsigned int *)phy_fb)[y * (c_display_get_width(self->m_display)) + x] = rgb;
    }
    else
    {
        ((unsigned short *)phy_fb)[y * (c_display_get_width(self->m_display)) + x] = GL_RGB_32_to_16(rgb);
    }
}

void c_surface_transparent_vir_fill_rect(c_surface *self, int x0, int y0, int x1, int y1, unsigned int rgb, unsigned int z_order)
{
    c_surface_transparent *transparent = (c_surface_transparent *)self;
    GUILITE_ASSERT(z_order <= (unsigned int)self->m_max_zorder);
    if (z_order > (unsigned int)self->m_top_zorder)
    {
        self->m_top_zorder = (Z_ORDER_LEVEL)z_order;
    }

    x0 = (x0 < 0) ? 0 : x0;
    y0 = (y0 < 0) ? 0 : y0;
    x1 = (x1 > self->m_layers[z_order].rect.m_right) ? self->m_layers[z_order].rect.m_right : x1;
    y1 = (y1 > self->m_layers[z_order].rect.m_bottom) ? self->m_layers[z_order].rect.m_bottom : y1;

    unsigned int *mem_fb;
    for (int y = y0; y <= y1; y++)
    {
        mem_fb = &((unsigned int *)self->m_layers[z_order].fb)[y * self->m_width + x0];
        for (int x = x0; x <= x1; x++)
        {
            *mem_fb++ = rgb;
        }
    }

    unsigned int a = GL_ARGB_A(rgb);
    unsigned int r = GL_RGB_R(rgb);
    unsigned int g = GL_RGB_G(rgb);
    unsigned int b = GL_RGB_B(rgb);
    int display_width = c_display_get_width(self->m_display);
    int display_height = c_display_get_height(self->m_display);

    c_rect upper_rect, lower_rect;
    if (z_order + 1 <= self->m_top_zorder)
    {
        upper_rect = self->m_layers[z_order + 1].rect;
    }
    if (z_order >= Z_ORDER_LEVEL_1)
    {
        lower_rect = self->m_layers[z_order - 1].rect;
    }

    void *phy_fb = c_display_get_phy_fb(self->m_display);
    for (int y = y0; y <= y1; y++)
    {
        unsigned int *phy_fb_32 = &((unsigned int *)phy_fb)[y * display_width + x0];
        unsigned short *phy_fb_16 = &((unsigned short *)phy_fb)[y * display_width + x0];
        *self->m_phy_write_index = *self->m_phy_write_index + 1;
        for (int x = x0; x <= x1; x++)
        {
            if (self->m_is_active && (x < display_width) && (y < display_height))
            {
                if (c_rect_pt_in_rect(&upper_rect, x, y))
                {
                    unsigned int rgb_ = c_surface_get_pixel(self, x, y, (z_order + 1));
                    unsigned int a_ = GL_ARGB_A(rgb_);
                    unsigned int r_ = GL_RGB_R(rgb_);
                    unsigned int g_ = GL_RGB_G(rgb_);
                    unsigned int b_ = GL_RGB_B(rgb_);
                    unsigned int alpha_r = (r_ * a_ + r * (255 - a_)) / 255;
                    unsigned int alpha_g = (g_ * a_ + g * (255 - a_)) / 255;
                    unsigned int alpha_b = (b_ * a_ + b * (255 - a_)) / 255;
                    rgb = GL_RGB(alpha_r, alpha_g, alpha_b);
                }
                else if (c_rect_pt_in_rect(&lower_rect, x, y))
                {
                    unsigned int rgb_ = c_surface_get_pixel(self, x, y, (z_order - 1));
                    unsigned int r_ = GL_RGB_R(rgb_);
                    unsigned int g_ = GL_RGB_G(rgb_);
                    unsigned int b_ = GL_RGB_B(rgb_);
                    unsigned int alpha_r = (r * a + r_ * (255 - a)) / 255;
                    unsigned int alpha_g = (g * a + g_ * (255 - a)) / 255;
                    unsigned int alpha_b = (b * a + b_ * (255 - a)) / 255;
                    rgb = GL_RGB(alpha_r, alpha_g, alpha_b);
                }
                if (transparent->m_display_color_bytes == 4)
                {
                    *phy_fb_32++ = rgb;
                }
                else
                {
                    *phy_fb_16++ = GL_RGB_32_to_16(rgb);
                }
            }
        }
    }
}

static const c_surface_vir_api c_surface_transparent_vir_api_table = {
        c_surface_transparent_vir_draw_pixel,
        c_surface_transparent_vir_fill_rect,
        c_surface_vir_fill_rect_low_level,
        c_surface_vir_draw_pixel_low_level,
};

void c_surface_transparent_init(c_surface_transparent *self, unsigned int width, unsigned int height, unsigned int color_bytes, Z_ORDER_LEVEL max_zorder)
{
    c_surface_init_with_order(&self->base, width, height, color_bytes, max_zorder);
    self->base.m_api = &c_surface_transparent_vir_api_table;

    c_rect layer_rect;
    c_rect_init(&layer_rect, 0, 0, width, height);
    for (int i = Z_ORDER_LEVEL_0; i <= self->base.m_max_zorder; i++)
    {
        GUILITE_ASSERT(self->base.m_layers[i].fb = guilite_calloc(c_rect_width(&layer_rect) * c_rect_height(&layer_rect), SURFACE_COLOR_BYTES));
        self->base.m_layers[i].rect = layer_rect;
    }
}
