#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"
#include "button.h"

#define MAX_ITEM_NUM 4
#define ITEM_HEIGHT  45

typedef struct c_list_box c_list_box;
struct c_list_box
{
    c_wnd base;
    short m_selected_item;
    short m_item_total;
    char *m_item_array[MAX_ITEM_NUM];
    c_rect m_list_wnd_rect;    // rect relative to parent wnd.
    c_rect m_list_screen_rect; // rect relative to physical screen(frame buffer)
    WND_CALLBACK on_change;
};

void c_list_box_set_on_change(c_list_box *self, WND_CALLBACK on_change);
short c_list_box_get_item_count(c_list_box *self);
int c_list_box_add_item(c_list_box *self, char *str);
void c_list_box_clear_item(c_list_box *self);
void c_list_box_select_item(c_list_box *self, short index);
void c_list_box_vir_pre_create_wnd(c_wnd *self);
void c_list_box_vir_on_paint(c_wnd *self);
void c_list_box_vir_on_focus(c_wnd *self);
void c_list_box_vir_on_kill_focus(c_wnd *self);
void c_list_box_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key);
void c_list_box_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action);
void c_list_box_update_list_size(c_list_box *self);
void c_list_box_show_list(c_list_box *self);
void c_list_box_on_touch_down(c_list_box *self, int x, int y);
void c_list_box_on_touch_up(c_list_box *self, int x, int y);
void c_list_box_init(c_list_box *self);
