#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "api.h"
#include "display.h"
#include "wnd.h"
#include "word.h"

int c_wnd_vir_connect(c_wnd *self, c_wnd *parent, unsigned short resource_id, const char *str, short x, short y, short width, short height,
                      WND_TREE *p_child_tree)
{
    if (0 == resource_id)
    {
        GUILITE_ASSERT(false);
        return -1;
    }

    self->m_id = resource_id;
    c_wnd_set_str(self, str);
    self->m_parent = parent;
    self->m_status = STATUS_NORMAL;

    if (parent)
    {
        self->m_z_order = parent->m_z_order;
        self->m_surface = parent->m_surface;
    }
    if (0 == self->m_surface)
    {
        GUILITE_ASSERT(false);
        return -2;
    }

    /* (cs.x = x * 1024 / 768) for 1027*768=>800*600 quickly*/
    self->m_wnd_rect.m_left = x;
    self->m_wnd_rect.m_top = y;
    self->m_wnd_rect.m_right = (x + width - 1);
    self->m_wnd_rect.m_bottom = (y + height - 1);

    self->m_api->pre_create_wnd(self);

    if (0 != parent)
    {
        c_wnd_add_child_2_tail(parent, self);
    }

    if (c_wnd_load_child_wnd(self, p_child_tree) >= 0)
    {
        self->m_api->on_init_children(self);
    }

    return 0;
}

void c_wnd_disconnect(c_wnd *self)
{
    if (0 != self->m_top_child)
    {
        c_wnd *child = self->m_top_child;
        c_wnd *next_child = 0;

        while (child)
        {
            next_child = child->m_next_sibling;
            c_wnd_disconnect(child);
            child = next_child;
        }
    }

    if (0 != self->m_parent)
    {
        c_wnd_unlink_child(self->m_parent, self);
    }
    self->m_focus_child = 0;
    self->m_attr = 0;
}

void c_wnd_vir_on_init_children(c_wnd *self)
{
}

void c_wnd_vir_on_paint(c_wnd *self)
{
}

void c_wnd_vir_show_window(c_wnd *self)
{
    if (ATTR_VISIBLE == (self->m_attr & ATTR_VISIBLE))
    {
        self->m_api->on_paint(self);
        c_wnd *child = self->m_top_child;
        if (0 != child)
        {
            while (child)
            {
                child->m_api->show_window(child);
                child = child->m_next_sibling;
            }
        }
    }
}

unsigned short c_wnd_get_id(const c_wnd *self)
{
    return self->m_id;
}

int c_wnd_get_z_order(c_wnd *self)
{
    return self->m_z_order;
}

c_wnd *c_wnd_get_wnd_ptr(const c_wnd *self, unsigned short id)
{
    c_wnd *child = self->m_top_child;

    while (child)
    {
        if (c_wnd_get_id(child) == id)
        {
            break;
        }

        child = child->m_next_sibling;
    }

    return child;
}

unsigned int c_wnd_get_attr(const c_wnd *self)
{
    return self->m_attr;
}

void c_wnd_set_str(c_wnd *self, const char *str)
{
    self->m_str = str;
}

void c_wnd_set_attr(c_wnd *self, WND_ATTRIBUTION attr)
{
    self->m_attr = attr;
}

bool c_wnd_is_focus_wnd(const c_wnd *self)
{
    return ((self->m_attr & ATTR_VISIBLE) && (self->m_attr & ATTR_FOCUS)) ? true : false;
}

void c_wnd_set_font_color(c_wnd *self, unsigned int color)
{
    self->m_font_color = color;
}

unsigned int c_wnd_get_font_color(c_wnd *self)
{
    return self->m_font_color;
}

void c_wnd_set_bg_color(c_wnd *self, unsigned int color)
{
    self->m_bg_color = color;
}

unsigned int c_wnd_get_bg_color(c_wnd *self)
{
    return self->m_bg_color;
}

void c_wnd_set_font_type(c_wnd *self, const LATTICE_FONT_INFO *font_type)
{
    self->m_font = font_type;
}

const void *c_wnd_get_font_type(c_wnd *self)
{
    return self->m_font;
}

void c_wnd_get_wnd_rect(const c_wnd *self, c_rect *rect)
{
    *rect = self->m_wnd_rect;
}

void c_wnd_get_screen_rect(const c_wnd *self, c_rect *rect)
{
    int l = 0;
    int t = 0;
    c_wnd_wnd2screen(self, &l, &t);
    c_rect_set_rect(rect, l, t, c_rect_width(&self->m_wnd_rect), c_rect_height(&self->m_wnd_rect));
}

