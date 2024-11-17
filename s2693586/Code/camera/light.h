#ifndef LIGHT_H
#define LIGHT_H

#include "../camera/vector3.h" // Include Vector3 class for representing 3D vectors

// Class representing a light source in the scene
// A light source is defined by its position in 3D space and its intensity (RGB values)
class Light
{
public:
    Vector3 position;  // Position of the light source in world space
    Vector3 intensity; // Intensity of the light, represented as RGB values

    // Constructor
    // Initializes the light source with a specified position and intensity
    Light(const Vector3 &position, const Vector3 &intensity)
        : position(position), intensity(intensity) {}
};

#endif // LIGHT_H
