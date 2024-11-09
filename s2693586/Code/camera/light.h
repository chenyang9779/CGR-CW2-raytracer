#ifndef LIGHT_H
#define LIGHT_H

#include "vector3.h"  // Assuming you have a Vector3 class defined in "vector3.h"

class Light {
public:
    Vector3 position;
    Vector3 color;  // RGB values of the light color, normalized between 0-1
    float intensity;

    Light(const Vector3& position, const Vector3& color, float intensity)
        : position(position), color(color), intensity(intensity) {}
};

#endif