#pragma once
#include "dot.h"

class cam{
public:
    using dot_t = dot;
    using fp_t = float;
protected:
    dot_t m_pos;
    fp_t m_zoom;
public:
    cam();

    const dot_t& pos()const;
    dot_t& pos();
    void set_pos(const dot_t& pos);
    const fp_t& zoom()const;
    fp_t& zoom();
    void set_zoom(const fp_t& zoom);
};
