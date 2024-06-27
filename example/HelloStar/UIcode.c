#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

typedef struct
{
    int m_start_x, m_start_y;
    float m_x, m_y, m_x_factor, m_y_factor, m_size_factor, m_size;
} c_star;

void c_star_initialize(c_star *self)
{
    self->m_x = self->m_start_x = rand() % UI_WIDTH;
    self->m_y = self->m_start_y = rand() % UI_HEIGHT;
    self->m_size = 1;
    self->m_x_factor = UI_WIDTH;
    self->m_y_factor = UI_HEIGHT;
    self->m_size_factor = 1;
}

void c_star_move(c_star *self)
{
    s_surface->m_api->fill_rect(s_surface, self->m_x, self->m_y, self->m_x + self->m_size - 1, self->m_y + self->m_size - 1, COLOR_BACKGROUND,
                                Z_ORDER_LEVEL_0); // clear star footprint

    self->m_x_factor -= 6;
    self->m_y_factor -= 6;
    self->m_size += self->m_size / 20;
    if (self->m_x_factor < 1 || self->m_y_factor < 1)
    {
        c_star_initialize(self);
        return;
    }
    if (self->m_start_x > (UI_WIDTH / 2) && self->m_start_y > (UI_HEIGHT / 2))
    {
        self->m_x = (UI_WIDTH / 2) + (UI_WIDTH * (self->m_start_x - (UI_WIDTH / 2)) / self->m_x_factor);
        self->m_y = (UI_HEIGHT / 2) + (UI_HEIGHT * (self->m_start_y - (UI_HEIGHT / 2)) / self->m_y_factor);
    }
    else if (self->m_start_x <= (UI_WIDTH / 2) && self->m_start_y > (UI_HEIGHT / 2))
    {
        self->m_x = (UI_WIDTH / 2) - (UI_WIDTH * ((UI_WIDTH / 2) - self->m_start_x) / self->m_x_factor);
        self->m_y = (UI_HEIGHT / 2) + (UI_HEIGHT * (self->m_start_y - (UI_HEIGHT / 2)) / self->m_y_factor);
    }
    else if (self->m_start_x > (UI_WIDTH / 2) && self->m_start_y <= (UI_HEIGHT / 2))
    {
        self->m_x = (UI_WIDTH / 2) + (UI_WIDTH * (self->m_start_x - (UI_WIDTH / 2)) / self->m_x_factor);
        self->m_y = (UI_HEIGHT / 2) - (UI_HEIGHT * ((UI_HEIGHT / 2) - self->m_start_y) / self->m_y_factor);
    }
    else if (self->m_start_x <= (UI_WIDTH / 2) && self->m_start_y <= (UI_HEIGHT / 2))
    {
        self->m_x = (UI_WIDTH / 2) - (UI_WIDTH * ((UI_WIDTH / 2) - self->m_start_x) / self->m_x_factor);
        self->m_y = (UI_HEIGHT / 2) - (UI_HEIGHT * ((UI_HEIGHT / 2) - self->m_start_y) / self->m_y_factor);
    }

    if (self->m_x < 0 || (self->m_x + self->m_size - 1) >= UI_WIDTH || self->m_y < 0 || (self->m_y + self->m_size - 1) >= UI_HEIGHT)
    {
        c_star_initialize(self);
        return;
    }
    s_surface->m_api->fill_rect(s_surface, self->m_x, self->m_y, self->m_x + self->m_size - 1, self->m_y + self->m_size - 1, GL_RGB_WHITE,
                                Z_ORDER_LEVEL_0); // draw star
}

//////////////////////// start UI ////////////////////////
c_star stars[100];
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

    // initialize stars
    for (int i = 0; i < sizeof(stars) / sizeof(c_star); i++)
    {
        c_star_initialize(&stars[i]);
    }

    while (1)
    {
        for (int i = 0; i < sizeof(stars) / sizeof(c_star); i++)
        {
            c_star_move(&stars[i]);
        }
        guilite_thread_sleep(50);
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
