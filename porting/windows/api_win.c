#include <string.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <assert.h>
#include <stdbool.h>
#include "GuiLite.h"

static void api_assert(const char *file, int line)
{
    static char s_buf[0x200];
    memset(s_buf, 0, sizeof(s_buf));
    sprintf_s(s_buf, sizeof(s_buf), "vvvvvvvvvvvvvvvvvvvvvvvvvvvv\n\nAssert@ file = %s, line = %d\n\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n", file, line);
    OutputDebugStringA(s_buf);
    printf(s_buf);
    fflush(stdout);
}

static void api_log(const char *format, va_list argptr)
{
    char msg_str[0x200];
    int len = vsnprintf(msg_str, sizeof(msg_str), format, argptr);
    printf(msg_str);
    fflush(stdout);
    OutputDebugStringA(msg_str);
}

static int guilite_total_calloc_size = 0;
void* api_calloc(int count, int size)
{
    guilite_total_calloc_size += count * size;
    printf("guilite_calloc, total: %d, size: %d, count: %d\n", guilite_total_calloc_size, size, count);
    return calloc(count, size);
}

void api_free(void* ptr)
{
    free(ptr);
}

void* api_malloc(int size)
{
    return malloc(size);
}

int api_sprintf(char* str, const char* format, va_list argptr)
{
    return vsprintf(str, format, argptr);
}

static const guilite_api guilite_api_impl = {
        api_assert,
        api_log,
        api_calloc,
        api_free,
        api_malloc,
        api_sprintf,
};

void guilite_init(void)
{
    guilite_register_api(&guilite_api_impl);
}

void guilite_thread_sleep(unsigned int milli_seconds)
{
    Sleep(milli_seconds);
}
