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
    ID_PAGE1,
    ID_PAGE2,
    ID_PAGE3,
    ID_PAGE4,
    ID_PAGE5
};

typedef struct c_page
{
    c_wnd base;
} c_page;

void c_page_vir_on_paint(c_wnd *self)
{
    c_page *page = (c_page *)self;
    c_rect rect;
    c_wnd_get_screen_rect(self, &rect);
    const BITMAP_INFO *bmp = NULL;
    switch (self->m_id)
    {
    case ID_PAGE1:
        bmp = (const BITMAP_INFO *)c_theme_get_image(IMAGE_CUSTOM1);
        break;
    case ID_PAGE2:
        bmp = (const BITMAP_INFO *)c_theme_get_image(IMAGE_CUSTOM2);
        break;
    case ID_PAGE3:
        bmp = (const BITMAP_INFO *)c_theme_get_image(IMAGE_CUSTOM3);
        break;
    case ID_PAGE4:
        bmp = (const BITMAP_INFO *)c_theme_get_image(IMAGE_CUSTOM4);
        break;
    case ID_PAGE5:
        bmp = (const BITMAP_INFO *)c_theme_get_image(IMAGE_CUSTOM5);
        break;
    default:
        break;
    }
    c_image_draw_image_simple(self->m_surface, self->m_z_order, bmp, rect.m_left, rect.m_top);
}

static const c_wnd_vir_api c_page_vir_api_table = {
        c_wnd_vir_connect,     c_wnd_vir_on_init_children, c_page_vir_on_paint, c_wnd_vir_show_window,   c_wnd_vir_on_touch,
        c_wnd_vir_on_navigate, c_wnd_vir_pre_create_wnd,   c_wnd_vir_on_focus,  c_wnd_vir_on_kill_focus,
};

void c_page_init(c_page *self)
{
    c_wnd_init(&self->base);
    self->base.m_api = &c_page_vir_api_table;
}

//////////////////////// layout UI ////////////////////////
c_page s_page1, s_page2, s_page3, s_page4, s_page5;
static c_slide_group s_root;
static WND_TREE s_root_children[] = {{NULL, 0, 0, 0, 0, 0, 0}};

//////////////////////// start UI ////////////////////////
extern const BITMAP_INFO ten_bmp, jack_bmp, queen_bmp, king_bmp, ace_bmp;
void load_resource(void)
{
    c_theme_init();
    c_theme_add_image(IMAGE_CUSTOM1, &ten_bmp);
    c_theme_add_image(IMAGE_CUSTOM2, &jack_bmp);
    c_theme_add_image(IMAGE_CUSTOM3, &queen_bmp);
    c_theme_add_image(IMAGE_CUSTOM4, &king_bmp);
    c_theme_add_image(IMAGE_CUSTOM5, &ace_bmp);

    // root init
    c_slide_group_init(&s_root);
    // init child wnds.
    c_page_init(&s_page1);
    c_page_init(&s_page2);
    c_page_init(&s_page3);
    c_page_init(&s_page4);
    c_page_init(&s_page5);
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    load_resource();
    static c_display display;
    c_display_init_with_surface_param(&display, phy_fb, screen_width, screen_height, UI_WIDTH, UI_HEIGHT, color_bytes, (1 + 5) /*1 root + 5 pages*/, driver);
    c_surface *surface = c_display_alloc_surface_simple(&display, Z_ORDER_LEVEL_1);

    s_display = &display;

    c_surface_set_active(surface, true);

    c_wnd_set_surface(&s_root.base, surface);
    s_root.base.m_api->connect(&s_root.base, NULL, ID_ROOT, 0, 0, 0, UI_WIDTH, UI_HEIGHT, s_root_children);

    c_slide_group_add_slide_with_child_tree(&s_root, (c_wnd *)&s_page1, ID_PAGE1, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
    c_slide_group_add_slide_with_child_tree(&s_root, (c_wnd *)&s_page2, ID_PAGE2, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
    c_slide_group_add_slide_with_child_tree(&s_root, (c_wnd *)&s_page3, ID_PAGE3, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
    c_slide_group_add_slide_with_child_tree(&s_root, (c_wnd *)&s_page4, ID_PAGE4, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
    c_slide_group_add_slide_with_child_tree(&s_root, (c_wnd *)&s_page5, ID_PAGE5, 0, 0, UI_WIDTH, UI_HEIGHT, NULL);
    c_slide_group_set_active_slide_simple(&s_root, 0);
    s_root.base.m_api->show_window(&s_root.base);

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
    is_down ? s_root.base.m_api->on_touch(&s_root.base, x, y, TOUCH_DOWN) : s_root.base.m_api->on_touch(&s_root.base, x, y, TOUCH_UP);
}

//////////////////////// on navigate ////////////////////////
void uicode_on_navigate(unsigned int key)
{
    s_root.base.m_api->on_navigate(&s_root.base, key);
}