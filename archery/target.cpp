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
    texture = load_and_bind_tex("images/target.png");

    glNewList(handle, GL_COMPILE);
        glPushMatrix();

            // texturise
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_2D);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            draw_capped_cylinder(radius, 0.2f);

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
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
