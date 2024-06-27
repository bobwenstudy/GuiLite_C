#include "dialog.h"
#include "widgets.h"

int c_dialog_open_dialog(c_dialog *self, c_dialog *p_dlg, bool modal_mode)
{
    if (0 == p_dlg)
    {
        GUILITE_ASSERT(false);
        return 0;
    }
    c_dialog *cur_dlg = c_dialog_get_the_dialog(self, c_wnd_get_surface(&p_dlg->base));
    if (cur_dlg == p_dlg)
    {
        return 1;
    }

    if (cur_dlg)
    {
        c_wnd_set_attr(&cur_dlg->base, 0);
    }

    c_rect rc;
    c_wnd_get_screen_rect(&p_dlg->base, &rc);
    c_surface_activate_layer(c_wnd_get_surface(&p_dlg->base), rc, p_dlg->base.m_z_order);

    c_wnd_set_attr(&p_dlg->base, modal_mode ? (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS | ATTR_PRIORITY) : (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS));
    p_dlg->base.m_api->show_window(&p_dlg->base);
    c_dialog_set_me_the_dialog(p_dlg);
    return 1;
}

int c_dialog_open_dialog_simple(c_dialog *self, c_dialog *p_dlg)
{
    return c_dialog_open_dialog(self, p_dlg, true);
}

int c_dialog_close_dialog(c_dialog *self, c_surface *surface)
{
    c_dialog *dlg = c_dialog_get_the_dialog(self, surface);
    c_rect rect;
    c_rect_init_empty(&rect);

    if (0 == dlg)
    {
        return 0;
    }

    c_wnd_set_attr(&dlg->base, 0);
    c_surface_activate_layer(surface, rect, dlg->base.m_z_order); // inactivate the layer of dialog by empty rect.

    // clear the dialog
    for (int i = 0; i < SURFACE_CNT_MAX; i++)
    {
        if (c_dialog_ms_the_dialogs[i].surface == surface)
        {
            c_dialog_ms_the_dialogs[i].dialog = 0;
            return 1;
        }
    }
    GUILITE_ASSERT(false);
    return -1;
}

c_dialog *c_dialog_get_the_dialog(c_dialog *self, c_surface *surface)
{
    for (int i = 0; i < SURFACE_CNT_MAX; i++)
    {
        if (c_dialog_ms_the_dialogs[i].surface == surface)
        {
            return c_dialog_ms_the_dialogs[i].dialog;
        }
    }
    return 0;
}

void c_dialog_vir_pre_create_wnd(c_wnd *self)
{
    self->m_attr = 0; // no focus/visible
    self->m_z_order = Z_ORDER_LEVEL_1;
    self->m_bg_color = GL_RGB(33, 42, 53);
}
void c_dialog_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    c_surface_fill_rect_ex(self->m_surface, rect, self->m_bg_color, self->m_z_order);

    if (self->m_str)
    {
        c_word_draw_string(self->m_surface, self->m_z_order, self->m_str, rect.m_left + 35, rect.m_top, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255),
                           GL_ARGB(0, 0, 0, 0));
    }
}

int c_dialog_set_me_the_dialog(c_dialog *self)
{
    c_surface *surface = c_wnd_get_surface(&self->base);
    for (int i = 0; i < SURFACE_CNT_MAX; i++)
    {
        if (c_dialog_ms_the_dialogs[i].surface == surface)
        {
            c_dialog_ms_the_dialogs[i].dialog = self;
            return 0;
        }
    }

    for (int i = 0; i < SURFACE_CNT_MAX; i++)
    {
        if (c_dialog_ms_the_dialogs[i].surface == 0)
        {
            c_dialog_ms_the_dialogs[i].dialog = self;
            c_dialog_ms_the_dialogs[i].surface = surface;
            return 1;
        }
    }
    GUILITE_ASSERT(false);
    return -2;
}

static const c_wnd_vir_api c_dialog_vir_api_table = {
        c_wnd_vir_connect,     c_wnd_vir_on_init_children,  c_dialog_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_dialog_vir_pre_create_wnd, c_wnd_vir_on_focus,    c_wnd_vir_on_kill_focus,
};

void c_dialog_init(c_dialog *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_dialog_vir_api_table;
}
