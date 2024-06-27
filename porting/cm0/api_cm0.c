#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "GuiLite.h"

static void api_assert(const char *file, int line)
{
    // TODO: implement assert
}

static void api_log(const char *format, va_list argptr)
{
    // TODO: implement assert
}

void* api_calloc(int count, int size)
{
    // TODO: implement calloc
    // return calloc(count, size);
    return NULL;
}

void api_free(void* ptr)
{
    // TODO: implement free
    // free(ptr);
}

void* api_malloc(int size)
{
    // TODO: implement malloc
    // return malloc(size);
    return NULL;
}

int api_sprintf(char* str, const char* format, va_list argptr)
{
    // TODO: implement sprintf
    // return vsprintf(str, format, argptr);
    return 0;
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
    // TODO: implement guilite_thread_sleep
}
