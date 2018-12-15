#include "arrow.h"
#include "util.h"

#include <GL/glut.h>
#include <ctime>

clock_t prev_tick, curr_tick;
vec3 gravity = {0, -9.81, 0};

Arrow::Arrow(float t, float l) {
    thickness = t;
    length = l;

    pos = {3, 3, 0};
    vel = {0, 0, 0};
    fired = false;
}

void Arrow::make_handle() {
    handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            draw_capped_cylinder(thickness, length);
        glPopMatrix();
    glEndList();
}

void Arrow::simulate() {

    prev_tick = curr_tick;
    curr_tick = clock();

    float dt = ((float) (curr_tick - prev_tick)) / CLOCKS_PER_SEC;
    dt /= 1000;

    vec3 dv = gravity * dt;

    vel += dv;

    vec3 diff = vel * dt;
    pos += vel;

    if (pos.y <= 0) {
        pos.y = 0;
        vel.y *= -0.5;

        vel.x *= 0.8;
        vel.z *= 0.8;
    }

    glutPostRedisplay();
}

void Arrow::draw() {
    glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glCallList(handle);
    glPopMatrix();
}
