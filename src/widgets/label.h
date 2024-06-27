#pragma once

#include "../core/api.h"
#include "../core/wnd.h"
#include "../core/resource.h"
#include "../core/word.h"
#include "../core/display.h"
#include "../core/theme.h"

typedef struct c_label c_label;
struct c_label
{
    c_wnd base;
};

void c_label_init(c_label *self);
