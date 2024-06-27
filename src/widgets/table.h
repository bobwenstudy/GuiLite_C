#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"

#define MAX_COL_NUM 30
#define MAX_ROW_NUM 30

typedef struct c_table c_table;
struct c_table
{
    c_wnd base;
    unsigned int m_align_type;
    unsigned int m_row_num;
    unsigned int m_col_num;
    unsigned int m_row_height[MAX_ROW_NUM];
    unsigned int m_col_width[MAX_COL_NUM];
};

void c_table_set_sheet_align(c_table *self, unsigned int align_type);
void c_table_set_row_num(c_table *self, unsigned int row_num);
void c_table_set_col_num(c_table *self, unsigned int col_num);
void c_table_set_row_height(c_table *self, unsigned int height);
void c_table_set_col_width(c_table *self, unsigned int width);
int c_table_set_row_height_with_index(c_table *self, unsigned int index, unsigned int height);
int c_table_set_col_width_with_index(c_table *self, unsigned int index, unsigned int width);
void c_table_set_item(c_table *self, int row, int col, char *str, unsigned int color);
unsigned int c_table_get_row_num(c_table *self);
unsigned int c_table_get_col_num(c_table *self);
c_rect c_table_get_item_rect(c_table *self, int row, int col);
void c_table_vir_pre_create_wnd(c_wnd *self);
void c_table_draw_item(c_table *self, int row, int col, const char *str, unsigned int color);
void c_table_init(c_table *self);
