#include <stdio.h>
#include <math.h>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "util.h"
#include "player.h"
#include "bow.h"
#include "arrow.h"

#define WIN_W 640
#define WIN_H 640
#define FRAME_RATE 60

const int FRAME_INTERVAL = 1 * 1000 / FRAME_RATE;

int init();
void idle();
void display();
void keyboard(unsigned char, int, int);
void special(int, int, int);
void mouse_click(int, int, int, int);
void mouse_motion(int, int);
void reshape(int, int);

bool first_mouse = true;    // avoid jump when first entering
bool warped = false;        // avoid jump when warping mouse back
bool escape_mouse = false;
bool paused = false;

// keep track of last mouse position
int last_x = 320;
int last_y = 320;

size_t g_target;    // target
size_t g_earth;     // the ground

size_t g_axes;

float slowmo = 10.f;

enum {
    HANDLE = 0,
    LIMB = 1,
    TIP = 2,
    STRING = 3,
    BOW_PARTS = 4
};

enum ArrowParts {
    SHAFT = 0,
    HEAD = 1,
    FLETCHING = 2,
    ARR_PARTS = 3
};

Player player({0, 2, 5});
Bow bow(0.02f, 0.8f);
Arrow arrow(0.01f, 0.4f);

size_t g_bow;
float bow_handle_len = 0.4f;
float bow_limb_len = 0.2f;
float bow_tip_len = 0.1f;
float bow_curve = 15.0f;
float bow_str_len;

size_t g_arrow;
float arrow_len;

struct light_t {
    size_t name;
    float ambient[4];
    float diffuse[4];   // color
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
	{1.0f, 2.0f, 3.5f, 1.0f}
};

light_t green_light = {
    GL_LIGHT1,
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{-3.0f, 2.0f, 3.5f, 1.0f}
};

light_t lights[3] = {
    white_light, red_light, green_light
};
unsigned int cur_light;
int num_lights = 3;

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

size_t makes_axes() {
    static const float verts[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };

    size_t handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glLineWidth(2);
        glBegin(GL_LINES);
            for (size_t i = 0; i < 3; i++) {
                glColor3fv(verts[i]);
                glVertex3f(0, 0, 0);
                glVertex3fv(verts[i]);
            }
        glEnd();
    glEndList();

    return handle;
}

void draw_axes() {
    glPushMatrix();
        glTranslatef(0, 0.01, 0);
        glScalef(2, 2, 2);
        glCallList(g_axes);
    glPopMatrix();
}

size_t make_target() {

    size_t handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        draw_capped_cylinder(2.0f, 0.1f, 32, 32);
    glEndList();

    return handle;
}

size_t make_arrow() {

    size_t handle = glGenLists(ARR_PARTS);

    glNewList(handle + SHAFT, GL_COMPILE);
        glPushMatrix();
            draw_capped_cylinder(0.008, arrow_len);
        glPopMatrix();
    glEndList();

    return handle;
}

size_t make_bow() {

    size_t handle = glGenLists(BOW_PARTS);

    glNewList(handle + HANDLE, GL_COMPILE);
        glPushMatrix();
            glRotatef(-90, 1, 0, 0);
            draw_capped_cylinder(0.01, bow_handle_len);
        glPopMatrix();
    glEndList();

    glNewList(handle + LIMB, GL_COMPILE);
        glPushMatrix();
            glRotatef(-90, 1, 0, 0);
            draw_capped_cylinder(0.01, bow_limb_len);
        glPopMatrix();
    glEndList();

    glNewList(handle + TIP, GL_COMPILE);
        glPushMatrix();
            glRotatef(-90, 1, 0, 0);
            draw_capped_cylinder(0.01, bow_tip_len);
        glPopMatrix();
    glEndList();

    glNewList(handle + STRING, GL_COMPILE);
        glPushMatrix();
            glRotatef(-90, 1, 0, 0);
            draw_capped_cylinder(0.005, bow_str_len);
        glPopMatrix();
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

    vec3 v0 = {1, 0, 1};
    vec3 v1 = {1, 0, -1};
    vec3 norm = cross(v0, v1);

    size_t handle = glGenLists(1);

    glNewList(handle, GL_COMPILE);
        glBegin(GL_QUADS);
            for (size_t i = 0; i < 4; i++)
                glVertex3fv(verts[i]);
            glNormal3f(norm.x, norm.y, norm.z);
        glEnd();
    glEndList();

    return handle;
}        

void draw_weapon() {

    float down = -(bow_limb_len * cos(bow_curve * M_PI / 180) + bow_tip_len * cos(2 * bow_curve * M_PI / 180));
    float back = -(bow_limb_len * sin(bow_curve * M_PI / 180) + bow_tip_len * sin(2 * bow_curve * M_PI / 180));

    glPushMatrix();

        /*
        glCallList(g_bow + HANDLE);

        // draw top half of bow
        glPushMatrix();
            glTranslatef(0, bow_handle_len, 0);
            glRotatef(bow_curve, 1, 0, 0);
            glCallList(g_bow + LIMB);

            glPushMatrix();
                glTranslatef(0, bow_limb_len, 0);
                glRotatef(bow_curve, 1, 0, 0);
                glCallList(g_bow + TIP);
            glPopMatrix();
        glPopMatrix();

        glRotatef(180, 1, 0, 0);

        // draw bottom half of bow
        glPushMatrix();
            glTranslatef(0, 0, 0);
            glRotatef(-bow_curve, 1, 0, 0);
            glCallList(g_bow + LIMB);

            glPushMatrix();
                glTranslatef(0, bow_limb_len, 0);
                glRotatef(-bow_curve, 1, 0, 0);
                glCallList(g_bow + TIP);
            glPopMatrix();
        glPopMatrix();

        // draw bow string
        glPushMatrix();
            glTranslatef(0, -bow_str_len - down, back);
            glCallList(g_bow + STRING);
        glPopMatrix();
        */

    glPopMatrix();
}
        
