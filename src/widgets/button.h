#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"

typedef void (*WND_CALLBACK)(c_wnd *, int, int);

typedef struct c_button c_button;
struct c_button
{
    c_wnd base;
    WND_CALLBACK on_click;
};

void c_button_set_on_click(c_button *self, WND_CALLBACK on_click);
void c_button_vir_on_paint(c_wnd *self);
void c_button_vir_on_focus(c_wnd *self);
void c_button_vir_on_kill_focus(c_wnd *self);
void c_button_vir_pre_create_wnd(c_wnd *self);
void c_button_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action);
void c_button_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key);
void c_button_init(c_button *self);
