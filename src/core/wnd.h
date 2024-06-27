#pragma once

#include "resource.h"
#include "api.h"
#include "display.h"
#include "wnd.h"
#include "word.h"

typedef enum
{
    ATTR_VISIBLE = 0x40000000L,
    ATTR_FOCUS = 0x20000000L,
    ATTR_PRIORITY = 0x10000000L // Handle touch action at high priority
} WND_ATTRIBUTION;

typedef enum
{
    STATUS_NORMAL,
    STATUS_PUSHED,
    STATUS_FOCUSED,
    STATUS_DISABLED
} WND_STATUS;

typedef enum
{
    NAV_FORWARD,
    NAV_BACKWARD,
    NAV_ENTER
} NAVIGATION_KEY;

typedef enum
{
    TOUCH_DOWN,
    TOUCH_UP
} TOUCH_ACTION;

typedef struct c_wnd c_wnd;

typedef struct struct_wnd_tree
{
    c_wnd *p_wnd;             // window instance
    unsigned int resource_id; // ID
    const char *str;          // caption
    short x;                  // position x
    short y;                  // position y
    short width;
    short height;
    struct struct_wnd_tree *p_child_tree; // sub tree
} WND_TREE;

typedef struct c_wnd_vir_api c_wnd_vir_api;
struct c_wnd_vir_api
{
    int (*connect)(c_wnd *wnd, c_wnd *parent, unsigned short resource_id, const char *str, short x, short y, short width, short height, WND_TREE *p_child_tree);
    void (*on_init_children)(c_wnd *wnd);
    void (*on_paint)(c_wnd *wnd);
    void (*show_window)(c_wnd *wnd);
    void (*on_touch)(c_wnd *wnd, int x, int y, TOUCH_ACTION action);
    void (*on_navigate)(c_wnd *wnd, NAVIGATION_KEY key);
    void (*pre_create_wnd)(c_wnd *wnd);
    void (*on_focus)(c_wnd *wnd);
    void (*on_kill_focus)(c_wnd *wnd);
};

struct c_wnd
{
    unsigned short m_id;
    WND_STATUS m_status;
    WND_ATTRIBUTION m_attr;
    c_rect m_wnd_rect;     // position relative to parent window.
    c_wnd *m_parent;       // parent window
    c_wnd *m_top_child;    // the first sub window would be navigated
    c_wnd *m_prev_sibling; // previous brother
    c_wnd *m_next_sibling; // next brother
    c_wnd *m_focus_child;  // current focused window
    const char *m_str;     // caption

    const void *m_font; // font face
    unsigned int m_font_color;
    unsigned int m_bg_color;

    int m_z_order; // the graphic level for rendering
    c_surface *m_surface;

    const c_wnd_vir_api *m_api; // virtual api
};

// declaration
int c_wnd_vir_connect(c_wnd *self, c_wnd *parent, unsigned short resource_id, const char *str, short x, short y, short width, short height,
                      WND_TREE *p_child_tree);
void c_wnd_disconnect(c_wnd *self);
void c_wnd_vir_on_init_children(c_wnd *self);
void c_wnd_vir_on_paint(c_wnd *self);
void c_wnd_vir_show_window(c_wnd *self);
unsigned short c_wnd_get_id(const c_wnd *self);
int c_wnd_get_z_order(c_wnd *self);
c_wnd *c_wnd_get_wnd_ptr(const c_wnd *self, unsigned short id);
unsigned int c_wnd_get_attr(const c_wnd *self);
void c_wnd_set_str(c_wnd *self, const char *str);
void c_wnd_set_attr(c_wnd *self, WND_ATTRIBUTION attr);
bool c_wnd_is_focus_wnd(const c_wnd *self);
void c_wnd_set_font_color(c_wnd *self, unsigned int color);
unsigned int c_wnd_get_font_color(c_wnd *self);
void c_wnd_set_bg_color(c_wnd *self, unsigned int color);
unsigned int c_wnd_get_bg_color(c_wnd *self);
void c_wnd_set_font_type(c_wnd *self, const LATTICE_FONT_INFO *font_type);
const void *c_wnd_get_font_type(c_wnd *self);
void c_wnd_get_wnd_rect(const c_wnd *self, c_rect *rect);
void c_wnd_get_screen_rect(const c_wnd *self, c_rect *rect);
c_wnd *c_wnd_set_child_focus(c_wnd *self, c_wnd *focus_child);
c_wnd *c_wnd_get_parent(const c_wnd *self);
c_wnd *c_wnd_get_last_child(const c_wnd *self);
int c_wnd_unlink_child(c_wnd *self, c_wnd *child);
c_wnd *c_wnd_get_prev_sibling(const c_wnd *self);
c_wnd *c_wnd_get_next_sibling(const c_wnd *self);
c_wnd *c_wnd_search_priority_sibling(c_wnd *self, c_wnd *root);
void c_wnd_vir_on_touch(c_wnd *self, int x, int y, TOUCH_ACTION action);
void c_wnd_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key);
c_surface *c_wnd_get_surface(c_wnd *self);
void c_wnd_set_surface(c_wnd *self, c_surface *surface);
void c_wnd_vir_pre_create_wnd(c_wnd *self);
void c_wnd_add_child_2_tail(c_wnd *self, c_wnd *child);
void c_wnd_wnd2screen(const c_wnd *self, int *x, int *y);
int c_wnd_load_child_wnd(c_wnd *self, WND_TREE *p_child_tree);
void c_wnd_set_active_child(c_wnd *self, c_wnd *child);
void c_wnd_vir_on_focus(c_wnd *self);
void c_wnd_vir_on_kill_focus(c_wnd *self);
void c_wnd_init(c_wnd *self);
