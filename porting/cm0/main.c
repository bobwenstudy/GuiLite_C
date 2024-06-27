#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include "GuiLite.h"
#include "UICode.h"

int main(void)
{
    // Just for code size calculatation
    uicode_create_ui(0, 0, 0, 0, NULL);

    uicode_get_current_ui(0, 0, false);

    uicode_on_navigate(0);

    uicode_on_touch(0, 0, false);
}
