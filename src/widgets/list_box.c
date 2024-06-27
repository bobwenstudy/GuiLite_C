#include "list_box.h"

void c_list_box_set_on_change(c_list_box *self, WND_CALLBACK on_change)
{
    self->on_change = on_change;
}
short c_list_box_get_item_count(c_list_box *self)
{
    return self->m_item_total;
}

int c_list_box_add_item(c_list_box *self, char *str)
{
    if (self->m_item_total >= MAX_ITEM_NUM)
    {
        GUILITE_ASSERT(false);
        return -1;
    }
    self->m_item_array[self->m_item_total++] = str;
    c_list_box_update_list_size(self);
    return 0;
}
void c_list_box_clear_item(c_list_box *self)
{
    self->m_selected_item = self->m_item_total = 0;
    memset(self->m_item_array, 0, sizeof(self->m_item_array));
    c_list_box_update_list_size(self);
}
void c_list_box_select_item(c_list_box *self, short index)
{
    if (index < 0 || index >= self->m_item_total)
    {
        GUILITE_ASSERT(false);
    }
    self->m_selected_item = index;
}

void c_list_box_vir_pre_create_wnd(c_wnd *self)
{
    c_list_box *list_box = (c_list_box *)self;
    self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
    memset(list_box->m_item_array, 0, sizeof(list_box->m_item_array));
    list_box->m_item_total = 0;
    list_box->m_selected_item = 0;
    self->m_font = c_theme_get_font(FONT_DEFAULT);
    self->m_font_color = c_theme_get_color(COLOR_WND_FONT);
}

void c_list_box_vir_on_paint(c_wnd *self)
{
    c_list_box *list_box = (c_list_box *)self;

    c_rect rect_empty;
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    c_rect_init_empty(&rect_empty);

    switch (self->m_status)
    {
    case STATUS_NORMAL:
        if (self->m_z_order > c_wnd_get_z_order(self->m_parent))
        {
            c_surface_activate_layer(self->m_surface, rect_empty, self->m_z_order); // inactivate the layer of list by empty rect.
            self->m_z_order = c_wnd_get_z_order(self->m_parent);
            self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
        }
        c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_NORMAL), self->m_z_order);
        c_word_draw_string_in_rect(self->m_surface, self->m_z_order, list_box->m_item_array[list_box->m_selected_item], &rect, self->m_font, self->m_font_color,
                                   c_theme_get_color(COLOR_WND_NORMAL), ALIGN_HCENTER | ALIGN_VCENTER);
        break;
    case STATUS_FOCUSED:
        if (self->m_z_order > c_wnd_get_z_order(self->m_parent))
        {
            c_surface_activate_layer(self->m_surface, rect_empty, self->m_z_order); // inactivate the layer of list by empty rect.
            self->m_z_order = c_wnd_get_z_order(self->m_parent);
            self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
        }
        c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_FOCUS), self->m_z_order);
        c_word_draw_string_in_rect(self->m_surface, self->m_z_order, list_box->m_item_array[list_box->m_selected_item], &rect, self->m_font, self->m_font_color,
                                   c_theme_get_color(COLOR_WND_FOCUS), ALIGN_HCENTER | ALIGN_VCENTER);
        break;
    case STATUS_PUSHED:
        c_surface_fill_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_PUSHED), self->m_z_order);
        c_surface_draw_rect_ex(self->m_surface, rect, c_theme_get_color(COLOR_WND_BORDER), 2, self->m_z_order);
        c_word_draw_string_in_rect(self->m_surface, self->m_z_order, list_box->m_item_array[list_box->m_selected_item], &rect, self->m_font,
                                   GL_RGB(2, 124, 165), GL_ARGB(0, 0, 0, 0), ALIGN_HCENTER | ALIGN_VCENTER);
        // draw list
        if (list_box->m_item_total > 0)
        {
            if (self->m_z_order == c_wnd_get_z_order(self->m_parent))
            {
                self->m_z_order++;
                c_surface_activate_layer(self->m_surface, list_box->m_list_screen_rect, self->m_z_order);
                self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS | ATTR_PRIORITY);
            }
            c_list_box_show_list(list_box);
        }
        break;
    default:
        GUILITE_ASSERT(false);
    }
}
void c_list_box_vir_on_focus(c_wnd *self)
{
    self->m_status = STATUS_FOCUSED;
    self->m_api->on_paint(self);
}
void c_list_box_vir_on_kill_focus(c_wnd *self)
{
    self->m_status = STATUS_NORMAL;
    self->m_api->on_paint(self);
}
void c_list_box_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key)
{
    c_list_box *list_box = (c_list_box *)self;
    switch (key)
    {
    case NAV_ENTER:
        if (STATUS_PUSHED == self->m_status)
        {
            if (list_box->on_change)
            {
                list_box->on_change(self->m_parent, self->m_id, list_box->m_selected_item);
            }
        }
        self->m_api->on_touch(self, self->m_wnd_rect.m_left, self->m_wnd_rect.m_top, TOUCH_DOWN);
        self->m_api->on_touch(self, self->m_wnd_rect.m_left, self->m_wnd_rect.m_top, TOUCH_UP);
        return;
    case NAV_BACKWARD:
        if (self->m_status != STATUS_PUSHED)
        {
            return c_wnd_vir_on_navigate(self, key);
        }
        list_box->m_selected_item = (list_box->m_selected_item > 0) ? (list_box->m_selected_item - 1) : list_box->m_selected_item;
        return c_list_box_show_list(list_box);
    case NAV_FORWARD:
        if (self->m_status != STATUS_PUSHED)
        {
            return c_wnd_vir_on_navigate(self, key);
        }
        list_box->m_selected_item = (list_box->m_selected_item < (list_box->m_item_total - 1)) ? (list_box->m_selected_item + 1) : list_box->m_selected_item;
        return c_list_box_show_list(list_box);
    }
}
void c_list_box_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action)
{
    c_list_box *list_box = (c_list_box *)self;
    if (action == TOUCH_DOWN)
    {
        c_list_box_on_touch_down(list_box, x, y);
    }
    else
    {
        c_list_box_on_touch_up(list_box, x, y);
    }
}

