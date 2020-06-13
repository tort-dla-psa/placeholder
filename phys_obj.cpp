#include "phys_obj.h"

phys_obj::phys_obj(const verts_t &verts)
    :m_verts(verts)
{
    m_movable = true;
}

phys_obj::phys_obj(verts_t &&verts)
    :m_verts(std::move(verts))
{
    m_movable = true;
}
phys_obj::phys_obj(const phys_obj &lhs)
{ *this = lhs; }

phys_obj::phys_obj(phys_obj &&lhs)
{ *this = std::move(lhs); }

phys_obj::~phys_obj(){}

bool phys_obj::operator==(const phys_obj &lhs)const{
    return m_pos == lhs.m_pos &&
    m_vel      == lhs.m_vel &&
    m_verts    == lhs.m_verts &&
    m_mass     == lhs.m_mass &&
    m_movable  == lhs.m_movable;
}
phys_obj& phys_obj::operator=(const phys_obj &lhs){
    m_pos       = lhs.m_pos;
    m_vel       = lhs.m_vel;
    m_verts     = lhs.m_verts;
    m_mass      = lhs.m_mass;
    m_movable   = lhs.m_movable;
    return *this;
}
phys_obj& phys_obj::operator=(phys_obj &&lhs){
    m_pos       = std::move(lhs.m_pos);
    m_vel       = std::move(lhs.m_vel);
    m_verts     = std::move(lhs.m_verts);
    m_mass      = std::move(lhs.m_mass);
    m_movable   = std::move(lhs.m_movable);
    return *this;
}

bool phys_obj::movable()const
{ return this->m_movable; }

void phys_obj::set_movable(bool movable)
{ this->m_movable = movable; }

const phys_obj::verts_t& phys_obj::verts()const
{ return this->m_verts; }

phys_obj::verts_t& phys_obj::verts()
{ return this->m_verts; }

void phys_obj::set_verts(const phys_obj::verts_t &verts)
{ this->m_verts = verts; }

phys_obj::dot_t phys_obj::mass_center()const{
    dot_t result = {0, 0};
    for(const auto &d:m_verts){
        result.x += d.x/m_verts.size();
        result.y += d.y/m_verts.size();
    }
    return result;
}

const phys_obj::dot_t& phys_obj::vel()const
{ return this->m_vel; }

void phys_obj::set_vel(const phys_obj::dot_t &vel)
{ this->m_vel = vel; }

const phys_obj::mass_t& phys_obj::mass()const
{ return this->m_mass; }

void phys_obj::set_mass(const phys_obj::mass_t &mass)
{ this->m_mass = mass; }
