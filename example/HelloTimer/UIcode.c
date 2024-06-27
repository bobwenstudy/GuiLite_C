#include "GuiLite.h"
#include <stdlib.h>
#include <math.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

const int FRAME_COUNT = 32;

extern const LATTICE_FONT_INFO Consolas_19;
extern const LATTICE_FONT_INFO _DengXian_36B;
extern const BITMAP_INFO humidity_bmp;
extern const BITMAP_INFO temperature_bmp;
extern const BITMAP_INFO weather_bmp;
extern const BITMAP_INFO grass_bmp;

static c_surface *s_surface;
static c_display *s_display;

typedef struct c_hand
{
    int m_pos_x, m_pos_y;
    unsigned int m_rgb;

    float m_start_len;
    float m_end_len;
    float m_len_step;

    float m_start_angle;
    float m_end_angle;
    float m_angle_step;
} c_hand;

void c_hand_set(c_hand *self, int x, int y, float start_len, float end_len, float start_angle, float end_angle, unsigned int rgb)
{
    self->m_pos_x = x;
    self->m_pos_y = y;
    self->m_rgb = rgb;
    self->m_start_len = start_len;
    self->m_end_len = end_len;
    self->m_start_angle = start_angle;
    self->m_end_angle = end_angle;

    self->m_len_step = ((self->m_end_len - self->m_start_len) / FRAME_COUNT);
    self->m_angle_step = ((self->m_end_angle - self->m_start_angle) / FRAME_COUNT);
}
bool c_hand_show(c_hand *self)
{
    if (fabs(self->m_start_len - self->m_end_len) < 1.0)
    {
        return true;
    }
    // erase footprint
    int x = (self->m_start_len) * cos(self->m_start_angle) + self->m_pos_x;
    int y = (self->m_start_len) * sin(self->m_start_angle) + self->m_pos_y;
    c_surface_draw_line(s_surface, self->m_pos_x, self->m_pos_y, x, y, 0, Z_ORDER_LEVEL_0);

    self->m_start_len += self->m_len_step;
    self->m_start_angle += self->m_angle_step;

    x = (self->m_start_len) * cos(self->m_start_angle) + self->m_pos_x;
    y = (self->m_start_len) * sin(self->m_start_angle) + self->m_pos_y;
    c_surface_draw_line(s_surface, self->m_pos_x, self->m_pos_y, x, y, self->m_rgb, Z_ORDER_LEVEL_0);

    return false;
}

typedef struct c_clock
{
    int m_x;
    int m_y;
    float m_start_radius;
    float m_end_radius;
    float m_step;

    c_hand m_hour;
    c_hand m_minute;
    c_hand m_second;
    float pi;
} c_clock;

void c_clock_init(c_clock *self)
{
    self->m_x = 120;
    self->m_y = 120;
    self->pi = 3.1415926535 / 180;
}

