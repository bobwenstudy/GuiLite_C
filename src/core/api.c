#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"

void c_fifo_init(c_fifo *fifo)
{
    memset(fifo->m_buf, 0, FIFO_BUFFER_LEN);
    fifo->m_head = 0;
    fifo->m_tail = 0;
    fifo->m_read_sem = NULL;
    fifo->m_write_mutex = NULL;
}

int c_fifo_read(c_fifo *fifo, void *buf, int len)
{
    return 0;
}

int c_fifo_write(c_fifo *fifo, void *buf, int len)
{
    return 0;
}

// Initialize an empty rectangle
void c_rect_init_empty(c_rect *rect)
{
    rect->m_left = rect->m_top = rect->m_right = rect->m_bottom = -1;
}

// Check if a rectangle is empty
bool c_rect_is_empty(c_rect *rect)
{
    return (rect->m_left == -1) && (rect->m_top == -1) && (rect->m_right == -1) && (rect->m_bottom == -1);
}

// Initialize a rectangle
void c_rect_init(c_rect *rect, int left, int top, int width, int height)
{
    c_rect_set_rect(rect, left, top, width, height);
}

// Set the properties of the rectangle
void c_rect_set_rect(c_rect *rect, int left, int top, int width, int height)
{
    GUILITE_ASSERT(width > 0 && height > 0);
    rect->m_left = left;
    rect->m_top = top;
    rect->m_right = left + width - 1;
    rect->m_bottom = top + height - 1;
}

// Check if a point is inside the rectangle
bool c_rect_pt_in_rect(const c_rect *rect, int x, int y)
{
    return x >= rect->m_left && x <= rect->m_right && y >= rect->m_top && y <= rect->m_bottom;
}

// Compare two rectangles for equality
bool c_rect_equal(const c_rect *rect1, const c_rect *rect2)
{
    return (rect1->m_left == rect2->m_left) && (rect1->m_top == rect2->m_top) && (rect1->m_right == rect2->m_right) && (rect1->m_bottom == rect2->m_bottom);
}

// Get the width of the rectangle
int c_rect_width(const c_rect *rect)
{
    return rect->m_right - rect->m_left + 1;
}

// Get the height of the rectangle
int c_rect_height(const c_rect *rect)
{
    return rect->m_bottom - rect->m_top + 1;
}

static const guilite_api *g_api;
void guilite_register_api(const guilite_api *api)
{
    g_api = api;
}

void guilite_log(const char *format, ...)
{
    if (g_api && g_api->log)
    {
        va_list argptr;
        va_start(argptr, format);
        (*g_api->log)(format, argptr);
        va_end(argptr);
    }
}

void guilite_assert(const char *file, int line)
{
    if (g_api && g_api->assert)
    {
        g_api->assert(file, line);

        while (1)
        {
        }; // Hang here to catch the assert
    }
}

void *guilite_calloc(int count, int size)
{
    if (g_api && g_api->calloc)
    {
        return g_api->calloc(count, size);
    }

    return NULL;
}

void guilite_free(void *ptr)
{
    if (g_api && g_api->free)
    {
        g_api->free(ptr);
    }
}

void *guilite_malloc(int size)
{
    if (g_api && g_api->malloc)
    {
        return g_api->malloc(size);
    }

    return NULL;
}

void guilite_sprintf(char *str, const char *format, ...)
{
    if (g_api && g_api->sprintf)
    {
        va_list argptr;
        va_start(argptr, format);
        g_api->sprintf(str, format, argptr);
        va_end(argptr);
    }
}
