#include "wave_buffer.h"

void c_wave_buffer_init(c_wave_buffer *self)
{
    self->m_head = self->m_tail = self->m_min_old = self->m_max_old = self->m_min_older = self->m_max_older = self->m_last_data = self->m_read_cache_sum =
            self->m_refresh_sequence = 0;
    memset(self->m_wave_buf, 0, sizeof(self->m_wave_buf));
    memset(self->m_read_cache_min, 0, sizeof(self->m_read_cache_min));
    memset(self->m_read_cache_mid, 0, sizeof(self->m_read_cache_mid));
    memset(self->m_read_cache_max, 0, sizeof(self->m_read_cache_max));
}
int c_wave_buffer_write_wave_data(c_wave_buffer *self, short data)
{
    if ((self->m_tail + 1) % WAVE_BUFFER_LEN == self->m_head)
    { // full
        // log_out("wave buf full\n");
        return BUFFER_FULL;
    }
    self->m_wave_buf[self->m_tail] = data;
    self->m_tail = (self->m_tail + 1) % WAVE_BUFFER_LEN;
    return 1;
}
int c_wave_buffer_read_wave_data_by_frame(c_wave_buffer *self, short *max, short *min, short frame_len, unsigned int sequence, short offset)
{
    if (self->m_refresh_sequence != sequence)
    {
        self->m_refresh_sequence = sequence;
        self->m_read_cache_sum = 0;
    }
    else if (offset < self->m_read_cache_sum) //(self->m_refresh_sequence == sequence && offset < self->m_fb_sum)
    {
        *max = self->m_read_cache_max[offset];
        *min = self->m_read_cache_min[offset];
        return self->m_read_cache_mid[offset];
    }

    self->m_read_cache_sum++;
    GUILITE_ASSERT(self->m_read_cache_sum <= WAVE_READ_CACHE_LEN);
    int i, data;
    int tmp_min = self->m_last_data;
    int tmp_max = self->m_last_data;
    int mid = (self->m_min_old + self->m_max_old) >> 1;

    i = 0;
    while (i++ < frame_len)
    {
        data = c_wave_buffer_read_data(self);
        if (BUFFER_EMPTY == data)
        {
            break;
        }
        self->m_last_data = data;

        if (data < tmp_min)
        {
            tmp_min = data;
        }
        if (data > tmp_max)
        {
            tmp_max = data;
        }
    }

    *min = self->m_read_cache_min[offset] = MIN(self->m_min_old, MIN(tmp_min, self->m_min_older));
    *max = self->m_read_cache_max[offset] = MAX(self->m_max_old, MAX(tmp_max, self->m_max_older));

    self->m_min_older = self->m_min_old;
    self->m_max_older = self->m_max_old;
    self->m_min_old = tmp_min;
    self->m_max_old = tmp_max;
    return (self->m_read_cache_mid[offset] = mid);
}
void c_wave_buffer_reset(c_wave_buffer *self)
{
    self->m_head = self->m_tail;
}

void c_wave_buffer_clear_data(c_wave_buffer *self)
{
    self->m_head = self->m_tail = 0;
    memset(self->m_wave_buf, 0, sizeof(self->m_wave_buf));
}
short c_wave_buffer_get_cnt(c_wave_buffer *self)
{
    return (self->m_tail >= self->m_head) ? (self->m_tail - self->m_head) : (self->m_tail - self->m_head + WAVE_BUFFER_LEN);
}

int c_wave_buffer_read_data(c_wave_buffer *self)
{
    if (self->m_head == self->m_tail)
    { // empty
        // log_out("wave buf empty\n");
        return BUFFER_EMPTY;
    }
    int ret = self->m_wave_buf[self->m_head];
    self->m_head = (self->m_head + 1) % WAVE_BUFFER_LEN;
    return ret;
}
