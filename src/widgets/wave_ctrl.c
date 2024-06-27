#include "wave_ctrl.h"

void c_wave_ctrl_vir_on_init_children(c_wnd *self) // should be pre_create
{
    c_wave_ctrl *wave_ctrl = (c_wave_ctrl *)self;
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);

    wave_ctrl->m_wave_left = rect.m_left + WAVE_MARGIN;
    wave_ctrl->m_wave_right = rect.m_right - WAVE_MARGIN;
    wave_ctrl->m_wave_top = rect.m_top + WAVE_MARGIN;
    wave_ctrl->m_wave_bottom = rect.m_bottom - WAVE_MARGIN;
    wave_ctrl->m_wave_cursor = wave_ctrl->m_wave_left;

    wave_ctrl->m_bg_fb = (unsigned int *)guilite_calloc(c_rect_width(&rect) * c_rect_height(&rect), 4);
}

void c_wave_ctrl_vir_on_paint(c_wnd *self)
{
    c_wave_ctrl *wave_ctrl = (c_wave_ctrl *)self;
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    c_surface_fill_rect_ex(self->m_surface, rect, wave_ctrl->m_back_color, self->m_z_order);

    // show name
    c_word_draw_string(self->m_surface, self->m_z_order, wave_ctrl->m_wave_name, wave_ctrl->m_wave_left + 10, rect.m_top, wave_ctrl->m_wave_name_font,
                       wave_ctrl->m_wave_name_color, GL_ARGB(0, 0, 0, 0));
    // show unit
    c_word_draw_string(self->m_surface, self->m_z_order, wave_ctrl->m_wave_unit, wave_ctrl->m_wave_left + 60, rect.m_top, wave_ctrl->m_wave_unit_font,
                       wave_ctrl->m_wave_unit_color, GL_ARGB(0, 0, 0, 0));

    c_wave_ctrl_save_background(wave_ctrl);
}

void c_wave_ctrl_set_wave_name(c_wave_ctrl *self, char *wave_name)
{
    self->m_wave_name = wave_name;
}
void c_wave_ctrl_set_wave_unit(c_wave_ctrl *self, char *wave_unit)
{
    self->m_wave_unit = wave_unit;
}

void c_wave_ctrl_set_wave_name_font(c_wave_ctrl *self, const LATTICE_FONT_INFO *wave_name_font_type)
{
    self->m_wave_name_font = wave_name_font_type;
}
void c_wave_ctrl_set_wave_unit_font(c_wave_ctrl *self, const LATTICE_FONT_INFO *wave_unit_font_type)
{
    self->m_wave_unit_font = wave_unit_font_type;
}

