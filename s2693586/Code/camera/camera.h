#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h"
#include "ray.h"

class Camera
{
public:
    Camera(const Vector3 &position, const Vector3 &lookAt, const Vector3 &up, float fov, int width, int height, float exposure, float aperture = 0.0f, float focalDistance = 1.0f);
    Ray generateRay(float x, float y) const;

private:
    Vector3 position;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    float fov;
    int width;
    int height;
    float aspectRatio;
    float scale;

    // Defocus properties
    float aperture;      // Aperture size (controls blur strength)
    float focalDistance; // Focal plane distance (controls focus depth)

    // Helper function to sample a random point on the aperture (lens)
    Vector3 sampleAperture() const;

public:
    float exposure;
};

#endif // CAMERA_H