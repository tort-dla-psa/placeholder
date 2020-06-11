#include "gfx_funcs.h"

using namespace gfx_func;
polygon rotate(const polygon &p, const polygon::fp_t &rad, const polygon::dot_t &pos = {0, 0}){
    polygon p_ = p;
    for(auto dt:p_){
        auto x = std::cos(rad)*dt.x - std::sin(rad)*dt.y;
        auto y = std::sin(rad)*dt.x + std::cos(rad)*dt.y;
        dt.x = std::move(x);
        dt.y = std::move(y);
    }
    return p_;
}
void rotate_ref(polygon &p, const polygon::fp_t &rad){
    for(auto dt:p){
        auto x = std::cos(rad)*dt.x - std::sin(rad)*dt.y;
        auto y = std::sin(rad)*dt.x + std::cos(rad)*dt.y;
        dt.x = std::move(x);
        dt.y = std::move(y);
    }
}
polygon zoom(const polygon &p, const polygon::fp_t &rad){
    polygon p_ = p;
    return p_;
}
void zoom_ref(polygon &p, const polygon::fp_t &rad){
}
