#include "GuiLite.h"
#include <stdlib.h>
#include <math.h>
#include "UIcode.h"

#define SHAPE_SIZE 50

#define COLOR_BACKGROUND GL_RGB_BLACK

static c_surface *s_surface;
static c_display *s_display;

// 3D engine
void multiply(int m, int n, int p, double *a, double *b, double *c) // a[m][n] * b[n][p] = c[m][p]
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

void rotateX(double angle, double *point, double *output) // rotate matrix for X
{
    static double rotation[3][3];
    rotation[0][0] = 1;
    rotation[1][1] = cos(angle);
    rotation[1][2] = 0 - sin(angle);
    rotation[2][1] = sin(angle);
    rotation[2][2] = cos(angle);
    multiply(3, 3, 1, (double *)rotation, point, output);
}

void rotateY(double angle, double *point, double *output) // rotate matrix for Y
{
    static double rotation[3][3];
    rotation[0][0] = cos(angle);
    rotation[0][2] = sin(angle);
    rotation[1][1] = 1;
    rotation[2][0] = 0 - sin(angle);
    rotation[2][2] = cos(angle);
    multiply(3, 3, 1, (double *)rotation, point, output);
}

void rotateZ(double angle, double *point, double *output) // rotate matrix for Z
{
    static double rotation[3][3];
    rotation[0][0] = cos(angle);
    rotation[0][1] = 0 - sin(angle);
    rotation[1][0] = sin(angle);
    rotation[1][1] = cos(angle);
    rotation[2][2] = 1;
    multiply(3, 3, 1, (double *)rotation, point, output);
}

void projectOnXY(double *point, double *output, double zFactor)
{
    static double projection[2][3]; // project on X/Y face
    projection[0][0] = zFactor;     // the raio of point.z and camera.z
    projection[1][1] = zFactor;     // the raio of point.z and camera.z
    multiply(2, 3, 1, (double *)projection, point, output);
}

void projectOnXYSimple(double *point, double *output)
{
    projectOnXY(point, output, 1);
}

// Shape
typedef struct Shape Shape;

typedef struct Shape_vir_api Shape_vir_api;
struct Shape_vir_api
{
    void (*draw)(Shape *self, int x, int y, bool isErase);
    void (*rotate)(Shape *self);
};
struct Shape
{
    double angle;

    const Shape_vir_api *m_api; // virtual api
};
void Shape_vir_draw(Shape *self, int x, int y, bool isErase)
{
}
void Shape_vir_rotate(Shape *self)
{
}

static const Shape_vir_api Shape_vir_api_table = {
        Shape_vir_draw,
        Shape_vir_rotate,
};

void Shape_init(Shape *self)
{
    self->angle = 0.5;
    self->m_api = &Shape_vir_api_table;
}

double Cube_points[8][3] = {{-SHAPE_SIZE, -SHAPE_SIZE, -SHAPE_SIZE}, // x, y, z
                            {SHAPE_SIZE, -SHAPE_SIZE, -SHAPE_SIZE},  {SHAPE_SIZE, SHAPE_SIZE, -SHAPE_SIZE}, {-SHAPE_SIZE, SHAPE_SIZE, -SHAPE_SIZE},
                            {-SHAPE_SIZE, -SHAPE_SIZE, SHAPE_SIZE},  {SHAPE_SIZE, -SHAPE_SIZE, SHAPE_SIZE}, {SHAPE_SIZE, SHAPE_SIZE, SHAPE_SIZE},
                            {-SHAPE_SIZE, SHAPE_SIZE, SHAPE_SIZE}};

typedef struct
{
    Shape base;

    double points2d[8][2];
} Cube;
void Cube_vir_draw(Shape *self, int x, int y, bool isErase)
{
    Cube *cube = (Cube *)self;

    for (int i = 0; i < 4; i++)
    {
        c_surface_draw_line(s_surface, cube->points2d[i][0] + x, cube->points2d[i][1] + y, cube->points2d[(i + 1) % 4][0] + x,
                            cube->points2d[(i + 1) % 4][1] + y, (isErase) ? 0 : 0xffff0000, Z_ORDER_LEVEL_0);
        c_surface_draw_line(s_surface, cube->points2d[i + 4][0] + x, cube->points2d[i + 4][1] + y, cube->points2d[((i + 1) % 4) + 4][0] + x,
                            cube->points2d[((i + 1) % 4) + 4][1] + y, (isErase) ? 0 : 0xff00ff00, Z_ORDER_LEVEL_0);
        c_surface_draw_line(s_surface, cube->points2d[i][0] + x, cube->points2d[i][1] + y, cube->points2d[(i + 4)][0] + x, cube->points2d[(i + 4)][1] + y,
                            (isErase) ? 0 : 0xffffff00, Z_ORDER_LEVEL_0);
    }
}
void Cube_vir_rotate(Shape *self)
{
    Cube *cube = (Cube *)self;
    double rotateOut1[3][1], rotateOut2[3][1], rotateOut3[3][1];
    for (int i = 0; i < 8; i++)
    {
        rotateX(self->angle, Cube_points[i], (double *)rotateOut1);
        rotateY(self->angle, (double *)rotateOut1, (double *)rotateOut2);
        rotateZ(self->angle, (double *)rotateOut2, (double *)rotateOut3);
        projectOnXYSimple((double *)rotateOut3, (double *)cube->points2d[i]);
    }
    self->angle += 0.1;
}

