#include "GuiLite.h"
#include <stdlib.h>
#include <math.h>
#include "UIcode.h"

#define CIRCLE_X     (UI_WIDTH / 2)
#define CIRCLE_Y     (UI_HEIGHT / 2)
#define CIRCLE_BOLD  3
#define PI           3.1415926535
#define SMALL_RADIUS 35
#define BIG_RADIUS   50

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

// Draw circle
int drawCircle(int x, int y, int r, unsigned int color, int arcAngle, double *points)
{
    const double pi = PI / 180;
    int tmpX, tmpY, lastX, lastY;
    lastX = lastY = 0;
    int sumDiffPoint = 0;

    if (arcAngle != 360)
    {
        for (double angle = 0.5; angle < arcAngle; angle += 0.5) // Bigger circle, Smaller angle step
        {
            tmpX = r * cos(angle * pi);
            tmpY = r * sin(angle * pi);
            if (tmpX == lastX && tmpY == lastY)
            {
                continue;
            }
            lastX = tmpX;
            lastY = tmpY;
            s_surface->m_api->draw_pixel(s_surface, x + tmpX, y + tmpY, color, Z_ORDER_LEVEL_0);
        }
        return 0;
    }

    for (double angle = 0.5; angle < 90; angle += 0.5) // Bigger circle, Smaller angle step
    {
        tmpX = r * cos(angle * pi);
        tmpY = r * sin(angle * pi);
        if (tmpX == lastX && tmpY == lastY)
        {
            continue;
        }
        lastX = tmpX;
        lastY = tmpY;

        if (points)
        {
            int index = sumDiffPoint * 3;
            points[index++] = x + tmpX;
            points[index++] = y + tmpY;
            points[index++] = 0;

            points[index++] = x - tmpX;
            points[index++] = y + tmpY;
            points[index++] = 0;

            points[index++] = x + tmpX;
            points[index++] = y - tmpY;
            points[index++] = 0;

            points[index++] = x - tmpX;
            points[index++] = y - tmpY;
            points[index++] = 0;
        }
        else
        {
            s_surface->m_api->draw_pixel(s_surface, x + tmpX, y + tmpY, color, Z_ORDER_LEVEL_0);
            s_surface->m_api->draw_pixel(s_surface, x - tmpX, y + tmpY, color, Z_ORDER_LEVEL_0);
            s_surface->m_api->draw_pixel(s_surface, x + tmpX, y - tmpY, color, Z_ORDER_LEVEL_0);
            s_surface->m_api->draw_pixel(s_surface, x - tmpX, y - tmpY, color, Z_ORDER_LEVEL_0);
        }
        sumDiffPoint += 4;
    }
    return sumDiffPoint;
}

int drawCircleWithAngle(int x, int y, int r, unsigned int color, int arcAngle)
{
    return drawCircle(x, y, r, color, arcAngle, 0);
}

int drawCircleSimple(int x, int y, int r, unsigned int color)
{
    return drawCircleWithAngle(x, y, r, color, 360);
}

// Circle
typedef struct
{
    double points[364][3];
    double points2d[364][2];
    int radius, curRadius, bold;
    int x, y, totalPixels;
    unsigned int rgb;
    bool isRoateX;
} Circle;

void Circle_init(Circle *self, int x, int y, int radius, unsigned int rgb, int bold, bool isRoateX)
{
    self->radius = radius;
    self->isRoateX = isRoateX;
    self->rgb = rgb;
    self->x = x;
    self->y = y;
    self->bold = bold;
    self->curRadius = 0;
    self->totalPixels = drawCircle(0, 0, radius, 0, 360, (double *)self->points);
}

void Circle_draw(Circle *self, int x, int y, bool isErase)
{
    for (int i = 0; i < self->totalPixels; i++)
    {
        s_surface->m_api->draw_pixel(s_surface, self->points2d[i][0] + x, self->points2d[i][1] + y, (isErase) ? 0 : self->rgb, Z_ORDER_LEVEL_0);
    }
}

void Circle_rotate(Circle *self, double angle)
{
    Circle_draw(self, self->x, self->y, true); // erase footprint
    double rotateOut1[3][1];
    for (int i = 0; i < self->totalPixels; i++)
    {
        if (self->isRoateX)
        {
            rotateX(angle, self->points[i], (double *)rotateOut1);
        }
        else
        {
            rotateY(angle, self->points[i], (double *)rotateOut1);
        }
        projectOnXYSimple((double *)rotateOut1, (double *)self->points2d[i]);
    }
    Circle_draw(self, self->x, self->y, false); // refresh self
}

void Circle_updateRadius(Circle *self, int step)
{
    int newRadus = self->radius + step;
    if (self->curRadius == newRadus)
    {
        return;
    }

    for (int i = 0; i < self->bold; i++)
    {
        int tmpRadius = (newRadus - i);
        if (tmpRadius <= self->curRadius && tmpRadius > (self->curRadius - self->bold))
        {
            continue;
        }
        drawCircleSimple(self->x, self->y, tmpRadius, self->rgb);
    }

    for (int i = 0; i < self->bold; i++)
    {
        int tmpRadius = (self->curRadius - i);
        if (tmpRadius <= newRadus && tmpRadius > (newRadus - self->bold))
        {
            continue;
        }
        drawCircleSimple(self->x, self->y, tmpRadius, 0);
    }

    self->curRadius = newRadus;
}

//////////////////////// start UI ////////////////////////
Circle theSmallCircle;
Circle theBigCircle;

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
    Circle_init(&theSmallCircle, CIRCLE_X, CIRCLE_Y, SMALL_RADIUS, GL_RGB(25, 68, 97), 8, false);
    Circle_init(&theBigCircle, CIRCLE_X, CIRCLE_Y, BIG_RADIUS, GL_RGB(59, 152, 215), 8, false);

    double time = 0.0;
    while (time < PI * 2)
    {
        drawCircleWithAngle(CIRCLE_X, CIRCLE_Y, SMALL_RADIUS, GL_RGB(25, 68, 97), time * 180 / PI);
        drawCircleWithAngle(CIRCLE_X, CIRCLE_Y, BIG_RADIUS, GL_RGB(59, 152, 215), time * 180 / PI);
        time += 0.2;
        guilite_thread_sleep(40);
    }

    time = 0.0;
    while (1)
    {
        if (time >= PI * 4)
        {
            s_surface->m_api->fill_rect(s_surface, (CIRCLE_X - BIG_RADIUS), (CIRCLE_Y - BIG_RADIUS), (CIRCLE_X + BIG_RADIUS), (CIRCLE_Y + BIG_RADIUS), 0,
                                        Z_ORDER_LEVEL_0);
            theSmallCircle.curRadius = 0;
            theBigCircle.curRadius = 0;
            time = 0;
        }
        else if (time >= PI * 3)
        {
            s_surface->m_api->fill_rect(s_surface, (CIRCLE_X - BIG_RADIUS), (CIRCLE_Y - BIG_RADIUS), (CIRCLE_X + BIG_RADIUS), (CIRCLE_Y + BIG_RADIUS), 0,
                                        Z_ORDER_LEVEL_0);
            Circle_rotate(&theSmallCircle, time);
            Circle_rotate(&theBigCircle, time);
        }
        else
        {
            Circle_updateRadius(&theSmallCircle, 3 * sin(time));
            Circle_updateRadius(&theBigCircle, 3 * sin(time + PI));
        }
        time += 0.3;
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
