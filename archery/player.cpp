#include "player.h"

#include <stdio.h>
#include <ctime>
#include <GL/glut.h>

Player::Player(vec3 p) {
    pos = p;
    pitch = yaw = roll = 0.f;
    power = 0.f;

    capacity = MAX_CAPACITY;
    curr_arrow = 0;
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
    if (curr_arrow == capacity) {
        printf("No arrows left!\n");
        return;
    }

    a.state = NOCKED;
    a.pulled = 0.f;
    power = 0.f;
}

void Player::pull(Arrow &a, float amnt) {
    power += amnt;
    power = (power > 1.5f) ? 1.5f : power;
    a.pulled = power / 1.5f;
}

void Player::fire(Arrow &a) {

    if (a.state != NOCKED) {
        printf("Must nock!\n");
        return;
    }

    a.state = FIRED;
    a.pos = pos;
    a.vel = {
        sinf(yaw * M_PI / 180),
        -sinf(pitch * M_PI / 180),
        -cosf(yaw * M_PI / 180)
    };
    a.vel *= power;

    printf("Fired arrow %d with power %.5f\n", curr_arrow, power);

    curr_arrow++;
}

void Player::draw_scene() {

    if (quiver[curr_arrow].state == NOCKED)
        quiver[curr_arrow].draw_nocked();

    /*
    see();

    for (size_t i = 0; i < curr_arrow; i++) {

        if (quiver[i].state == FIRED) {
            if (quiver[i].has_hit(target)) {
                int s = quiver[i].get_score(target);
                score += s;
                quiver[i].state = STUCK;
            } else {
                quiver[i].simulate();
                quiver[i].draw_flight();
            }
        }

        if (quiver[i].state == STUCK) {
            quiver[i].draw_stuck_in(target);
        }
            
    }
    */

}
