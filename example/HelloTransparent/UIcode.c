#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"
#include "surface_transparent.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

static const unsigned char s_wave_data1[] = {
        0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
        0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
        0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
        0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
        0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7E, 0x7E, 0x7F, 0x81, 0x81, 0x82, 0x82, 0x83,
        0x84, 0x84, 0x84, 0x84, 0x83, 0x82, 0x82, 0x81, 0x81, 0x7F, 0x7F, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
        0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x7A, 0x82, 0x89, 0x91, 0x98, 0x9F, 0xA2, 0x9B, 0x94, 0x8C,
        0x85, 0x7E, 0x78, 0x79, 0x7A, 0x7B, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
        0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7E, 0x7F, 0x7F, 0x81, 0x81, 0x82, 0x83, 0x84,
        0x84, 0x85, 0x86, 0x87, 0x87, 0x87, 0x88, 0x88, 0x89, 0x89, 0x89, 0x8A, 0x8A, 0x8A, 0x8A, 0x89, 0x89, 0x88, 0x88, 0x87, 0x87, 0x85, 0x85, 0x85,
        0x83, 0x82, 0x81, 0x81, 0x7F, 0x7E, 0x7E, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D};

static c_surface_transparent *s_surface;
static c_display *s_display;

//////////////////////// define widgets & map message ////////////////////////
enum WND_ID
{
    ID_ROOT = 1,
    ID_WAVE,
    ID_DIALOG,
    ID_BUTTON
};

typedef struct c_wave
{
    c_wave_ctrl base;
} c_wave;

void c_wave_vir_on_paint(c_wnd *self)
{
    c_wave *wave = (c_wave *)self;
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    c_image_draw_image_simple(&s_surface->base, Z_ORDER_LEVEL_0, c_theme_get_image(IMAGE_CUSTOM1), rect.m_left, rect.m_top);
    c_wave_ctrl_save_background(&wave->base);
}
void c_wave_vir_on_init_children(c_wnd *self) // should be pre_create
{
    c_wave *wave = (c_wave *)self;
    c_wave_ctrl_vir_on_init_children(self);
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    wave->base.m_wave_top = rect.m_top + 270;
    wave->base.m_wave_bottom = wave->base.m_wave_top + 100;
}

static const c_wnd_vir_api c_wave_vir_api_table = {
        c_wnd_vir_connect,     c_wave_vir_on_init_children, c_wave_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_wnd_vir_pre_create_wnd,    c_wnd_vir_on_focus,  c_wnd_vir_on_kill_focus,
};

void c_wave_init(c_wave *self)
{
    c_wave_ctrl_init(&self->base);
    self->base.base.m_api = &c_wave_vir_api_table;
}

typedef struct c_my_dialog
{
    c_dialog base;
} c_my_dialog;

void c_my_dialog_on_clicked(c_wnd *self, int ctrl_id, int param);

void c_my_dialog_vir_on_init_children(c_wnd *self)
{
    c_button_set_on_click(((c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON)), c_my_dialog_on_clicked);
}
void c_my_dialog_vir_on_paint(c_wnd *self)
{
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    s_surface->base.m_api->fill_rect(&s_surface->base, rect.m_left, rect.m_top, rect.m_right, rect.m_bottom, GL_ARGB(220, 0, 0, 0), self->m_z_order);
}
void c_my_dialog_on_clicked(c_wnd *self, int ctrl_id, int param)
{
    c_dialog_close_dialog((c_dialog *)self, self->m_surface);
    c_surface_transparent_clear_layer((c_surface_transparent *)self->m_surface, self->m_z_order);
}

static const c_wnd_vir_api c_my_dialog_vir_api_table = {
        c_wnd_vir_connect,     c_my_dialog_vir_on_init_children, c_my_dialog_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_dialog_vir_pre_create_wnd,      c_wnd_vir_on_focus,       c_wnd_vir_on_kill_focus,
};

void c_my_dialog_init(c_my_dialog *self)
{
    c_dialog_init(&self->base);
    self->base.base.m_api = &c_my_dialog_vir_api_table;
}

typedef struct c_my_ui
{
    c_wnd base;
    c_wave_buffer m_wave_buffer;
} c_my_ui;

void c_my_ui_on_clicked(c_wnd *self, int ctrl_id, int param);

