#ifndef RAY_H
#define RAY_H

#include "vector3.h"

class Ray {
public:
    Vector3 origin;
    Vector3 direction;

    Ray() : origin(Vector3()), direction(Vector3(0, 0, 1)) {}
    Ray(const Vector3& origin, const Vector3& direction) : origin(origin), direction(direction.normalize()) {}
};

#endif // RAY_H