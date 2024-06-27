#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

enum WND_ID
{
    ID_ROOT = 1,
    ID_LABEL,
    ID_BUTTON,
};

typedef struct c_my_ui
{
    c_wnd base;
} c_my_ui;

void c_my_ui_on_button_clicked(c_wnd *self, int ctrl_id, int param);

void c_my_ui_vir_on_init_children(c_wnd *self)
{
    c_button *button = (c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON);
    c_button_set_on_click(button, c_my_ui_on_button_clicked);
}

void c_my_ui_vir_on_paint(c_wnd *self)
{
    // c_surface_draw_rect_simple(self->m_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, GL_RGB(0, 255, 0), Z_ORDER_LEVEL_0);
    // c_surface_draw_rect_simple(self->m_surface, 2, 2, UI_WIDTH - 3, UI_HEIGHT - 3, GL_RGB(0, 255, 0), Z_ORDER_LEVEL_0);
}
static const char test_str[][30] = {"Hello GuiLite", "Welcome to GuiLite", "Enjoy it"};

static int str_index = 0;
void c_my_ui_on_button_clicked(c_wnd *self, int ctrl_id, int param)
{
    c_label *label = (c_label *)c_wnd_get_wnd_ptr(self, ID_LABEL);
    c_wnd_set_str(&label->base, test_str[str_index ++]);
    label->base.m_api->show_window(&label->base);
    if(str_index >= sizeof(test_str) / sizeof(test_str[0]))
    {
        str_index = 0;
    }
    
    // c_word_draw_value(s_surface, Z_ORDER_LEVEL_0, 123456, 3, 100, 100, c_theme_get_font(FONT_DEFAULT), GL_RGB(0, 255, 00),
    //                    GL_ARGB(0, 0, 0, 0));
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

// Layout Widgets
static c_my_ui s_my_ui;
static c_label s_label;
static c_button s_button;

WND_TREE s_main_widgets[] = {
                             {(c_wnd *)&s_label, ID_LABEL, "label", 10, 20, 230, 50},
                             {(c_wnd *)&s_button, ID_BUTTON, "button", 50, 100, 100, 50},
                             {NULL, 0, 0, 0, 0, 0, 0}};

// Create GUI
extern const LATTICE_FONT_INFO Arial_18;
extern const LATTICE_FONT_INFO Arial_23;
extern const LATTICE_FONT_INFO Consolas_24B;
void load_resource(void)
{
    c_theme_init();
    // for button
    c_theme_add_font(FONT_DEFAULT, &Arial_18);
    c_theme_add_color(COLOR_WND_FONT, GL_RGB(255, 255, 243));
    c_theme_add_color(COLOR_WND_NORMAL, GL_RGB(59, 75, 94));
    c_theme_add_color(COLOR_WND_PUSHED, GL_RGB(33, 42, 53));
    c_theme_add_color(COLOR_WND_FOCUS, GL_RGB(78, 198, 76));
    c_theme_add_color(COLOR_WND_BORDER, GL_RGB(46, 59, 73));

    // my_ui init
    c_my_ui_init(&s_my_ui);
    // init child wnds.
    c_label_init(&s_label);
    c_button_init(&s_button);
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    load_resource();
    static c_surface surface;
    static c_display display;
    c_surface_init_with_order(&surface, UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_2);
    c_display_init_with_surface(&display, phy_fb, screen_width, screen_height, &surface, driver);
    s_surface = &surface;
    s_display = &display;

    c_wnd_set_surface(&s_my_ui.base, &surface);
    s_my_ui.base.m_api->connect(&s_my_ui.base, NULL, ID_ROOT, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_main_widgets);
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