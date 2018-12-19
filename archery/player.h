#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include "bow.h"
#include "arrow.h"

class Player {
public:
    vec3 pos;
    float pitch;
    float yaw;
    float roll;

    void see();
};

#endif
