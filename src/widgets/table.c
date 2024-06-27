#include "table.h"

void c_table_set_sheet_align(c_table *self, unsigned int align_type)
{
    self->m_align_type = align_type;
}
void c_table_set_row_num(c_table *self, unsigned int row_num)
{
    self->m_row_num = row_num;
}
void c_table_set_col_num(c_table *self, unsigned int col_num)
{
    self->m_col_num = col_num;
}
void c_table_set_row_height(c_table *self, unsigned int height)
{
    for (unsigned int i = 0; i < self->m_row_num; i++)
    {
        self->m_row_height[i] = height;
    }
}
void c_table_set_col_width(c_table *self, unsigned int width)
{
    for (unsigned int i = 0; i < self->m_col_num; i++)
    {
        self->m_col_width[i] = width;
    }
}
int c_table_set_row_height_with_index(c_table *self, unsigned int index, unsigned int height)
{
    if (self->m_row_num > index)
    {
        self->m_row_height[index] = height;
        return index;
    }
    return -1;
}
int c_table_set_col_width_with_index(c_table *self, unsigned int index, unsigned int width)
{
    if (self->m_col_num > index)
    {
        self->m_col_width[index] = width;
        return index;
    }
    return -1;
}
void c_table_set_item(c_table *self, int row, int col, char *str, unsigned int color)
{
    c_table_draw_item(self, row, col, str, color);
}

unsigned int c_table_get_row_num(c_table *self)
{
    return self->m_row_num;
}
unsigned int c_table_get_col_num(c_table *self)
{
    return self->m_col_num;
}
c_rect c_table_get_item_rect(c_table *self, int row, int col)
{
    static c_rect rect;
    if (row >= MAX_ROW_NUM || col >= MAX_COL_NUM)
    {
        return rect;
    }

    unsigned int width = 0;
    unsigned int height = 0;
    for (int i = 0; i < col; i++)
    {
        width += self->m_col_width[i];
    }
    for (int j = 0; j < row; j++)
    {
        height += self->m_row_height[j];
    }

    c_rect wRect;
    c_wnd_get_screen_rect(&self->base, &wRect);

    rect.m_left = wRect.m_left + width;
    rect.m_right = rect.m_left + self->m_col_width[col];
    if (rect.m_right > wRect.m_right)
    {
        rect.m_right = wRect.m_right;
    }
    rect.m_top = wRect.m_top + height;
    rect.m_bottom = rect.m_top + self->m_row_height[row];
    if (rect.m_bottom > wRect.m_bottom)
    {
        rect.m_bottom = wRect.m_bottom;
    }
    return rect;
}

void c_table_vir_pre_create_wnd(c_wnd *self)
{
    c_table *table = (c_table *)self;
    self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE);
    self->m_font = c_theme_get_font(FONT_DEFAULT);
    self->m_font_color = c_theme_get_color(COLOR_WND_FONT);
}
void c_table_draw_item(c_table *self, int row, int col, const char *str, unsigned int color)
{
    c_rect rect = c_table_get_item_rect(self, row, col);
    self->base.m_surface->m_api->fill_rect(self->base.m_surface, rect.m_left + 1, rect.m_top + 1, rect.m_right - 1, rect.m_bottom - 1, color,
                                           self->base.m_z_order);
    c_word_draw_string_in_rect(self->base.m_surface, self->base.m_z_order, str, &rect, self->base.m_font, self->base.m_font_color, GL_ARGB(0, 0, 0, 0),
                               self->m_align_type);
}

static const c_wnd_vir_api c_table_vir_api_table = {
        c_wnd_vir_connect,     c_wnd_vir_on_init_children, c_wnd_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_table_vir_pre_create_wnd, c_wnd_vir_on_focus, c_wnd_vir_on_kill_focus,
};

void c_table_init(c_table *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_table_vir_api_table;
}
