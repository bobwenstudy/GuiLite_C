#include "api.h"
#include "resource.h"
#include "display.h"
#include "image.h"

#define DEFAULT_MASK_COLOR 0xFF080408

void c_image_operator_vir_draw_image(c_image_operator *self, c_surface *surface, int z_order, const void *image_info, int x, int y, unsigned int mask_rgb)
{
}

void c_image_operator_vir_draw_image_with_src(c_image_operator *self, c_surface *surface, int z_order, const void *image_info, int x, int y, int src_x,
                                              int src_y, int width, int height, unsigned int mask_rgb)
{
}

static const c_image_operator_vir_api c_image_operator_vir_api_table = {
        c_image_operator_vir_draw_image,
        c_image_operator_vir_draw_image_with_src,
};

void c_image_operator_init(c_image_operator *self)
{
    self->m_api = &c_image_operator_vir_api_table;
}

void c_bitmap_operator_vir_draw_image(c_image_operator *self, c_surface *surface, int z_order, const void *image_info, int x, int y, unsigned int mask_rgb)
{
    GUILITE_ASSERT(image_info);
    BITMAP_INFO *pBitmap = (BITMAP_INFO *)image_info;
    unsigned short *lower_fb_16 = 0;
    unsigned int *lower_fb_32 = 0;
    int lower_fb_width = 0;
    c_rect lower_fb_rect;
    if (z_order >= Z_ORDER_LEVEL_1)
    {
        lower_fb_16 = (unsigned short *)surface->m_layers[z_order - 1].fb;
        lower_fb_32 = (unsigned int *)surface->m_layers[z_order - 1].fb;
        lower_fb_rect = surface->m_layers[z_order - 1].rect;
        lower_fb_width = c_rect_width(&lower_fb_rect);
    }
    unsigned int mask_rgb_16 = GL_RGB_32_to_16(mask_rgb);
    int xsize = pBitmap->width;
    int ysize = pBitmap->height;
    const unsigned short *pData = (const unsigned short *)pBitmap->pixel_color_array;
    int color_bytes = surface->m_color_bytes;
    for (int y_ = y; y_ < y + ysize; y_++)
    {
        for (int x_ = x; x_ < x + xsize; x_++)
        {
            unsigned int rgb = *pData++;
            if (mask_rgb_16 == rgb)
            {
                if (c_rect_pt_in_rect(&lower_fb_rect, x_, y_))
                {
                    surface->m_api->draw_pixel(
                            surface, x_, y_,
                            (color_bytes == 4) ? lower_fb_32[(y_ - lower_fb_rect.m_top) * lower_fb_width + (x_ - lower_fb_rect.m_left)]
                                               : GL_RGB_16_to_32(lower_fb_16[(y_ - lower_fb_rect.m_top) * lower_fb_width + (x_ - lower_fb_rect.m_left)]),
                            z_order);
                }
            }
            else
            {
                surface->m_api->draw_pixel(surface, x_, y_, GL_RGB_16_to_32(rgb), z_order);
            }
        }
    }
}

void c_bitmap_operator_vir_draw_image_with_src(c_image_operator *self, c_surface *surface, int z_order, const void *image_info, int x, int y, int src_x,
                                               int src_y, int width, int height, unsigned int mask_rgb)
{
    GUILITE_ASSERT(image_info);
    BITMAP_INFO *pBitmap = (BITMAP_INFO *)image_info;
    if (0 == pBitmap || (src_x + width > pBitmap->width) || (src_y + height > pBitmap->height))
    {
        return;
    }

    unsigned short *lower_fb_16 = 0;
    unsigned int *lower_fb_32 = 0;
    int lower_fb_width = 0;
    c_rect lower_fb_rect;
    if (z_order >= Z_ORDER_LEVEL_1)
    {
        lower_fb_16 = (unsigned short *)surface->m_layers[z_order - 1].fb;
        lower_fb_32 = (unsigned int *)surface->m_layers[z_order - 1].fb;
        lower_fb_rect = surface->m_layers[z_order - 1].rect;
        lower_fb_width = c_rect_width(&lower_fb_rect);
    }
    unsigned int mask_rgb_16 = GL_RGB_32_to_16(mask_rgb);
    const unsigned short *pData = (const unsigned short *)pBitmap->pixel_color_array;
    int color_bytes = surface->m_color_bytes;
    for (int y_ = 0; y_ < height; y_++)
    {
        const unsigned short *p = &pData[src_x + (src_y + y_) * pBitmap->width];
        for (int x_ = 0; x_ < width; x_++)
        {
            unsigned int rgb = *p++;
            if (mask_rgb_16 == rgb)
            {
                if (c_rect_pt_in_rect(&lower_fb_rect, x + x_, y + y_))
                {
                    surface->m_api->draw_pixel(
                            surface, x + x_, y + y_,
                            (color_bytes == 4) ? lower_fb_32[(y + y_ - lower_fb_rect.m_top) * lower_fb_width + x + x_ - lower_fb_rect.m_left]
                                               : GL_RGB_16_to_32(lower_fb_16[(y + y_ - lower_fb_rect.m_top) * lower_fb_width + x + x_ - lower_fb_rect.m_left]),
                            z_order);
                }
            }
            else
            {
                surface->m_api->draw_pixel(surface, x + x_, y + y_, GL_RGB_16_to_32(rgb), z_order);
            }
        }
    }
}

static const c_image_operator_vir_api c_bitmap_operator_vir_api_table = {
        c_bitmap_operator_vir_draw_image,
        c_bitmap_operator_vir_draw_image_with_src,
};

void c_bitmap_operator_init(c_bitmap_operator *self)
{
    c_image_operator_init(&self->base);
    self->base.m_api = &c_bitmap_operator_vir_api_table;
}

static c_image_operator *imageOperator;

void c_image_draw_image(c_surface *surface, int z_order, const void *image_info, int x, int y, unsigned int mask_rgb)
{
    imageOperator->m_api->draw_image(imageOperator, surface, z_order, image_info, x, y, mask_rgb);
}

void c_image_draw_image_simple(c_surface *surface, int z_order, const void *image_info, int x, int y)
{
    c_image_draw_image(surface, z_order, image_info, x, y, DEFAULT_MASK_COLOR);
}

void c_image_draw_image_with_src(c_surface *surface, int z_order, const void *image_info, int x, int y, int src_x, int src_y, int width, int height,
                                 unsigned int mask_rgb)
{
    imageOperator->m_api->draw_image_with_src(imageOperator, surface, z_order, image_info, x, y, src_x, src_y, width, height, mask_rgb);
}

void c_image_draw_image_with_src_simple(c_surface *surface, int z_order, const void *image_info, int x, int y, int src_x, int src_y, int width, int height)
{
    c_image_draw_image_with_src(surface, z_order, image_info, x, y, src_x, src_y, width, height, DEFAULT_MASK_COLOR);
}

void c_image_init(c_image_operator *operator)
{
    imageOperator = operator;
}
