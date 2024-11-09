#ifndef LIGHT_H
#define LIGHT_H

#include "../camera/vector3.h"

class Light
{
public:
    Vector3 position;
    Vector3 intensity; // Intensity as a vector, representing RGB values

    Light(const Vector3 &position, const Vector3 &intensity)
        : position(position), intensity(intensity) {}
};

#endif // LIGHT_H
