#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <iostream>
#include <ctime>

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "util.h"
#include "player.h"
#include "bow.h"
#include "arrow.h"
#include "target.h"

#define WIN_W 800
#define WIN_H 800

#define WORLD_X 100
#define WORLD_Y 100
#define WORLD_Z 100

#define FRAME_RATE 60

#define MAX_TARGETS 16

const int FRAME_INTERVAL = 1 * 1000 / FRAME_RATE;

bool first_mouse = true;    // avoid jump when first entering
bool warped = false;        // avoid jump when warping mouse back
bool escape_mouse = false;

bool paused = false;
bool want_help = false;

// keep track of last mouse position
int last_x;
int last_y;

// measure current time and start time to find total
int start_ms, curr_ms;
float total_time, curr_time;
bool first_finish = true;

// to calculate time spent drawing bowstring
int pull_last, pull_now;
float dt_pull;
bool pulling = false;

// for framerate
int frame_last, frame_now;
int dt_frame;

float g_distance = 0.f;
int num_targets = 1;

// to vary target positions
int g_count = 0;
float g_target_rads = 0.0f;

// whether or not to draw axes and lights
bool g_axes_flag = false;

size_t g_wall;
size_t g_earth;
size_t g_axes;
size_t g_skybox;

enum Difficulty {
    STATIONARY = 0,
    MOVING,
    MOVING_PLUS_PLUS,
    NUM_DIFFICULTIES
};
int g_difficulty;

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
    {1.0f, 5.0f, 1.0f, 1.0f}
};

light_t red_light = {
    GL_LIGHT1,
	{0.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.0f, 5.0f, 1.0f, 1.0f}
};

light_t green_light = {
    GL_LIGHT2,
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 1.0f, 0.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, 5.0f, 0.0f, 1.0f}
};

light_t blue_light = {
    GL_LIGHT3,
	{0.0f, 0.0f, 0.0f, 1.0f},
	{0.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 5.0f, 0.0f, 1.0f}
};

light_t lights[] = {
   red_light, green_light, blue_light
};

unsigned int curr_light;
int num_lights = 0;

void set_material(const material_t &mat) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
    glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);
};

void set_light(const light_t &light) {
    glLightfv(light.name, GL_AMBIENT, light.ambient);
    glLightfv(light.name, GL_DIFFUSE, light.diffuse);
    glLightfv(light.name, GL_SPECULAR, light.specular);
    glLightfv(light.name, GL_POSITION, light.position);

    /*
	float direction[3] = {
				-light.position[0],
				-light.position[1],
				-light.position[2]};
	glLightfv(light.name, GL_SPOT_DIRECTION, direction);
	glLightf(light.name, GL_SPOT_CUTOFF, 5.0f);
    */

    glEnable(light.name);
}

Player player({0, 2, 10.f});
Bow bow(0.02f, 0.6f);
Arrow quiver[MAX_CAPACITY];
Target targets[MAX_TARGETS];
Target target({0, 2.5, -2}, 1.0f, 0.4f);

size_t make_axes() {
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
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
        glTranslatef(0, 0.01, 0);
        glScalef(2, 2, 2);
        glCallList(g_axes);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

float g_light_angle;
void draw_lights() {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
        glRotatef(g_light_angle, 0, 1, 0);
        glPointSize(4);

        for (size_t i = 0; i < 3; i++) {
            glBegin(GL_POINTS);
                glColor3fv(lights[i].diffuse);
                glVertex4fv(lights[i].position);
            glEnd();
        }
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

size_t make_earth() {
    size_t handle = glGenLists(1);
    int tex = load_and_bind_tex("images/green.png");

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, tex);

            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_2D);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glScalef(WORLD_X, 0.5f, WORLD_Z);
            glTranslatef(0, -0.5, 0);
            glRotatef(90, 0, 1, 0);
            glutSolidCube(1);

            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
        glPopMatrix();
    glEndList();
    
    return handle;
}

void draw_earth() {
    glCallList(g_earth);
}

size_t make_wall() {
    size_t handle = glGenLists(1);
    int tex = load_and_bind_tex("images/bricks-texture.png");

    glNewList(handle, GL_COMPILE);
        glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, tex);

            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_2D);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTranslatef(0, 0, 0);
            glScalef(WORLD_X / 2, 3, 0.5);
            glutSolidCube(1);

            glDisable(GL_TEXTURE_2D);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
        glPopMatrix();
    glEndList();
    
    return handle;
}

