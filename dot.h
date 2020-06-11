#pragma once

struct dot{
    using fp_t = float;
    fp_t x, y;
    bool operator == (const dot &rhs)const {
        return x == rhs.x &&
            y == rhs.y;
    }
};

