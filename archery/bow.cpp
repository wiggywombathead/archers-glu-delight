#include "bow.h"
#include "util.h"

#include <GL/glut.h>

enum {
    UPPER = 0,
    LOWER,
    BOW_PARTS
};

Bow::Bow(float t, float l) {
    thickness = t;
    length = l;
}

void Bow::make_handle() {
    handle = glGenLists(BOW_PARTS);
    texture = load_and_bind_tex("images/arrow.png");

        /*
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
        */

    glNewList(handle + UPPER, GL_COMPILE);
        glPushMatrix();
            glRotatef(-70, 1, 0, 0);
            draw_capped_cylinder(thickness, length);
            glTranslatef(0, 0, length);
            glutSolidSphere(thickness, 32, 32);
        glPopMatrix();
    glEndList();

    glNewList(handle + LOWER, GL_COMPILE);
        glPushMatrix();
            glRotatef(70, 1, 0, 0);
            draw_capped_cylinder(thickness, length);
            glTranslatef(0, 0, length);
            glutSolidSphere(thickness, 32, 32);
        glPopMatrix();
    glEndList();
}

void Bow::draw() {
    glPushMatrix();
        glTranslatef(0.4, -0.1, -1.1);
        glPushMatrix();
            glRotatef(bent, 1, 0, 0);
            glCallList(handle + UPPER);
        glPopMatrix();
        glPushMatrix();
            glRotatef(-bent, 1, 0, 0);
            glCallList(handle + LOWER);
        glPopMatrix();
    glPopMatrix();
}