void draw_toeline() {
    glCallList(g_wall);
}

void draw_skybox() {
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_skybox);

    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, g_skybox);

        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_2D);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        glTranslatef(0, 0, 0);
        glScalef(WORLD_X, WORLD_Y, WORLD_Z);
        glutSolidCube(1);

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

bool all_hit() {
    for (size_t i = 0; i < num_targets; i++)
        if (!targets[i].hit)
            return false;
    return true;
}

void simulate_arrows() {
    for (size_t i = 0; i < player.curr_arrow; i++) {

        Arrow *a = &quiver[i];

        // avoid unnecessary simulation of arrow
        if (a->state == DEAD)
            continue;

        // update arrow's position based on the target it is in
        //   useful when targets can move
        if (a->state == STUCK) {
            a->stick_in();
            continue;
        }

        // only simulate arrow if arrow has not hit any
        bool hit_one = false;

        for (size_t j = 0; j < num_targets; j++) {

            if (a->state == FIRED) {

                // detect first collision
                if (a->colliding_with(targets[j])) {
                    int score = a->get_score(targets[j]);
                    printf("Hit target %d! (%d)\n", j, score);
                    player.score += score;
                    targets[j].hit = true;
                    hit_one = true;
                    break;
                }
            }
        }

        if (!hit_one)
            a->simulate();

        hit_one = false;
    }
}

void draw_arrows() {
    for (size_t i = 0; i < player.curr_arrow; i++) {
        Arrow *a = &quiver[i];

        switch (a->state) {
        case DEAD:
        case FIRED:
            a->draw_flight();
            break;
        case STUCK:
            a->draw_stuck();
            break;
        }
    }
}

void move_targets() {
   float dx, dy;
   vec3 motion;
    for (size_t i = 0; i < num_targets; i++) {
        switch (g_difficulty) {
        case STATIONARY:
            return;
        case MOVING:
            g_target_rads = g_count * 180 / M_PI;
            dx = cos(g_target_rads / 5000)/15;
            dy = 0;
            break;
        case MOVING_PLUS_PLUS:
            g_target_rads = g_count * 180 / M_PI;
            dx = cos(g_target_rads / 5000)/15;
            dy = -sin(g_target_rads / 1500)/20;
            break;
        }
        motion = {dx, dy, 0};
        targets[i].move(motion);
    }
    g_count++;
}

void draw_targets() {
    for (size_t i = 0; i < num_targets; i++)
        targets[i].draw();
}

void display_help() {
    const char *messages[] = {
        "Press RMB to nock arrow",
        "Press and hold LMB to fire",
        "",
        "r : reset level",
        "m : show/hide mouse",
        "",
        "+ , - : increase/decrease number of targets",
        "< , > : change difficulty",
        "[ , ] : change foul-line distance",
        "",
        "q : quit",
        "",
        "- NOTES -",
        "In a bid to cut costs, this target range was",
        "constructed on a laser grid. Please do not walk",
        "past the wall - it is there for your safety"
    };
    int n_mess = sizeof(messages) / sizeof(messages[0]);

    int y = (ORTHO_SIZE + (n_mess + 1) * 40) / 2;
    draw_centered(y, "- CONTROLS -");
    y -= 40;

    for (size_t i = 0; i < n_mess; i++)
        draw_centered(y - i * 40, messages[i]);
}

void display_hud() {
    std::string score_str = "Score: " + std::to_string(player.score);
    draw_text(20, 960, score_str.c_str());

    std::string arrows_remaining = "Arrows: " + 
        std::to_string(player.capacity - player.curr_arrow) + "/" +
        std::to_string(player.capacity);
    draw_text(20, 920, arrows_remaining.c_str());

    if (paused)
        draw_centered(20, "[paused]");
}

void display_hints() {
    const char *msgs[] = {
        "Press 'h' for help",
    };
    int n_msgs = sizeof(msgs) / sizeof(msgs[0]);
    int y = 960;
    for (size_t i = 0; i < n_msgs; i++)
        draw_raligned(980, y - i * 40, msgs[i]);
}

