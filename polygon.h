#pragma once
#include <list>
#include "dot.h"

class polygon{
public:
    using dot_t = dot;
    using verts_t = std::list<dot_t>;
    using fp_t = float;
private:
    dot_t m_pos;
    verts_t m_verts;
    fp_t m_angle;
public:
    polygon(const verts_t &verts={});

    const dot_t& pos()const;
    void set_pos(const dot_t &pos);
    const fp_t& angle()const;
    void set_angle(const fp_t &angle);

    verts_t::iterator begin();
    verts_t::iterator end();
    verts_t& verts(const verts_t &verts);
    const verts_t& verts(const verts_t &verts)const;
    dot_t& add_vert(const dot_t &d, const size_t &pos);
    dot_t& add_vert(const dot_t &d);
    dot_t& del_vert(const dot_t &d);
};