void c_clock_set_hands(c_clock *self, float start_radius, float end_radius, float hour_start_len, float hour_end_len, float hour_start_angle,
                       float hour_end_angle, unsigned int hour_rgb, float minute_start_len, float minute_end_len, float minute_start_angle,
                       float minute_end_angle, unsigned int minute_rgb, float second_start_len, float second_end_len, float second_start_angle,
                       float second_end_angle, unsigned int second_rgb)
{
    c_hand_set(&self->m_hour, self->m_x, self->m_y, hour_start_len, hour_end_len, hour_start_angle, hour_end_angle, hour_rgb);
    c_hand_set(&self->m_minute, self->m_x, self->m_y, minute_start_len, minute_end_len, minute_start_angle, minute_end_angle, minute_rgb);
    c_hand_set(&self->m_second, self->m_x, self->m_y, second_start_len, second_end_len, second_start_angle, second_end_angle, second_rgb);
    self->m_start_radius = start_radius;
    self->m_end_radius = end_radius;
    self->m_step = ((self->m_end_radius - self->m_start_radius) / FRAME_COUNT);
}
void c_clock_drawCircle(c_clock *self, int r, unsigned int rgb)
{
    int tmpX, tmpY, lastX, lastY;
    lastX = lastY = 0;

    for (float angle = 0.5; angle < 90; angle += 0.5) // Bigger circle, Smaller angle step
    {
        tmpX = r * cos(angle * self->pi);
        tmpY = r * sin(angle * self->pi);
        if (tmpX == lastX && tmpY == lastY)
        {
            continue;
        }

        s_surface->m_api->draw_pixel(s_surface, self->m_x + tmpX, self->m_y + tmpY, rgb, Z_ORDER_LEVEL_0);
        s_surface->m_api->draw_pixel(s_surface, self->m_x - tmpX, self->m_y + tmpY, rgb, Z_ORDER_LEVEL_0);
        s_surface->m_api->draw_pixel(s_surface, self->m_x + tmpX, self->m_y - tmpY, rgb, Z_ORDER_LEVEL_0);
        s_surface->m_api->draw_pixel(s_surface, self->m_x - tmpX, self->m_y - tmpY, rgb, Z_ORDER_LEVEL_0);
    }
}
bool c_clock_drawSacle(c_clock *self)
{
    if (fabs(self->m_start_radius - self->m_end_radius) < 1.0)
    {
        return true;
    }

    c_clock_drawCircle(self, self->m_start_radius, 0);
    self->m_start_radius += self->m_step;
    c_clock_drawCircle(self, self->m_start_radius, GL_RGB(0, 175, 235));
    return false;
}
void c_clock_show(c_clock *self)
{
    bool complete = false;
    while (!complete)
    {
        complete = c_clock_drawSacle(self);
        complete &= c_hand_show(&self->m_hour);
        complete &= c_hand_show(&self->m_minute);
        complete &= c_hand_show(&self->m_second);
        guilite_thread_sleep(20);
    }
}

typedef struct c_weather
{
    int m_weather_x, m_weather_y;
    int m_temperature_x, m_temperature_y;
    int m_humidity_x, m_humidity_y;
    int m_temp_value_x, m_temp_value_y;
    int m_humidity_value_x, m_humidity_value_y;
    int m_day_x, m_day_y;
    int m_distance, m_frame_count;
} c_weather;

void c_weather_init(c_weather *self)
{
    self->m_weather_x = (16);
    self->m_weather_y = (96);
    self->m_temperature_x = (129);
    self->m_temperature_y = (73);
    self->m_humidity_x = (126);
    self->m_humidity_y = (144);
    self->m_temp_value_x = (174);
    self->m_temp_value_y = (88);
    self->m_humidity_value_x = (174);
    self->m_humidity_value_y = (159);
    self->m_day_x = (0);
    self->m_day_y = (25);
    self->m_distance = (60);
    self->m_frame_count = (6);
}

void c_weather_show(c_weather *self)
{
    int step = self->m_distance / self->m_frame_count;
    int distance = self->m_distance;
    while (distance > 0)
    {
        c_rect tmp;
        c_rect_init(&tmp, self->m_day_x, self->m_day_y - distance, 240, 48);
        // erase footprint
        c_word_draw_string_in_rect(s_surface, Z_ORDER_LEVEL_0, "Today", &tmp, c_theme_get_font(FONT_DEFAULT), 0, 0, ALIGN_HCENTER);

        s_surface->m_api->fill_rect(s_surface, self->m_weather_x - distance, self->m_weather_y, self->m_weather_x - distance + step,
                                    self->m_weather_y + weather_bmp.height, 0, Z_ORDER_LEVEL_0);

        s_surface->m_api->fill_rect(s_surface, self->m_temperature_x + distance + temperature_bmp.width - step, self->m_temperature_y,
                                    self->m_temperature_x + distance + temperature_bmp.width, self->m_temperature_y + temperature_bmp.height, 0,
                                    Z_ORDER_LEVEL_0);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "25C", self->m_temp_value_x + distance, self->m_temp_value_y, c_theme_get_font(FONT_DEFAULT), 0, 0);

        s_surface->m_api->fill_rect(s_surface, self->m_humidity_x + distance + humidity_bmp.width - step, self->m_humidity_y,
                                    self->m_humidity_x + distance + humidity_bmp.width, self->m_humidity_y + humidity_bmp.height, 0, Z_ORDER_LEVEL_0);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "75%", self->m_humidity_value_x + distance, self->m_humidity_value_y, c_theme_get_font(FONT_DEFAULT), 0,
                           0);

        distance -= step;

        // show at new position
        c_rect_init(&tmp, self->m_day_x, self->m_day_y - distance, 240, 48);
        c_word_draw_string_in_rect(s_surface, Z_ORDER_LEVEL_0, "Today", &tmp, c_theme_get_font(FONT_DEFAULT), GL_RGB(0, 163, 233), 0, ALIGN_HCENTER);

        c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &weather_bmp, self->m_weather_x - distance, self->m_weather_y);

        c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &temperature_bmp, self->m_temperature_x + distance, self->m_temperature_y);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "25C", self->m_temp_value_x + distance, self->m_temp_value_y, c_theme_get_font(FONT_DEFAULT),
                           GL_RGB(0, 163, 233), 0);

        c_surface_draw_hline(s_surface, 156, 240, 132, GL_RGB(0, 163, 233), Z_ORDER_LEVEL_0);

        c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &humidity_bmp, self->m_humidity_x + distance, self->m_humidity_y);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "75%", self->m_humidity_value_x + distance, self->m_humidity_value_y, c_theme_get_font(FONT_DEFAULT),
                           GL_RGB(0, 163, 233), 0);

        guilite_thread_sleep(20);
    }
}

