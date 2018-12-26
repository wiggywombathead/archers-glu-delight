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

    float power;

    Player(vec3);
    void see();
    void pull();

    void nock(Arrow&);
    void fire(Arrow&);
};

#endif