void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75, 1, 0.4, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glPushMatrix();

        // draw weapon
        bow.draw();

        set_material(brass);
        if (arrow.state == NOCKED)
            arrow.draw_nocked();

        // translate everything to camera position/view
        player.see();

        // simulate arrows
        if (arrow.state == FIRED) {
            if (!paused)
                arrow.simulate();
            arrow.draw_flight();
        }

        if (arrow.state == STUCK) {
            arrow.draw_flight();
        }

        // draw the target
        glPushMatrix();
            glRotatef(90, 0, 1, 0);
            glTranslatef(0, 2, 0);
            glScalef(0.5f, 0.5f, 0.5f);
            set_material(brass);
            // glCallList(g_target);
        glPopMatrix();

        // simulate physics
        // simulate_physics();

        // draw the ground
        glPushMatrix();
            set_material(porcelain);
            glTranslatef(0, -1, 0);
            glScalef(50, 2, 50);
            glutSolidCube(1);
            // glCallList(g_earth);
        glPopMatrix();

        glDisable(GL_LIGHTING);

        // draw axes
        draw_axes();

        glPushMatrix();
            glBegin(GL_POINTS);
            glPointSize(4);
            glColor3f(1, 1, 1);
            glVertex4fv(white_light.position);
            glColor3f(0, 1, 0);
            glVertex4fv(red_light.position);
            glEnd();
        glPopMatrix();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);

    // draw crosshair
    glPointSize(2.0f);
    glColor3f(1, 1, 1);
    glBegin(GL_POINTS);
        glVertex2f(WIN_W / 2, WIN_H / 2);
    glEnd();

    glutSwapBuffers();
}

void keyboard(unsigned char k, int, int) {

    switch (k) {
    case 'q':
        exit(1);
    case 'p':
        paused = !paused;
        break;
    case 'w':
        player.pos.x += 0.2 * sin(player.yaw * M_PI / 180);
        player.pos.z -= 0.2 * cos(player.yaw * M_PI / 180);
        break;
    case 's':
        player.pos.x -= 0.2 * sin(player.yaw * M_PI / 180);
        player.pos.z += 0.2 * cos(player.yaw * M_PI / 180);
        break;
    case 'a':
        player.pos.x -= 0.2 * sin((90+player.yaw) * M_PI / 180);
        player.pos.z += 0.2 * cos((90+player.yaw) * M_PI / 180);
        break;
    case 'd':
        player.pos.x += 0.2 * sin((90+player.yaw) * M_PI / 180);
        player.pos.z -= 0.2 * cos((90+player.yaw) * M_PI / 180);
        break;
    case 'm':
        escape_mouse = !escape_mouse;
        if (escape_mouse)
            glutSetCursor(GLUT_CURSOR_INHERIT);
        else
            glutSetCursor(GLUT_CURSOR_NONE);
        break;
    case '[':
        slowmo += 0.5f;
        break;
    case ']':
        slowmo -= 0.5f;
        if (slowmo < 1)
            slowmo = 0;
        break;
    case ',':
        glDisable(lights[cur_light].name);
        cur_light++;
        cur_light %= num_lights;
        set_light(lights[cur_light]);
        break;
    case '.':
        glDisable(lights[cur_light].name);
        cur_light = (cur_light == 0) ? num_lights - 1 : cur_light - 1;
        set_light(lights[cur_light]);
        break;
    }

    glutPostRedisplay();
}

void special(int k, int, int) {

    switch (k) {
    case GLUT_KEY_LEFT:
        break;
    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_UP:
        break;
    case GLUT_KEY_DOWN:
        break;
    }

    glutPostRedisplay();
}

void mouse_click(int button, int state, int x, int y) {

    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_UP)
            arrow.fire();
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_UP)
            arrow.nock();
        break;
    }
}

void mouse_motion(int x, int y) {

    if (first_mouse) {
        player.pitch = player.yaw = player.roll = 0;
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

    player.yaw += dx;
    player.pitch += dy;

    if (player.pitch < -90)
        player.pitch = -90;

    if (player.pitch > 90)
        player.pitch = 90;

    glutPostRedisplay();

    if (last_x > 3 * WIN_W / 4 || last_x < WIN_W / 4 || 
            last_y > 3 * WIN_H / 4|| last_y < WIN_H / 4) {

        if (!escape_mouse) {
            warped = true;
            glutWarpPointer(WIN_W / 2, WIN_H / 2);
        }
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75, 1, 0.4, 100);
}

int init() {

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    glutSetCursor(GLUT_CURSOR_NONE);

    set_light(lights[cur_light]);

    bow.make_handle();
    arrow.make_handle();

    bow_str_len = 2 * (
            bow_handle_len / 2 + 
            bow_limb_len * cos(bow_curve * M_PI / 180) + 
            bow_tip_len * cos(2 * bow_curve * M_PI / 180)
    );

    g_axes = makes_axes();
    g_target = make_target();
    g_earth = make_earth();
    g_bow = make_bow();

    return 0;
}

int main(int argc, char *argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutInitWindowSize(640, 640);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Archery");

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse_click);
    glutPassiveMotionFunc(mouse_motion);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    // glutIdleFunc(idle);

    init();

    glutMainLoop();

    return 0;
}
