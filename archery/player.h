#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#include "bow.h"
#include "arrow.h"
#include "target.h"

#define MAX_CAPACITY 16

class Player {
public:
    vec3 pos;
    float pitch;
    float yaw;
    float roll;
    float sensitivity;

    int curr_arrow;
    unsigned int capacity;
    float power;
    int score;

    Player();
    Player(vec3);
    void init();

    /* get functions */
    bool out_of_arrows();
    int get_score();

    /* gameplay functions */
    void see();
    void nock(Arrow&);
    void pull(Bow&, Arrow&, float);
    void fire(Bow&, Arrow&);
};

#endif
