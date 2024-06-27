#include "edit.h"
#include "widgets.h"
#include <string.h>

const char *c_edit_get_text(c_edit *self)
{
    return self->m_str;
}
void c_edit_set_text(c_edit *self, const char *str)
{
    if (str != 0 && strlen(str) < sizeof(self->m_str))
    {
        strcpy(self->m_str, str);
    }
}
void c_edit_set_keyboard_style(c_edit *self, KEYBOARD_STYLE kb_sytle)
{
    self->m_kb_style = kb_sytle;
}

void c_edit_vir_pre_create_wnd(c_wnd *self)
{
    c_edit *edit = (c_edit *)self;
    self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
    edit->m_kb_style = STYLE_ALL_BOARD;
    self->m_font = c_theme_get_font(FONT_DEFAULT);
    self->m_font_color = c_theme_get_color(COLOR_WND_FONT);

    memset(edit->m_str_input, 0, sizeof(edit->m_str_input));
    memset(edit->m_str, 0, sizeof(edit->m_str));
    c_edit_set_text(edit, self->m_str);
}

void c_edit_vir_on_paint(c_wnd *self)
{
    c_edit *edit = (c_edit *)self;
    c_rect rect, kb_rect;
    c_wnd_get_screen_rect(self, &rect);
    c_wnd_get_screen_rect(&c_edit_s_keyboard.base, &kb_rect);
    switch (self->m_status)
    {
    case STATUS_NORMAL:
        if ((c_wnd_get_attr(&c_edit_s_keyboard.base) & ATTR_VISIBLE) == ATTR_VISIBLE)
        {
            c_keyboard_close_keyboard(&c_edit_s_keyboard);
            self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
        }
        c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_NORMAL), self->m_z_order);
        c_word_draw_string_in_rect(self->m_surface, c_wnd_get_z_order(self->m_parent), edit->m_str, &rect, self->m_font, self->m_font_color,
                                   c_theme_get_color(COLOR_WND_NORMAL), ALIGN_HCENTER | ALIGN_VCENTER);
        break;
    case STATUS_FOCUSED:
        if ((c_wnd_get_attr(&c_edit_s_keyboard.base) & ATTR_VISIBLE) == ATTR_VISIBLE)
        {
            c_keyboard_close_keyboard(&c_edit_s_keyboard);
            self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
        }
        c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_FOCUS), self->m_z_order);
        c_word_draw_string_in_rect(self->m_surface, c_wnd_get_z_order(self->m_parent), edit->m_str, &rect, self->m_font, self->m_font_color,
                                   c_theme_get_color(COLOR_WND_FOCUS), ALIGN_HCENTER | ALIGN_VCENTER);
        break;
    case STATUS_PUSHED:
        if ((c_wnd_get_attr(&c_edit_s_keyboard.base) & ATTR_VISIBLE) != ATTR_VISIBLE)
        {
            self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS | ATTR_PRIORITY);
            c_keyboard_open_keyboard(&c_edit_s_keyboard, self, IDD_KEY_BOARD, edit->m_kb_style, c_edit_on_key_board_click);
        }
        self->m_surface->m_api->fill_rect(self->m_surface, rect.m_left, rect.m_top, rect.m_right, rect.m_bottom, c_theme_get_color(COLOR_WND_PUSHED),
                                          c_wnd_get_z_order(self->m_parent));
        c_surface_draw_rect(self->m_surface, rect.m_left, rect.m_top, rect.m_right, rect.m_bottom, c_theme_get_color(COLOR_WND_BORDER),
                            c_wnd_get_z_order(self->m_parent), 2);

        if (strlen(edit->m_str_input))
        {
            c_word_draw_string_in_rect(self->m_surface, c_wnd_get_z_order(self->m_parent), edit->m_str_input, &rect, self->m_font, self->m_font_color,
                                       c_theme_get_color(COLOR_WND_PUSHED), ALIGN_HCENTER | ALIGN_VCENTER);
        }
        else
        {
            c_word_draw_string_in_rect(self->m_surface, c_wnd_get_z_order(self->m_parent), edit->m_str, &rect, self->m_font, self->m_font_color,
                                       c_theme_get_color(COLOR_WND_PUSHED), ALIGN_HCENTER | ALIGN_VCENTER);
        }
        break;
    default:
        GUILITE_ASSERT(false);
    }
}

void c_edit_vir_on_focus(c_wnd *self)
{
    self->m_status = STATUS_FOCUSED;
    self->m_api->on_paint(self);
}

void c_edit_vir_on_kill_focus(c_wnd *self)
{
    self->m_status = STATUS_NORMAL;
    self->m_api->on_paint(self);
}