void idle() {
    
    if (pulling) {
        pull_last = pull_now;
        pull_now = glutGet(GLUT_ELAPSED_TIME);

        /* dunno why it does this, but it fixes the issue */
        if (pull_last > pull_now)
            return;
         
        dt_pull = ((float) (pull_now - pull_last)) / 1000.f;
        float amount = dt_pull * 2.5f;;

        player.pull(bow, quiver[player.curr_arrow], amount);
    }

    white_light.position[0] = player.pos.x;
    white_light.position[1] = player.pos.y;
    white_light.position[2] = player.pos.z;
    white_light.position[3] = 20;
    set_light(white_light);
    
    draw_lights();

    glPushMatrix();
        for (size_t i = 0; i < num_lights; i++) {
            set_light(lights[i]);
        }
    glPopMatrix();

    g_light_angle += 3;
    glutPostRedisplay();
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, 1, 0.2, 150);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();

        // draw weapon
        set_material(porcelain);
        bow.draw();

        // draw current arrow
        if (quiver[player.curr_arrow].state == NOCKED) {
            set_material(brass);
            quiver[player.curr_arrow].draw_nocked();
        }

        // translate everything to camera position/view
        player.see();

        if (!paused) {
            simulate_arrows();
            move_targets();
        }

        // draw the arrows
        set_material(brass);
        draw_arrows();

        // draw the targets
        set_material(flat);
        draw_targets();

        // draw the ground
        draw_earth();

        set_material(brass);
        draw_toeline();

        if (g_axes_flag) {
            draw_lights();
            draw_axes();
        }

        // simple skybox
        draw_skybox();

    glPopMatrix();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);

    // display various messages
    display_hud();
    display_hints();

    if (want_help)
        display_help();

    // draw crosshair
    glPointSize(2.0f);
    glColor3f(1, 1, 1);
    glBegin(GL_POINTS);
        glVertex2f(WIN_W / 2, WIN_H / 2);
    glEnd();

    // timing
    if (!paused) {
        curr_ms = glutGet(GLUT_ELAPSED_TIME);
        curr_time = (curr_ms - start_ms) / 1000.f;
    }

    if (all_hit()) {
        if (first_finish) {
            total_time = curr_time;
            first_finish = false;
        }

        std::string time_str = std::to_string(total_time);
        draw_centered(960, time_str.c_str());
        std::string msg = "All targets hit in " + std::to_string(total_time) + "s";
        draw_centered(80, msg.c_str());
    } else {
        std::string time_str = std::to_string(curr_time);
        draw_centered(960, time_str.c_str());
    }

    if (player.out_of_arrows()) {
        draw_centered(160, "Out of arrows!");
        std::string msg = "You scored " + std::to_string(player.get_score()) + " points";
        draw_centered(120, msg.c_str());
        draw_centered(40, "Press r to restart");
    }

    glutSwapBuffers();

    // cap to 60 fps
    frame_last = frame_now;
    frame_now = glutGet(GLUT_ELAPSED_TIME);
    dt_frame = frame_now - frame_last;

    if (dt_frame < FRAME_INTERVAL);
        usleep(1000 * FRAME_INTERVAL - dt_frame);

} 

void reset() {

    player.score = 0;
    player.curr_arrow = 0;
    player.pos = {0, 2.5, 5};

    target.pos = {0, 2.5, -2};
    g_count = 0;    // for centred movement when reset

    // store all arrows
    for (size_t i = 0; i < player.capacity; i++) {
        quiver[i].state = STASHED;
    }

    // re-randomise target positions
    float x, y, z;
    for (size_t i = 0; i < MAX_TARGETS; i++) {
        x = rand() % 40 - 20;   // -20 <= x <= 20
        y = rand() % 8 + 2;     // 2 <= y <= 15
        z = rand() % 10 - 10;   // -10 <= z <= 0
        targets[i].pos = {x, y, z};
        targets[i].hit = false;
    }

    start_ms = glutGet(GLUT_ELAPSED_TIME);
    first_finish = true;
}

