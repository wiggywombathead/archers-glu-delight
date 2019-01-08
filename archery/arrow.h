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
    Target *stuck_in;  // the target this arrow is in

    int texture;

    Arrow();
    Arrow(float t, float l);
    void make_handle();

    void point();
    void simulate();
    void draw_nocked();
    void draw_flight();
    bool colliding_with(Target&);
    void stick_in();
    void draw_stuck();
    int get_score(Target&);
};

#endif