void c_weather_disappear(c_weather *self)
{
    int step = self->m_distance / self->m_frame_count;
    int distance = 0;
    while (distance < self->m_distance)
    {
        c_rect tmp;
        c_rect_init(&tmp, self->m_day_x, self->m_day_y - distance, 240, 48);
        // erase footprint
        c_word_draw_string_in_rect(s_surface, Z_ORDER_LEVEL_0, "Today", &tmp, c_theme_get_font(FONT_DEFAULT), 0, 0, ALIGN_HCENTER);

        s_surface->m_api->fill_rect(s_surface, self->m_weather_x - distance + weather_bmp.width, self->m_weather_y,
                                    self->m_weather_x - distance + weather_bmp.width + step, self->m_weather_y + weather_bmp.height, 0, Z_ORDER_LEVEL_0);

        s_surface->m_api->fill_rect(s_surface, self->m_temperature_x + distance - step, self->m_temperature_y, self->m_temperature_x + distance,
                                    self->m_temperature_y + temperature_bmp.height, 0, Z_ORDER_LEVEL_0);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "25C", self->m_temp_value_x + distance, self->m_temp_value_y, c_theme_get_font(FONT_DEFAULT), 0, 0);

        s_surface->m_api->fill_rect(s_surface, self->m_humidity_x + distance - step, self->m_humidity_y, self->m_humidity_x + distance,
                                    self->m_humidity_y + humidity_bmp.height, 0, Z_ORDER_LEVEL_0);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "75%", self->m_humidity_value_x + distance, self->m_humidity_value_y, c_theme_get_font(FONT_DEFAULT), 0,
                           0);

        distance += step;

        // show at new position
        c_rect_init(&tmp, self->m_day_x, self->m_day_y - distance, 240, 48);
        c_word_draw_string_in_rect(s_surface, Z_ORDER_LEVEL_0, "Today", &tmp, c_theme_get_font(FONT_DEFAULT), GL_RGB(0, 163, 233), 0, ALIGN_HCENTER);

        c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &weather_bmp, self->m_weather_x - distance, self->m_weather_y);

        c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &temperature_bmp, self->m_temperature_x + distance, self->m_temperature_y);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "25C", self->m_temp_value_x + distance, self->m_temp_value_y, c_theme_get_font(FONT_DEFAULT),
                           GL_RGB(0, 163, 233), 0);

        c_surface_draw_hline(s_surface, 156, 240, 132, GL_RGB(0, 163, 233), Z_ORDER_LEVEL_0);

        c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &humidity_bmp, self->m_humidity_x + distance, self->m_humidity_y);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "75%", self->m_humidity_value_x + distance, self->m_humidity_value_y, c_theme_get_font(FONT_DEFAULT),
                           GL_RGB(0, 163, 233), 0);

        guilite_thread_sleep(20);
    }
}

typedef struct c_time
{
    int m_x, m_y, m_width, m_height;
    c_rect m_time_rect;
} c_time;

