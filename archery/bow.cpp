#include "bow.h"
#include "util.h"

#include <GL/glut.h>

Bow::Bow(float t, float l) {
    thickness = t;
    length = l;
}

void Bow::make_handle() {
    handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            glRotatef(-90, 1, 0, 0);
            draw_capped_cylinder(thickness, length);
        glPopMatrix();
    glEndList();
}

void Bow::draw() {
    glPushMatrix();
        glTranslatef(0.4, -0.2, -1);
        glRotatef(15, 0, 1, 0);
        glCallList(handle);
    glPopMatrix();
}