void keyboard(unsigned char k, int, int) {

    float dx, dz;

    switch (k) {
    /* HELP, PAUSE, RESET, QUIT */
    case 'h':
        want_help = !want_help;
        break;
    case 'p':
        paused = !paused;
        if (paused)
            glutIdleFunc(NULL);
        else
            glutIdleFunc(idle);
        break;
    case 'r':
        reset();
        break;
    case 'q':
        exit(1);
    /* PLAYER MOVEMENT */
    case 'w':
        dx = 0.2 * sin(player.yaw * M_PI / 180);
        dz = 0.2 * cos(player.yaw * M_PI / 180);
        player.pos.x += dx;
        player.pos.z -= dz;
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
    /* GAME MODE MANAGEMENT */
    case 'm':
        escape_mouse = !escape_mouse;
        if (escape_mouse)
            glutSetCursor(GLUT_CURSOR_INHERIT);
        else
            glutSetCursor(GLUT_CURSOR_NONE);
        break;
    case 'l':
        g_axes_flag = !g_axes_flag;
        break;
    case '[':
        g_distance -= 0.5f;
        break;
    case ']':
        g_distance += 0.5f;
        break;
    case '<':
        g_difficulty = (g_difficulty - 1) % NUM_DIFFICULTIES;
        break;
    case '>':
        g_difficulty = (g_difficulty + 1) % NUM_DIFFICULTIES;
        break;
    case '-':
        num_targets--;
        num_targets = num_targets < 0 ? 0 : num_targets;
        break;
    case '+':
        num_targets++;
        num_targets = num_targets > MAX_TARGETS ? MAX_TARGETS : num_targets;
        break;
    }

    if (player.pos.z < 0.f)
        player.pos.z = 0.f;

    glutPostRedisplay();
}

void special(int k, int, int) {

    switch (k) {
    case GLUT_KEY_LEFT:
        target.move({-1, 0, 0});
        break;
    case GLUT_KEY_RIGHT:
        target.move({1, 0, 0});
        break;
    case GLUT_KEY_UP:
        target.move({0, 1, 0});
        break;
    case GLUT_KEY_DOWN:
        target.move({0, -1, 0});
        break;
    }

    glutPostRedisplay();
}

void mouse_click(int button, int state, int x, int y) {

    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (paused) break;
        if (state == GLUT_DOWN) {
            pulling = true;
            pull_now = clock();

            // avoid a jump when releasing fire button
            warped = true;
        }

        if (state == GLUT_UP) {
            pulling = false;
            player.fire(bow, quiver[player.curr_arrow]);
        }

        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) {
            player.nock(quiver[player.curr_arrow]);
        }

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

    dx *= player.sensitivity;
    dy *= player.sensitivity;

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

int init(int argc, char *argv[]) {

	if (argc > 2)
		create_and_compile_shaders(argv[1], argv[2]);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glShadeModel(GL_SMOOTH);
    glutSetCursor(GLUT_CURSOR_NONE);

    for (size_t i = 0; i < num_lights; i++)
        set_light(lights[i]);

    player.pos = {0, 2.5, 5};

    g_skybox = load_and_bind_tex("images/clouds.png");

    // initialise quiver
    for (size_t i = 0; i < MAX_CAPACITY; i++) {
        quiver[i] = Arrow(0.01f, 1.0f);
        quiver[i].make_handle();
    }

    srand(time(NULL));
    float x, y, z;

    // initialise targets
    for (size_t i = 0; i < MAX_TARGETS; i++) {
        x = rand() % 40 - 20;   // -20 <= x <= 20
        y = rand() % 9 + 1;     // 1 <= y <= 10
        z = rand() % 9 - 10;   // -10 <= z <= -2
        targets[i] = Target({x, y, z}, 1.0f, 0.4f);
        targets[i].make_handle();
    }

    bow.make_handle();
    target.make_handle();
    g_earth = make_earth();
    g_wall = make_wall();
    g_axes = make_axes();

    return 0;
}

int main(int argc, char *argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutInitWindowSize(WIN_W, WIN_H);

    int posx = (glutGet(GLUT_SCREEN_WIDTH) - WIN_W) / 2;
    int posy = (glutGet(GLUT_SCREEN_HEIGHT) -  WIN_H) / 2;
    glutInitWindowPosition(posx, posy);

    glutCreateWindow("Archery");

#ifndef __APPLE__ 
	GLenum err = glewInit();
	if (GLEW_OK!=err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}

	fprintf(stderr, "Using GLEW %s\n", glewGetString(GLEW_VERSION));


	if (glewIsSupported("GL_VERSION_2_0")) {
		printf("Ready for OpenGL 2.0\n");
    } else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}
#endif

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse_click);
    glutMotionFunc(mouse_motion);
    glutPassiveMotionFunc(mouse_motion);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    // glutFullScreen();

	fprintf(stderr, "Open GL version %s\n", glGetString(GL_VERSION));
    init(argc, argv);

    glutMainLoop();

    return 0;
}
