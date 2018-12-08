#include <stdio.h>
#include <math.h>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define WIN_W 640
#define WIN_H 640

int init();
void display();
void keyboard(unsigned char, int, int);
void special(int, int, int);
void mouse_click(int, int, int, int);
void mouse_motion(int, int);
void reshape(int, int);

void cross(const float *a, const float *b, float *n) {
    n[0] = (a[1] * b[2]) - (a[2] * b[1]);
    n[1] = (a[2] * b[0]) - (a[0] * b[2]);
    n[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

void camera_see();

// angle of teapot
float g_angle_y, g_angle_z;

// avoid jump when first entering
bool first_mouse = true;

// avoid jump when warping mouse back
bool warped = false;

// keep track of last mouse position
int last_x = 320;
int last_y = 320;

size_t g_target;    // target
size_t g_earth;     // the ground

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

const material_t porcelain = {
    {0.1f, 0.1f, 0.1f, 1.0f},
    {0.5f, 0.5f, 0.5f, 1.0f},
    {0.5f, 0.5f, 0.5f, 1.0f},
    100.f
};

const material_t flat = {
    {0.2f, 0.2f, 0.2f, 1.0f},
    {0.2f, 0.2f, 0.2f, 1.0f},
    {0.2f, 0.2f, 0.2f, 1.0f},
    100
};

light_t white_light = {
    GL_LIGHT0,
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {0.0f, 5.0f, 0.0f, 1.0f}
};

light_t red_light = {
    GL_LIGHT1,
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

struct position_t {
    float x, y, z, w;
};

struct camera_t {
    position_t pos;
    float reference[3];
    float up[3];

    float pitch;
    float yaw;
    float roll;
};

camera_t camera = {
    {0, 2, 3},
    {0, 0, 0},
    {0, 1, 0},

    0.0f, 0.0f, 0.0f
};

void draw_capped_cylinder(const float r, const float h, const int slices, const int stacks) {
    GLUquadricObj *obj = gluNewQuadric();
    gluQuadricNormals(obj, GLU_SMOOTH);

    gluCylinder(obj, r, r, h, slices, stacks);

    // top cap
    glPushMatrix();
        glTranslatef(0, 0, h);
        gluDisk(obj, 0, r, slices, stacks);
    glPopMatrix();

    // bottom cap
    glPushMatrix();
        glRotatef(180, 1, 0, 0);
        gluDisk(obj, 0, r, slices, stacks);
    glPopMatrix();
}

size_t make_target() {

    size_t handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        draw_capped_cylinder(2.0f, 0.1f, 32, 32);
    glEndList();

    return handle;
}

size_t make_earth() {

    const float verts[4][3] = {
        { 1, 0, 1 },
        { 1, 0, -1 },
        { -1, 0, -1 },
        { -1, 0, 1 }
    };

    float norm[3];
    cross(verts[0], verts[1], norm);

    size_t handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glNormal3f(norm[0], norm[1], norm[2]);
        glBegin(GL_QUADS);
            for (size_t i = 0; i < 4; i++)
                glVertex3fv(verts[i]);
        glEnd();
    glEndList();

    return handle;
}        

int main(int argc, char *argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowSize(640, 640);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Archery");

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse_click);
    glutPassiveMotionFunc(mouse_motion);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    init();

    glutMainLoop();

    return 0;
}

int init() {

    set_light(white_light);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glutSetCursor(GLUT_CURSOR_NONE);

    g_target = make_target();
    g_earth = make_earth();

    return 0;
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75, 1, 0.2, 50);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glPushMatrix();

        glPushMatrix();
            glTranslatef(0.4, -0.3, -0.8);
            glRotatef(90, 0, 1, 0);
            set_material(porcelain);
            glutSolidTeapot(0.15);
        glPopMatrix();
        
        camera_see();

        set_material(brass);
        glPushMatrix();
            glRotatef(90, 0, 1, 0);
            glTranslatef(0, 2, 0);
            glScalef(0.5f, 0.5f, 0.5f);
            glCallList(g_target);
        glPopMatrix();

        glPushMatrix();
            glScalef(10, 10, 10);
            set_material(flat);
            glCallList(g_earth);
        glPopMatrix();

    glPopMatrix();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);

    glPointSize(2.0f);
    glBegin(GL_POINTS);
        glVertex2f(WIN_W / 2, WIN_H / 2);
    glEnd();

    glutSwapBuffers();
}

void keyboard(unsigned char k, int, int) {

    switch (k) {
    case 'q':
        exit(1);
    case 'w':
        camera.pos.x += 0.2 * sin(camera.yaw * M_PI / 180);
        camera.pos.z -= 0.2 * cos(camera.yaw * M_PI / 180);
        break;
    case 's':
        camera.pos.x -= 0.2 * sin(camera.yaw * M_PI / 180);
        camera.pos.z += 0.2 * cos(camera.yaw * M_PI / 180);
        break;
    case 'a':
        camera.pos.x -= 0.2 * sin((90+camera.yaw) * M_PI / 180);
        camera.pos.z += 0.2 * cos((90+camera.yaw) * M_PI / 180);
        break;
    case 'd':
        camera.pos.x += 0.2 * sin((90+camera.yaw) * M_PI / 180);
        camera.pos.z -= 0.2 * cos((90+camera.yaw) * M_PI / 180);
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

void camera_see() {
    glRotatef(camera.pitch, 1.0f, 0.0f, 0.0f);  // rotate around x for pitch
    glRotatef(camera.yaw, 0.0f, 1.0f, 0.0f);    // rotate around y for yaw

    // translate screen to position of camera
    glTranslatef(-camera.pos.x, -camera.pos.y, -camera.pos.z);
}

void mouse_click(int button, int state, int x, int y) {

    // TODO: shoot things
    switch (button) {
    case GLUT_LEFT_BUTTON:
        std::cout << "Clicked at " << x << ", " << y << std::endl;
    }
}

void mouse_motion(int x, int y) {

    if (first_mouse) {
        camera.pitch = camera.yaw = camera.roll = 0;
        last_x = x;
        last_y = y;
        first_mouse = false;
    }

    if (warped) {
        last_x = x;
        last_y = y;
        warped = false;
    }

    int dx = x - last_x;
    int dy = y - last_y;

    last_x = x;
    last_y = y;

    float sensitivity = 1.0;
    dx *= sensitivity;
    dy *= sensitivity;

    camera.yaw += dx;
    camera.pitch += dy;

    if (camera.pitch < -90)
        camera.pitch = -90;

    if (camera.pitch > 90)
        camera.pitch = 90;

    glutPostRedisplay();

    if (last_x > 3 * WIN_W / 4 || last_x < WIN_W / 4 || 
            last_y > 3 * WIN_H / 4|| last_y < WIN_H / 4) {
        warped = true;
        glutWarpPointer(WIN_W / 2, WIN_H / 2);
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75, 1, 0.2, 50);
}
