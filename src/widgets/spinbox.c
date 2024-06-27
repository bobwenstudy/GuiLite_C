#include "spinbox.h"

short c_spin_box_get_value(c_spin_box *self)
{
    return self->m_value;
}
void c_spin_box_set_value(c_spin_box *self, unsigned short value)
{
    self->m_value = self->m_cur_value = value;
}
void c_spin_box_set_max_min(c_spin_box *self, short max, short min)
{
    self->m_max = max;
    self->m_min = min;
}
void c_spin_box_set_step(c_spin_box *self, short step)
{
    self->m_step = step;
}
short c_spin_box_get_min(c_spin_box *self)
{
    return self->m_min;
}
short c_spin_box_get_max(c_spin_box *self)
{
    return self->m_max;
}
short c_spin_box_get_step(c_spin_box *self)
{
    return self->m_step;
}
void c_spin_box_set_value_digit(c_spin_box *self, short digit)
{
    self->m_digit = digit;
}
short c_spin_box_get_value_digit(c_spin_box *self)
{
    return self->m_digit;
}
void c_spin_box_set_on_change(c_spin_box *self, WND_CALLBACK on_change)
{
    self->on_change = on_change;
}

void c_spin_box_vir_on_paint(c_wnd *self)
{
    c_spin_box *pin_box = (c_spin_box *)self;
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    rect.m_right = rect.m_left + (c_rect_width(&rect) * 2 / 3);

    c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_NORMAL), self->m_z_order);
    c_word_draw_value_in_rect(self->m_surface, c_wnd_get_z_order(self->m_parent), pin_box->m_cur_value, pin_box->m_digit, &rect, self->m_font,
                              self->m_font_color, c_theme_get_color(COLOR_WND_NORMAL), ALIGN_HCENTER | ALIGN_VCENTER);
}
void c_spin_box_vir_pre_create_wnd(c_wnd *self)
{
    c_spin_box *pin_box = (c_spin_box *)self;
    self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE);
    self->m_font = c_theme_get_font(FONT_DEFAULT);
    self->m_font_color = c_theme_get_color(COLOR_WND_FONT);
    pin_box->m_max = 6;
    pin_box->m_min = 1;
    pin_box->m_digit = 0;
    pin_box->m_step = 1;

    // link arrow button position.
    c_rect rect;
    c_wnd_get_wnd_rect(self, &rect);
    pin_box->m_bt_down.m_spin_box = pin_box->m_bt_up.m_spin_box = pin_box;
    pin_box->m_bt_up.base.base.m_api->connect(&pin_box->m_bt_up.base.base, self->m_parent, ID_BT_ARROW_UP, "+", (rect.m_left + c_rect_width(&rect) * 2 / 3),
                                              rect.m_top, (c_rect_width(&rect) / 3), (c_rect_height(&rect) / 2), NULL);
    pin_box->m_bt_down.base.base.m_api->connect(&pin_box->m_bt_down.base.base, self->m_parent, ID_BT_ARROW_DOWN, "-",
                                                (rect.m_left + c_rect_width(&rect) * 2 / 3), (rect.m_top + c_rect_height(&rect) / 2), (c_rect_width(&rect) / 3),
                                                (c_rect_height(&rect) / 2), NULL);
}
void c_spin_box_on_arrow_up_bt_click(c_spin_box *self)
{
    if (self->m_cur_value + self->m_step > self->m_max)
    {
        return;
    }
    self->m_cur_value += self->m_step;
    if (self->on_change)
    {
        (self->on_change)(self->base.m_parent, self->base.m_id, self->m_cur_value);
    }
    self->base.m_api->on_paint(&self->base);
}
void c_spin_box_on_arrow_down_bt_click(c_spin_box *self)
{
    if (self->m_cur_value - self->m_step < self->m_min)
    {
        return;
    }
    self->m_cur_value -= self->m_step;
    if (self->on_change)
    {
        (self->on_change)(self->base.m_parent, self->base.m_id, self->m_cur_value);
    }
    self->base.m_api->on_paint(&self->base);
}

static const c_wnd_vir_api c_spin_box_vir_api_table = {
        c_wnd_vir_connect,     c_wnd_vir_on_init_children,    c_spin_box_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_spin_box_vir_pre_create_wnd, c_wnd_vir_on_focus,      c_wnd_vir_on_kill_focus,
};

void c_spin_box_init(c_spin_box *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_spin_box_vir_api_table;

    c_spin_button_init(&self->m_bt_up);
    c_spin_button_init(&self->m_bt_down);
}

void c_spin_button_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action)
{
    c_spin_button *spin_button = (c_spin_button *)self;
    if (action == TOUCH_UP)
    {
        (self->m_id == ID_BT_ARROW_UP) ? c_spin_box_on_arrow_up_bt_click(spin_button->m_spin_box) : c_spin_box_on_arrow_down_bt_click(spin_button->m_spin_box);
    }
    c_button_vir_on_touch(self, x, y, action);
}

static const c_wnd_vir_api c_spin_button_vir_api_table = {
        c_wnd_vir_connect,        c_wnd_vir_on_init_children,  c_button_vir_on_paint, c_wnd_vir_show_window,      c_spin_button_vir_on_touch,
        c_button_vir_on_navigate, c_button_vir_pre_create_wnd, c_button_vir_on_focus, c_button_vir_on_kill_focus,
};

void c_spin_button_init(c_spin_button *self)
{
    c_button_init(&self->base);
    self->base.base.m_api = &c_spin_button_vir_api_table;
}
