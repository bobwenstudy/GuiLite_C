#include "keyboard.h"
#include "widgets.h"

int c_keyboard_open_keyboard(c_keyboard *self, c_wnd *user, unsigned short resource_id, KEYBOARD_STYLE style, WND_CALLBACK on_click)
{
    c_rect user_rect;
    c_wnd_get_wnd_rect(user, &user_rect);
    if ((style != STYLE_ALL_BOARD) && (style != STYLE_NUM_BOARD))
    {
        GUILITE_ASSERT(false);
        return -1;
    }
    if (style == STYLE_ALL_BOARD)
    { // Place keyboard at the bottom of user's parent window.
        c_rect user_parent_rect;
        c_wnd_get_wnd_rect(c_wnd_get_parent(user), &user_parent_rect);
        c_wnd_vir_connect(&self->base, user, resource_id, 0, (0 - user_rect.m_left), (c_rect_height(&user_parent_rect) - user_rect.m_top - KEYBOARD_HEIGHT - 1),
                          KEYBOARD_WIDTH, KEYBOARD_HEIGHT, g_key_board_children);
    }
    else if (style == STYLE_NUM_BOARD)
    { // Place keyboard below the user window.
        c_wnd_vir_connect(&self->base, user, resource_id, 0, 0, c_rect_height(&user_rect), NUM_BOARD_WIDTH, NUM_BOARD_HEIGHT, g_number_board_children);
    }

    self->m_on_click = on_click;
    c_rect rc;
    c_wnd_get_screen_rect(&self->base, &rc);
    c_surface_activate_layer(self->base.m_surface, rc, self->base.m_z_order);
    self->base.m_api->show_window(&self->base);
    return 0;
}

void c_keyboard_close_keyboard(c_keyboard *self)
{
    c_wnd_disconnect(&self->base);
    c_rect rc;
    c_rect_init_empty(&rc);
    c_surface_activate_layer(self->base.m_surface, rc, self->base.m_z_order); // inactivate the layer of keyboard by empty rect.
}

void c_keyboard_vir_on_init_children(c_wnd *self)
{
    c_wnd *child = self->m_top_child;
    if (0 != child)
    {
        while (child)
        {
            c_button_set_on_click((c_button *)child, c_keyboard_on_key_clicked);
            child = c_wnd_get_next_sibling(child);
        }
    }
}

KEYBOARD_STATUS c_keyboard_get_cap_status(c_keyboard *self)
{
    return self->m_cap_status;
}
char *c_keyboard_get_str(c_keyboard *self)
{
    return self->m_str;
}

void c_keyboard_vir_pre_create_wnd(c_wnd *self)
{
    c_keyboard *keyboard = (c_keyboard *)self;
    self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS | ATTR_PRIORITY);
    keyboard->m_cap_status = STATUS_UPPERCASE;
    self->m_z_order = c_surface_get_max_z_order(self->m_surface);
    memset(keyboard->m_str, 0, sizeof(keyboard->m_str));
    keyboard->m_str_len = 0;
}

void c_keyboard_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    c_surface_fill_rect_ex(self->m_surface, rect, GL_RGB(0, 0, 0), self->m_z_order);
}

void c_keyboard_on_key_clicked(c_wnd *self, int id, int param)
{
    c_keyboard *keyboard = (c_keyboard *)self;
    switch (id)
    {
    case 0x14:
        c_keyboard_on_caps_clicked(keyboard, id, param);
        break;
    case '\n':
        c_keyboard_on_enter_clicked(keyboard, id, param);
        break;
    case 0x1B:
        c_keyboard_on_esc_clicked(keyboard, id, param);
        break;
    case 0x7F:
        c_keyboard_on_del_clicked(keyboard, id, param);
        break;
    default:
        c_keyboard_on_char_clicked(keyboard, id, param);
        break;
    }
}
void c_keyboard_on_char_clicked(c_keyboard *self, int id, int param)
{ // id = char ascii code.
    if (self->m_str_len >= sizeof(self->m_str))
    {
        return;
    }
    if ((id >= '0' && id <= '9') || id == ' ' || id == '.')
    {
        goto InputChar;
    }

    if (id >= 'A' && id <= 'Z')
    {
        if (STATUS_LOWERCASE == self->m_cap_status)
        {
            id += 0x20;
        }
        goto InputChar;
    }
    if (id == 0x90)
        return; // TBD
    GUILITE_ASSERT(false);
InputChar:
    self->m_str[self->m_str_len++] = id;
    self->m_on_click(self->base.m_parent, self->base.m_id, CLICK_CHAR);
}
void c_keyboard_on_del_clicked(c_keyboard *self, int id, int param)
{
    if (self->m_str_len <= 0)
    {
        return;
    }
    self->m_str[--self->m_str_len] = 0;
    self->m_on_click(self->base.m_parent, self->base.m_id, CLICK_CHAR);
}
void c_keyboard_on_caps_clicked(c_keyboard *self, int id, int param)
{
    self->m_cap_status = (self->m_cap_status == STATUS_LOWERCASE) ? STATUS_UPPERCASE : STATUS_LOWERCASE;
    self->base.m_api->show_window(&self->base);
}
void c_keyboard_on_enter_clicked(c_keyboard *self, int id, int param)
{
    memset(self->m_str, 0, sizeof(self->m_str));
    self->m_on_click(self->base.m_parent, self->base.m_id, CLICK_ENTER);
}
void c_keyboard_on_esc_clicked(c_keyboard *self, int id, int param)
{
    memset(self->m_str, 0, sizeof(self->m_str));
    self->m_on_click(self->base.m_parent, self->base.m_id, CLICK_ESC);
}

