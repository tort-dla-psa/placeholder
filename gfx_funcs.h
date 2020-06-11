#pragma once
#include "dot.h"
#include "polygon.h"
#include <cmath>

namespace gfx_func{
    polygon rotate(const polygon &p, const polygon::fp_t &rad, const polygon::dot_t &pos = {0, 0});
    void rotate_ref(polygon &p, const polygon::fp_t &rad);
    polygon zoom(const polygon &p, const polygon::fp_t &rad);
    void zoom_ref(polygon &p, const polygon::fp_t &rad);
};

