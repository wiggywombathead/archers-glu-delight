#ifndef ARROW_H
#define ARROW_H

#include <cstddef>
#include "util.h"
#include "target.h"

enum State {
    STASHED,
    NOCKED,
    FIRED,
    STUCK,
    DEAD    // hasn't hit anything and won't
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

    float pitch, yaw, roll;     // arrow orientation
    float pulled;               // percentage pulled

    int texture;

    Arrow();
    Arrow(float t, float l);
    void make_handle();

    void point();
    void simulate();
    void draw_nocked();
    void draw_flight();
    bool has_hit(Target&);
    void stick_in(Target&);
    void draw_stuck_in(Target&);
    int get_score(Target&);
};

#endif
