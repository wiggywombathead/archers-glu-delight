#ifndef ARROW_H
#define ARROW_H

#include <cstddef>
#include "util.h"

enum State {
    STASHED,
    NOCKED,
    FIRED,
    STUCK
};

class Arrow {
public:
    size_t handle;
    float thickness;
    float length;
    State state;

    vec3 pos;
    vec3 vel;
    float pitch, yaw;

    Arrow(float t, float l);
    void make_handle();

    void simulate();
    void draw_nocked();
    void draw_flight();
};

#endif