static const Shape_vir_api Cube_vir_api_table = {
        Cube_vir_draw,
        Cube_vir_rotate,
};

void Cube_init(Cube *self)
{
    Shape_init(&self->base);
    self->base.m_api = &Cube_vir_api_table;
    memset(self->points2d, 0, sizeof(self->points2d));
}

double Pyramid_points[5][3] = {{0, -SHAPE_SIZE, 0}, // top
                               {-SHAPE_SIZE, SHAPE_SIZE, -SHAPE_SIZE},
                               {SHAPE_SIZE, SHAPE_SIZE, -SHAPE_SIZE},
                               {SHAPE_SIZE, SHAPE_SIZE, SHAPE_SIZE},
                               {-SHAPE_SIZE, SHAPE_SIZE, SHAPE_SIZE}};

typedef struct
{
    Shape base;

    double points2d[8][2];
} Pyramid;
void Pyramid_vir_draw(Shape *self, int x, int y, bool isErase)
{
    Pyramid *pyramid = (Pyramid *)self;
    c_surface_draw_line(s_surface, pyramid->points2d[0][0] + x, pyramid->points2d[0][1] + y, pyramid->points2d[1][0] + x, pyramid->points2d[1][1] + y,
                        (isErase) ? 0 : 0xff007acc, Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, pyramid->points2d[0][0] + x, pyramid->points2d[0][1] + y, pyramid->points2d[2][0] + x, pyramid->points2d[2][1] + y,
                        (isErase) ? 0 : 0xff007acc, Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, pyramid->points2d[0][0] + x, pyramid->points2d[0][1] + y, pyramid->points2d[3][0] + x, pyramid->points2d[3][1] + y,
                        (isErase) ? 0 : 0xff007acc, Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, pyramid->points2d[0][0] + x, pyramid->points2d[0][1] + y, pyramid->points2d[4][0] + x, pyramid->points2d[4][1] + y,
                        (isErase) ? 0 : 0xff007acc, Z_ORDER_LEVEL_0);

    c_surface_draw_line(s_surface, pyramid->points2d[1][0] + x, pyramid->points2d[1][1] + y, pyramid->points2d[2][0] + x, pyramid->points2d[2][1] + y,
                        (isErase) ? 0 : 0xff007acc, Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, pyramid->points2d[2][0] + x, pyramid->points2d[2][1] + y, pyramid->points2d[3][0] + x, pyramid->points2d[3][1] + y,
                        (isErase) ? 0 : 0xff007acc, Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, pyramid->points2d[3][0] + x, pyramid->points2d[3][1] + y, pyramid->points2d[4][0] + x, pyramid->points2d[4][1] + y,
                        (isErase) ? 0 : 0xff007acc, Z_ORDER_LEVEL_0);
    c_surface_draw_line(s_surface, pyramid->points2d[4][0] + x, pyramid->points2d[4][1] + y, pyramid->points2d[1][0] + x, pyramid->points2d[1][1] + y,
                        (isErase) ? 0 : 0xff007acc, Z_ORDER_LEVEL_0);
}
void Pyramid_vir_rotate(Shape *self)
{
    Pyramid *pyramid = (Pyramid *)self;
    double rotateOut1[3][1], rotateOut2[3][1];
    for (int i = 0; i < 5; i++)
    {
        rotateY(self->angle, Pyramid_points[i], (double *)rotateOut1);
        rotateX(0.1, (double *)rotateOut1, (double *)rotateOut2);
        double zFactor = SHAPE_SIZE / (2.2 * SHAPE_SIZE - rotateOut2[2][0]);
        projectOnXY((double *)rotateOut2, (double *)pyramid->points2d[i], zFactor);
    }
    self->angle += 0.1;
}

static const Shape_vir_api Pyramid_vir_api_table = {
        Pyramid_vir_draw,
        Pyramid_vir_rotate,
};

void Pyramid_init(Pyramid *self)
{
    Shape_init(&self->base);
    self->base.m_api = &Pyramid_vir_api_table;
    memset(self->points2d, 0, sizeof(self->points2d));
}

//////////////////////// start UI ////////////////////////
Cube theCube;
Pyramid thePyramid;
;

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
    Cube_init(&theCube);
    Pyramid_init(&thePyramid);

    while (1)
    {
        theCube.base.m_api->draw(&theCube.base, 120, 100, true); // erase footprint
        theCube.base.m_api->rotate(&theCube.base);
        theCube.base.m_api->draw(&theCube.base, 120, 100, false); // refresh cube

        thePyramid.base.m_api->draw(&thePyramid.base, 120, 250, true); // erase footprint
        thePyramid.base.m_api->rotate(&thePyramid.base);
        thePyramid.base.m_api->draw(&thePyramid.base, 120, 250, false); // refresh pyramid
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
}

//////////////////////// on navigate ////////////////////////
void uicode_on_navigate(unsigned int key)
{
}
