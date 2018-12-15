#ifndef ARROW_H
#define ARROW_H

#include <cstddef>
#include "util.h"

class Arrow {
public:
    size_t handle;
    float thickness;
    float length;

    vec3 pos;
    vec3 vel;

    Arrow(float t, float l);
    void make_handle();
};

#endif
