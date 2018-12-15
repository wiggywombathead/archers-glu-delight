#include "camera.h"

#include <GL/glut.h>

void Player::see() {
    // rotate around x for pitch
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);

    // rotate around y for yaw
    glRotatef(yaw, 0.0f, 1.0f, 0.0f);    

    // translate screen to position of camera
    glTranslatef(-pos.x, -pos.y, -pos.z);
}
