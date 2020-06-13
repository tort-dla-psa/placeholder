#include "cam.h"
using dt_t = cam::dot_t;
using fp_t = cam::fp_t;

cam::cam(){
    m_pos = {0, 0};
    m_zoom = 1;
}

const dt_t& cam::pos()const
{ return m_pos; }

dt_t& cam::pos()
{ return m_pos; }

void cam::set_pos(const dt_t& pos)
{ this->m_pos = pos; }

const fp_t& cam::zoom()const
{ return m_zoom; }

fp_t& cam::zoom()
{ return m_zoom; }

void cam::set_zoom(const fp_t& zoom)
{ this->m_zoom = zoom; }
