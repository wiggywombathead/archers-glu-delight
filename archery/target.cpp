#include "target.h"

#include <GL/glut.h>
#include <iostream>

Target::Target(vec3 p, float r, float t) {
    pos = p;
    radius = r;
    thickness = t;
}

void Target::make_handle() {
    handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            draw_capped_cylinder(radius, 0.1f);
        glPopMatrix();
    glEndList();
}

void Target::draw() {
    glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glCallList(handle);
    glPopMatrix();
}

void Target::move(vec3 d) {
    pos += d;
}

bool Target::hit_by(Arrow& a) {
    if (
        (a.pos.x >= pos.x - radius) && (a.pos.x <= pos.x + radius) &&
        (a.pos.y >= pos.y - radius) && (a.pos.y <= pos.y + radius) &&
        (a.pos.z >= pos.z + thickness) && (a.pos.z <= pos.z)
    ) {
        printf("Hit!\n");
    }
}
