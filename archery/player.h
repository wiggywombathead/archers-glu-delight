#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include "bow.h"
#include "arrow.h"
#include "target.h"

#define MAX_CAPACITY 32

class Player {
public:
    vec3 pos;
    float pitch;
    float yaw;
    float roll;

    Arrow quiver[MAX_CAPACITY];
    Target target;
    int curr_arrow;
    unsigned int capacity;
    float power;
    int score;

    Player(vec3);
    void init();
    void see();

    void nock(Arrow&);
    void fire(Arrow&);
    void pull(Arrow&, float);

    void draw_scene();
};

#endif