void c_my_ui_vir_on_init_children(c_wnd *self)
{
    c_my_ui *my_ui = (c_my_ui *)self;

    c_wave_ctrl *p_wave = (c_wave_ctrl *)c_wnd_get_wnd_ptr(self, ID_WAVE);
    c_wave_ctrl_set_wave_speed(p_wave, 1);
    c_wave_ctrl_set_wave_color(p_wave, GL_RGB(0, 255, 0));
    c_wave_ctrl_set_wave_in_out_rate(p_wave, 256, 17);
    c_wave_ctrl_set_wave(p_wave, &my_ui->m_wave_buffer);
    c_wave_ctrl_set_max_min(p_wave, 170, 110);
}

void c_my_ui_vir_on_paint(c_wnd *self)
{
    c_image_draw_image_simple(&s_surface->base, Z_ORDER_LEVEL_0, c_theme_get_image(IMAGE_CUSTOM1), 0, 0);
}

static const c_wnd_vir_api c_my_ui_vir_api_table = {
        c_wnd_vir_connect,     c_my_ui_vir_on_init_children, c_my_ui_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_wnd_vir_pre_create_wnd,     c_wnd_vir_on_focus,   c_wnd_vir_on_kill_focus,
};

void c_my_ui_init(c_my_ui *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_my_ui_vir_api_table;

    c_wave_buffer_init(&self->m_wave_buffer);
}

//////////////////////// layout UI ////////////////////////
static c_my_ui s_my_ui;
static c_my_dialog s_my_dialog;
static c_button s_button;
static c_wave s_wave;

WND_TREE s_dialog_widgets[] = {{(c_wnd *)&s_button, ID_BUTTON, "CLOSE", 90, 130, 200, 100, NULL}, {NULL, 0, 0, 0, 0, 0, 0}};

WND_TREE s_main_widgets[] = {{(c_wnd *)&s_wave, ID_WAVE, 0, 0, 0, UI_WIDTH, UI_HEIGHT},
                             {(c_wnd *)&s_my_dialog, ID_DIALOG, 0, 100, 240, 350, 360, s_dialog_widgets},
                             {NULL, 0, 0, 0, 0, 0, 0}};

//////////////////////// start UI ////////////////////////
extern const LATTICE_FONT_INFO Consolas_24B;
extern const BITMAP_INFO background_bmp;
void load_resource(void)
{
    c_theme_init();
    c_theme_add_image(IMAGE_CUSTOM1, &background_bmp);
    // for button
    c_theme_add_font(FONT_DEFAULT, &Consolas_24B);
    c_theme_add_color(COLOR_WND_FONT, GL_RGB(255, 255, 255));
    c_theme_add_color(COLOR_WND_NORMAL, GL_RGB(0, 117, 107));
    c_theme_add_color(COLOR_WND_FOCUS, GL_RGB(0, 117, 107));
    c_theme_add_color(COLOR_WND_PUSHED, GL_RGB(0, 117, 107));
    c_theme_add_color(COLOR_WND_BORDER, GL_RGB(0, 117, 107));

    // my_ui init
    c_my_ui_init(&s_my_ui);
    // init child wnds.
    c_wave_init(&s_wave);

    c_my_dialog_init(&s_my_dialog);
    c_button_init(&s_button);
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    load_resource();
    static c_surface_transparent surface;
    static c_display display;
    c_surface_transparent_init(&surface, UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_1);
    c_display_init_with_surface(&display, phy_fb, screen_width, screen_height, &surface.base, driver);
    s_surface = &surface;
    s_display = &display;

    c_surface_transparent_correct_color_after_display(s_surface);

    c_wnd_set_surface(&s_my_ui.base, &surface.base);
    s_my_ui.base.m_api->connect(&s_my_ui.base, NULL, ID_ROOT, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_main_widgets);
    s_my_ui.base.m_api->show_window(&s_my_ui.base);

    unsigned int data_index1 = 0;
    unsigned int wave_index = 0;
    while (1)
    {
        if (wave_index % 200 == 0)
        {
            c_dialog_open_dialog_simple(NULL, &s_my_dialog.base);
        }

        c_wave_buffer_write_wave_data(&s_my_ui.m_wave_buffer, s_wave_data1[data_index1++ % sizeof(s_wave_data1)]);
        c_wave_buffer_write_wave_data(&s_my_ui.m_wave_buffer, s_wave_data1[data_index1++ % sizeof(s_wave_data1)]);
        c_wave_buffer_write_wave_data(&s_my_ui.m_wave_buffer, s_wave_data1[data_index1++ % sizeof(s_wave_data1)]);
        c_wave_buffer_write_wave_data(&s_my_ui.m_wave_buffer, s_wave_data1[data_index1++ % sizeof(s_wave_data1)]);
        c_wave_ctrl_refresh_wave(&s_wave.base, wave_index++);

        guilite_thread_sleep(17);
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