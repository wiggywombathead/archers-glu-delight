#ifndef TARGET_H
#define TARGET_H

#include <cstddef>
#include "util.h"

class Target {
public:
    size_t handle;
    float radius;
    float thickness;
    float margin;   // extra box added when calculating collision
    vec3 pos;

    unsigned int segments = 5;
    int texture;

    Target();
    Target(vec3, float, float);
    void make_handle();

    void draw();
    void move(vec3);
};

#endif
