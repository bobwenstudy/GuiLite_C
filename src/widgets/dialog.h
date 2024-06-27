#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"

typedef struct c_dialog c_dialog;

typedef struct
{
    c_dialog *dialog;
    c_surface *surface;
} DIALOG_ARRAY;

struct c_dialog
{
    c_wnd base;
};

int c_dialog_open_dialog(c_dialog *self, c_dialog *p_dlg, bool modal_mode);
int c_dialog_open_dialog_simple(c_dialog *self, c_dialog *p_dlg);
int c_dialog_close_dialog(c_dialog *self, c_surface *surface);
c_dialog *c_dialog_get_the_dialog(c_dialog *self, c_surface *surface);
void c_dialog_vir_pre_create_wnd(c_wnd *self);
void c_dialog_vir_on_paint(c_wnd *self);
int c_dialog_set_me_the_dialog(c_dialog *self);
void c_dialog_init(c_dialog *self);