void c_wave_ctrl_set_wave_name_color(c_wave_ctrl *self, unsigned int wave_name_color)
{
    self->m_wave_name_color = wave_name_color;
}
void c_wave_ctrl_set_wave_unit_color(c_wave_ctrl *self, unsigned int wave_unit_color)
{
    self->m_wave_unit_color = wave_unit_color;
}
void c_wave_ctrl_set_wave_color(c_wave_ctrl *self, unsigned int color)
{
    self->m_wave_color = color;
}
void c_wave_ctrl_set_wave_in_out_rate(c_wave_ctrl *self, unsigned int data_rate, unsigned int refresh_rate)
{
    self->m_wave_data_rate = data_rate;
    self->m_wave_refresh_rate = refresh_rate;
    int read_times_per_second = self->m_wave_speed * 1000 / self->m_wave_refresh_rate;

    memset(self->m_frame_len_map, 0, sizeof(self->m_frame_len_map));
    for (unsigned int i = 1; i < sizeof(self->m_frame_len_map) + 1; i++)
    {
        self->m_frame_len_map[i - 1] = data_rate * i / read_times_per_second - data_rate * (i - 1) / read_times_per_second;
    }
    self->m_frame_len_map_index = 0;
}
void c_wave_ctrl_set_wave_speed(c_wave_ctrl *self, unsigned int speed)
{
    self->m_wave_speed = speed;
    c_wave_ctrl_set_wave_in_out_rate(self, self->m_wave_data_rate, self->m_wave_refresh_rate);
}
void c_wave_ctrl_set_max_min(c_wave_ctrl *self, short max_data, short min_data)
{
    self->m_max_data = max_data;
    self->m_min_data = min_data;
}
void c_wave_ctrl_set_wave(c_wave_ctrl *self, c_wave_buffer *wave)
{
    self->m_wave = wave;
}
c_wave_buffer *c_wave_ctrl_get_wave(c_wave_ctrl *self)
{
    return self->m_wave;
}
void c_wave_ctrl_clear_data(c_wave_ctrl *self)
{
    if (self->m_wave == 0)
    {
        GUILITE_ASSERT(false);
        return;
    }
    c_wave_buffer_clear_data(self->m_wave);
}
bool c_wave_ctrl_is_data_enough(c_wave_ctrl *self)
{
    if (self->m_wave == 0)
    {
        GUILITE_ASSERT(false);
        return false;
    }
    return (c_wave_buffer_get_cnt(self->m_wave) - self->m_frame_len_map[self->m_frame_len_map_index] * self->m_wave_speed);
}
void c_wave_ctrl_refresh_wave(c_wave_ctrl *self, unsigned char frame)
{
    if (self->m_wave == 0)
    {
        GUILITE_ASSERT(false);
        return;
    }

    short max, min, mid;
    for (short offset = 0; offset < self->m_wave_speed; offset++)
    {
        // get wave value
        mid = c_wave_buffer_read_wave_data_by_frame(self->m_wave, &max, &min, self->m_frame_len_map[self->m_frame_len_map_index++], frame, offset);
        self->m_frame_len_map_index %= sizeof(self->m_frame_len_map);

        // map to wave ctrl
        int y_min, y_max;
        if (self->m_max_data == self->m_min_data)
        {
            GUILITE_ASSERT(false);
        }
        y_max = self->m_wave_bottom + WAVE_LINE_WIDTH -
                (self->m_wave_bottom - self->m_wave_top) * (min - self->m_min_data) / (self->m_max_data - self->m_min_data);
        y_min = self->m_wave_bottom - WAVE_LINE_WIDTH -
                (self->m_wave_bottom - self->m_wave_top) * (max - self->m_min_data) / (self->m_max_data - self->m_min_data);
        mid = self->m_wave_bottom - (self->m_wave_bottom - self->m_wave_top) * (mid - self->m_min_data) / (self->m_max_data - self->m_min_data);

        CORRECT(y_min, self->m_wave_bottom, self->m_wave_top);
        CORRECT(y_max, self->m_wave_bottom, self->m_wave_top);
        CORRECT(mid, self->m_wave_bottom, self->m_wave_top);

        if (self->m_wave_cursor > self->m_wave_right)
        {
            self->m_wave_cursor = self->m_wave_left;
        }
        c_wave_ctrl_draw_smooth_vline(self, y_min, y_max, mid, self->m_wave_color);
        c_wave_ctrl_restore_background(self);
        self->m_wave_cursor++;
    }
}
void c_wave_ctrl_clear_wave(c_wave_ctrl *self)
{
    self->base.m_surface->m_api->fill_rect(self->base.m_surface, self->m_wave_left, self->m_wave_top, self->m_wave_right, self->m_wave_bottom,
                                           self->m_back_color, self->base.m_z_order);
    self->m_wave_cursor = self->m_wave_left;
}

