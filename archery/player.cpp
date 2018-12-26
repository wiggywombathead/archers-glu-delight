#include "player.h"

#include <stdio.h>
#include <ctime>
#include <GL/glut.h>

clock_t prev_pull, curr_pull;

Player::Player(vec3 p) {
    pos = p;

    pitch = yaw = roll = 0.f;
    power = 0.f;
}

void Player::see() {
    // rotate around x for pitch
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);

    // rotate around y for yaw
    glRotatef(yaw, 0.0f, 1.0f, 0.0f);    

    // translate screen to position of camera
    glTranslatef(-pos.x, -pos.y, -pos.z);
}

void Player::nock(Arrow &a) {
    a.state = NOCKED;
    power = 0.f;
}

void Player::fire(Arrow &a) {
    if (a.state != NOCKED)
        return;

    a.state = FIRED;
    a.pos = pos;
    a.vel = {
        sinf(yaw * M_PI / 180),
        -sinf(pitch * M_PI / 180),
        -cosf(yaw * M_PI / 180)
    };
    a.vel *= power;
    printf("Fired with power %f\n", power);
}
