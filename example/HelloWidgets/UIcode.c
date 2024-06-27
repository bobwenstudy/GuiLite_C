#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

enum WND_ID
{
    ID_ROOT = 1,
    ID_LABEL_1,
    ID_LABEL_2,
    ID_LABEL_3,
    ID_BUTTON,
    ID_SPIN_BOX,
    ID_LIST_BOX,
    ID_EDIT_1,
    ID_EDIT_2,
    ID_DIALOG,
    ID_DIALOG_EDIT,
    ID_DIALOG_BUTTON,
    ID_DIALOG_EXIT_BUTTON
};

static char str[16];

typedef struct c_my_ui
{
    c_wnd base;
} c_my_ui;

void c_my_ui_on_button_clicked(c_wnd *self, int ctrl_id, int param);
void c_my_ui_on_spinbox_change(c_wnd *self, int ctrl_id, int value);
void c_my_ui_on_listbox_confirm(c_wnd *self, int ctrl_id, int value);

void c_my_ui_vir_on_init_children(c_wnd *self)
{
    c_button *button = (c_button *)c_wnd_get_wnd_ptr(self, ID_BUTTON);
    c_button_set_on_click(button, c_my_ui_on_button_clicked);

    c_edit *edit = (c_edit *)c_wnd_get_wnd_ptr(self, ID_EDIT_1);
    c_edit_set_keyboard_style(edit, STYLE_ALL_BOARD);

    edit = (c_edit *)c_wnd_get_wnd_ptr(self, ID_EDIT_2);
    c_edit_set_keyboard_style(edit, STYLE_NUM_BOARD);

    c_list_box *list_box = (c_list_box *)c_wnd_get_wnd_ptr(self, ID_LIST_BOX);
    c_list_box_set_on_change(list_box, c_my_ui_on_listbox_confirm);
    c_list_box_clear_item(list_box);
    c_list_box_add_item(list_box, (char *)"Item 0");
    c_list_box_add_item(list_box, (char *)"Item 1");
    c_list_box_add_item(list_box, (char *)"Item 2");
    c_list_box_select_item(list_box, 0);

    c_spin_box *spin_box = (c_spin_box *)c_wnd_get_wnd_ptr(self, ID_SPIN_BOX);
    c_spin_box_set_on_change(spin_box, c_my_ui_on_spinbox_change);
    c_spin_box_set_max_min(spin_box, 9, 0);
    c_spin_box_set_step(spin_box, 1);
    c_spin_box_set_value(spin_box, 5);
}

