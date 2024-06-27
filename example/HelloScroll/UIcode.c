#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"
#include "scroll.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

//////////////////////// define widgets & map message ////////////////////////
enum WND_ID
{
    ID_ROOT = 1,
    ID_SCROLL_VIEW_1,
    ID_SCROLL_VIEW_2,
    ID_BUTTON_1,
    ID_BUTTON_2,
    ID_BUTTON_3,
    ID_BUTTON_4,
    ID_BUTTON_5,
    ID_BUTTON_6,
    ID_BUTTON_7,
    ID_BUTTON_8,
    ID_BUTTON_9,
    ID_LABEL_1
};

extern const BITMAP_INFO image1_bmp, image2_bmp, image3_bmp, image4_bmp, image5_bmp, image6_bmp, image7_bmp, image8_bmp, image9_bmp;

typedef struct c_scroll_view_v
{
    c_scroll_view base;
} c_scroll_view_v;

void c_scroll_view_v_on_button_clicked(c_wnd *self, int ctrl_id, int param);

void c_scroll_view_v_vir_on_init_children(c_wnd *self)
{
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_1)), c_scroll_view_v_on_button_clicked);
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_2)), c_scroll_view_v_on_button_clicked);
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_3)), c_scroll_view_v_on_button_clicked);
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_4)), c_scroll_view_v_on_button_clicked);
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_5)), c_scroll_view_v_on_button_clicked);
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_6)), c_scroll_view_v_on_button_clicked);
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_7)), c_scroll_view_v_on_button_clicked);
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_8)), c_scroll_view_v_on_button_clicked);
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON_9)), c_scroll_view_v_on_button_clicked);

    c_scroll_view_vir_on_init_children(self);
}
void c_scroll_view_v_on_button_clicked(c_wnd *self, int ctrl_id, int param)
{
    switch (ctrl_id)
    {
    case ID_BUTTON_1:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image1_bmp, 230, 10);
        break;
    case ID_BUTTON_2:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image2_bmp, 230, 10);
        break;
    case ID_BUTTON_3:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image3_bmp, 230, 10);
        break;
    case ID_BUTTON_4:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image4_bmp, 410, 10);
        break;
    case ID_BUTTON_5:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image5_bmp, 230, 150);
        break;
    case ID_BUTTON_6:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image6_bmp, 410, 150);
        break;
    case ID_BUTTON_7:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image7_bmp, 230, 290);
        break;
    case ID_BUTTON_8:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image8_bmp, 230, 290);
        break;
    case ID_BUTTON_9:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &image9_bmp, 230, 290);
        break;
    default:
        GUILITE_ASSERT(false);
        break;
    }
}

static const c_wnd_vir_api c_scroll_view_v_vir_api_table = {
        c_wnd_vir_connect,     c_scroll_view_v_vir_on_init_children, c_scroll_view_vir_on_paint, c_scroll_view_vir_show_window, c_scroll_view_vir_on_touch,
        c_wnd_vir_on_navigate, c_scroll_view_vir_pre_create_wnd,     c_wnd_vir_on_focus,         c_wnd_vir_on_kill_focus,
};

void c_scroll_view_v_init(c_scroll_view_v *self)
{
    c_scroll_view_init(&self->base);
    self->base.base.m_api = &c_scroll_view_v_vir_api_table;
}

//////////////////////// layout UI ////////////////////////
// Layout Widgets
static c_wnd s_my_ui;
static c_scroll_view_v s_scroll_view_1;
static c_scroll_view s_scroll_view_2;
static c_button s_button_1, s_button_2, s_button_3, s_button_4, s_button_5, s_button_6, s_button_7, s_button_8, s_button_9;
static c_label label_1;

WND_TREE s_scroll_widgets_1[] = {{(c_wnd *)&s_button_1, ID_BUTTON_1, "Button 1", 0, 0, 220, 50},
                                 {(c_wnd *)&s_button_2, ID_BUTTON_2, "Button 2", 0, 60, 220, 50},
                                 {(c_wnd *)&s_button_3, ID_BUTTON_3, "Button 3", 0, 120, 220, 50},
                                 {(c_wnd *)&s_button_4, ID_BUTTON_4, "Button 4", 0, 180, 220, 50},
                                 {(c_wnd *)&s_button_5, ID_BUTTON_5, "Button 5", 0, 240, 220, 50},
                                 {(c_wnd *)&s_button_6, ID_BUTTON_6, "Button 6", 0, 300, 220, 50},
                                 {(c_wnd *)&s_button_7, ID_BUTTON_7, "Button 7", 0, 360, 220, 50},
                                 {(c_wnd *)&s_button_8, ID_BUTTON_8, "Button 8", 0, 420, 220, 50},
                                 {(c_wnd *)&s_button_9, ID_BUTTON_9, "Button 9", 0, 480, 220, 50},

                                 {NULL, 0, 0, 0, 0, 0, 0}};

WND_TREE s_scroll_widgets_2[] = {{(c_wnd *)&label_1, ID_LABEL_1, " You never fight alone; God bless Wuhan, God bless everyone of us!", 0, 0, 800, 100},

                                 {NULL, 0, 0, 0, 0, 0, 0}};

static WND_TREE s_main_widgets[] = {{(c_wnd *)&s_scroll_view_1, ID_SCROLL_VIEW_1, 0, 0, 0, 600, 290, s_scroll_widgets_1},
                                    {(c_wnd *)&s_scroll_view_2, ID_SCROLL_VIEW_2, 0, 0, 300, 600, 100, s_scroll_widgets_2},
                                    {NULL, 0, 0, 0, 0, 0, 0}};

//////////////////////// start UI ////////////////////////
extern const LATTICE_FONT_INFO Consolas_24B;
void load_resource(void)
{
    c_theme_init();
    // for button
    c_theme_add_font(FONT_DEFAULT, &Consolas_24B);
    c_theme_add_color(COLOR_WND_FONT, GL_RGB(0, 0, 0));
    c_theme_add_color(COLOR_WND_NORMAL, GL_RGB(220, 220, 220));
    c_theme_add_color(COLOR_WND_FOCUS, GL_RGB(180, 180, 180));

    // my_ui init
    c_wnd_init(&s_my_ui);
    // init child wnds.
    c_scroll_view_v_init(&s_scroll_view_1);
    c_scroll_view_init(&s_scroll_view_2);

    c_button_init(&s_button_1);
    c_button_init(&s_button_2);
    c_button_init(&s_button_3);
    c_button_init(&s_button_4);
    c_button_init(&s_button_5);
    c_button_init(&s_button_6);
    c_button_init(&s_button_7);
    c_button_init(&s_button_8);
    c_button_init(&s_button_9);

    c_label_init(&label_1);
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

    s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);

    c_wnd_set_surface(&s_my_ui, &surface);
    s_my_ui.m_api->connect(&s_my_ui, NULL, ID_ROOT, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_main_widgets);
    s_my_ui.m_api->show_window(&s_my_ui);

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
    is_down ? s_my_ui.m_api->on_touch(&s_my_ui, x, y, TOUCH_DOWN) : s_my_ui.m_api->on_touch(&s_my_ui, x, y, TOUCH_UP);
}

//////////////////////// on navigate ////////////////////////
void uicode_on_navigate(unsigned int key)
{
    s_my_ui.m_api->on_navigate(&s_my_ui, key);
}