void c_time_init(c_time *self)
{
    self->m_x = (0);
    self->m_y = (100);
    self->m_width = (240);
    self->m_height = (100);
    c_rect_init(&self->m_time_rect, 30, 130, 170, 60);
}
void c_time_show(c_time *self)
{
    static int count;
    char time[32];

    for (int i = 0; i < 10; i++)
    {
        for (int y = self->m_y; y < self->m_height + self->m_y; y += 19)
        {
            for (int x = self->m_x; x < self->m_width + self->m_x; x += 9)
            {
                if (c_rect_pt_in_rect(&self->m_time_rect, x, y) && c_rect_pt_in_rect(&self->m_time_rect, x + 9, y + 19))
                {
                    continue;
                }
                if (rand() % 2)
                {
                    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "0", x, y, c_theme_get_font(FONT_CUSTOM1), GL_RGB(141, 219, 153), GL_ARGB(255, 0, 0, 0));
                }
                else
                {
                    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "1", x, y, c_theme_get_font(FONT_CUSTOM1), GL_RGB(141, 219, 153), GL_ARGB(255, 0, 0, 0));
                }
            }
        }

        memset(time, 0, sizeof(time));
        guilite_sprintf(time, "16:47:%02d", count++ % 60);
        c_word_draw_string_in_rect(s_surface, Z_ORDER_LEVEL_0, time, &self->m_time_rect, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255),
                                   GL_ARGB(255, 0, 0, 0), ALIGN_HCENTER);
        guilite_thread_sleep(1000);
    }
}

//////////////////////// start UI ////////////////////////
extern const BITMAP_INFO background_bmp;
extern const LATTICE_FONT_INFO KaiTi_33B;
void load_resource(void)
{
    c_theme_init();
    c_theme_add_font(FONT_DEFAULT, &_DengXian_36B);
    c_theme_add_font(FONT_CUSTOM1, &Consolas_19);
}

void switchUI(void)
{
    static int index = 0;
    c_weather weather;
    c_clock clock;
    c_time time;

    c_weather_init(&weather);
    c_clock_init(&clock);
    c_time_init(&time);

    switch (index++ % 5)
    {
    case 0:
        c_weather_show(&weather);
        return;
    case 1:
        c_weather_disappear(&weather);
        s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH, 240, 0, Z_ORDER_LEVEL_0);
        return;
    case 2:
        s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH, 240, 0, Z_ORDER_LEVEL_0);
        c_clock_set_hands(&clock, 140.0, 100.0, 0.0, 40.0, 0.0, 2.0, GL_RGB(255, 255, 255), 0.0, 70.0, 0.0, 4.0, GL_RGB(255, 255, 255), 0.0, 90.0, 0.0, 6.0,
                          GL_RGB(237, 125, 124));
        c_clock_show(&clock);
        return;
    case 3:
        c_clock_set_hands(&clock, 100.0, 140.0, 40.0, 0.0, 2.0, 0.0, GL_RGB(255, 255, 255), 70.0, 0.0, 4.0, 0.0, GL_RGB(255, 255, 255), 90.0, 0.0, 6.0, 0.0,
                          GL_RGB(237, 125, 124));
        c_clock_show(&clock);
        s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH, 270, 0, Z_ORDER_LEVEL_0);
        break;
    case 4:
        c_time_show(&time);
        s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH, 240, 0, Z_ORDER_LEVEL_0);
        break;
    default:
        return;
    }
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    load_resource();
    static c_surface surface;
    static c_display display;
    c_surface_init_with_order(&surface, UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_0);
    c_display_init_with_surface(&display, phy_fb, screen_width, screen_height, &surface, driver);
    s_surface = &surface;
    s_display = &display;

    s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH, UI_HEIGHT, 0, Z_ORDER_LEVEL_0);
    c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &grass_bmp, 0, 271);

    while (1)
    {
        switchUI();
        guilite_thread_sleep(1000);
    }
}

//////////////////////// get current UI ////////////////////////
void *uicode_get_current_ui(int *width, int *height, bool force_update)
{
    // guilite_log("uicode_get_current_ui width: %d, height: %d, force_update: %d\n", *width, *height, force_update);
    if (s_display)
    {
        return c_display_get_updated_fb(s_display, width, height, force_update);
    }
    return NULL;
}

//////////////////////// on touch ////////////////////////
void uicode_on_touch(int x, int y, bool is_down)
{
}

//////////////////////// on navigate ////////////////////////
void uicode_on_navigate(unsigned int key)
{
}