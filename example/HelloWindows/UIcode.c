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
    ID_START_BUTTON,
    ID_START_MENU
};

typedef struct c_start_button
{
    c_button base;
} c_start_button;

void c_start_button_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);

    extern const BITMAP_INFO start_icon_bmp;
    extern const BITMAP_INFO start_icon_click_bmp;
    switch (self->m_status)
    {
    case STATUS_NORMAL:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &start_icon_bmp, rect.m_left, rect.m_top);
        break;
    case STATUS_FOCUSED:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &start_icon_bmp, rect.m_left, rect.m_top);
        break;
    case STATUS_PUSHED:
        c_image_draw_image_simple(self->m_surface, self->m_z_order, &start_icon_click_bmp, rect.m_left, rect.m_top);
        break;
    default:
        GUILITE_ASSERT(false);
        break;
    }
}

static const c_wnd_vir_api c_start_button_vir_api_table = {
        c_wnd_vir_connect,        c_wnd_vir_on_init_children,  c_start_button_vir_on_paint, c_wnd_vir_show_window,      c_button_vir_on_touch,
        c_button_vir_on_navigate, c_button_vir_pre_create_wnd, c_button_vir_on_focus,       c_button_vir_on_kill_focus,
};

void c_start_button_init(c_start_button *self)
{
    c_button_init(&self->base);
    self->base.base.m_api = &c_start_button_vir_api_table;
}

typedef struct c_my_ui
{
    c_wnd base;
} c_my_ui;

void c_my_ui_on_clicked(c_wnd *self, int ctrl_id, int param);

void c_my_ui_vir_on_init_children(c_wnd *self)
{
    c_button_set_on_click((c_button *)c_wnd_get_wnd_ptr(self, ID_START_BUTTON), c_my_ui_on_clicked);
}

void c_my_ui_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    extern const BITMAP_INFO desktop_bmp;
    int block_width = 80;
    int block_height = 60;
    int block_rows = desktop_bmp.height / block_height;
    int block_cols = desktop_bmp.width / block_width;
    int block_sum = block_rows * block_cols;
    bool *block_map = (bool *)guilite_calloc(block_sum, sizeof(bool));
    if (!block_map)
    {
        return;
    }
    int sum = 0;
    while (sum < block_sum)
    {
        int x = rand() % block_cols;
        int y = rand() % block_rows;
        if (block_map[x + (y * block_cols)] == false)
        {
            c_image_draw_image_with_src_simple(self->m_surface, self->m_z_order, &desktop_bmp, rect.m_left + x * block_width, rect.m_top + y * block_height,
                                               x * block_width, y * block_height, block_width, block_height);
            block_map[x + (y * block_cols)] = true;
            sum++;
            guilite_thread_sleep(10);
        }
    }
    guilite_free(block_map);
}

void c_my_ui_on_clicked(c_wnd *self, int ctrl_id, int param)
{
    static bool is_open = false;
    (is_open) ? c_dialog_close_dialog(NULL, self->m_surface) : c_dialog_open_dialog(NULL, (c_dialog *)c_wnd_get_wnd_ptr(self, ID_START_MENU), false);
    is_open = !is_open;
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

typedef struct c_start_menu
{
    c_dialog base;
} c_start_menu;

void c_start_menu_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    extern const BITMAP_INFO start_menu_bmp;
    int step = 10;
    for (int i = 0; i < start_menu_bmp.height; i += step)
    {
        guilite_thread_sleep(10);
        c_image_draw_image_with_src(self->m_surface, self->m_z_order, &start_menu_bmp, rect.m_left, rect.m_top + i, 0, i, start_menu_bmp.width, step,
                                    GL_RGB(0, 0, 0));
    }
}

static const c_wnd_vir_api c_start_menu_vir_api_table = {
        c_wnd_vir_connect,     c_wnd_vir_on_init_children,  c_start_menu_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_dialog_vir_pre_create_wnd, c_wnd_vir_on_focus,        c_wnd_vir_on_kill_focus,
};

void c_start_menu_init(c_start_menu *self)
{
    c_dialog_init(&self->base);
    self->base.base.m_api = &c_start_menu_vir_api_table;
}

//////////////////////// layout UI ////////////////////////
static c_my_ui s_my_ui;
static c_start_button s_start_button;
static c_start_menu s_start_menu;

static WND_TREE s_main_widgets[] = {{(c_wnd *)&s_start_menu, ID_START_MENU, 0, 400, 50, 475, 633, NULL},
                                    {(c_wnd *)&s_start_button, ID_START_BUTTON, 0, 0, 682, 47, 38, NULL},
                                    {NULL, 0, 0, 0, 0, 0, 0}};

//////////////////////// start UI ////////////////////////
void load_resource(void)
{
    c_theme_init();

    // my_ui init
    c_my_ui_init(&s_my_ui);
    // init child wnds.
    c_start_menu_init(&s_start_menu);
    c_start_button_init(&s_start_button);
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    load_resource();
    static c_surface surface;
    static c_display display;
    c_surface_init_with_order(&surface, UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_1);
    c_display_init_with_surface(&display, phy_fb, screen_width, screen_height, &surface, driver);
    s_surface = &surface;
    s_display = &display;

    c_wnd_set_surface(&s_my_ui.base, &surface);
    s_my_ui.base.m_api->connect(&s_my_ui.base, NULL, ID_DESKTOP, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_main_widgets);
    s_my_ui.base.m_api->show_window(&s_my_ui.base);

    while (1)
    {
        guilite_thread_sleep(1000000);
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