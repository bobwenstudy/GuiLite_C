#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"
#include "button.h"

// Changing key width/height will change the width/height of keyboard
#define KEY_WIDTH  65
#define KEY_HEIGHT 38

#define KEYBOARD_WIDTH   ((KEY_WIDTH + 2) * 10)
#define KEYBOARD_HEIGHT  ((KEY_HEIGHT + 2) * 4)
#define NUM_BOARD_WIDTH  ((KEY_WIDTH + 2) * 4)
#define NUM_BOARD_HEIGHT ((KEY_HEIGHT + 2) * 4)

#define CAPS_WIDTH   (KEY_WIDTH * 3 / 2)
#define DEL_WIDTH    (KEY_WIDTH * 3 / 2 + 1)
#define ESC_WIDTH    (KEY_WIDTH * 2 + 2)
#define SWITCH_WIDTH (KEY_WIDTH * 3 / 2)
#define SPACE_WIDTH  (KEY_WIDTH * 3 + 2 * 2)
#define DOT_WIDTH    (KEY_WIDTH * 3 / 2 + 3)
#define ENTER_WIDTH  (KEY_WIDTH * 2 + 2)

#define POS_X(c) ((KEY_WIDTH * c) + (c + 1) * 2)
#define POS_Y(r) ((KEY_HEIGHT * r) + (r + 1) * 2)

#define KEYBORAD_CLICK           0x5014
#define ON_KEYBORAD_UPDATE(func) {MSG_TYPE_WND, KEYBORAD_CLICK, 0, msgCallback(&func)},

typedef enum
{
    STATUS_UPPERCASE,
    STATUS_LOWERCASE
} KEYBOARD_STATUS;

typedef enum
{
    STYLE_ALL_BOARD,
    STYLE_NUM_BOARD
} KEYBOARD_STYLE;

typedef enum
{
    CLICK_CHAR,
    CLICK_ENTER,
    CLICK_ESC
} CLICK_STATUS;

typedef struct c_keyboard c_keyboard;
struct c_keyboard
{
    c_wnd base;
    char m_str[32];
    int m_str_len;
    KEYBOARD_STATUS m_cap_status;
    WND_CALLBACK m_on_click;
};

typedef struct c_keyboard_button c_keyboard_button;
struct c_keyboard_button
{
    c_button base;
};

int c_keyboard_open_keyboard(c_keyboard *self, c_wnd *user, unsigned short resource_id, KEYBOARD_STYLE style, WND_CALLBACK on_click);
void c_keyboard_close_keyboard(c_keyboard *self);
void c_keyboard_vir_on_init_children(c_wnd *self);
KEYBOARD_STATUS c_keyboard_get_cap_status(c_keyboard *self);
char *c_keyboard_get_str(c_keyboard *self);
void c_keyboard_vir_pre_create_wnd(c_wnd *self);
void c_keyboard_vir_on_paint(c_wnd *self);
void c_keyboard_on_key_clicked(c_wnd *self, int id, int param);
void c_keyboard_on_char_clicked(c_keyboard *self, int id, int param);
void c_keyboard_on_del_clicked(c_keyboard *self, int id, int param);
void c_keyboard_on_caps_clicked(c_keyboard *self, int id, int param);
void c_keyboard_on_enter_clicked(c_keyboard *self, int id, int param);
void c_keyboard_on_esc_clicked(c_keyboard *self, int id, int param);
void c_keyboard_init(c_keyboard *self);

void c_keyboard_button_vir_on_paint(c_wnd *self);
void c_keyboard_button_init(c_keyboard_button *self);
