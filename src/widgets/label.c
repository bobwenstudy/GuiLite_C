#include "label.h"

void c_label_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    unsigned int bg_color = self->m_bg_color ? self->m_bg_color : c_wnd_get_bg_color(self->m_parent);
    c_wnd_get_screen_rect(self, &rect);
    if (self->m_str)
    {
        self->m_surface->m_api->fill_rect(self->m_surface, rect.m_left, rect.m_top, rect.m_right, rect.m_bottom, bg_color, self->m_z_order);
        c_word_draw_string_in_rect(self->m_surface, self->m_z_order, self->m_str, &rect, self->m_font, self->m_font_color, bg_color,
                                   ALIGN_LEFT | ALIGN_VCENTER);
    }
}

void c_label_vir_pre_create_wnd(c_wnd *self)
{
    self->m_attr = ATTR_VISIBLE;
    self->m_font_color = c_theme_get_color(COLOR_WND_FONT);
    self->m_font = c_theme_get_font(FONT_DEFAULT);
}

static const c_wnd_vir_api c_label_vir_api_table = {
        c_wnd_vir_connect,     c_wnd_vir_on_init_children, c_label_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_label_vir_pre_create_wnd, c_wnd_vir_on_focus,   c_wnd_vir_on_kill_focus,
};

void c_label_init(c_label *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_label_vir_api_table;
}
