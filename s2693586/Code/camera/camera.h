#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h"
#include "ray.h"

class Camera
{
public:
    Camera(const Vector3 &position, const Vector3 &lookAt, const Vector3 &up, float fov, int width, int height);
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
};

#endif // CAMERA_H