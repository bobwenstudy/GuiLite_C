#include "GuiLite.h"
#include <stdlib.h>
#include <math.h>
#include "UIcode.h"

#define MOLECULE_TOTAL 10
#define BOND_TOTAL     45 // BOND_TOTAL = 1 + 2 + ... + (MOLECULE_TOTAL - 1)

#define VELOCITY          2
#define BOND_MAX_DISTANCE 80

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

typedef struct
{
    float x, y, vx, vy;
    unsigned int color;
} Molecule;

void Molecule_init(Molecule *self)
{
    self->x = rand() % UI_WIDTH;
    self->y = rand() % UI_HEIGHT;
    self->vx = VELOCITY * ((0 == rand() % 2) ? -1 : 1);
    self->vy = VELOCITY * ((0 == rand() % 2) ? -1 : 1);
    self->color = GL_RGB(rand() % 5 * 32 + 127, rand() % 5 * 32 + 127, rand() % 5 * 32 + 127);
}

void Molecule_draw(Molecule *self, unsigned int color)
{
    s_surface->m_api->draw_pixel(s_surface, self->x - 2, self->y - 2, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x - 1, self->y - 2, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x, self->y - 2, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x + 1, self->y - 2, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x + 2, self->y - 2, color, Z_ORDER_LEVEL_0);

    s_surface->m_api->draw_pixel(s_surface, self->x - 3, self->y - 1, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x + 3, self->y - 1, color, Z_ORDER_LEVEL_0);

    s_surface->m_api->draw_pixel(s_surface, self->x - 3, self->y, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x + 3, self->y, color, Z_ORDER_LEVEL_0);

    s_surface->m_api->draw_pixel(s_surface, self->x - 3, self->y + 1, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x + 3, self->y + 1, color, Z_ORDER_LEVEL_0);

    s_surface->m_api->draw_pixel(s_surface, self->x - 2, self->y + 2, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x - 1, self->y + 2, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x, self->y + 2, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x + 1, self->y + 2, color, Z_ORDER_LEVEL_0);
    s_surface->m_api->draw_pixel(s_surface, self->x + 2, self->y + 2, color, Z_ORDER_LEVEL_0);
}

void Molecule_move(Molecule *self)
{
    Molecule_draw(self, 0);
    if (self->x <= 0 || self->x >= UI_WIDTH)
    {
        self->vx = (0 - self->vx);
    }
    if (self->y < 0 || self->y >= UI_HEIGHT)
    {
        self->vy = (0 - self->vy);
    }
    self->x += self->vx;
    self->y += self->vy;
    Molecule_draw(self, self->color);
}

typedef struct
{
    Molecule *m0, *m1;
    float x0, y0, x1, y1;
} Bond;

static const unsigned int Bond_color = GL_RGB(0, 162, 232);
static Bond Bond_bonds[BOND_TOTAL];

void Bond_init(Bond *self)
{
    self->m0 = self->m1 = 0;
    self->x0 = self->y0 = self->x1 = self->y1 = 0;
}

static void Bond_createBond(Bond *self, Molecule *m0, Molecule *m1)
{
    float distance = sqrtf((m0->x - m1->x) * (m0->x - m1->x) + (m0->y - m1->y) * (m0->y - m1->y));

    int index = -1;
    for (int i = 0; i < BOND_TOTAL; i++)
    {
        if ((Bond_bonds[i].m0 == m0 && Bond_bonds[i].m1 == m1) || (Bond_bonds[i].m0 == m1 && Bond_bonds[i].m1 == m0))
        {
            index = i;
            break;
        }
    }

    if (index >= 0)
    { // has been registered
        if (distance > BOND_MAX_DISTANCE)
        { // unregister
            c_surface_draw_line(s_surface, Bond_bonds[index].x0, Bond_bonds[index].y0, Bond_bonds[index].x1, Bond_bonds[index].y1, GL_RGB(0, 0, 0),
                                Z_ORDER_LEVEL_0);
            Bond_bonds[index].m0 = Bond_bonds[index].m1 = 0;
            return;
        }
        else
        { // update bond & draw
            c_surface_draw_line(s_surface, Bond_bonds[index].x0, Bond_bonds[index].y0, Bond_bonds[index].x1, Bond_bonds[index].y1, GL_RGB(0, 0, 0),
                                Z_ORDER_LEVEL_0);
            c_surface_draw_line(s_surface, m0->x, m0->y, m1->x, m1->y, Bond_color, Z_ORDER_LEVEL_0);
            Bond_bonds[index].x0 = m0->x;
            Bond_bonds[index].y0 = m0->y;
            Bond_bonds[index].x1 = m1->x;
            Bond_bonds[index].y1 = m1->y;
        }
        return;
    }

    if (distance > BOND_MAX_DISTANCE)
    {
        return;
    }
    // register new bond
    index = -1;
    for (int i = 0; i < BOND_TOTAL; i++)
    {
        if (Bond_bonds[i].m0 == 0 && Bond_bonds[i].m1 == 0)
        {
            index = i;
            break;
        }
    }
    if (index < 0)
    { // Bond_bonds full
        GUILITE_ASSERT(false);
        return;
    }

    // register
    Bond_bonds[index].m0 = m0;
    Bond_bonds[index].m1 = m1;
    Bond_bonds[index].x0 = m0->x;
    Bond_bonds[index].y0 = m0->y;
    Bond_bonds[index].x1 = m1->x;
    Bond_bonds[index].y1 = m1->y;
    c_surface_draw_line(s_surface, m0->x, m0->y, m1->x, m1->y, Bond_color, Z_ORDER_LEVEL_0);
}

//////////////////////// start UI ////////////////////////
void uicode_create_ui(void *phy_fb, int screen_width, int screen_height, int color_bytes, DISPLAY_DRIVER *driver)
{
    guilite_init();
    static c_surface surface;
    static c_display display;
    c_surface_init_with_order(&surface, UI_WIDTH, UI_HEIGHT, color_bytes, Z_ORDER_LEVEL_0);
    c_display_init_with_surface(&display, phy_fb, screen_width, screen_height, &surface, driver);
    s_surface = &surface;
    s_display = &display;

    s_surface->m_api->fill_rect(s_surface, 0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, COLOR_BACKGROUND, Z_ORDER_LEVEL_0);

    // initialize Bond
    for (int i = 0; i < BOND_TOTAL; i++)
    {
        Bond_init(&Bond_bonds[i]);
    }

    Molecule molecules[MOLECULE_TOTAL];
    for (int i = 0; i < MOLECULE_TOTAL; i++)
    {
        Molecule_init(&molecules[i]);
    }
    while (1)
    {
        for (int i = 0; i < MOLECULE_TOTAL; i++)
        {
            Molecule_move(&molecules[i]);
        }

        for (int i = 0; i < MOLECULE_TOTAL; i++)
        {
            for (int sub_i = i + 1; sub_i < MOLECULE_TOTAL; sub_i++)
            {
                Bond_createBond(NULL, &molecules[i], &molecules[sub_i]);
            }
        }
        guilite_thread_sleep(40);
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
