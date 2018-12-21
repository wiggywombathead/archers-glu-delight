#include "arrow.h"
#include "util.h"
#include "player.h"

#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <cstdio>

clock_t prev_tick, curr_tick;
vec3 gravity = {0, -9.81, 0};

extern float slowmo;

// player whose arrow this is
extern Player player;   

Arrow::Arrow(float t, float l) {
    thickness = t;
    length = l;

    pos = {0, 3, 0};
    vel = {0, 0, 0};
    state = FIRED;
}

void Arrow::make_handle() {
    handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            draw_capped_cylinder(thickness, length);
        glPopMatrix();
    glEndList();
}

void Arrow::nock() {
    state = NOCKED;
}

void Arrow::fire() {
    state = FIRED;
    pos = player.pos;
    vel = {
        sinf(player.yaw * M_PI / 180),
        -sinf(player.pitch * M_PI / 180),
        -cosf(player.yaw * M_PI / 180)
    };
    vel *= .1f;
}

void Arrow::simulate() {

    prev_tick = curr_tick;
    curr_tick = clock();

    float dt = ((float) (curr_tick - prev_tick)) / CLOCKS_PER_SEC;

    /* HACK - check if we dip below 30fps and assume we are paused */
    if (dt > 0.03)
        dt = 0.001f;

    // dt /= slowmo;

    vec3 dv = gravity * dt;

    vel += dv;
    pos += vel;

    if (pos.y <= 0) {
        pos.y = 0;
        vel.y *= -0.5;

        vel.x *= 0.8;
        vel.z *= 0.8;
    }

    //glutPostRedisplay();
}

void Arrow::draw_nocked() {
    glPushMatrix();
        glTranslatef(0.4, -0.2, -1);
        glRotatef(15, 0, 1, 0);
        glCallList(handle);
    glPopMatrix();
    glutPostRedisplay();
}

void Arrow::draw_flight() {

    yaw = atan2(vel.y, vel.z) * 180.f / M_PI;
    pitch = atan2(vel.x, vel.z) * 180.f / M_PI;

    if (vel.z < 0) {
        yaw += 180;
        pitch += 180;
    }

    glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glRotatef(-yaw, 1, 0, 0);
        glRotatef(pitch, 0, 1, 0);
        glCallList(handle);
    glPopMatrix();

    glutPostRedisplay();
}
