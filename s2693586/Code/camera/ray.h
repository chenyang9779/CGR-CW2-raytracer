#ifndef RAY_H
#define RAY_H

#include "vector3.h" // Include Vector3 class for representing 3D vectors

// Class representing a ray in 3D space
// A ray is defined by an origin point and a normalized direction vector
class Ray
{
public:
    Vector3 origin;    // The starting point of the ray
    Vector3 direction; // The direction of the ray (normalized)

    // Default constructor
    // Initializes the ray with an origin at (0, 0, 0) and a default direction (0, 1, 0)
    Ray() : origin(Vector3()), direction(Vector3(0, 1, 0)) {}

    // Parameterized constructor
    // Initializes the ray with a specified origin and direction
    // The direction vector is automatically normalized
    Ray(const Vector3 &origin, const Vector3 &direction) : origin(origin), direction(direction.normalize()) {}
};

#endif // RAY_H
