#pragma once

struct dot{
    using fp_t = float;
    fp_t x, y;

    bool operator==(const dot &lhs)const{
        return x == lhs.x &&
            y == lhs.y;
    }
    dot& operator-=(const dot &rhs)
    { return *this = *this-rhs; }

    dot& operator+=(const dot &rhs)
    { return *this = *this+rhs; }

    dot& operator*=(const dot &rhs)
    { return *this = *this*rhs; }

    dot& operator/=(const dot &rhs)
    { return *this = *this/rhs; }

    dot& operator-=(const fp_t &rhs)
    { return *this = *this-dot{rhs,rhs}; }

    dot& operator+=(const fp_t &rhs)
    { return *this = *this+dot{rhs,rhs}; }

    dot& operator*=(const fp_t &rhs)
    { return *this = *this*dot{rhs,rhs}; }

    dot& operator/=(const fp_t &rhs)
    { return *this = *this/dot{rhs,rhs}; }

    friend dot operator*(const dot &lhs, const dot &rhs)
    { return dot{lhs.x*rhs.x, lhs.y*rhs.y}; }

    friend dot operator/(const dot &lhs, const dot &rhs)
    { return dot{lhs.x/rhs.x, lhs.y/rhs.y}; }

    friend dot operator+(const dot &lhs, const dot &rhs)
    { return dot{lhs.x+rhs.x, lhs.y+rhs.y}; }

    friend dot operator-(const dot &lhs, const dot &rhs)
    { return dot{lhs.x-rhs.x, lhs.y-rhs.y}; }

    friend dot operator+(const dot &lhs, const fp_t &rhs)
    { return lhs + dot{rhs, rhs}; }

    friend dot operator-(const dot &lhs, const fp_t &rhs)
    { return lhs - dot{rhs, rhs}; }

    friend dot operator*(const dot &lhs, const fp_t &rhs)
    { return lhs * dot{rhs, rhs}; }

    friend dot operator/(const dot &lhs, const fp_t &rhs)
    { return lhs / dot{rhs, rhs}; }

    friend bool operator == (dot &lhs, const dot &rhs){
        return lhs.x == rhs.x &&
            lhs.y == rhs.y;
    }
};

