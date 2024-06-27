#include "GuiLite.h"
#include <stdlib.h>
#include <math.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

#define SQUARE_SIZE_1 10
#define SQUARE_SIZE_2 14
#define PI            3.14159

static c_surface *s_surface;
static c_display *s_display;

//////////////////////// define widgets & map message ////////////////////////
static volatile int s_mode;

typedef struct pendulum
{
    int r1;
    int r2;
    int x1;
    int x2;
    int y1;
    int y2;
    float m1;
    float m2;
    float a1;
    float a2;
    float a1_v;
    float a2_v;
    float g;
} pendulum;

void pendulum_init(pendulum *self)
{
    self->r1 = 90;
    self->r2 = 90;
    self->x1 = 0;
    self->x2 = 0;
    self->y1 = 0;
    self->y2 = 0;
    self->m1 = 25;
    self->m2 = 50;
    self->a1 = PI / 2;
    self->a2 = PI / 2;
    self->a1_v = 0;
    self->a2_v = 0;
    self->g = 0.5;
}

void pendulum_draw(pendulum *self, int x, int y, const char *text1, const char *text2)
{
    // caculate acceleration
    float num1 = -self->g * (2 * self->m1 + self->m2) * sin(self->a1);
    float num2 = -self->m2 * self->g * sin(self->a1 - 2 * self->a2);
    float num3 = -2 * sin(self->a1 - self->a2) * self->m2;
    float num4 = self->a2_v * self->a2_v * self->r2 + self->a1_v * self->a1_v * self->r1 * cos(self->a1 - self->a2);
    float den = self->r1 * (2 * self->m1 + self->m2 - self->m2 * cos(2 * self->a1 - 2 * self->a2));
    float a1_a = (num1 + num2 + num3 * num4) / den;

    num1 = 2 * sin(self->a1 - self->a2);
    num2 = (self->a1_v * self->a1_v * self->r1 * (self->m1 + self->m2));
    num3 = self->g * (self->m1 + self->m2) * cos(self->a1);
    num4 = self->a2_v * self->a2_v * self->r2 * self->m2 * cos(self->a1 - self->a2);
    den = self->r2 * (2 * self->m1 + self->m2 - self->m2 * cos(2 * self->a1 - 2 * self->a2));
    float a2_a = (num1 * (num2 + num3 + num4)) / den;

    self->a1_v += a1_a;
    self->a2_v += a2_a;
    self->a1 += self->a1_v;
    self->a2 += self->a2_v;
    // earse footprint
    c_surface_draw_line(s_surface, x, y, self->x1 + x, self->y1 + y, 0, Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, self->x1 + x, self->y1 + y, self->x2 + x, self->y2 + y, 0, Z_ORDER_LEVEL_0);

    if (text1 && text2)
    {
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, text1, self->x1 + x - 14, self->y1 + y - 10, c_theme_get_font(FONT_DEFAULT), 0, 0);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, text2, self->x2 + x - 18, self->y2 + y - 10, c_theme_get_font(FONT_DEFAULT), 0, 0);
    }
    else
    {
        c_rect tmp_1;
        c_rect_init(&tmp_1, self->x1 + x - SQUARE_SIZE_1 / 2, self->y1 + y - SQUARE_SIZE_1 / 2, SQUARE_SIZE_1, SQUARE_SIZE_1);
        c_rect tmp_2;
        c_rect_init(&tmp_2, self->x2 + x - SQUARE_SIZE_2 / 2, self->y2 + y - SQUARE_SIZE_2 / 2, SQUARE_SIZE_2, SQUARE_SIZE_2);
        c_surface_draw_rect_ex(s_surface, tmp_1, 0, 1, Z_ORDER_LEVEL_0);
        c_surface_draw_rect_ex(s_surface, tmp_2, 0, 1, Z_ORDER_LEVEL_0);
    }
    // draw new picture
    self->x1 = self->r1 * sin(self->a1);
    self->y1 = self->r1 * cos(self->a1);
    self->x2 = self->r2 * sin(self->a2) + self->x1;
    self->y2 = self->r2 * cos(self->a2) + self->y1;

    c_surface_draw_line(s_surface, x, y, self->x1 + x, self->y1 + y, GL_RGB(0, 162, 232), Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, self->x1 + x, self->y1 + y, self->x2 + x, self->y2 + y, GL_RGB(0, 162, 232), Z_ORDER_LEVEL_0);

    if (text1 && text2)
    {
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, text1, self->x1 + x - 14, self->y1 + y - 10, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255), 0);
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, text2, self->x2 + x - 18, self->y2 + y - 10, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255), 0);
    }
    else
    {
        c_rect tmp_1;
        c_rect_init(&tmp_1, self->x1 + x - SQUARE_SIZE_1 / 2, self->y1 + y - SQUARE_SIZE_1 / 2, SQUARE_SIZE_1, SQUARE_SIZE_1);
        c_rect tmp_2;
        c_rect_init(&tmp_2, self->x2 + x - SQUARE_SIZE_2 / 2, self->y2 + y - SQUARE_SIZE_2 / 2, SQUARE_SIZE_2, SQUARE_SIZE_2);
        c_surface_draw_rect_ex(s_surface, tmp_1, GL_RGB(255, 255, 255), 1, Z_ORDER_LEVEL_0);
        c_surface_draw_rect_ex(s_surface, tmp_2, GL_RGB(255, 255, 255), 1, Z_ORDER_LEVEL_0);
    }
}

void pendulum_draw_simple(pendulum *self, int x, int y)
{
    pendulum_draw(self, x, y, NULL, NULL);
}

//////////////////////// start UI ////////////////////////
extern const LATTICE_FONT_INFO Consolas_19;
void load_resource(void)
{
    c_theme_init();
    // for button
    c_theme_add_font(FONT_DEFAULT, &Consolas_19);
}

//////////////////////// start UI ////////////////////////
pendulum g_pendulum;
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

    s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, COLOR_BACKGROUND, Z_ORDER_LEVEL_0);

    pendulum_init(&g_pendulum);

    while (true)
    {
        s_mode ? pendulum_draw(&g_pendulum, 120, 100, "Gui", "Lite") : pendulum_draw_simple(&g_pendulum, 120, 100);
        guilite_thread_sleep(30);
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
    s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, COLOR_BACKGROUND, Z_ORDER_LEVEL_0);
    s_mode = s_mode ? 0 : 1;
}