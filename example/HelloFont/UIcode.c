#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

//////////////////////// define widgets & map message ////////////////////////
enum WND_ID
{
    ID_DESKTOP = 1,
    ID_BUTTON
};

#define FONT_SIZE 57
#define START_X   300
#define START_Y   20

#ifdef WIN32
// For Visual studio, one chinese character need 3 UTF-8 bytes
static const char *s_text = "\xe6\x9c\x9d\xe8\xbe\x9e\xe7\x99\xbd\xe5\xb8\x9d\xe5\xbd\xa9\xe4\xba\x91\xe9\x97\xb4\xe5\x8d\x83\xe9\x87\x8c\xe6\xb1\x9f\xe9\x99"
                            "\xb5\xe4\xb8\x80\xe6\x97\xa5\xe8\xbf\x98\xe4\xb8\xa4\xe5\xb2\xb8\xe7\x8c\xbf\xe5\xa3\xb0\xe5\x95\xbc\xe4\xb8\x8d\xe4\xbd\x8f\xe8"
                            "\xbd\xbb\xe8\x88\x9f\xe5\xb7\xb2\xe8\xbf\x87\xe4\xb8\x87\xe9\x87\x8d\xe5\xb1\xb1";
#else
// For Unix, all chinese charaters would be encoded in UTF-8 perfectly.
static const char *s_text = "朝辞白帝彩云间千里江陵一日还两岸猿声啼不住轻舟已过万重山";
#endif

typedef struct c_my_ui
{
    c_wnd base;
} c_my_ui;

void c_my_ui_on_clicked(c_wnd *self, int ctrl_id, int param);

void c_my_ui_vir_on_init_children(c_wnd *self)
{
    c_button *button = (c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON);
    c_button_set_on_click(button, (WND_CALLBACK)&c_my_ui_on_clicked);
}

void c_my_ui_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    c_image_draw_image_simple(self->m_surface, self->m_z_order, c_theme_get_image(IMAGE_CUSTOM1), rect.m_left, rect.m_top);
}

void c_my_ui_on_clicked(c_wnd *self, int ctrl_id, int param)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    c_image_draw_image_simple(self->m_surface, self->m_z_order, c_theme_get_image(IMAGE_CUSTOM1), rect.m_left, rect.m_top);

    char one_word[4];
    const char *tmp = s_text;
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 7; y++)
        {
            memset(one_word, 0, sizeof(one_word));
            memcpy(one_word, tmp, 3);
            c_word_draw_string(self->m_surface, self->m_z_order, one_word, (START_X - x * FONT_SIZE), (START_Y + y * FONT_SIZE), c_theme_get_font(FONT_DEFAULT),
                               GL_RGB(0, 0, 0), GL_ARGB(0, 0, 0, 0));
            guilite_thread_sleep(500);
            tmp += 3;
        }
}

static const c_wnd_vir_api c_my_ui_vir_api_table = {
        c_wnd_vir_connect,     c_my_ui_vir_on_init_children, c_my_ui_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_wnd_vir_pre_create_wnd,     c_wnd_vir_on_focus,   c_wnd_vir_on_kill_focus,
};

void c_my_ui_init(c_my_ui *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_my_ui_vir_api_table;
}

//////////////////////// layout UI ////////////////////////
static c_my_ui s_my_ui;
static c_button s_button;

static WND_TREE s_main_widgets[] = {{(c_wnd *)&s_button, ID_BUTTON, "PLAY", 0, 540, 150, 40, NULL}, {NULL, 0, 0, 0, 0, 0, 0}};

//////////////////////// start UI ////////////////////////
extern const BITMAP_INFO background_bmp;
extern const LATTICE_FONT_INFO KaiTi_33B;
void load_resource(void)
{
    c_theme_init();
    c_theme_add_image(IMAGE_CUSTOM1, &background_bmp);
    // for button
    c_theme_add_font(FONT_DEFAULT, &KaiTi_33B);
    c_theme_add_color(COLOR_WND_FONT, GL_RGB(255, 255, 255));
    c_theme_add_color(COLOR_WND_NORMAL, GL_RGB(59, 75, 94));
    c_theme_add_color(COLOR_WND_PUSHED, GL_RGB(33, 42, 53));
    c_theme_add_color(COLOR_WND_FOCUS, GL_RGB(43, 118, 219));
    c_theme_add_color(COLOR_WND_BORDER, GL_RGB(46, 59, 73));

    // my_ui init
    c_my_ui_init(&s_my_ui);
    // init child wnds.
    c_button_init(&s_button);
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

    c_wnd_set_surface(&s_my_ui.base, &surface);
    s_my_ui.base.m_api->connect(&s_my_ui.base, NULL, ID_DESKTOP, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_main_widgets);
    s_my_ui.base.m_api->show_window(&s_my_ui.base);

    while (1)
    {
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
    is_down ? s_my_ui.base.m_api->on_touch(&s_my_ui.base, x, y, TOUCH_DOWN) : s_my_ui.base.m_api->on_touch(&s_my_ui.base, x, y, TOUCH_UP);
}

//////////////////////// on navigate ////////////////////////
void uicode_on_navigate(unsigned int key)
{
    s_my_ui.base.m_api->on_navigate(&s_my_ui.base, key);
}