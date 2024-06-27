#pragma once

#include "../core/api.h"
#include "../core/resource.h"
#include "../core/display.h"
#include <string.h>
#include <stdio.h>

#define VALUE_STR_LEN 16

typedef struct c_font_operator c_font_operator;

typedef struct c_font_operator_vir_api c_font_operator_vir_api;
struct c_font_operator_vir_api
{
    void (*draw_string)(c_font_operator *self, c_surface *surface, int z_order, const void *string, int x, int y, const void *font, unsigned int font_color,
                        unsigned int bg_color);
    void (*draw_string_in_rect)(c_font_operator *self, c_surface *surface, int z_order, const void *string, c_rect rect, const void *font,
                                unsigned int font_color, unsigned int bg_color, unsigned int align_type);
    void (*draw_value)(c_font_operator *self, c_surface *surface, int z_order, int value, int dot_position, int x, int y, const void *font,
                       unsigned int font_color, unsigned int bg_color);
    void (*draw_value_in_rect)(c_font_operator *oselfp, c_surface *surface, int z_order, int value, int dot_position, c_rect rect, const void *font,
                               unsigned int font_color, unsigned int bg_color, unsigned int align_type);
    int (*get_str_size)(c_font_operator *self, const void *string, const void *font, int *width, int *height);
    void (*get_string_pos)(c_font_operator *self, const void *string, const void *font, c_rect rect, unsigned int align_type, int *x, int *y);
};

struct c_font_operator
{
    const c_font_operator_vir_api *m_api; // virtual api
};

void c_font_operator_vir_get_string_pos(c_font_operator *self, const void *string, const void *font, c_rect rect, unsigned int align_type, int *x, int *y);
void c_font_operator_init(c_font_operator *self);

typedef struct c_lattice_font_op c_lattice_font_op;
struct c_lattice_font_op
{
    c_font_operator base;
};

void c_lattice_font_op_value_2_string(c_lattice_font_op *self, int value, int dot_position, char *buf, int len);
int c_lattice_font_op_draw_single_char(c_lattice_font_op *self, c_surface *surface, int z_order, unsigned int utf8_code, int x, int y,
                                       const LATTICE_FONT_INFO *font, unsigned int font_color, unsigned int bg_color);
void c_lattice_font_op_draw_lattice(c_lattice_font_op *self, c_surface *surface, int z_order, int x, int y, int width, int height, const unsigned char *p_data,
                                    unsigned int font_color, unsigned int bg_color);
const LATTICE *c_lattice_font_op_get_lattice(c_lattice_font_op *self, const LATTICE_FONT_INFO *font, unsigned int utf8_code);
int c_lattice_font_op_get_utf8_code(c_lattice_font_op *self, const char *s, unsigned int *output_utf8_code);
void c_lattice_font_op_init(c_lattice_font_op *self);

typedef struct
{
    c_font_operator *fontOperator;
} c_word;

void c_word_draw_string(void *surface, int z_order, const void *string, int x, int y, const void *font, unsigned int font_color, unsigned int bg_color);
void c_word_draw_string_in_rect(void *surface, int z_order, const void *string, void *rect, const void *font, unsigned int font_color, unsigned int bg_color,
                                unsigned int align_type);
void c_word_draw_value_in_rect(void *surface, int z_order, int value, int dot_position, void *rect, const void *font, unsigned int font_color,
                               unsigned int bg_color, unsigned int align_type);
void c_word_draw_value(void *surface, int z_order, int value, int dot_position, int x, int y, const void *font, unsigned int font_color, unsigned int bg_color);
int c_word_get_str_size(const void *string, const void *font, int *width, int *height);
void c_word_init(c_font_operator *fontOperator);
