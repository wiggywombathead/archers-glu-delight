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
    int score;

    Player(vec3);
    void see();

    void nock(Arrow&);
    void fire(Arrow&);
    void pull(Arrow&, float);
};

#endif