c_wnd *c_wnd_set_child_focus(c_wnd *self, c_wnd *focus_child)
{
    GUILITE_ASSERT(0 != focus_child);
    GUILITE_ASSERT(focus_child->m_parent == self);

    c_wnd *old_focus_child = self->m_focus_child;
    if (c_wnd_is_focus_wnd(focus_child))
    {
        if (focus_child != old_focus_child)
        {
            if (old_focus_child)
            {
                old_focus_child->m_api->on_kill_focus(old_focus_child);
            }
            self->m_focus_child = focus_child;
            self->m_focus_child->m_api->on_focus(self->m_focus_child);
        }
    }
    return self->m_focus_child;
}

c_wnd *c_wnd_get_parent(const c_wnd *self)
{
    return self->m_parent;
}

c_wnd *c_wnd_get_last_child(const c_wnd *self)
{
    if (0 == self->m_top_child)
    {
        return 0;
    }

    c_wnd *child = self->m_top_child;

    while (child->m_next_sibling)
    {
        child = child->m_next_sibling;
    }

    return child;
}

int c_wnd_unlink_child(c_wnd *self, c_wnd *child)
{
    if ((0 == child) || (self != child->m_parent))
    {
        return -1;
    }

    if (0 == self->m_top_child)
    {
        return -2;
    }

    bool find = false;

    c_wnd *tmp_child = self->m_top_child;
    if (tmp_child == child)
    {
        self->m_top_child = child->m_next_sibling;
        if (0 != child->m_next_sibling)
        {
            child->m_next_sibling->m_prev_sibling = 0;
        }

        find = true;
    }
    else
    {
        while (tmp_child->m_next_sibling)
        {
            if (child == tmp_child->m_next_sibling)
            {
                tmp_child->m_next_sibling = child->m_next_sibling;
                if (0 != child->m_next_sibling)
                {
                    child->m_next_sibling->m_prev_sibling = tmp_child;
                }

                find = true;
                break;
            }

            tmp_child = tmp_child->m_next_sibling;
        }
    }

    if (true == find)
    {
        if (self->m_focus_child == child)
        {
            self->m_focus_child = 0;
        }

        child->m_next_sibling = 0;
        child->m_prev_sibling = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

c_wnd *c_wnd_get_prev_sibling(const c_wnd *self)
{
    return self->m_prev_sibling;
}

c_wnd *c_wnd_get_next_sibling(const c_wnd *self)
{
    return self->m_next_sibling;
}

c_wnd *c_wnd_search_priority_sibling(c_wnd *self, c_wnd *root)
{
    c_wnd *priority_wnd = 0;
    while (root)
    {
        if ((root->m_attr & ATTR_PRIORITY) && (root->m_attr & ATTR_VISIBLE))
        {
            priority_wnd = root;
            break;
        }
        root = root->m_next_sibling;
    }

    return priority_wnd;
}

void c_wnd_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action)
{
    x -= self->m_wnd_rect.m_left;
    y -= self->m_wnd_rect.m_top;

    c_wnd *priority_wnd = c_wnd_search_priority_sibling(self, self->m_top_child);
    if (priority_wnd)
    {
        return priority_wnd->m_api->on_touch(priority_wnd, x, y, action);
    }

    c_wnd *child = self->m_top_child;
    while (child)
    {
        if (c_wnd_is_focus_wnd(child))
        {
            c_rect rect;
            c_wnd_get_wnd_rect(child, &rect);
            if (true == c_rect_pt_in_rect(&rect, x, y))
            {
                return child->m_api->on_touch(child, x, y, action);
            }
        }
        child = child->m_next_sibling;
    }
}

void c_wnd_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key)
{
    c_wnd *priority_wnd = c_wnd_search_priority_sibling(self, self->m_top_child);
    if (priority_wnd)
    {
        return priority_wnd->m_api->on_navigate(priority_wnd, key);
    }

    if (!c_wnd_is_focus_wnd(self))
    {
        return;
    }
    if (key != NAV_BACKWARD && key != NAV_FORWARD)
    {
        if (self->m_focus_child)
        {
            self->m_focus_child->m_api->on_navigate(self->m_focus_child, key);
        }
        return;
    }

    // Move focus
    c_wnd *old_focus_wnd = self->m_focus_child;
    // No current focus self, new one.
    if (!old_focus_wnd)
    {
        c_wnd *child = self->m_top_child;
        c_wnd *new_focus_wnd = 0;
        while (child)
        {
            if (c_wnd_is_focus_wnd(child))
            {
                new_focus_wnd = child;
                c_wnd_set_child_focus(new_focus_wnd->m_parent, new_focus_wnd);
                child = child->m_top_child;
                continue;
            }
            child = child->m_next_sibling;
        }
        return;
    }
    // Move focus from old self to next self
    c_wnd *next_focus_wnd = (key == NAV_FORWARD) ? old_focus_wnd->m_next_sibling : old_focus_wnd->m_prev_sibling;
    while (next_focus_wnd && (!c_wnd_is_focus_wnd(next_focus_wnd)))
    { // Search neighbor of old focus self
        next_focus_wnd = (key == NAV_FORWARD) ? next_focus_wnd->m_next_sibling : next_focus_wnd->m_prev_sibling;
    }
    if (!next_focus_wnd)
    { // Search whole brother self
        next_focus_wnd = (key == NAV_FORWARD) ? old_focus_wnd->m_parent->m_top_child : c_wnd_get_last_child(old_focus_wnd->m_parent);
        while (next_focus_wnd && (!c_wnd_is_focus_wnd(next_focus_wnd)))
        {
            next_focus_wnd = (key == NAV_FORWARD) ? next_focus_wnd->m_next_sibling : next_focus_wnd->m_prev_sibling;
        }
    }
    if (next_focus_wnd)
    {
        c_wnd_set_child_focus(next_focus_wnd->m_parent, next_focus_wnd);
    }
}