void c_wave_ctrl_draw_smooth_vline(c_wave_ctrl *self, int y_min, int y_max, int mid, unsigned int rgb)
{
    int dy = y_max - y_min;
    short r = GL_RGB_R(rgb);
    short g = GL_RGB_G(rgb);
    short b = GL_RGB_B(rgb);
    int index = (dy >> 1) + 2;
    int y;

    self->base.m_surface->m_api->draw_pixel(self->base.m_surface, self->m_wave_cursor, mid, rgb, self->base.m_z_order);

    if (dy < 1)
    {
        return;
    }

    unsigned char cur_r, cur_g, cur_b;
    unsigned int cur_rgb;
    for (int i = 1; i <= (dy >> 1) + 1; ++i)
    {
        if ((mid + i) <= y_max)
        {
            y = mid + i;
            cur_r = r * (index - i) / index;
            cur_g = g * (index - i) / index;
            cur_b = b * (index - i) / index;
            cur_rgb = GL_RGB(cur_r, cur_g, cur_b);
            self->base.m_surface->m_api->draw_pixel(self->base.m_surface, self->m_wave_cursor, y, cur_rgb, self->base.m_z_order);
        }
        if ((mid - i) >= y_min)
        {
            y = mid - i;
            cur_r = r * (index - i) / index;
            cur_g = g * (index - i) / index;
            cur_b = b * (index - i) / index;
            cur_rgb = GL_RGB(cur_r, cur_g, cur_b);
            self->base.m_surface->m_api->draw_pixel(self->base.m_surface, self->m_wave_cursor, y, cur_rgb, self->base.m_z_order);
        }
    }
}
void c_wave_ctrl_restore_background(c_wave_ctrl *self)
{
    int x = self->m_wave_cursor + WAVE_CURSOR_WIDTH;
    if (x > self->m_wave_right)
    {
        x -= (self->m_wave_right - self->m_wave_left + 1);
    }

    c_rect rect;
    c_wnd_get_screen_rect(&self->base, &rect);
    int width = c_rect_width(&rect);
    int top = rect.m_top;
    int left = rect.m_left;
    for (int y_pos = (self->m_wave_top - 1); y_pos <= (self->m_wave_bottom + 1); y_pos++)
    {
        (self->m_bg_fb) ? self->base.m_surface->m_api->draw_pixel(self->base.m_surface, x, y_pos, self->m_bg_fb[(y_pos - top) * width + (x - left)],
                                                                  self->base.m_z_order)
                        : self->base.m_surface->m_api->draw_pixel(self->base.m_surface, x, y_pos, 0, self->base.m_z_order);
    }
}
void c_wave_ctrl_save_background(c_wave_ctrl *self)
{
    if (!self->m_bg_fb)
    {
        return;
    }
    c_rect rect;
    c_wnd_get_screen_rect(&self->base, &rect);

    unsigned int *p_des = self->m_bg_fb;
    for (int y = rect.m_top; y <= rect.m_bottom; y++)
    {
        for (int x = rect.m_left; x <= rect.m_right; x++)
        {
            *p_des++ = c_surface_get_pixel(self->base.m_surface, x, y, self->base.m_z_order);
        }
    }
}

static const c_wnd_vir_api c_wave_ctrl_vir_api_table = {
        c_wnd_vir_connect,     c_wave_ctrl_vir_on_init_children, c_wave_ctrl_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_wnd_vir_pre_create_wnd,         c_wnd_vir_on_focus,       c_wnd_vir_on_kill_focus,
};

void c_wave_ctrl_init(c_wave_ctrl *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_wave_ctrl_vir_api_table;

    self->m_wave = 0;
    self->m_bg_fb = 0;
    self->m_wave_name_font = self->m_wave_unit_font = 0;
    self->m_wave_name = self->m_wave_unit = 0;
    self->m_max_data = 500;
    self->m_min_data = 0;
    self->m_wave_speed = 1;
    self->m_wave_data_rate = 0;
    self->m_wave_refresh_rate = 1000;
    self->m_frame_len_map_index = 0;

    self->m_wave_name_color = self->m_wave_unit_color = self->m_wave_color = GL_RGB(255, 0, 0);
    self->m_back_color = GL_RGB(0, 0, 0);
}
