#include "GuiLite.h"
#include <stdlib.h>
#include <math.h>
#include "UIcode.h"

#define POINT_COL     25
#define POINT_ROW     20
#define X_SPACE       18
#define Y_SPACE       25
#define STRING_LENGHT 12

#define FRICTION    0.98
#define GRAVITY_ACC 0.05
#define DELTA_TIME  0.1

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

typedef struct
{
    float m_x, m_y, m_vx, m_vy;
    float m_last_x, m_last_y;
    bool m_fixed;
} c_point;

void c_point_set(c_point *self, float x, float y, bool fixed)
{
    self->m_x = self->m_last_x = x;
    self->m_y = self->m_last_y = y;
    self->m_fixed = fixed;
    self->m_vx = self->m_vy = 0;
}
void c_point_update_location(c_point *self, float dt)
{
    if (self->m_fixed)
    {
        return;
    }
    self->m_last_x = self->m_x;
    self->m_last_y = self->m_y;
    s_surface->m_api->draw_pixel(s_surface, self->m_x, self->m_y, GL_RGB(0, 0, 0), Z_ORDER_LEVEL_0);
    self->m_vx *= FRICTION;
    self->m_vy *= FRICTION;
    self->m_x += self->m_vx * dt;
    self->m_y += self->m_vy * dt;
    s_surface->m_api->draw_pixel(s_surface, self->m_x, self->m_y, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);
}
float c_point_distance(c_point *self, c_point *p)
{
    float dx = self->m_x - p->m_x;
    float dy = self->m_y - p->m_y;
    return sqrt(dx * dx + dy * dy);
}

typedef struct
{
    c_point *m_p1, *m_p2;
    float m_length;
} c_string;

void c_string_set(c_string *self, c_point *p1, c_point *p2, float length)
{
    self->m_p1 = p1;
    self->m_p2 = p2;
    self->m_length = length;
}
void c_string_update_point_velocity(c_string *self)
{
    float force = (c_point_distance(self->m_p1, self->m_p2) - self->m_length) / 2;
    float dx = self->m_p1->m_x - self->m_p2->m_x;
    float dy = self->m_p1->m_y - self->m_p2->m_y;
    float d = sqrt(dx * dx + dy * dy);

    float nx = dx / d;
    float ny = dy / d;
    if (!self->m_p1->m_fixed)
    {
        self->m_p1->m_vx -= nx * force;
        self->m_p1->m_vy -= ny * force;
        self->m_p1->m_vy += GRAVITY_ACC;
    }
    if (!self->m_p2->m_fixed)
    {
        self->m_p2->m_vx += nx * force;
        self->m_p2->m_vy += ny * force;
        self->m_p2->m_vy += GRAVITY_ACC;
    }
}
void c_string_draw(c_string *self)
{
    if (self->m_p1->m_x == self->m_p1->m_last_x && self->m_p1->m_y == self->m_p1->m_last_y && self->m_p2->m_x == self->m_p2->m_last_x &&
        self->m_p2->m_y == self->m_p2->m_last_y)
    {
        return;
    }
    c_surface_draw_line(s_surface, self->m_p1->m_last_x, self->m_p1->m_last_y, self->m_p2->m_last_x, self->m_p2->m_last_y, GL_RGB(0, 0, 0), Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, self->m_p1->m_x, self->m_p1->m_y, self->m_p2->m_x, self->m_p2->m_y, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);
}

c_point points[POINT_COL][POINT_ROW];
c_string strings[(POINT_COL - 1) * POINT_ROW + POINT_COL * (POINT_ROW - 1)];

void trigger(int x, int y, bool is_down)
{
    if (is_down)
    {
        points[POINT_COL / 2][POINT_ROW / 2].m_x = points[POINT_COL / 2][POINT_ROW / 2].m_last_x = x;
        points[POINT_COL / 2][POINT_ROW / 2].m_y = points[POINT_COL / 2][POINT_ROW / 2].m_last_y = y;
        points[POINT_COL / 2][POINT_ROW / 2].m_fixed = true;
    }
    else
    {
        points[POINT_COL / 2][POINT_ROW / 2].m_fixed = false;
    }
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    static c_surface surface;
    static c_display display;
    c_surface_init_with_order(&surface, UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_0);
    c_display_init_with_surface(&display, phy_fb, screen_width, screen_height, &surface, driver);
    s_surface = &surface;
    s_display = &display;

    s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, COLOR_BACKGROUND, Z_ORDER_LEVEL_0);

    // init points
    for (int y = 0; y < POINT_ROW; y++)
    {
        for (int x = 0; x < POINT_COL; x++)
        {
            c_point_set(&points[x][y], X_SPACE * 2 + x * X_SPACE, Y_SPACE * 2 + y * Y_SPACE, y == 0);
        }
    }

    // init strings
    int sum = 0;
    for (int y = 0; y < POINT_ROW; y++)
    {
        for (int x = 0; x < POINT_COL - 1; x++)
        {
            c_string_set(&strings[sum++], &points[x][y], &points[x + 1][y], STRING_LENGHT);
        }
    }
    for (int y = 0; y < POINT_ROW - 1; y++)
    {
        for (int x = 0; x < POINT_COL; x++)
        {
            c_string_set(&strings[sum++], &points[x][y], &points[x][y + 1], STRING_LENGHT);
        }
    }
    GUILITE_ASSERT(sum == sizeof(strings) / sizeof(c_string));

    // update
    while (1)
    {
        for (int i = 0; i < sum; i++)
        {
            c_string_update_point_velocity(&strings[i]);
        }

        for (int y = 0; y < POINT_ROW; y++)
        {
            for (int x = 0; x < POINT_COL; x++)
            {
                c_point_update_location(&points[x][y], DELTA_TIME);
            }
        }

        for (int i = 0; i < sum; i++)
        {
            c_string_draw(&strings[i]);
        }
        guilite_thread_sleep(10);
#if 0
		// Auto trigger for MCU
		static int count;
		if (++count % 500 == 0)
		{
			trigger(0, 0, true);
			trigger(0, 0, false);
		}
#endif
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
    trigger(x, y, is_down);
}

//////////////////////// on navigate ////////////////////////
void uicode_on_navigate(unsigned int key)
{
}
