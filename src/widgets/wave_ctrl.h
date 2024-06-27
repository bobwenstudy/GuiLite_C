#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"
#include "wave_buffer.h"

#define CORRECT(x, high_limit, low_limit)                                                                                                                      \
    {                                                                                                                                                          \
        x = (x > high_limit) ? high_limit : x;                                                                                                                 \
        x = (x < low_limit) ? low_limit : x;                                                                                                                   \
    }

#define WAVE_CURSOR_WIDTH 8
#define WAVE_LINE_WIDTH   1
#define WAVE_MARGIN       5

typedef enum
{
    FILL_MODE,
    SCAN_MODE
} E_WAVE_DRAW_MODE;

typedef struct c_wave_ctrl c_wave_ctrl;
struct c_wave_ctrl
{
    c_wnd base;

    char *m_wave_name;
    char *m_wave_unit;

    const LATTICE_FONT_INFO *m_wave_name_font;
    const LATTICE_FONT_INFO *m_wave_unit_font;

    unsigned int m_wave_name_color;
    unsigned int m_wave_unit_color;

    unsigned int m_wave_color;
    unsigned int m_back_color;

    int m_wave_left;
    int m_wave_right;
    int m_wave_top;
    int m_wave_bottom;

    short m_max_data;
    short m_min_data;

    c_wave_buffer *m_wave;
    unsigned int *m_bg_fb; // background frame buffer, could be used to draw scale line.
    int m_wave_cursor;
    int m_wave_speed;                 // pixels per refresh
    unsigned int m_wave_data_rate;    // data sample rate
    unsigned int m_wave_refresh_rate; // refresh cycle in millisecond
    unsigned char m_frame_len_map[64];
    unsigned char m_frame_len_map_index;
};

void c_wave_ctrl_vir_on_init_children(c_wnd *self);
void c_wave_ctrl_vir_on_paint(c_wnd *self);
void c_wave_ctrl_set_wave_name(c_wave_ctrl *self, char *wave_name);
void c_wave_ctrl_set_wave_unit(c_wave_ctrl *self, char *wave_unit);
void c_wave_ctrl_set_wave_name_font(c_wave_ctrl *self, const LATTICE_FONT_INFO *wave_name_font_type);
void c_wave_ctrl_set_wave_unit_font(c_wave_ctrl *self, const LATTICE_FONT_INFO *wave_unit_font_type);
void c_wave_ctrl_set_wave_name_color(c_wave_ctrl *self, unsigned int wave_name_color);
void c_wave_ctrl_set_wave_unit_color(c_wave_ctrl *self, unsigned int wave_unit_color);
void c_wave_ctrl_set_wave_color(c_wave_ctrl *self, unsigned int color);
void c_wave_ctrl_set_wave_in_out_rate(c_wave_ctrl *self, unsigned int data_rate, unsigned int refresh_rate);
void c_wave_ctrl_set_wave_speed(c_wave_ctrl *self, unsigned int speed);
void c_wave_ctrl_set_max_min(c_wave_ctrl *self, short max_data, short min_data);
void c_wave_ctrl_set_wave(c_wave_ctrl *self, c_wave_buffer *wave);
c_wave_buffer *c_wave_ctrl_get_wave(c_wave_ctrl *self);
void c_wave_ctrl_clear_data(c_wave_ctrl *self);
bool c_wave_ctrl_is_data_enough(c_wave_ctrl *self);
void c_wave_ctrl_refresh_wave(c_wave_ctrl *self, unsigned char frame);
void c_wave_ctrl_clear_wave(c_wave_ctrl *self);
void c_wave_ctrl_draw_smooth_vline(c_wave_ctrl *self, int y_min, int y_max, int mid, unsigned int rgb);
void c_wave_ctrl_restore_background(c_wave_ctrl *self);
void c_wave_ctrl_save_background(c_wave_ctrl *self);
void c_wave_ctrl_init(c_wave_ctrl *self);
