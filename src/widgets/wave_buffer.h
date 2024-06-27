#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"

#define WAVE_BUFFER_LEN     1024
#define WAVE_READ_CACHE_LEN 8
#define BUFFER_EMPTY        -1111
#define BUFFER_FULL         -2222;

typedef struct c_wave_buffer c_wave_buffer;
struct c_wave_buffer
{
    short m_wave_buf[WAVE_BUFFER_LEN];
    short m_head;
    short m_tail;

    int m_min_old;
    int m_max_old;
    int m_min_older;
    int m_max_older;
    int m_last_data;

    short m_read_cache_min[WAVE_READ_CACHE_LEN];
    short m_read_cache_mid[WAVE_READ_CACHE_LEN];
    short m_read_cache_max[WAVE_READ_CACHE_LEN];
    short m_read_cache_sum;
    unsigned int m_refresh_sequence;
};

void c_wave_buffer_init(c_wave_buffer *self);
int c_wave_buffer_write_wave_data(c_wave_buffer *self, short data);
int c_wave_buffer_read_wave_data_by_frame(c_wave_buffer *self, short *max, short *min, short frame_len, unsigned int sequence, short offset);
void c_wave_buffer_reset(c_wave_buffer *self);
void c_wave_buffer_clear_data(c_wave_buffer *self);
short c_wave_buffer_get_cnt(c_wave_buffer *self);
int c_wave_buffer_read_data(c_wave_buffer *self);
