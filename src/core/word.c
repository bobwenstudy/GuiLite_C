#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "api.h"
#include "display.h"
#include "word.h"

void c_font_operator_vir_draw_string(c_font_operator *self, c_surface *surface, int z_order, const void *string, int x, int y, const void *font,
                                     unsigned int font_color, unsigned int bg_color)
{
}
void c_font_operator_vir_draw_string_in_rect(c_font_operator *self, c_surface *surface, int z_order, const void *string, c_rect rect, const void *font,
                                             unsigned int font_color, unsigned int bg_color, unsigned int align_type)
{
}
void c_font_operator_vir_draw_value(c_font_operator *self, c_surface *surface, int z_order, int value, int dot_position, int x, int y, const void *font,
                                    unsigned int font_color, unsigned int bg_color)
{
}
void c_font_operator_vir_draw_value_in_rect(c_font_operator *oselfp, c_surface *surface, int z_order, int value, int dot_position, c_rect rect,
                                            const void *font, unsigned int font_color, unsigned int bg_color, unsigned int align_type)
{
}
int c_font_operator_vir_get_str_size(c_font_operator *self, const void *string, const void *font, int *width, int *height)
{
    return 0;
}

void c_font_operator_vir_get_string_pos(c_font_operator *self, const void *string, const void *font, c_rect rect, unsigned int align_type, int *x, int *y)
{
    int x_size, y_size;
    self->m_api->get_str_size(self, string, font, &x_size, &y_size);
    int height = rect.m_bottom - rect.m_top + 1;
    int width = rect.m_right - rect.m_left + 1;
    *x = *y = 0;
    switch (align_type & ALIGN_HMASK)
    {
    case ALIGN_HCENTER:
        if (width > x_size)
        {
            *x = (width - x_size) / 2;
        }
        break;
    case ALIGN_LEFT:
        *x = 0;
        break;
    case ALIGN_RIGHT:
        if (width > x_size)
        {
            *x = width - x_size;
        }
        break;
    default:
        GUILITE_ASSERT(0);
        break;
    }
    switch (align_type & ALIGN_VMASK)
    {
    case ALIGN_VCENTER:
        if (height > y_size)
        {
            *y = (height - y_size) / 2;
        }
        break;
    case ALIGN_TOP:
        *y = 0;
        break;
    case ALIGN_BOTTOM:
        if (height > y_size)
        {
            *y = height - y_size;
        }
        break;
    default:
        GUILITE_ASSERT(0);
        break;
    }
}

static const c_font_operator_vir_api c_font_operator_vir_api_table = {
        c_font_operator_vir_draw_string,        c_font_operator_vir_draw_string_in_rect, c_font_operator_vir_draw_value,
        c_font_operator_vir_draw_value_in_rect, c_font_operator_vir_get_str_size,        c_font_operator_vir_get_string_pos,
};

void c_font_operator_init(c_font_operator *self)
{
    self->m_api = &c_font_operator_vir_api_table;
}

void c_lattice_font_op_vir_draw_string(c_font_operator *self, c_surface *surface, int z_order, const void *string, int x, int y, const void *font,
                                       unsigned int font_color, unsigned int bg_color)
{
    const char *s = (const char *)string;
    if (0 == s)
    {
        return;
    }

    int offset = 0;
    unsigned int utf8_code;
    while (*s)
    {
        s += c_lattice_font_op_get_utf8_code((c_lattice_font_op *)self, s, &utf8_code);
        offset += c_lattice_font_op_draw_single_char((c_lattice_font_op *)self, surface, z_order, utf8_code, (x + offset), y, (const LATTICE_FONT_INFO *)font,
                                                     font_color, bg_color);
    }
}

void c_lattice_font_op_vir_draw_string_in_rect(c_font_operator *self, c_surface *surface, int z_order, const void *string, c_rect rect, const void *font,
                                               unsigned int font_color, unsigned int bg_color, unsigned int align_type)
{
    const char *s = (const char *)string;
    if (0 == s)
    {
        return;
    }
    int x, y;
    self->m_api->get_string_pos(self, s, (const LATTICE_FONT_INFO *)font, rect, align_type, &x, &y);
    self->m_api->draw_string(self, surface, z_order, string, rect.m_left + x, rect.m_top + y, font, font_color, bg_color);
}

void c_lattice_font_op_vir_draw_value(c_font_operator *self, c_surface *surface, int z_order, int value, int dot_position, int x, int y, const void *font,
                                      unsigned int font_color, unsigned int bg_color)
{
    char buf[VALUE_STR_LEN];
    c_lattice_font_op_value_2_string((c_lattice_font_op *)self, value, dot_position, buf, VALUE_STR_LEN);
    self->m_api->draw_string(self, surface, z_order, buf, x, y, (const LATTICE_FONT_INFO *)font, font_color, bg_color);
}