void c_edit_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key)
{
    c_edit *edit = (c_edit *)self;
    switch (key)
    {
    case NAV_ENTER:
        if (self->m_status == STATUS_PUSHED)
        {
            c_edit_s_keyboard.base.m_api->on_navigate(&c_edit_s_keyboard.base, key);
        }
        else
        {
            self->m_api->on_touch(self, self->m_wnd_rect.m_left, self->m_wnd_rect.m_top, TOUCH_DOWN);
            self->m_api->on_touch(self, self->m_wnd_rect.m_left, self->m_wnd_rect.m_top, TOUCH_UP);
        }
        return;
    case NAV_BACKWARD:
    case NAV_FORWARD:
        if (self->m_status == STATUS_PUSHED)
        {
            c_edit_s_keyboard.base.m_api->on_navigate(&c_edit_s_keyboard.base, key);
        }
        else
        {
            c_wnd_vir_on_navigate(self, key);
        }
        return;
    }
}

void c_edit_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action)
{
    c_edit *edit = (c_edit *)self;
    if (action == TOUCH_DOWN)
    {
        c_edit_on_touch_down(edit, x, y);
    }
    else
    {
        c_edit_on_touch_up(edit, x, y);
    }
}

void c_edit_on_key_board_click(c_wnd *self, int id, int param)
{
    c_edit *edit = (c_edit *)self;
    switch (param)
    {
    case CLICK_CHAR:
        strcpy(edit->m_str_input, c_keyboard_get_str(&c_edit_s_keyboard));
        self->m_api->on_paint(self);
        break;
    case CLICK_ENTER:
        if (strlen(edit->m_str_input))
        {
            memcpy(edit->m_str, edit->m_str_input, sizeof(edit->m_str_input));
        }
        self->m_status = STATUS_FOCUSED;
        self->m_api->on_paint(self);
        break;
    case CLICK_ESC:
        memset(edit->m_str_input, 0, sizeof(edit->m_str_input));
        self->m_status = STATUS_FOCUSED;
        self->m_api->on_paint(self);
        break;
    default:
        GUILITE_ASSERT(false);
        break;
    }
}

void c_edit_on_touch_down(c_edit *self, int x, int y)
{
    c_rect kb_rect_relate_2_edit_parent;
    c_wnd_get_wnd_rect(&c_edit_s_keyboard.base, &kb_rect_relate_2_edit_parent);
    kb_rect_relate_2_edit_parent.m_left += self->base.m_wnd_rect.m_left;
    kb_rect_relate_2_edit_parent.m_right += self->base.m_wnd_rect.m_left;
    kb_rect_relate_2_edit_parent.m_top += self->base.m_wnd_rect.m_top;
    kb_rect_relate_2_edit_parent.m_bottom += self->base.m_wnd_rect.m_top;

    if (c_rect_pt_in_rect(&self->base.m_wnd_rect, x, y))
    { // click edit box
        if (STATUS_NORMAL == self->base.m_status)
        {
            c_wnd_set_child_focus(self->base.m_parent, &self->base);
        }
    }
    else if (c_rect_pt_in_rect(&kb_rect_relate_2_edit_parent, x, y))
    { // click key board
        c_wnd_vir_on_touch(&self->base, x, y, TOUCH_DOWN);
    }
    else
    {
        if (STATUS_PUSHED == self->base.m_status)
        {
            self->base.m_status = STATUS_FOCUSED;
            self->base.m_api->on_paint(&self->base);
        }
    }
}
void c_edit_on_touch_up(c_edit *self, int x, int y)
{
    if (STATUS_FOCUSED == self->base.m_status)
    {
        self->base.m_status = STATUS_PUSHED;
        self->base.m_api->on_paint(&self->base);
    }
    else if (STATUS_PUSHED == self->base.m_status)
    {
        if (c_rect_pt_in_rect(&self->base.m_wnd_rect, x, y))
        { // click edit box
            self->base.m_status = STATUS_FOCUSED;
            self->base.m_api->on_paint(&self->base);
        }
        else
        {
            c_wnd_vir_on_touch(&self->base, x, y, TOUCH_UP);
        }
    }
}

static const c_wnd_vir_api c_edit_vir_api_table = {
        c_wnd_vir_connect,      c_wnd_vir_on_init_children, c_edit_vir_on_paint, c_wnd_vir_show_window,    c_edit_vir_on_touch,
        c_edit_vir_on_navigate, c_edit_vir_pre_create_wnd,  c_edit_vir_on_focus, c_edit_vir_on_kill_focus,
};

void c_edit_init(c_edit *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_edit_vir_api_table;
}
