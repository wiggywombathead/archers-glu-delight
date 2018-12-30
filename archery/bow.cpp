#include "bow.h"
#include "util.h"

#include <GL/glut.h>

Bow::Bow(float t, float l) {
    thickness = t;
    length = l;
}

void Bow::make_handle() {
    handle = glGenLists(1);
    texture = load_and_bind_tex("images/arrow.png");

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_2D);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

            glRotatef(-80, 1, 0, 0);
            draw_capped_cylinder(thickness, length);

            glRotatef(170, 1, 0, 0);
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