void c_lattice_font_op_vir_draw_value_in_rect(c_font_operator *self, c_surface *surface, int z_order, int value, int dot_position, c_rect rect,
                                              const void *font, unsigned int font_color, unsigned int bg_color, unsigned int align_type)
{
    char buf[VALUE_STR_LEN];
    c_lattice_font_op_value_2_string((c_lattice_font_op *)self, value, dot_position, buf, VALUE_STR_LEN);
    self->m_api->draw_string_in_rect(self, surface, z_order, buf, rect, (const LATTICE_FONT_INFO *)font, font_color, bg_color, align_type);
}

int c_lattice_font_op_vir_get_str_size(c_font_operator *self, const void *string, const void *font, int *width, int *height)
{
    const char *s = (const char *)string;
    if (0 == s || 0 == font)
    {
        *width = *height = 0;
        return -1;
    }

    int lattice_width = 0;
    unsigned int utf8_code;
    int utf8_bytes;
    while (*s)
    {
        utf8_bytes = c_lattice_font_op_get_utf8_code((c_lattice_font_op *)self, s, &utf8_code);
        const LATTICE *p_lattice = c_lattice_font_op_get_lattice((c_lattice_font_op *)self, (const LATTICE_FONT_INFO *)font, utf8_code);
        lattice_width += p_lattice ? p_lattice->width : ((const LATTICE_FONT_INFO *)font)->height;
        s += utf8_bytes;
    }
    *width = lattice_width;
    *height = ((const LATTICE_FONT_INFO *)font)->height;
    return 0;
}

void c_lattice_font_op_value_2_string(c_lattice_font_op *self, int value, int dot_position, char *buf, int len)
{
    memset(buf, 0, len);
    switch (dot_position)
    {
    case 0:
        guilite_sprintf(buf, "%d", value);
        break;
    // TODO: close for reducing code size
    // case 1:
    //     guilite_sprintf(buf, "%.1f", value * 1.0 / 10);
    //     break;
    // case 2:
    //     guilite_sprintf(buf, "%.2f", value * 1.0 / 100);
    //     break;
    // case 3:
    //     guilite_sprintf(buf, "%.3f", value * 1.0 / 1000);
    //     break;
    default:
        GUILITE_ASSERT(false);
        break;
    }
}

int c_lattice_font_op_draw_single_char(c_lattice_font_op *self, c_surface *surface, int z_order, unsigned int utf8_code, int x, int y,
                                       const LATTICE_FONT_INFO *font, unsigned int font_color, unsigned int bg_color)
{
    unsigned int error_color = 0xFFFFFFFF;
    if (font)
    {
        const LATTICE *p_lattice = c_lattice_font_op_get_lattice(self, font, utf8_code);
        if (p_lattice)
        {
            c_lattice_font_op_draw_lattice(self, surface, z_order, x, y, p_lattice->width, font->height, p_lattice->pixel_buffer, font_color, bg_color);
            return p_lattice->width;
        }
    }
    else
    {
        error_color = GL_RGB(255, 0, 0);
    }

    int len = 16;
    for (int y_ = 0; y_ < len; y_++)
    {
        for (int x_ = 0; x_ < len; x_++)
        {
            int diff = (x_ - y_);
            int sum = (x_ + y_);
            (diff == 0 || diff == -1 || diff == 1 || sum == len || sum == (len - 1) || sum == (len + 1))
                    ? surface->m_api->draw_pixel(surface, (x + x_), (y + y_), error_color, z_order)
                    : surface->m_api->draw_pixel(surface, (x + x_), (y + y_), 0, z_order);
        }
    }
    return len;
}

