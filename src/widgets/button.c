#include "button.h"

void c_button_set_on_click(c_button *self, WND_CALLBACK on_click)
{
    self->on_click = on_click;
}

void c_button_vir_on_paint(c_wnd *self)
{
    c_button *button = (c_button *)self;
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);

    switch (button->base.m_status)
    {
    case STATUS_NORMAL:
        c_surface_fill_rect_ex(button->base.m_surface, rect, c_theme_get_color(COLOR_WND_NORMAL), button->base.m_z_order);
        if (button->base.m_str)
        {
            c_word_draw_string_in_rect(button->base.m_surface, button->base.m_z_order, button->base.m_str, &rect, button->base.m_font,
                                       button->base.m_font_color, c_theme_get_color(COLOR_WND_NORMAL), ALIGN_HCENTER | ALIGN_VCENTER);
        }
        break;
    case STATUS_FOCUSED:
        c_surface_fill_rect_ex(button->base.m_surface, rect, c_theme_get_color(COLOR_WND_FOCUS), button->base.m_z_order);
        if (button->base.m_str)
        {
            c_word_draw_string_in_rect(button->base.m_surface, button->base.m_z_order, button->base.m_str, &rect, button->base.m_font,
                                       button->base.m_font_color, c_theme_get_color(COLOR_WND_FOCUS), ALIGN_HCENTER | ALIGN_VCENTER);
        }
        break;
    case STATUS_PUSHED:
        c_surface_fill_rect_ex(button->base.m_surface, rect, c_theme_get_color(COLOR_WND_PUSHED), button->base.m_z_order);
        c_surface_draw_rect_ex(button->base.m_surface, rect, c_theme_get_color(COLOR_WND_BORDER), 2, button->base.m_z_order);
        if (button->base.m_str)
        {
            c_word_draw_string_in_rect(button->base.m_surface, button->base.m_z_order, button->base.m_str, &rect, button->base.m_font,
                                       button->base.m_font_color, c_theme_get_color(COLOR_WND_PUSHED), ALIGN_HCENTER | ALIGN_VCENTER);
        }
        break;
    default:
        GUILITE_ASSERT(false);
        break;
    }
}

void c_button_vir_on_focus(c_wnd *self)
{
    c_button *button = (c_button *)self;
    button->base.m_status = STATUS_FOCUSED;
    button->base.m_api->on_paint(self);
}

void c_button_vir_on_kill_focus(c_wnd *self)
{
    c_button *button = (c_button *)self;
    button->base.m_status = STATUS_NORMAL;
    button->base.m_api->on_paint(self);
}

void c_button_vir_pre_create_wnd(c_wnd *self)
{
    c_button *button = (c_button *)self;
    button->on_click = 0;
    button->base.m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
    button->base.m_font = c_theme_get_font(FONT_DEFAULT);
    button->base.m_font_color = c_theme_get_color(COLOR_WND_FONT);
}

void c_button_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action)
{
    c_button *button = (c_button *)self;
    if (action == TOUCH_DOWN)
    {
        c_wnd_set_child_focus(button->base.m_parent, (c_wnd *)button);
        button->base.m_status = STATUS_PUSHED;
        button->base.m_api->on_paint(self);
    }
    else
    {
        button->base.m_status = STATUS_FOCUSED;
        button->base.m_api->on_paint(self);
        if (button->on_click)
        {
            (button->on_click)(button->base.m_parent, button->base.m_id, 0);
        }
    }
}

void c_button_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key)
{
    c_button *button = (c_button *)self;
    switch (key)
    {
    case NAV_ENTER:
        button->base.m_api->on_touch(self, button->base.m_wnd_rect.m_left, button->base.m_wnd_rect.m_top, TOUCH_DOWN);
        button->base.m_api->on_touch(self, button->base.m_wnd_rect.m_left, button->base.m_wnd_rect.m_top, TOUCH_UP);
        break;
    case NAV_FORWARD:
    case NAV_BACKWARD:
        break;
    }
    return c_wnd_vir_on_navigate(self, key);
}

static const c_wnd_vir_api c_button_vir_api_table = {
        c_wnd_vir_connect,        c_wnd_vir_on_init_children,  c_button_vir_on_paint, c_wnd_vir_show_window,      c_button_vir_on_touch,
        c_button_vir_on_navigate, c_button_vir_pre_create_wnd, c_button_vir_on_focus, c_button_vir_on_kill_focus,
};

void c_button_init(c_button *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_button_vir_api_table;
}
