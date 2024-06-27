#include "GuiLite.h"
#include <stdlib.h>
#include "UIcode.h"

#define COLOR_BACKGROUND GL_RGB_BLACK

#define MARIO_WITH   16
#define MARIO_HEIGHT 32

#define FULL_STEP    9
#define MARIO_INIT_X 0
#define MARIO_INIT_Y (UI_HEIGHT - 1)
#define RYU_X        85
#define RYU_Y        90
#define ACC_X        0
#define ACC_Y        1

extern const BITMAP_INFO title_bmp, background_bmp, step1_bmp, step2_bmp, step3_bmp, jump_bmp;

extern const BITMAP_INFO frame_00_bmp, frame_01_bmp, frame_02_bmp, frame_03_bmp, frame_04_bmp, frame_05_bmp, frame_06_bmp, frame_07_bmp, frame_08_bmp,
        frame_09_bmp, frame_10_bmp, frame_11_bmp, frame_12_bmp, frame_13_bmp;

const BITMAP_INFO *s_frames[] = {&frame_00_bmp, &frame_01_bmp, &frame_02_bmp, &frame_03_bmp, &frame_04_bmp, &frame_05_bmp, &frame_06_bmp,
                                 &frame_07_bmp, &frame_08_bmp, &frame_09_bmp, &frame_10_bmp, &frame_11_bmp, &frame_12_bmp, &frame_13_bmp};

static c_surface *s_surface;
static c_display *s_display;

//////////////////////// define widgets & map message ////////////////////////
enum WND_ID
{
    ID_DESKTOP = 1,
    ID_BUTTON
};

void do_shoryuken(void)
{
    for (int i = 0; i < sizeof(s_frames) / sizeof(const BITMAP_INFO *); i++)
    {
        c_theme_add_image(IMAGE_CUSTOM1, s_frames[i]);
        c_image_draw_image(s_surface, Z_ORDER_LEVEL_0, c_theme_get_image(IMAGE_CUSTOM1), RYU_X, RYU_Y, GL_RGB(85, 136, 221));
        guilite_thread_sleep(20);
    }
    s_surface->m_api->fill_rect(s_surface, RYU_X, RYU_Y, RYU_X + frame_00_bmp.width - 1, RYU_Y + frame_00_bmp.height - 1, GL_RGB(131, 110, 83),
                                Z_ORDER_LEVEL_0);
}

typedef struct c_mario
{
    int m_x, m_y, m_x_velocity, m_y_velocity;
    unsigned int m_step;
    bool m_is_jump;
} c_mario;

void c_mario_init(c_mario *self)
{
    self->m_x = MARIO_INIT_X;
    self->m_y = MARIO_INIT_Y;
    self->m_step = 0;
    self->m_is_jump = false;
    self->m_x_velocity = 3;
    self->m_y_velocity = 0;
}
void c_mario_jump(c_mario *self)
{
    self->m_is_jump = true;
    self->m_y_velocity = -9;
}
void c_mario_move(c_mario *self)
{
    if (self->m_step++ == FULL_STEP)
    {
        self->m_step = 0;
    }
    self->m_x_velocity += ACC_X;
    self->m_x += self->m_x_velocity;
    if (self->m_is_jump)
    {
        self->m_y_velocity += ACC_Y;
        self->m_y += self->m_y_velocity;
    }
    if ((self->m_x + MARIO_WITH - 1) >= UI_WIDTH)
    {
        self->m_x = 0;
    }
    if (self->m_y >= MARIO_INIT_Y)
    {
        self->m_y = MARIO_INIT_Y;
        self->m_y_velocity = 0;
        self->m_is_jump = false;
    }
    if (self->m_y < UI_HEIGHT - background_bmp.height + MARIO_HEIGHT)
    {
        self->m_y = UI_HEIGHT - background_bmp.height + MARIO_HEIGHT;
        self->m_y_velocity = 0;
    }

    if (self->m_x == 93)
    {
        c_mario_jump(self);
    }
    if (self->m_x == 117)
    {
        do_shoryuken();
    }
}
void c_mario_draw(c_mario *self, bool erase_footprint)
{
    const BITMAP_INFO *mario_bmp;
    if (self->m_is_jump)
    {
        mario_bmp = &jump_bmp;
    }
    else
    {
        if (self->m_step < (FULL_STEP / 3))
        {
            mario_bmp = &step1_bmp;
        }
        else if (self->m_step < (FULL_STEP * 2 / 3))
        {
            mario_bmp = &step2_bmp;
        }
        else
        {
            mario_bmp = &step3_bmp;
        }
    }

    if (erase_footprint)
    {
        c_rect tmp;
        c_rect_init_empty(&tmp);
        c_surface_activate_layer(s_surface, tmp, Z_ORDER_LEVEL_1); // inactivate the layer 1 by empty rect, and show the layers below.
    }
    else
    {
        c_rect mario_rect;
        c_rect_init(&mario_rect, self->m_x, self->m_y - mario_bmp->height, mario_bmp->width, mario_bmp->height);
        c_surface_activate_layer(s_surface, mario_rect, Z_ORDER_LEVEL_1); // activate the layer 1.
        c_image_draw_image(s_surface, Z_ORDER_LEVEL_1, mario_bmp, self->m_x, self->m_y - mario_bmp->height,
                           GL_RGB(255, 255, 255)); // draw everything inside the active rect.
    }
}

//////////////////////// start UI ////////////////////////
c_mario the_mario;
void load_resource(void)
{
    c_theme_init();

    // init mario
    c_mario_init(&the_mario);
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    load_resource();
    static c_surface surface;
    static c_display display;

    c_rect rect;
    c_rect_init(&rect, 0, UI_HEIGHT - background_bmp.height, UI_WIDTH, background_bmp.height);
    c_surface_init(&surface, UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_1, rect);
    c_display_init_with_surface(&display, phy_fb, screen_width, screen_height, &surface, driver);
    s_surface = &surface;
    s_display = &display;

    s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, GL_RGB(131, 110, 83), Z_ORDER_LEVEL_0);
    c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &title_bmp, 30, 20);
    c_image_draw_image_simple(s_surface, Z_ORDER_LEVEL_0, &background_bmp, 3, UI_HEIGHT - background_bmp.height);

    while (1)
    {
        c_mario_draw(&the_mario, true);
        c_mario_move(&the_mario);
        c_mario_draw(&the_mario, false);
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
    if (is_down)
    {
        c_mario_jump(&the_mario);
    }
}

//////////////////////// on navigate ////////////////////////
void uicode_on_navigate(unsigned int key)
{
}