void c_my_ui_vir_on_paint(c_wnd *self)
{
    c_surface_draw_rect_simple(self->m_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, GL_RGB(0, 255, 0), Z_ORDER_LEVEL_0);
    c_surface_draw_rect_simple(self->m_surface, 2, 2, UI_WIDTH - 3, UI_HEIGHT - 3, GL_RGB(0, 255, 0), Z_ORDER_LEVEL_0);
}
void c_my_ui_on_button_clicked(c_wnd *self, int ctrl_id, int param)
{
    static int s_cnt;
    guilite_sprintf(str, "%d click", ++s_cnt);
    c_label *label = (c_label *)c_wnd_get_wnd_ptr(self, ID_LABEL_1);
    c_wnd_set_str(&label->base, str);
    label->base.m_api->show_window(&label->base);

    switch (ctrl_id)
    {
    case ID_BUTTON:
        c_dialog_open_dialog_simple(NULL, (c_dialog *)c_wnd_get_wnd_ptr(self, ID_DIALOG));
        break;
    }
}
void c_my_ui_on_spinbox_change(c_wnd *self, int ctrl_id, int value)
{
    c_label *label = (c_label *)c_wnd_get_wnd_ptr(self, ID_LABEL_2);
    c_wnd_set_str(&label->base, "change");
    label->base.m_api->show_window(&label->base);
}
void c_my_ui_on_listbox_confirm(c_wnd *self, int ctrl_id, int value)
{
    guilite_sprintf(str, "choose %d", value);
    c_label *label = (c_label *)c_wnd_get_wnd_ptr(self, ID_LABEL_3);
    c_wnd_set_str(&label->base, str);
    label->base.m_api->show_window(&label->base);
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

typedef struct c_my_dialog
{
    c_dialog base;
} c_my_dialog;

void c_my_dialog_on_button_clicked(c_wnd *self, int ctrl_id, int param);

void c_my_dialog_vir_on_init_children(c_wnd *self)
{
    c_button *button = (c_button *)c_wnd_get_wnd_ptr(self, ID_DIALOG_EXIT_BUTTON);
    c_button_set_on_click(button, c_my_dialog_on_button_clicked);

    c_edit *edit = (c_edit *)c_wnd_get_wnd_ptr(self, ID_DIALOG_EDIT);
    c_edit_set_keyboard_style(edit, STYLE_ALL_BOARD);
}

void c_my_dialog_on_button_clicked(c_wnd *self, int ctrl_id, int param)
{
    switch (ctrl_id)
    {
    case ID_DIALOG_EXIT_BUTTON:
        c_dialog_close_dialog((c_dialog *)self, self->m_surface);
        break;
    default:
        break;
    }
}

static const c_wnd_vir_api c_my_dialog_vir_api_table = {
        c_wnd_vir_connect,     c_my_dialog_vir_on_init_children, c_dialog_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_dialog_vir_pre_create_wnd,      c_wnd_vir_on_focus,    c_wnd_vir_on_kill_focus,
};

void c_my_dialog_init(c_my_dialog *self)
{
    c_dialog_init(&self->base);
    self->base.base.m_api = &c_my_dialog_vir_api_table;
}

// Layout Widgets
static c_my_ui s_my_ui;
static c_label s_label_1, s_label_2, s_label_3;
static c_button s_button;
static c_spin_box s_spin_box;
static c_list_box s_list_box;
static c_edit s_edit1, s_edit2;

static c_my_dialog s_my_dialog;
static c_button s_dialog_button;
static c_button s_dialog_exit_button;
static c_edit s_dialog_edit;
WND_TREE s_dialog_widgets[] = {{(c_wnd *)&s_dialog_edit, ID_DIALOG_EDIT, "GuiLite", 100, 100, 100, 50},
                               {(c_wnd *)&s_dialog_button, ID_DIALOG_BUTTON, "Button", 20, 200, 100, 50},
                               {(c_wnd *)&s_dialog_exit_button, ID_DIALOG_EXIT_BUTTON, "Exit", 160, 200, 100, 50},
                               {NULL, 0, 0, 0, 0, 0, 0}};

WND_TREE s_main_widgets[] = {{(c_wnd *)&s_edit1, ID_EDIT_1, "Hello", 150, 10, 100, 50},
                             {(c_wnd *)&s_edit2, ID_EDIT_2, "123", 400, 10, 100, 50},

                             {(c_wnd *)&s_label_1, ID_LABEL_1, "label 1", 150, 100, 100, 50},
                             {(c_wnd *)&s_label_2, ID_LABEL_2, "label 2", 150, 170, 100, 50},
                             {(c_wnd *)&s_label_3, ID_LABEL_3, "label 3", 150, 240, 100, 50},

                             {(c_wnd *)&s_button, ID_BUTTON, "Open", 400, 100, 100, 50},
                             {(c_wnd *)&s_spin_box, ID_SPIN_BOX, "spinBox", 400, 170, 100, 50},
                             {(c_wnd *)&s_list_box, ID_LIST_BOX, "listBox", 400, 240, 100, 50},

                             {(c_wnd *)&s_my_dialog, ID_DIALOG, "Dialog", 200, 100, 280, 312, s_dialog_widgets},
                             {NULL, 0, 0, 0, 0, 0, 0}};

// Create GUI
extern const LATTICE_FONT_INFO Consolas_24B;
void load_resource(void)
{
    c_theme_init();
    // for button
    c_theme_add_font(FONT_DEFAULT, &Consolas_24B);
    c_theme_add_color(COLOR_WND_FONT, GL_RGB(255, 255, 243));
    c_theme_add_color(COLOR_WND_NORMAL, GL_RGB(59, 75, 94));
    c_theme_add_color(COLOR_WND_PUSHED, GL_RGB(33, 42, 53));
    c_theme_add_color(COLOR_WND_FOCUS, GL_RGB(78, 198, 76));
    c_theme_add_color(COLOR_WND_BORDER, GL_RGB(46, 59, 73));

    // for keyboard
    c_widgets_init();

    // my_ui init
    c_my_ui_init(&s_my_ui);
    // init child wnds.
    c_label_init(&s_label_1);
    c_label_init(&s_label_2);
    c_label_init(&s_label_3);
    c_button_init(&s_button);
    c_spin_box_init(&s_spin_box);
    c_list_box_init(&s_list_box);
    c_edit_init(&s_edit1);
    c_edit_init(&s_edit2);

    // my_dialog init
    c_my_dialog_init(&s_my_dialog);
    // init child wnds.
    c_button_init(&s_dialog_button);
    c_button_init(&s_dialog_exit_button);
    c_edit_init(&s_dialog_edit);
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