#include "slide_group.h"

int c_slide_group_set_active_slide(c_slide_group *self, int index, bool is_redraw)
{
    if (index >= MAX_PAGES || index < 0)
    {
        return -1;
    }
    if (0 == self->m_slides[index])
    {
        return -2;
    }
    self->m_active_slide_index = index;

    for (int i = 0; i < MAX_PAGES; i++)
    {
        if (self->m_slides[i] == 0)
        {
            continue;
        }
        if (i == index)
        {
            c_surface_set_active(c_wnd_get_surface(self->m_slides[i]), true);
            c_wnd_add_child_2_tail(&self->base, self->m_slides[i]);
            if (is_redraw)
            {
                c_rect rc;
                c_wnd_get_screen_rect(&self->base, &rc);
                c_surface_flush_screen(c_wnd_get_surface(self->m_slides[i]), rc.m_left, rc.m_top, rc.m_right, rc.m_bottom);
            }
        }
        else
        {
            c_surface_set_active(c_wnd_get_surface(self->m_slides[i]), false);
        }
    }
    return 0;
}
int c_slide_group_set_active_slide_simple(c_slide_group *self, int index)
{
    return c_slide_group_set_active_slide(self, index, true);
}
c_wnd *c_slide_group_get_slide(c_slide_group *self, int index)
{
    return self->m_slides[index];
}
c_wnd *c_slide_group_get_active_slide(c_slide_group *self)
{
    return self->m_slides[self->m_active_slide_index];
}
int c_slide_group_get_active_slide_index(c_slide_group *self)
{
    return self->m_active_slide_index;
}
int c_slide_group_add_slide(c_slide_group *self, c_wnd *slide, unsigned short resource_id, short x, short y, short width, short height, WND_TREE *p_child_tree,
                            Z_ORDER_LEVEL max_zorder)
{
    if (0 == slide)
    {
        return -1;
    }

    c_surface *old_surface = c_wnd_get_surface(&self->base);
    c_surface *new_surface = c_display_alloc_surface_simple(c_surface_get_display(old_surface), max_zorder);
    c_surface_set_active(new_surface, false);
    c_wnd_set_surface(&self->base, new_surface);
    slide->m_api->connect(slide, &self->base, resource_id, 0, x, y, width, height, p_child_tree);
    c_wnd_set_surface(&self->base, old_surface);

    int i = 0;
    while (i < MAX_PAGES)
    {
        if (self->m_slides[i] == slide)
        { // slide has lived
            GUILITE_ASSERT(false);
            return -2;
        }
        i++;
    }

    // new slide
    i = 0;
    while (i < MAX_PAGES)
    {
        if (self->m_slides[i] == 0)
        {
            self->m_slides[i] = slide;
            slide->m_api->show_window(slide);
            return 0;
        }
        i++;
    }

    // no more slide can be add
    GUILITE_ASSERT(false);
    return -3;
}

int c_slide_group_add_slide_with_child_tree(c_slide_group *self, c_wnd *slide, unsigned short resource_id, short x, short y, short width, short height,
                                            WND_TREE *p_child_tree)
{
    return c_slide_group_add_slide(self, slide, resource_id, x, y, width, height, p_child_tree, Z_ORDER_LEVEL_0);
}
int c_slide_group_add_slide_simple(c_slide_group *self, c_wnd *slide, unsigned short resource_id, short x, short y, short width, short height)
{
    return c_slide_group_add_slide_with_child_tree(self, slide, resource_id, x, y, width, height, 0);
}
void c_slide_group_disabel_all_slide(c_slide_group *self)
{
    for (int i = 0; i < MAX_PAGES; i++)
    {
        if (self->m_slides[i])
        {
            c_surface_set_active(c_wnd_get_surface(self->m_slides[i]), false);
        }
    }
}

void c_slide_group_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key)
{
    c_slide_group *slide_group = (c_slide_group *)self;
    c_wnd *select_wnd = slide_group->m_slides[slide_group->m_active_slide_index];
    if (select_wnd)
    {
        select_wnd->m_api->on_navigate(select_wnd, key);
    }
}

