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
            draw_capped_cylinder(radius, 0.2f);
        glPopMatrix();
    glEndList();
}

void Target::draw() {
    glPushMatrix();
        glRotatef(180, 0, 1, 0);
        glTranslatef(pos.x, pos.y, pos.z);
        glCallList(handle);
    glPopMatrix();
}

void Target::move(vec3 d) {
    pos += d;
}
