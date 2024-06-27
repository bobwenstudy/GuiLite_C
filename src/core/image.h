#pragma once

typedef struct c_image_operator c_image_operator;

typedef struct c_image_operator_vir_api c_image_operator_vir_api;
struct c_image_operator_vir_api
{
    void (*draw_image)(c_image_operator *self, c_surface *surface, int z_order, const void *image_info, int x, int y, unsigned int mask_rgb);
    void (*draw_image_with_src)(c_image_operator *self, c_surface *surface, int z_order, const void *image_info, int x, int y, int src_x, int src_y, int width,
                                int height, unsigned int mask_rgb);
};

struct c_image_operator
{
    const c_image_operator_vir_api *m_api; // virtual api
};

typedef struct c_bitmap_operator c_bitmap_operator;
struct c_bitmap_operator
{
    c_image_operator base;
};

void c_bitmap_operator_init(c_bitmap_operator *self);
void c_image_draw_image(c_surface *surface, int z_order, const void *image_info, int x, int y, unsigned int mask_rgb);
void c_image_draw_image_simple(c_surface *surface, int z_order, const void *image_info, int x, int y);
void c_image_draw_image_with_src(c_surface *surface, int z_order, const void *image_info, int x, int y, int src_x, int src_y, int width, int height,
                                 unsigned int mask_rgb);
void c_image_draw_image_with_src_simple(c_surface *surface, int z_order, const void *image_info, int x, int y, int src_x, int src_y, int width, int height);
void c_image_init(c_image_operator *operator);