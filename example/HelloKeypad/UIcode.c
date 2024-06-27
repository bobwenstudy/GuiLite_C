#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

//////////////////////// define widgets & map message ////////////////////////
enum WND_ID
{
    ID_ROOT = 1,
    ID_LABEL1,
    ID_LABEL2,
    ID_LABEL3,
    ID_BUTTON1,
    ID_BUTTON2,
    ID_BUTTON3
};

typedef struct c_my_ui
{
    c_wnd base;
} c_my_ui;

void c_my_ui_on_clicked(c_wnd *self, int ctrl_id, int param);

void c_my_ui_vir_on_init_children(c_wnd *self)
{
    c_button *button1 = (c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON1);
    c_button_set_on_click(button1, (WND_CALLBACK)&c_my_ui_on_clicked);
    c_button *button2 = (c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON2);
    c_button_set_on_click(button2, (WND_CALLBACK)&c_my_ui_on_clicked);
    c_button *button3 = (c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON3);
    c_button_set_on_click(button3, (WND_CALLBACK)&c_my_ui_on_clicked);
}

void c_my_ui_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    c_surface_fill_rect_ex(self->m_surface, rect, COLOR_BACKGROUND, self->m_z_order);
}

void c_my_ui_on_clicked(c_wnd *self, int ctrl_id, int param)
{
    static int sum1, sum2, sum3;
    static char str1[8], str2[8], str3[8];
    c_button *button = (c_button *)c_wnd_get_wnd_ptr(self, ctrl_id);
    switch (ctrl_id)
    {
    case ID_BUTTON1:
        guilite_sprintf(str1, "%d", ++sum1);
        c_wnd_set_str(&button->base, str1);
        break;
    case ID_BUTTON2:
        guilite_sprintf(str2, "%d", ++sum2);
        c_wnd_set_str(&button->base, str2);
        break;
    case ID_BUTTON3:
        guilite_sprintf(str3, "%d", ++sum3);
        c_wnd_set_str(&button->base, str3);
        break;
    }
    button->base.m_api->show_window(&button->base);
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
static c_label s_label1, s_label2, s_label3;
static c_button s_button1, s_button2, s_button3;

static WND_TREE s_main_widgets[] = {
        {(c_wnd *)&s_label1, ID_LABEL1, "a: <<", 20, 20, 80, 40, NULL},     {(c_wnd *)&s_label2, ID_LABEL2, "d: >>", 20, 140, 80, 40, NULL},
        {(c_wnd *)&s_label3, ID_LABEL3, "s: click", 20, 260, 80, 40, NULL},

        {(c_wnd *)&s_button1, ID_BUTTON1, "0", 140, 20, 80, 40, NULL},      {(c_wnd *)&s_button2, ID_BUTTON2, "0", 140, 140, 80, 40, NULL},
        {(c_wnd *)&s_button3, ID_BUTTON3, "0", 140, 260, 80, 40, NULL},     {NULL, 0, 0, 0, 0, 0, 0}};

//////////////////////// start UI ////////////////////////
extern const LATTICE_FONT_INFO Consolas_28;
void load_resource(void)
{
    c_theme_init();
    // for button
    c_theme_add_font(FONT_DEFAULT, &Consolas_28);
    c_theme_add_color(COLOR_WND_FONT, GL_RGB(255, 255, 243));
    c_theme_add_color(COLOR_WND_NORMAL, GL_RGB(59, 75, 94));
    c_theme_add_color(COLOR_WND_PUSHED, GL_RGB(33, 42, 53));
    c_theme_add_color(COLOR_WND_FOCUS, GL_RGB(78, 198, 76));
    c_theme_add_color(COLOR_WND_BORDER, GL_RGB(46, 59, 73));

    // my_ui init
    c_my_ui_init(&s_my_ui);
    // init child wnds.
    c_label_init(&s_label1);
    c_label_init(&s_label2);
    c_label_init(&s_label3);

    c_button_init(&s_button1);
    c_button_init(&s_button2);
    c_button_init(&s_button3);
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