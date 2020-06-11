#include "polygon.h"
#include <algorithm>
polygon::polygon(const polygon::verts_t &verts)
    :m_verts(verts)
{}

const polygon::dot_t& polygon::pos()const
{ return this->m_pos; }

void polygon::set_pos(const polygon::dot_t &pos)
{ this->m_pos = pos; }

const polygon::fp_t& polygon::angle()const
{ return this->m_angle; }

void polygon::set_angle(const polygon::fp_t &angle)
{ this->m_angle = angle; }

polygon::verts_t& polygon::verts(const polygon::verts_t &verts)
{ return this->m_verts; }

const polygon::verts_t& polygon::verts(const polygon::verts_t &verts)const
{ return this->m_verts; }

polygon::dot_t& polygon::add_vert(const polygon::dot_t &d)
{ return m_verts.emplace_back(d); }

polygon::dot_t& polygon::add_vert(const polygon::dot_t &d, const size_t &pos)
{ return *m_verts.emplace(std::next(m_verts.begin(), pos), d); }

polygon::dot_t& polygon::del_vert(const polygon::dot_t &d){
    auto it = std::find(m_verts.begin(), m_verts.end(), d);
    auto next_it = m_verts.erase(it);
    return *next_it;
}

polygon::verts_t::iterator polygon::begin()
{ return m_verts.begin(); }

polygon::verts_t::iterator polygon::end()
{ return m_verts.end(); }