void c_lattice_font_op_draw_lattice(c_lattice_font_op *self, c_surface *surface, int z_order, int x, int y, int width, int height, const unsigned char *p_data,
                                    unsigned int font_color, unsigned int bg_color)
{
    unsigned int r, g, b, rgb;
    unsigned char blk_value = *p_data++;
    unsigned char blk_cnt = *p_data++;
    b = (GL_RGB_B(font_color) * blk_value + GL_RGB_B(bg_color) * (255 - blk_value)) >> 8;
    g = (GL_RGB_G(font_color) * blk_value + GL_RGB_G(bg_color) * (255 - blk_value)) >> 8;
    r = (GL_RGB_R(font_color) * blk_value + GL_RGB_R(bg_color) * (255 - blk_value)) >> 8;
    rgb = GL_RGB(r, g, b);
    for (int y_ = 0; y_ < height; y_++)
    {
        for (int x_ = 0; x_ < width; x_++)
        {
            GUILITE_ASSERT(blk_cnt);
            if (0x00 == blk_value)
            {
                if (GL_ARGB_A(bg_color))
                {
                    surface->m_api->draw_pixel(surface, x + x_, y + y_, bg_color, z_order);
                }
            }
            else
            {
                surface->m_api->draw_pixel(surface, (x + x_), (y + y_), rgb, z_order);
            }
            if (--blk_cnt == 0)
            {
                blk_value = *p_data++;
                blk_cnt = *p_data++;
                b = (GL_RGB_B(font_color) * blk_value + GL_RGB_B(bg_color) * (255 - blk_value)) >> 8;
                g = (GL_RGB_G(font_color) * blk_value + GL_RGB_G(bg_color) * (255 - blk_value)) >> 8;
                r = (GL_RGB_R(font_color) * blk_value + GL_RGB_R(bg_color) * (255 - blk_value)) >> 8;
                rgb = GL_RGB(r, g, b);
            }
        }
    }
}

const LATTICE *c_lattice_font_op_get_lattice(c_lattice_font_op *self, const LATTICE_FONT_INFO *font, unsigned int utf8_code)
{
    int first = 0;
    int last = font->count - 1;
    int middle = (first + last) / 2;

    while (first <= last)
    {
        if (font->lattice_array[middle].utf8_code < utf8_code)
            first = middle + 1;
        else if (font->lattice_array[middle].utf8_code == utf8_code)
        {
            return &font->lattice_array[middle];
        }
        else
        {
            last = middle - 1;
        }
        middle = (first + last) / 2;
    }
    return 0;
}

int c_lattice_font_op_get_utf8_code(c_lattice_font_op *self, const char *s, unsigned int *output_utf8_code)
{
    static unsigned char s_utf8_length_table[256] = {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1};

    unsigned char *us = (unsigned char *)s;
    int utf8_bytes = s_utf8_length_table[*us];
    switch (utf8_bytes)
    {
    case 1:
        *output_utf8_code = *us;
        break;
    case 2:
        *output_utf8_code = (*us << 8) | (*(us + 1));
        break;
    case 3:
        *output_utf8_code = (*us << 16) | ((*(us + 1)) << 8) | *(us + 2);
        break;
    case 4:
        *output_utf8_code = (*us << 24) | ((*(us + 1)) << 16) | (*(us + 2) << 8) | *(us + 3);
        break;
    default:
        GUILITE_ASSERT(false);
        break;
    }
    return utf8_bytes;
}

static const c_font_operator_vir_api c_lattice_font_op_vir_api_table = {
        c_lattice_font_op_vir_draw_string,        c_lattice_font_op_vir_draw_string_in_rect, c_lattice_font_op_vir_draw_value,
        c_lattice_font_op_vir_draw_value_in_rect, c_lattice_font_op_vir_get_str_size,        c_font_operator_vir_get_string_pos,
};

void c_lattice_font_op_init(c_lattice_font_op *self)
{
    c_font_operator_init(&self->base);
    self->base.m_api = &c_lattice_font_op_vir_api_table;
}

static c_font_operator *fontOperator;

void c_word_draw_string(void *surface, int z_order, const void *string, int x, int y, const void *font, unsigned int font_color, unsigned int bg_color)
{
    fontOperator->m_api->draw_string(fontOperator, surface, z_order, string, x, y, font, font_color, bg_color);
}

void c_word_draw_string_in_rect(void *surface, int z_order, const void *string, void *rect, const void *font, unsigned int font_color, unsigned int bg_color,
                                unsigned int align_type)
{
    fontOperator->m_api->draw_string_in_rect(fontOperator, surface, z_order, string, *(c_rect *)rect, font, font_color, bg_color, align_type);
}

void c_word_draw_value_in_rect(void *surface, int z_order, int value, int dot_position, void *rect, const void *font, unsigned int font_color,
                               unsigned int bg_color, unsigned int align_type)
{
    fontOperator->m_api->draw_value_in_rect(fontOperator, surface, z_order, value, dot_position, *(c_rect *)rect, font, font_color, bg_color, align_type);
}

void c_word_draw_value(void *surface, int z_order, int value, int dot_position, int x, int y, const void *font, unsigned int font_color, unsigned int bg_color)
{
    fontOperator->m_api->draw_value(fontOperator, surface, z_order, value, dot_position, x, y, font, font_color, bg_color);
}

int c_word_get_str_size(const void *string, const void *font, int *width, int *height)
{
    return fontOperator->m_api->get_str_size(fontOperator, string, font, width, height);
}

void c_word_init(c_font_operator *operator)
{
    fontOperator = operator;
}
