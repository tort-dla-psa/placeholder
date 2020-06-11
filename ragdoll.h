#pragma once
#include "rwqueue/readerwriterqueue.h"
#include "polygon.h"
#include "dot.h"

#define PROP_prv(access, type, name){\
access:\
    type m_##name;\
public:\
    const type& ##name()const{ return this->m_##name; }\
    type& ##name(){ return this->m_##name; }\
    void set_##name(const type &name){ this->m_##name = name; }\
}

class phys_obj{
public:
    using dot_t = dot;
    using verts_t = polygon;
protected:
    dot_t m_pos;
    dot_t m_vel;
    verts_t m_verts;
public:
    phys_obj(const verts_t &verts);
    ~phys_obj();

    const verts_t& verts()const;
    verts_t& verts();
    void set_verts(const verts_t &verts);
};

class ragdoll_handler{
public:
    using queue = moodycamel::BlockingReaderWriterQueue<int>;
private:
    queue m_q;
public:
    ragdoll_handler(const queue &q);
    void run();
    void stop();
};


ragdoll_handler::ragdoll_handler(const queue &q){}
void ragdoll_handler::run(){}
void ragdoll_handler::stop(){}
