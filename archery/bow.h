#ifndef BOW_H
#define BOW_H

#include <cstddef>

class Bow {
public:
    size_t handle;
    float thickness;
    float length;

    int texture;

    Bow(float t, float l);
    void make_handle();
    void draw();
}; 

#endif
