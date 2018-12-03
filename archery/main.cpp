#include <stdio.h>
#include <math.h>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int init();
void display();
void keyboard(unsigned char, int, int);
void special(int, int, int);
void mouse_motion(int, int);
void reshape(int, int);

// angle of teapot
float g_angle_y, g_angle_z;

// last mouse position
bool first_mouse = true;
float last_x = 320;
float last_y = 320;

struct light_t {
    size_t name;
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float position[4];
};

struct material_t {
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
};

const material_t brass = {
    {0.33f, 0.22f, 0.03f, 1.0f},
    {0.78f, 0.57f, 0.11f, 1.0f},
    {0.99f, 0.91f, 0.81f, 1.0f},
    27.8f
};

light_t light0 = {
    GL_LIGHT0,
	{0.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.0f, 0.75f, 0.5f, 1.0f}
};

void set_material(const material_t &mat) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat.shininess);
};

void set_light(const light_t &light) {
    glLightfv(light.name, GL_AMBIENT, light.ambient);
    glLightfv(light.name, GL_DIFFUSE, light.diffuse);
    glLightfv(light.name, GL_SPECULAR, light.specular);
    glLightfv(light.name, GL_POSITION, light.position);

    glEnable(light.name);
}

struct camera_t {
    float position[3];
    float reference[3];
    float up[3];

    float pitch;
    float yaw;
    float roll;
};

camera_t camera = {
    {0, 1, 3},
    {0, 0, 0},
    {0, 1, 0}
};

int main(int argc, char *argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowSize(640, 640);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Archery");

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutPassiveMotionFunc(mouse_motion);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    init();

    glutMainLoop();

    return 0;
}

int init() {

    set_light(light0);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    return 0;
}

void look() {
    gluLookAt(
        camera.position[0],
        camera.position[1],
        camera.position[2],
        camera.reference[0],
        camera.reference[1],
        camera.reference[2],
        camera.up[0],
        camera.up[1],
        camera.up[2]
    );
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    look();

    glPushMatrix();
        glRotatef(g_angle_y, 0, 1, 0);
        glRotatef(g_angle_z, 0, 0, 1);
        glutSolidTeapot(0.5f);
    glPopMatrix();

    glutSwapBuffers();

}

void keyboard(unsigned char k, int, int) {

    switch (k) {
    case 'q':
        exit(1);
    case 'w':
        camera.position[2] -= 0.2f;
        camera.reference[2] -= 0.2f;
        break;
    case 's':
        camera.position[2] += 0.2f;
        camera.reference[2] += 0.2f;
        break;
    case 'a':
        camera.position[0] -= 0.2f;
        camera.reference[0] -= 0.2f;
        break;
    case 'd':
        camera.position[0] += 0.2f;
        camera.reference[0] += 0.2f;
        break;
    }

    glutPostRedisplay();
}

void special(int k, int, int) {

    switch (k) {
    case GLUT_KEY_LEFT:
        g_angle_y -= 2.0f;
        break;
    case GLUT_KEY_RIGHT:
        g_angle_y += 2.0f;
        break;
    case GLUT_KEY_UP:
        g_angle_z += 2.0f;
        break;
    case GLUT_KEY_DOWN:
        g_angle_z -= 2.0f;
        break;
    }

    glutPostRedisplay();
}

void normalize(float *v, float *n) {
    float len = sqrt(pow(v[0],2) + pow(v[1], 2) + pow(v[2], 2));
    for (size_t i = 0; i < 2; i++)
        n[i] = v[i] / len;
}

void mouse_motion(int x, int y) {

    if (first_mouse) {
        last_x = x;
        last_y = y;
        first_mouse = false;
    }

    float dx = x - last_x;
    float dy = y - last_y;

    last_x = x;
    last_y = y;

    float sensitivity = 1.0f;
    dx *= sensitivity;
    dy *= sensitivity;

    camera.yaw += dx;
    camera.pitch += dy;

    float yaw_rad = camera.yaw * M_PI / 180;
    float pitch_rad = camera.pitch * M_PI / 180;

    // TODO
    float new_ref[3];
    new_ref[0] = cos(yaw_rad);
    new_ref[1] = sin(pitch_rad);
    new_ref[2] = sin(yaw_rad) * cos(pitch_rad);

    normalize(new_ref, camera.reference);

    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75, 1, 1, 5);
}
