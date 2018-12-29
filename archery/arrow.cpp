#include "arrow.h"
#include "util.h"
#include "player.h"

#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <cstdio>

clock_t prev_tick, curr_tick;
vec3 gravity = {0, -9.81, 0};

Arrow::Arrow(float t, float l) {
    thickness = t;
    length = l;

    pos = {0, 3, 0};
    vel = {0, 0, 0};
    state = NOCKED;
}

void Arrow::make_handle() {
    handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            draw_capped_cylinder(thickness, length);
            glutSolidSphere(thickness, 128, 128);
        glPopMatrix();
    glEndList();
}

void Arrow::simulate() {

    prev_tick = curr_tick;
    curr_tick = clock();

    float dt = ((float) (curr_tick - prev_tick)) / CLOCKS_PER_SEC;

    /* HACK - check if we dip below 30fps and assume we are paused */
    if (dt > 0.03)
        dt = 0.001f;

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
        glTranslatef(0.4, -0.2, -1.2 + pulled);
        glRotatef(15, 0, 1, 0);
        glCallList(handle);
    glPopMatrix();
    glutPostRedisplay();
}

void Arrow::point() {
    yaw = atan2(vel.y, vel.z) * 180.f / M_PI;
    pitch = atan2(vel.x, vel.z) * 180.f / M_PI;

    if (vel.z < 0) {
        yaw += 180;
        pitch += 180;
    }

    glRotatef(-yaw, 1, 0, 0);
    glRotatef(pitch, 0, 1, 0);
}

void Arrow::draw_flight() {

    glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        point();
        glCallList(handle);
    glPopMatrix();

    glutPostRedisplay();
}

bool Arrow::has_hit(Target& t) {
    if (
        (pos.x >= t.pos.x - t.radius) && (pos.x <= t.pos.x + t.radius) &&
        (pos.y >= t.pos.y - t.radius) && (pos.y <= t.pos.y + t.radius) &&
        (pos.z >= t.pos.z - t.thickness) && (pos.z <= t.pos.z)
    ) {
        if (state == FIRED) {
            offset = {
                pos.x - t.pos.x,
                pos.y - t.pos.y,
                pos.z - t.pos.z
            };
        }

        return true;
    } else {
        return false;
    }
}

void Arrow::draw_stuck(Target& t) {
    pos = {
        t.pos.x + offset.x,
        t.pos.y + offset.y,
        t.pos.z + offset.z
    };
    glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        point();
        glCallList(handle);
    glPopMatrix();

    glutPostRedisplay();
}

int Arrow::get_score(Target& t) {
    float seg_width = t.radius / t.segments;
    return t.segments + 1 - ceil(dist(pos, t.pos)/seg_width);
}