void c_list_box_update_list_size(c_list_box *self)
{
    self->m_list_wnd_rect = self->base.m_wnd_rect;
    self->m_list_wnd_rect.m_top = self->base.m_wnd_rect.m_bottom + 1;
    self->m_list_wnd_rect.m_bottom = self->m_list_wnd_rect.m_top + self->m_item_total * ITEM_HEIGHT;

    c_wnd_get_screen_rect(&self->base, &self->m_list_screen_rect);
    self->m_list_screen_rect.m_top = self->m_list_screen_rect.m_bottom + 1;
    self->m_list_screen_rect.m_bottom = self->m_list_screen_rect.m_top + self->m_item_total * ITEM_HEIGHT;
}
void c_list_box_show_list(c_list_box *self)
{
    // draw all items
    c_rect tmp_rect;
    for (int i = 0; i < self->m_item_total; i++)
    {
        tmp_rect.m_left = self->m_list_screen_rect.m_left;
        tmp_rect.m_right = self->m_list_screen_rect.m_right;
        tmp_rect.m_top = self->m_list_screen_rect.m_top + i * ITEM_HEIGHT;
        tmp_rect.m_bottom = tmp_rect.m_top + ITEM_HEIGHT;

        if (self->m_selected_item == i)
        {
            c_surface_fill_rect_ex(self->base.m_surface, tmp_rect, c_theme_get_color(COLOR_WND_FOCUS), self->base.m_z_order);
            c_word_draw_string_in_rect(self->base.m_surface, self->base.m_z_order, self->m_item_array[i], &tmp_rect, self->base.m_font, self->base.m_font_color,
                                       c_theme_get_color(COLOR_WND_FOCUS), ALIGN_HCENTER | ALIGN_VCENTER);
        }
        else
        {
            c_surface_fill_rect_ex(self->base.m_surface, tmp_rect, GL_RGB(17, 17, 17), self->base.m_z_order);
            c_word_draw_string_in_rect(self->base.m_surface, self->base.m_z_order, self->m_item_array[i], &tmp_rect, self->base.m_font, self->base.m_font_color,
                                       GL_RGB(17, 17, 17), ALIGN_HCENTER | ALIGN_VCENTER);
        }
    }
}
void c_list_box_on_touch_down(c_list_box *self, int x, int y)
{
    if (c_rect_pt_in_rect(&self->base.m_wnd_rect, x, y))
    { // click base
        if (STATUS_NORMAL == self->base.m_status)
        {
            c_wnd_set_child_focus(self->base.m_parent, &self->base);
        }
    }
    else if (c_rect_pt_in_rect(&self->m_list_wnd_rect, x, y))
    { // click extend list
        c_wnd_vir_on_touch(&self->base, x, y, TOUCH_DOWN);
    }
    else
    {
        if (STATUS_PUSHED == self->base.m_status)
        {
            self->base.m_status = STATUS_FOCUSED;
            self->base.m_api->on_paint(&self->base);
            if (self->on_change)
            {
                self->on_change(self->base.m_parent, self->base.m_id, self->m_selected_item);
            }
        }
    }
}
void c_list_box_on_touch_up(c_list_box *self, int x, int y)
{
    if (STATUS_FOCUSED == self->base.m_status)
    {
        self->base.m_status = STATUS_PUSHED;
        self->base.m_api->on_paint(&self->base);
    }
    else if (STATUS_PUSHED == self->base.m_status)
    {
        if (c_rect_pt_in_rect(&self->base.m_wnd_rect, x, y))
        { // click base
            self->base.m_status = STATUS_FOCUSED;
            self->base.m_api->on_paint(&self->base);
        }
        else if (c_rect_pt_in_rect(&self->m_list_wnd_rect, x, y))
        { // click extend list
            self->base.m_status = STATUS_FOCUSED;
            c_list_box_select_item(self, (y - self->m_list_wnd_rect.m_top) / ITEM_HEIGHT);
            self->base.m_api->on_paint(&self->base);
            if (self->on_change)
            {
                self->on_change(self->base.m_parent, self->base.m_id, self->m_selected_item);
            }
        }
        else
        {
            c_wnd_vir_on_touch(&self->base, x, y, TOUCH_UP);
        }
    }
}

static const c_wnd_vir_api c_list_box_vir_api_table = {
        c_wnd_vir_connect,          c_wnd_vir_on_init_children,    c_list_box_vir_on_paint, c_wnd_vir_show_window,        c_list_box_vir_on_touch,
        c_list_box_vir_on_navigate, c_list_box_vir_pre_create_wnd, c_list_box_vir_on_focus, c_list_box_vir_on_kill_focus,
};

void c_list_box_init(c_list_box *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_list_box_vir_api_table;
}
