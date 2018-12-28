#ifndef ARROW_H
#define ARROW_H

#include <cstddef>
#include "util.h"
#include "target.h"

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
    vec3 offset;        // offset from target centre
    vec3 vel;

    float pitch, yaw;   // arrow orientation
    float pulled;       // percentage pulled

    Arrow(float t, float l);
    void make_handle();

    void simulate();
    void point();
    void draw_nocked();
    void draw_flight();

    bool has_hit(Target&);
    int get_score(Target&);
    void draw_stuck(Target&);
};

#endif
