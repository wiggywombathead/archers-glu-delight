#include "arrow.h"
#include "util.h"
#include "player.h"

#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <cstdio>

clock_t prev_tick, curr_tick;
vec3 gravity = {0, -9.81, 0};

Arrow::Arrow() {
    pos = {0, 0, 0};
    thickness = length = 0.0f;
    pitch = yaw = 0.0f;
    state = STASHED;
}

Arrow::Arrow(float t, float l) {
    thickness = t;
    length = l;

    pos = {0, 3, 0};
    vel = {0, 0, 0};
    state = STASHED;
}

void Arrow::make_handle() {
    handle = glGenLists(1);
    texture = load_and_bind_tex("images/arrow.png");

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
        /*
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_2D);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        */

            // shaft
            draw_capped_cylinder(thickness, length);

            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_2D);

            // head
            glPushMatrix();
                glRotatef(180, 0, 1, 0);
                draw_cone(thickness, 0.2);
            glPopMatrix();

            // fletchings
            glPushMatrix();
                glTranslatef(0, 0, length);
                glRotatef(90, 0, 0, 1);
                glScalef(0.1, 0.01, 0.1);
                glutSolidCube(1);
            glPopMatrix();

            glPushMatrix();
                glTranslatef(0, 0, length);
                glScalef(0.1, 0.01, 0.1);
                glutSolidCube(1);
            glPopMatrix();

            glPushMatrix();
            glPopMatrix();
        glPopMatrix();
    glEndList();
}

void Arrow::simulate() {

    if (vel.len() < 0.01f) {
        vel = {0, -1, 0};
        state = DEAD;
    }

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

        vel.x *= 0.5;
        vel.z *= 0.5;
    }

    // glutPostRedisplay();
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
    vec3 tail = normalize(vel * -1) * length;
    if (
        ((pos.x >= t.pos.x - t.radius) && (pos.x <= t.pos.x + t.radius) &&
        (pos.y >= t.pos.y - t.radius) && (pos.y <= t.pos.y + t.radius) &&
        (pos.z <= t.pos.z + t.thickness + t.margin) && (pos.z >= t.pos.z - 5*t.margin)) ||

        ((tail.x >= t.pos.x - t.radius) && (tail.x <= t.pos.x + t.radius) &&
        (tail.y >= t.pos.y - t.radius) && (tail.y <= t.pos.y + t.radius) &&
        (tail.z <= t.pos.z + t.thickness + t.margin) && (tail.z >= t.pos.z - 5*t.margin))
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

void Arrow::draw_stuck_in(Target& t) {
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
    float dist2d = sqrt(
            pow(pos.x - t.pos.x, 2) + pow(pos.y - t.pos.y, 2)
    );
    return t.segments + 1 - ceil(dist2d / seg_width);
}
