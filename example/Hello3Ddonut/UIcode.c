#include "GuiLite.h"
#include <stdlib.h>
#include <math.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

#define WIDTH    50
#define X_OFFSET 20
#define X_RADIUS 25
#define HEIGHT   22
#define Y_OFFSET 12
#define Y_RADIUS 15

static char frame_buffer[WIDTH * HEIGHT + 1];
void build_frame(void)
{
    static float A, B;
    static float z[WIDTH * HEIGHT];
    memset(frame_buffer, ' ', sizeof(frame_buffer));
    memset(z, 0, sizeof(z));

    // calculate pixel
    for (float j = 0; j < 6.28; j += 0.07)
    { // angle 2*pi
        for (float i = 0; i < 6.28; i += 0.02)
        { // angle 2*pi
            float c = sin(i);
            float d = cos(j);
            float e = sin(A);
            float f = sin(j);
            float g = cos(A);
            float h = d + 2;
            float D = 1 / (c * h * e + f * g + 5);
            float l = cos(i);
            float m = cos(B);
            float n = sin(B);
            float t = c * h * g - f * e;
            int x = X_OFFSET + X_RADIUS * D * (l * h * m - t * n);
            int y = Y_OFFSET + Y_RADIUS * D * (l * h * n + t * m);
            int o = x + WIDTH * y;
            int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
            if (HEIGHT > y && y > 0 && x > 0 && WIDTH > x && D > z[o])
            {
                z[o] = D;
                frame_buffer[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
            }
        }
    }
    A += 0.00004 * WIDTH * HEIGHT;
    B += 0.00002 * WIDTH * HEIGHT;
}

void render_frame(void)
{
    for (int i = 0; i < HEIGHT; i++)
    {
        char tmp = frame_buffer[(i + 1) * WIDTH];
        frame_buffer[(i + 1) * WIDTH] = 0;
        c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, &frame_buffer[i * WIDTH], 0, i * 13, c_theme_get_font(FONT_DEFAULT), GL_RGB(245, 192, 86),
                           GL_RGB(0, 0, 0));
        frame_buffer[(i + 1) * WIDTH] = tmp;
    }
}

//////////////////////// start UI ////////////////////////
extern const LATTICE_FONT_INFO Consolas_13;
void load_resource(void)
{
    c_theme_init();
    c_theme_add_font(FONT_DEFAULT, &Consolas_13);
}
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

    while (1)
    {
        build_frame();
        render_frame();
        guilite_thread_sleep(5);
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
