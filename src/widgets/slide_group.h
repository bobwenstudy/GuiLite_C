#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"

#define MAX_PAGES 5

// #define SWIPE_STEP			300//for arm
#define SWIPE_STEP     10 // for PC & ANDROID
#define MOVE_THRESHOLD 10

typedef enum
{
    TOUCH_MOVE,
    TOUCH_IDLE
} TOUCH_STATE;

typedef struct c_gesture c_gesture;

typedef struct c_slide_group c_slide_group;

struct c_gesture
{
    int m_down_x;
    int m_down_y;
    int m_move_x;
    int m_move_y;
    TOUCH_STATE m_state;
    c_slide_group *m_slide_group;
};

struct c_slide_group
{
    c_wnd base;
    c_wnd *m_slides[MAX_PAGES];
    int m_active_slide_index;
    c_gesture m_gesture;
};

int c_slide_group_set_active_slide(c_slide_group *self, int index, bool is_redraw);
int c_slide_group_set_active_slide_simple(c_slide_group *self, int index);
c_wnd *c_slide_group_get_slide(c_slide_group *self, int index);
c_wnd *c_slide_group_get_active_slide(c_slide_group *self);
int c_slide_group_get_active_slide_index(c_slide_group *self);
int c_slide_group_add_slide(c_slide_group *self, c_wnd *slide, unsigned short resource_id, short x, short y, short width, short height, WND_TREE *p_child_tree,
                            Z_ORDER_LEVEL max_zorder);
int c_slide_group_add_slide_with_child_tree(c_slide_group *self, c_wnd *slide, unsigned short resource_id, short x, short y, short width, short height,
                                            WND_TREE *p_child_tree);
int c_slide_group_add_slide_simple(c_slide_group *self, c_wnd *slide, unsigned short resource_id, short x, short y, short width, short height);
void c_slide_group_disabel_all_slide(c_slide_group *self);
void c_slide_group_vir_on_navigate(c_wnd *self, NAVIGATION_KEY key);
void c_slide_group_init(c_slide_group *self);

void c_gesture_init(c_gesture *self, c_slide_group *group);
bool c_gesture_handle_swipe(c_gesture *self, int x, int y, TOUCH_ACTION action);
bool c_gesture_on_move(c_gesture *self, int x);
bool c_gesture_on_swipe(c_gesture *self, int x);
int c_gesture_swipe_left(c_gesture *self);
int c_gesture_swipe_right(c_gesture *self);
void c_gesture_move_left(c_gesture *self);
void c_gesture_move_right(c_gesture *self);
