#ifndef UTIL_H
#define UTIL_H

#include <ctime>
#include <cmath>
#include <ostream>

#define ORTHO_SIZE 1000

struct vec3 {
    float x, y, z;
    vec3 operator*(const float& k) {
        vec3 res;
        res.x = x * k;
        res.y = y * k;
        res.z = z * k;
        return res;
    }
    vec3 operator/(const float &k) {
        vec3 res;
        res.x = x / k;
        res.y = y / k;
        res.z = z / k;
        return res;
    }
    vec3& operator+=(const vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    vec3& operator+=(const float k) {
        x += k;
        y += k;
        z += k;
        return *this;
    }
    vec3& operator*=(const float k) {
        x *= k;
        y *= k;
        z *= k;
        return *this;
    }
    float operator*(const vec3& v) {
        return x * v.x + y * v.y + z * v.z;
    }
    float len() {
        return sqrt(x*x + y*y + z*z);
    }
    vec3 normalize() {
        float l = len();
        vec3 norm = {
            x / l,
            y / l,
            z / l
        };
        return norm;
    }
};

void draw_text(const int, const int, const char *);
void draw_centered(const int, const char *);
void draw_raligned(const int, const int, const char *);

unsigned int load_and_bind_tex(const char *f);
int png_load(const char *f, int *w, int *h, char **img);

void draw_capped_cylinder(const float r, const float h, const int slices=32, const int stacks=32);
void draw_cone(const float b, const float h, const int slices=32, const int stacks=32);

std::ostream& operator<<(std::ostream &strm, const vec3& v);
vec3 cross(vec3, vec3);
vec3 normalize(vec3);
float dot(vec3, vec3);
float dist(vec3, vec3);

void print_shader_info_log(unsigned int shader_obj);
void print_program_info_log(unsigned int shader_obj);
void create_and_compile_shaders(const char *vs_fname, const char *fs_fname);

#endif