static const c_wnd_vir_api c_keyboard_vir_api_table = {
        c_wnd_vir_connect,     c_keyboard_vir_on_init_children, c_keyboard_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_keyboard_vir_pre_create_wnd,   c_wnd_vir_on_focus,      c_wnd_vir_on_kill_focus,
};

void c_keyboard_init(c_keyboard *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_keyboard_vir_api_table;
    self->base.m_attr = 0;
}

void c_keyboard_button_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    switch (self->m_status)
    {
    case STATUS_NORMAL:
        c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_NORMAL), self->m_z_order);
        break;
    case STATUS_FOCUSED:
        c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_FOCUS), self->m_z_order);
        break;
    case STATUS_PUSHED:
        c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_PUSHED), self->m_z_order);
        c_surface_draw_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_BORDER), 2, self->m_z_order);
        break;
    default:
        GUILITE_ASSERT(false);
        break;
    }

    if (self->m_id == 0x14)
    {
        return c_word_draw_string_in_rect(self->m_surface, self->m_z_order, "Caps", &rect, self->m_font, self->m_font_color, GL_ARGB(0, 0, 0, 0),
                                          ALIGN_HCENTER);
    }
    else if (self->m_id == 0x1B)
    {
        return c_word_draw_string_in_rect(self->m_surface, self->m_z_order, "Esc", &rect, self->m_font, self->m_font_color, GL_ARGB(0, 0, 0, 0), ALIGN_HCENTER);
    }
    else if (self->m_id == ' ')
    {
        return c_word_draw_string_in_rect(self->m_surface, self->m_z_order, "Space", &rect, self->m_font, self->m_font_color, GL_ARGB(0, 0, 0, 0),
                                          ALIGN_HCENTER);
    }
    else if (self->m_id == '\n')
    {
        return c_word_draw_string_in_rect(self->m_surface, self->m_z_order, "Enter", &rect, self->m_font, self->m_font_color, GL_ARGB(0, 0, 0, 0),
                                          ALIGN_HCENTER);
    }
    else if (self->m_id == '.')
    {
        return c_word_draw_string_in_rect(self->m_surface, self->m_z_order, ".", &rect, self->m_font, self->m_font_color, GL_ARGB(0, 0, 0, 0), ALIGN_HCENTER);
    }
    else if (self->m_id == 0x7F)
    {
        return c_word_draw_string_in_rect(self->m_surface, self->m_z_order, "Back", &rect, self->m_font, self->m_font_color, GL_ARGB(0, 0, 0, 0),
                                          ALIGN_HCENTER);
    }
    else if (self->m_id == 0x90)
    {
        return c_word_draw_string_in_rect(self->m_surface, self->m_z_order, "?123", &rect, self->m_font, self->m_font_color, GL_ARGB(0, 0, 0, 0),
                                          ALIGN_HCENTER);
    }

    char letter[] = {0, 0};
    if (self->m_id >= 'A' && self->m_id <= 'Z')
    {
        letter[0] = (c_keyboard_get_cap_status((c_keyboard *)self->m_parent) == STATUS_UPPERCASE) ? self->m_id : (self->m_id + 0x20);
    }
    else if (self->m_id >= '0' && self->m_id <= '9')
    {
        letter[0] = (char)self->m_id;
    }
    c_word_draw_string_in_rect(self->m_surface, self->m_z_order, letter, &rect, self->m_font, self->m_font_color, GL_ARGB(0, 0, 0, 0), ALIGN_HCENTER);
}

static const c_wnd_vir_api c_keyboard_button_vir_api_table = {
        c_wnd_vir_connect,        c_wnd_vir_on_init_children,  c_keyboard_button_vir_on_paint, c_wnd_vir_show_window,      c_button_vir_on_touch,
        c_button_vir_on_navigate, c_button_vir_pre_create_wnd, c_button_vir_on_focus,          c_button_vir_on_kill_focus,
};

void c_keyboard_button_init(c_keyboard_button *self)
{
    c_button_init(&self->base);
    self->base.base.m_api = &c_keyboard_button_vir_api_table;
}
