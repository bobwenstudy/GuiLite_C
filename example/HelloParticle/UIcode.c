#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

#define EMITTER_X       120
#define EMITTER_Y       250
#define ACC_X           0
#define ACC_Y           1
#define PARTICAL_WITH   3
#define PARTICAL_HEIGHT 3

static c_surface *s_surface;
static c_display *s_display;

//////////////////////// define widgets & map message ////////////////////////

typedef struct c_particle
{
    int m_x, m_y, m_x_velocity, m_y_velocity;
} c_particle;

void c_particle_initialize(c_particle *self)
{
    self->m_x = EMITTER_X;
    self->m_y = EMITTER_Y;
    self->m_x_velocity = (rand() % 7) - 3;
    self->m_y_velocity = -15 - (rand() % 4);
}
void c_particle_move(c_particle *self)
{
    s_surface->m_api->fill_rect(s_surface, self->m_x, self->m_y, self->m_x + PARTICAL_WITH - 1, self->m_y + PARTICAL_HEIGHT - 1, 0,
                                Z_ORDER_LEVEL_0); // clear previous image
    self->m_x_velocity += ACC_X;
    self->m_y_velocity += ACC_Y;
    self->m_x += self->m_x_velocity;
    self->m_y += self->m_y_velocity;
    if (self->m_x < 0 || (self->m_x + PARTICAL_WITH - 1) >= UI_WIDTH || self->m_y < 0 || (self->m_y + PARTICAL_HEIGHT - 1) >= UI_HEIGHT)
    {
        c_particle_initialize(self);
    }
}
void c_particle_draw(c_particle *self)
{
    int red = rand() % 5 * 63;
    int green = rand() % 5 * 63;
    int blue = rand() % 5 * 63;
    s_surface->m_api->fill_rect(s_surface, self->m_x, self->m_y, self->m_x + PARTICAL_WITH - 1, self->m_y + PARTICAL_HEIGHT - 1, GL_RGB(red, green, blue),
                                Z_ORDER_LEVEL_0); // draw current image
}

//////////////////////// start UI ////////////////////////
extern const LATTICE_FONT_INFO Microsoft_YaHei_28;
void load_resource(void)
{
    c_theme_init();
    // for button
    c_theme_add_font(FONT_DEFAULT, &Microsoft_YaHei_28);
}

//////////////////////// start UI ////////////////////////
c_particle particle_array[100];
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

    s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, 0, Z_ORDER_LEVEL_0);

    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "\xe7\xa5\x9d\x47\x75\x69\x4c\x69\x74\x65\xe5\xbc\x80\xe5\x8f\x91\xe8\x80\x85\xef\xbc\x9a", 10, 10,
                       c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 0, 0), GL_ARGB(0, 0, 0, 0));
    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0,
                       "\xe5\xa4\xa9\xe5\xa5\xb3\xe6\x95\xa3\xe8\x8a\xb1\xef\xbc\x8c\xe6\x96\xb0\xe5\xb9\xb4\xe5\xbf\xab\xe4\xb9\x90\xe2\x9d\xa4", 10, 48,
                       c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 0, 0), GL_ARGB(0, 0, 0, 0));

    for (int i = 0; i < sizeof(particle_array) / sizeof(c_particle); i++)
    {
        c_particle_initialize(&particle_array[i]);
    }

    while (1)
    {
        for (int i = 0; i < sizeof(particle_array) / sizeof(c_particle); i++)
        {
            c_particle_move(&particle_array[i]);
            c_particle_draw(&particle_array[i]);
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