void c_slide_group_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action)
{
    c_slide_group *slide_group = (c_slide_group *)self;
    x -= self->m_wnd_rect.m_left;
    y -= self->m_wnd_rect.m_top;
    if (c_gesture_handle_swipe(&slide_group->m_gesture, x, y, action))
    {
        c_wnd *slide = slide_group->m_slides[slide_group->m_active_slide_index];
        if (slide)
        {
            slide->m_api->on_touch(slide, x, y, action);
        }
    }
}

static const c_wnd_vir_api c_slide_group_vir_api_table = {
        c_wnd_vir_connect,        c_wnd_vir_on_init_children, c_wnd_vir_on_paint,
        c_wnd_vir_show_window,    c_slide_group_vir_on_touch, c_slide_group_vir_on_navigate,
        c_wnd_vir_pre_create_wnd, c_wnd_vir_on_focus,         c_wnd_vir_on_kill_focus,
};

void c_slide_group_init(c_slide_group *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_slide_group_vir_api_table;

    c_gesture_init(&self->m_gesture, self);
    for (int i = 0; i < MAX_PAGES; i++)
    {
        self->m_slides[i] = 0;
    }
    self->m_active_slide_index = 0;
}

void c_gesture_init(c_gesture *self, c_slide_group *group)
{
    self->m_slide_group = group;
    self->m_state = TOUCH_IDLE;
    self->m_down_x = self->m_down_y = self->m_move_x = self->m_move_y = 0;
}
bool c_gesture_handle_swipe(c_gesture *self, int x, int y, TOUCH_ACTION action)
{
    if (action == TOUCH_DOWN) // MOUSE_LBUTTONDOWN
    {
        if (self->m_state == TOUCH_IDLE)
        {
            self->m_state = TOUCH_MOVE;
            self->m_move_x = self->m_down_x = x;
            return true;
        }
        else // TOUCH_MOVE
        {
            return c_gesture_on_move(self, x);
        }
    }
    else if (action == TOUCH_UP) // MOUSE_LBUTTONUP
    {
        if (self->m_state == TOUCH_MOVE)
        {
            self->m_state = TOUCH_IDLE;
            return c_gesture_on_swipe(self, x);
        }
        else
        {
            return false;
            // GUILITE_ASSERT(false);
        }
    }
    return true;
}

bool c_gesture_on_move(c_gesture *self, int x)
{
    if (self->m_slide_group == 0)
    {
        return true;
    }
    if (abs(x - self->m_move_x) < MOVE_THRESHOLD)
    {
        return false;
    }

    c_slide_group_disabel_all_slide(self->m_slide_group);
    self->m_move_x = x;
    if ((self->m_move_x - self->m_down_x) > 0)
    {
        c_gesture_move_right(self);
    }
    else
    {
        c_gesture_move_left(self);
    }
    return false;
}
bool c_gesture_on_swipe(c_gesture *self, int x)
{
    if (self->m_slide_group == 0)
    {
        return true;
    }
    if ((self->m_down_x == self->m_move_x) && (abs(x - self->m_down_x) < MOVE_THRESHOLD))
    {
        return true;
    }

    c_slide_group_disabel_all_slide(self->m_slide_group);
    int page = -1;
    self->m_move_x = x;
    if ((self->m_move_x - self->m_down_x) > 0)
    {
        page = c_gesture_swipe_right(self);
    }
    else
    {
        page = c_gesture_swipe_left(self);
    }
    if (page >= 0)
    {
        c_slide_group_set_active_slide_simple(self->m_slide_group, page);
    }
    else
    {
        c_slide_group_set_active_slide(self->m_slide_group, c_slide_group_get_active_slide_index(self->m_slide_group), false);
    }
    return false;
}
int c_gesture_swipe_left(c_gesture *self)
{
    if (self->m_slide_group == 0)
    {
        return -1;
    }
    int index = c_slide_group_get_active_slide_index(self->m_slide_group);
    if ((index + 1) >= MAX_PAGES || c_slide_group_get_slide(self->m_slide_group, index + 1) == 0 || c_slide_group_get_slide(self->m_slide_group, index) == 0)
    {
        return -2;
    }
    c_surface *s1 = c_wnd_get_surface(c_slide_group_get_slide(self->m_slide_group, index + 1));
    c_surface *s2 = c_wnd_get_surface(c_slide_group_get_slide(self->m_slide_group, index));
    if (c_surface_get_display(s1) != c_surface_get_display(s2))
    {
        return -3;
    }

    int step = self->m_down_x - self->m_move_x;
    c_rect rc;
    c_wnd_get_screen_rect(&self->m_slide_group->base, &rc);
    while (step < c_rect_width(&rc))
    {
        c_display_swipe_surface(c_surface_get_display(s1), s2, s1, rc.m_left, rc.m_right, rc.m_top, rc.m_bottom, step);
        step += SWIPE_STEP;
    }
    if (step != c_rect_width(&rc))
    {
        c_display_swipe_surface(c_surface_get_display(s1), s2, s1, rc.m_left, rc.m_right, rc.m_top, rc.m_bottom, c_rect_width(&rc));
    }
    return (index + 1);
}

