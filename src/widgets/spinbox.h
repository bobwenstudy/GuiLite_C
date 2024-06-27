#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"
#include "button.h"

#define ID_BT_ARROW_UP   0x1111
#define ID_BT_ARROW_DOWN 0x2222

typedef struct c_spin_box c_spin_box;
typedef struct c_spin_button c_spin_button;
struct c_spin_button
{
    c_button base;
    c_spin_box *m_spin_box;
};

typedef struct c_spin_box c_spin_box;
struct c_spin_box
{
    c_wnd base;
    short m_cur_value;
    short m_value;
    short m_step;
    short m_max;
    short m_min;
    short m_digit;
    c_spin_button m_bt_up;
    c_spin_button m_bt_down;
    WND_CALLBACK on_change;
};

short c_spin_box_get_value(c_spin_box *self);
void c_spin_box_set_value(c_spin_box *self, unsigned short value);
void c_spin_box_set_max_min(c_spin_box *self, short max, short min);
void c_spin_box_set_step(c_spin_box *self, short step);
short c_spin_box_get_min(c_spin_box *self);
short c_spin_box_get_max(c_spin_box *self);
short c_spin_box_get_step(c_spin_box *self);
void c_spin_box_set_value_digit(c_spin_box *self, short digit);
short c_spin_box_get_value_digit(c_spin_box *self);
void c_spin_box_set_on_change(c_spin_box *self, WND_CALLBACK on_change);
void c_spin_box_vir_on_paint(c_wnd *self);
void c_spin_box_vir_pre_create_wnd(c_wnd *self);
void c_spin_box_on_arrow_up_bt_click(c_spin_box *self);
void c_spin_box_on_arrow_down_bt_click(c_spin_box *self);
void c_spin_box_init(c_spin_box *self);

void c_spin_button_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action);
void c_spin_button_init(c_spin_button *self);