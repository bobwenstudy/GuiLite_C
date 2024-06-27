#include "GuiLite.h"
#include <stdlib.h>
#include <math.h>
#include "UIcode.h"

#define SPACE     13
#define ROW       15
#define COL       15
#define POINT_CNT ROW *COL
#define AMPLITUDE 50

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

// 3D engine

void multiply(int m, int n, int p, float *a, float *b, float *c) // a[m][n] * b[n][p] = c[m][p]
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
        {
            c[i * p + j] = 0;
            for (int k = 0; k < n; k++)
            {
                c[i * p + j] += a[i * n + k] * b[k * p + j];
            }
        }
    }
}

void rotateX(float angle, float *point, float *output) // rotate matrix for X
{
    static float rotation[3][3];
    rotation[0][0] = 1;
    rotation[1][1] = cos(angle);
    rotation[1][2] = 0 - sin(angle);
    rotation[2][1] = sin(angle);
    rotation[2][2] = cos(angle);
    multiply(3, 3, 1, (float *)rotation, point, output);
}

void rotateY(float angle, float *point, float *output) // rotate matrix for Y
{
    static float rotation[3][3];
    rotation[0][0] = cos(angle);
    rotation[0][2] = sin(angle);
    rotation[1][1] = 1;
    rotation[2][0] = 0 - sin(angle);
    rotation[2][2] = cos(angle);
    multiply(3, 3, 1, (float *)rotation, point, output);
}

void rotateZ(float angle, float *point, float *output) // rotate matrix for Z
{
    static float rotation[3][3];
    rotation[0][0] = cos(angle);
    rotation[0][1] = 0 - sin(angle);
    rotation[1][0] = sin(angle);
    rotation[1][1] = cos(angle);
    rotation[2][2] = 1;
    multiply(3, 3, 1, (float *)rotation, point, output);
}

void projectOnXY(float *point, float *output, float zFactor)
{
    static float projection[2][3]; // project on X/Y face
    projection[0][0] = zFactor;    // the raio of point.z and camera.z
    projection[1][1] = zFactor;    // the raio of point.z and camera.z
    multiply(2, 3, 1, (float *)projection, point, output);
}

void projectOnXYSimple(float *point, float *output)
{
    projectOnXY(point, output, 1);
}

// Cwave
float Cwave_points[POINT_CNT][3]; // x, y, z

typedef struct Cwave Cwave;

typedef struct Cwave_vir_api Cwave_vir_api;
struct Cwave_vir_api
{
    void (*draw)(Cwave *self, int x, int y, bool isErase);
    void (*swing)(Cwave *self, float rotate_angle_diff);
};
struct Cwave
{
    float points2d[POINT_CNT][2];
    float angle, rotate_angle;

    const Cwave_vir_api *m_api; // virtual api
};
void Cwave_vir_draw(Cwave *self, int x, int y, bool isErase)
{
    for (int i = 0; i < POINT_CNT; i++)
    {
        float factor = (1 + Cwave_points[i][2] / AMPLITUDE) / 2;
        unsigned int color = GL_RGB(210 * factor, 130 * factor, 255 * factor);
        s_surface->m_api->fill_rect(s_surface, self->points2d[i][0] + x - 1, self->points2d[i][1] + y - 1, self->points2d[i][0] + x + 1,
                                    self->points2d[i][1] + y + 1, (isErase) ? 0 : color, Z_ORDER_LEVEL_0);
    }
}

void Cwave_vir_swing(Cwave *self, float rotate_angle_diff)
{
    if (rotate_angle_diff == 0.0)
    {
        self->angle += 0.1;
        for (int y = 0; y < ROW; y++)
        {
            for (int x = 0; x < COL; x++)
            {
                float offset = sqrt((x - COL / 2) * (x - COL / 2) + (y - ROW / 2) * (y - ROW / 2)) / 2;
                Cwave_points[y * COL + x][2] = sin(self->angle + offset) * AMPLITUDE;
            }
        }
    }
    else
    {
        self->rotate_angle += rotate_angle_diff;
        if (self->rotate_angle > 1.0)
        {
            self->rotate_angle = 1.0;
        }
        if (self->rotate_angle < 0.0)
        {
            self->rotate_angle = 0.0;
        }
    }

    float rotateOut1[3][1];
    for (int i = 0; i < POINT_CNT; i++)
    {
        rotateX(self->rotate_angle, Cwave_points[i], (float *)rotateOut1);
        float zFactor = UI_WIDTH / (UI_WIDTH - rotateOut1[2][0]);
        projectOnXY((float *)rotateOut1, (float *)self->points2d[i], zFactor);
    }
}

static const Cwave_vir_api Cwave_vir_api_table = {
        Cwave_vir_draw,
        Cwave_vir_swing,
};

void Cwave_init(Cwave *self)
{
    self->rotate_angle = 0; // 1.57;
    self->angle = 0;
    memset(self->points2d, 0, sizeof(self->points2d));
    for (int y = 0; y < ROW; y++)
    {
        for (int x = 0; x < COL; x++)
        {
            Cwave_points[y * COL + x][0] = x * SPACE - (UI_WIDTH / 2);
            Cwave_points[y * COL + x][1] = y * SPACE - (UI_WIDTH / 2);
        }
    }
    self->m_api = &Cwave_vir_api_table;
}

//////////////////////// start UI ////////////////////////
Cwave theCwave;

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

    // initialize circle
    Cwave_init(&theCwave);
    unsigned int step = 0;

    while (1)
    {
        theCwave.m_api->draw(&theCwave, 30 + (UI_WIDTH / 2), UI_HEIGHT / 2, true); // erase footprint

        if (step > 400)
        {
            step = 0;
        }
        else if (step > 300)
        {
            theCwave.m_api->swing(&theCwave, -0.01);
        }
        else if (step > 200)
        {
            theCwave.m_api->swing(&theCwave, 0.0);
        }
        else if (step > 100)
        {
            theCwave.m_api->swing(&theCwave, 0.01);
        }
        else
        {
            theCwave.m_api->swing(&theCwave, 0.0);
        }

        theCwave.m_api->draw(&theCwave, 30 + (UI_WIDTH / 2), UI_HEIGHT / 2, false); // refresh Cwave
        guilite_thread_sleep(25);
        step++;
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
