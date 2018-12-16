#ifndef UTIL_H
#define UTIL_H

#include <ctime>
#include <cmath>

struct vec3 {
    float x, y, z;
    vec3 operator*(const float& k) {
        struct vec3 res;
        res.x = x * k;
        res.y = y * k;
        res.z = z * k;
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
    /*
    std::ostream& operator<<(std::ostream &strm, const vec3 &v) {
        return strm << "<" << v.x << ", " << v.y << "," << v.z << ">";
    }
    */
};

void draw_capped_cylinder(const float r, const float h, const int slices=32, const int stacks=32);
vec3 cross(vec3, vec3);

#endif
