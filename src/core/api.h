#pragma once
#include <stdbool.h>
#include <stdarg.h>

#define REAL_TIME_TASK_CYCLE_MS 50
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))

#define GL_ARGB(a, r, g, b) ((((unsigned int)(a)) << 24) | (((unsigned int)(r)) << 16) | (((unsigned int)(g)) << 8) | ((unsigned int)(b)))
#define GL_ARGB_A(rgb)      ((((unsigned int)(rgb)) >> 24) & 0xFF)

#define GL_RGB(r, g, b)      ((0xFF << 24) | (((unsigned int)(r)) << 16) | (((unsigned int)(g)) << 8) | ((unsigned int)(b)))
#define GL_RGB_R(rgb)        ((((unsigned int)(rgb)) >> 16) & 0xFF)
#define GL_RGB_G(rgb)        ((((unsigned int)(rgb)) >> 8) & 0xFF)
#define GL_RGB_B(rgb)        (((unsigned int)(rgb)) & 0xFF)
#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))
#define GL_RGB_16_to_32(rgb)                                                                                                                                   \
    ((0xFF << 24) | ((((unsigned int)(rgb)) & 0x1F) << 3) | ((((unsigned int)(rgb)) & 0x7E0) << 5) | ((((unsigned int)(rgb)) & 0xF800) << 8))

#define GL_RGB_BLACK       (GL_RGB(0, 0, 0))
#define GL_RGB_WHITE       (GL_RGB(255, 255, 255))
#define GL_RGB_RED         (GL_RGB(255, 0, 0))
#define GL_RGB_GREEN       (GL_RGB(0, 255, 0))
#define GL_RGB_BLUE        (GL_RGB(0, 0, 255))
#define GL_RGB_YELLOW      (GL_RGB(255, 255, 0))
#define GL_RGB_MAGENTA     (GL_RGB(255, 0, 255))
#define GL_RGB_CYAN        (GL_RGB(0, 255, 255))
#define GL_RGB_GRAY        (GL_RGB(128, 128, 128))
#define GL_RGB_DARK_GRAY   (GL_RGB(169, 169, 169))
#define GL_RGB_LIGHT_GRAY  (GL_RGB(211, 211, 211))
#define GL_RGB_ORANGE      (GL_RGB(255, 165, 0))
#define GL_RGB_PINK        (GL_RGB(255, 192, 203))
#define GL_RGB_PURPLE      (GL_RGB(128, 0, 128))
#define GL_RGB_BROWN       (GL_RGB(165, 42, 42))
#define GL_RGB_TRANSPARENT (0x00000000)

#define ALIGN_HCENTER 0x00000000L
#define ALIGN_LEFT    0x01000000L
#define ALIGN_RIGHT   0x02000000L
#define ALIGN_HMASK   0x03000000L

#define ALIGN_VCENTER 0x00000000L
#define ALIGN_TOP     0x00100000L
#define ALIGN_BOTTOM  0x00200000L
#define ALIGN_VMASK   0x00300000L

typedef struct
{
    unsigned short year;
    unsigned short month;
    unsigned short date;
    unsigned short day;
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
} T_TIME;

typedef struct
{
    void (*assert)(const char *file, int line);
    void (*log)(const char *format, va_list argptr);
    void* (*calloc)(int count, int size);
    void (*free)(void *ptr);
    void* (*malloc)(int size);
    int (*sprintf)(char *str, const char *format, va_list argptr);
} guilite_api;

void guilite_register_api(const guilite_api *api);
void guilite_assert(const char *file, int line);
void guilite_log(const char *format, ...);
void *guilite_calloc(int count, int size);
void guilite_free(void *ptr);
void *guilite_malloc(int size);
void guilite_sprintf(char *str, const char *format, ...);
#define GUILITE_ASSERT(condition)                                                                                                                              \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        if (!(condition))                                                                                                                                      \
            guilite_assert(__FILE__, __LINE__);                                                                                                                \
    } while (0)

void guilite_init(void);

void guilite_thread_sleep(unsigned int milli_seconds);

#define FIFO_BUFFER_LEN 1024

typedef struct
{
    unsigned char m_buf[FIFO_BUFFER_LEN];
    int m_head;
    int m_tail;
    void *m_read_sem;
    void *m_write_mutex;
} c_fifo;

void c_fifo_init(c_fifo *fifo);
int c_fifo_read(c_fifo *fifo, void *buf, int len);
int c_fifo_write(c_fifo *fifo, void *buf, int len);

typedef struct
{
    int m_left;
    int m_top;
    int m_right;
    int m_bottom;
} c_rect;

// Initialize an empty rectangle
void c_rect_init_empty(c_rect *rect);
// Check if a rectangle is empty
bool c_rect_is_empty(c_rect *rect);
// Initialize a rectangle
void c_rect_init(c_rect *rect, int left, int top, int width, int height);
// Set the properties of the rectangle
void c_rect_set_rect(c_rect *rect, int left, int top, int width, int height);
// Check if a point is inside the rectangle
bool c_rect_pt_in_rect(const c_rect *rect, int x, int y);
// Compare two rectangles for equality
bool c_rect_equal(const c_rect *rect1, const c_rect *rect2);
// Get the width of the rectangle
int c_rect_width(const c_rect *rect);
// Get the height of the rectangle
int c_rect_height(const c_rect *rect);

void *guilite_calloc(int count, int size);