int c_gesture_swipe_right(c_gesture *self)
{
    if (self->m_slide_group == 0)
    {
        return -1;
    }
    int index = c_slide_group_get_active_slide_index(self->m_slide_group);
    if (index <= 0 || c_slide_group_get_slide(self->m_slide_group, index - 1) == 0 || c_slide_group_get_slide(self->m_slide_group, index) == 0)
    {
        return -2;
    }
    c_surface *s1 = c_wnd_get_surface(c_slide_group_get_slide(self->m_slide_group, index - 1));
    c_surface *s2 = c_wnd_get_surface(c_slide_group_get_slide(self->m_slide_group, index));
    if (c_surface_get_display(s1) != c_surface_get_display(s2))
    {
        return -3;
    }

    c_rect rc;
    c_wnd_get_screen_rect(&self->m_slide_group->base, &rc);
    int step = c_rect_width(&rc) - (self->m_move_x - self->m_down_x);
    while (step > 0)
    {
        c_display_swipe_surface(c_surface_get_display(s1), s1, s2, rc.m_left, rc.m_right, rc.m_top, rc.m_bottom, step);
        step -= SWIPE_STEP;
    }
    if (step != 0)
    {
        c_display_swipe_surface(c_surface_get_display(s1), s1, s2, rc.m_left, rc.m_right, rc.m_top, rc.m_bottom, 0);
    }
    return (index - 1);
}
void c_gesture_move_left(c_gesture *self)
{
    int index = c_slide_group_get_active_slide_index(self->m_slide_group);
    if ((index + 1) >= MAX_PAGES || c_slide_group_get_slide(self->m_slide_group, index + 1) == 0 || c_slide_group_get_slide(self->m_slide_group, index) == 0)
    {
        return;
    }
    c_surface *s1 = c_wnd_get_surface(c_slide_group_get_slide(self->m_slide_group, index + 1));
    c_surface *s2 = c_wnd_get_surface(c_slide_group_get_slide(self->m_slide_group, index));
    c_rect rc;
    c_wnd_get_screen_rect(&self->m_slide_group->base, &rc);
    if (c_surface_get_display(s1) == c_surface_get_display(s2))
    {
        c_display_swipe_surface(c_surface_get_display(s1), s2, s1, rc.m_left, rc.m_right, rc.m_top, rc.m_bottom, (self->m_down_x - self->m_move_x));
    }
}
void c_gesture_move_right(c_gesture *self)
{
    int index = c_slide_group_get_active_slide_index(self->m_slide_group);
    if (index <= 0 || c_slide_group_get_slide(self->m_slide_group, index - 1) == 0 || c_slide_group_get_slide(self->m_slide_group, index) == 0)
    {
        return;
    }
    c_surface *s1 = c_wnd_get_surface(c_slide_group_get_slide(self->m_slide_group, index - 1));
    c_surface *s2 = c_wnd_get_surface(c_slide_group_get_slide(self->m_slide_group, index));
    c_rect rc;
    c_wnd_get_screen_rect(&self->m_slide_group->base, &rc);
    if (c_surface_get_display(s1) == c_surface_get_display(s2))
    {
        c_display_swipe_surface(c_surface_get_display(s1), s1, s2, rc.m_left, rc.m_right, rc.m_top, rc.m_bottom,
                                (c_rect_width(&rc) - (self->m_move_x - self->m_down_x)));
    }
}
