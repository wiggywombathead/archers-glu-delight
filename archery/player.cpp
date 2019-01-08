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
    sensitivity = 1.0f;
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

    glutPostRedisplay();
}

void Player::pull(Bow& b, Arrow &a, float amnt) {

    if (a.state != NOCKED)
        return;

    power += amnt;
    power = (power > 1.5f) ? 1.5f : power;
    a.pulled = power / 1.5f;    // max arrow recess is 1.5
    b.bent = power * 20;        // max bend is 1.5 * 20 = 30 degrees
}

void Player::fire(Bow& b, Arrow &a) {

    if (a.state != NOCKED) {
        printf("Must nock arrow first!\n");
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

    b.bent = 0.f;

    printf("Fired arrow %d with power %.5f\n", curr_arrow, power);

    curr_arrow++;
}

bool Player::out_of_arrows() {
    return curr_arrow == capacity;
}

int Player::get_score() {
    return score;
}
