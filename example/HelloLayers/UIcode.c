#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

#define LAYER_1_X 70
#define LAYER_1_Y 100

#define SHADES_CNT     10
#define SHADE_HEIGHT   10
#define LAYER_1_WIDTH  80
#define LAYER_1_HEIGHT SHADES_CNT *SHADE_HEIGHT

static c_surface *s_surface;
static c_display *s_display;
c_rect overlapped_rect;

//////////////////////// start UI ////////////////////////
void draw_on_layer_0(void)
{
    int const start_y = 70;
    c_rect tmp_rect;
    c_rect_init(&tmp_rect, 0, 0, UI_WIDTH, UI_HEIGHT);
    c_surface_fill_rect_ex(s_surface, tmp_rect, 0, Z_ORDER_LEVEL_0);
    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "We could not to do", 30, start_y + 30, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255),
                       GL_ARGB(0, 0, 0, 0));
    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "everything for you.", 30, start_y + 50, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255),
                       GL_ARGB(0, 0, 0, 0));
    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "", 30, start_y + 50, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255), GL_ARGB(0, 0, 0, 0));

    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "Just keep GUI:", 30, start_y + 90, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255),
                       GL_ARGB(0, 0, 0, 0));
    c_word_draw_string(s_surface, Z_ORDER_LEVEL_0, "stupid simple.", 30, start_y + 110, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255),
                       GL_ARGB(0, 0, 0, 0));
}

void draw_on_layer_1(void)
{
    // activate the layer 1.
    c_surface_activate_layer(s_surface, overlapped_rect, Z_ORDER_LEVEL_1);
    // draw everything inside the active rect.
    c_rect tmp_rect;
    c_rect_init(&tmp_rect, LAYER_1_X, LAYER_1_Y, LAYER_1_WIDTH, LAYER_1_HEIGHT);
    c_surface_fill_rect_ex(s_surface, tmp_rect, GL_RGB(0, 122, 204), Z_ORDER_LEVEL_1);
    c_word_draw_string(s_surface, Z_ORDER_LEVEL_1, "GuiLite", LAYER_1_X + 5, LAYER_1_Y + 20, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255),
                       GL_RGB(0, 122, 204));
    c_word_draw_string(s_surface, Z_ORDER_LEVEL_1, "4 KLOC", LAYER_1_X + 5, LAYER_1_Y + 60, c_theme_get_font(FONT_DEFAULT), GL_RGB(255, 255, 255),
                       GL_RGB(0, 122, 204));
}

void clear_layer_1(void)
{
    c_rect tmp_rect;
    c_rect_init_empty(&tmp_rect);
    c_surface_activate_layer(s_surface, tmp_rect, Z_ORDER_LEVEL_1); // inactivate the layer 1 by empty rect, and show the layers below.
}

extern const LATTICE_FONT_INFO Consolas_19;
void load_resource(void)
{
    c_theme_init();
    // for button
    c_theme_add_font(FONT_DEFAULT, &Consolas_19);

    c_rect_init(&overlapped_rect, LAYER_1_X, LAYER_1_Y, LAYER_1_WIDTH, LAYER_1_HEIGHT);
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    load_resource();
    static c_surface surface;
    static c_display display;
    c_surface_init(&surface, UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_1, overlapped_rect);
    c_display_init_with_surface(&display, phy_fb, screen_width, screen_height, &surface, driver);
    s_surface = &surface;
    s_display = &display;

    draw_on_layer_0();
    while (1)
    {
        draw_on_layer_1();
        guilite_thread_sleep(3000);
        clear_layer_1();
        guilite_thread_sleep(3000);
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