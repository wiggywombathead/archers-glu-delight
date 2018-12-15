#include "arrow.h"
#include "util.h"

#include <GL/glut.h>

Arrow::Arrow(float t, float l) {
    thickness = t;
    length = l;
}

void Arrow::make_handle() {
    handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            draw_capped_cylinder(thickness, length);
        glPopMatrix();
    glEndList();
}
