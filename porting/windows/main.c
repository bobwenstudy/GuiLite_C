#include <stdio.h>
#include <string.h>
#include "windows.h"

#include "GuiLite.h"

int main(void)
{
    printf("Hello, GuiLite!\n");

    // https://blog.csdn.net/qq_59527512/article/details/136656055
    return WinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);
}
