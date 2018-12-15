#ifndef BOW_H
#define BOW_H

#include <cstddef>

class Bow {
public:
    size_t handle;
    float thickness;
    float length;

    Bow(float t, float l);
    void make_handle();
    void draw();
}; 

#endif