c_surface *c_wnd_get_surface(c_wnd *self)
{
    return self->m_surface;
}

void c_wnd_set_surface(c_wnd *self, c_surface *surface)
{
    self->m_surface = surface;
}

void c_wnd_vir_pre_create_wnd(c_wnd *self)
{
}

void c_wnd_add_child_2_tail(c_wnd *self, c_wnd *child)
{
    if (0 == child)
        return;
    if (child == c_wnd_get_wnd_ptr(self, child->m_id))
        return;

    if (0 == self->m_top_child)
    {
        self->m_top_child = child;
        child->m_prev_sibling = 0;
        child->m_next_sibling = 0;
    }
    else
    {
        c_wnd *last_child = c_wnd_get_last_child(self);
        if (0 == last_child)
        {
            GUILITE_ASSERT(false);
        }
        last_child->m_next_sibling = child;
        child->m_prev_sibling = last_child;
        child->m_next_sibling = 0;
    }
}

void c_wnd_wnd2screen(const c_wnd *self, int *x, int *y)
{
    c_wnd *parent = self->m_parent;
    c_rect rect;

    *x += self->m_wnd_rect.m_left;
    *y += self->m_wnd_rect.m_top;

    while (0 != parent)
    {
        c_wnd_get_wnd_rect(parent, &rect);
        *x += rect.m_left;
        *y += rect.m_top;

        parent = parent->m_parent;
    }
}

int c_wnd_load_child_wnd(c_wnd *self, WND_TREE *p_child_tree)
{
    if (0 == p_child_tree)
    {
        return 0;
    }
    int sum = 0;

    WND_TREE *p_cur = p_child_tree;
    while (p_cur->p_wnd)
    {
        p_cur->p_wnd->m_api->connect(p_cur->p_wnd, self, p_cur->resource_id, p_cur->str, p_cur->x, p_cur->y, p_cur->width, p_cur->height, p_cur->p_child_tree);
        p_cur++;
        sum++;
    }
    return sum;
}
void c_wnd_set_active_child(c_wnd *self, c_wnd *child)
{
    self->m_focus_child = child;
}

void c_wnd_vir_on_focus(c_wnd *self)
{
}
void c_wnd_vir_on_kill_focus(c_wnd *self)
{
}

static const c_wnd_vir_api c_wnd_vir_api_table = {
        c_wnd_vir_connect,     c_wnd_vir_on_init_children, c_wnd_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_wnd_vir_pre_create_wnd,   c_wnd_vir_on_focus, c_wnd_vir_on_kill_focus,
};

void c_wnd_init(c_wnd *self)
{
    self->m_status = STATUS_NORMAL;
    self->m_attr = (WND_ATTRIBUTION)(ATTR_VISIBLE | ATTR_FOCUS);
    self->m_parent = NULL;
    self->m_top_child = NULL;

    self->m_prev_sibling = NULL;
    self->m_next_sibling = NULL;

    self->m_str = NULL;
    self->m_font_color = 0;
    self->m_bg_color = 0;

    self->m_id = 0;
    self->m_z_order = Z_ORDER_LEVEL_0;
    self->m_focus_child = NULL;
    self->m_surface = NULL;

    self->m_api = &c_wnd_vir_api_table;
}
