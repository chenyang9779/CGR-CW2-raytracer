#include "camera.h"
#include <cmath>

Camera::Camera(const Vector3 &position, const Vector3 &lookAt, const Vector3 &up, float fov, int width, int height, float exposure)
    : position(position), fov(fov), width(width), height(height), exposure(exposure)
{
    forward = (lookAt - position).normalize();
    right = forward.cross(up).normalize();
    this->up = right.cross(forward).normalize();
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    scale = std::tan(fov * 0.5 * M_PI / 180.0f);
}

Ray Camera::generateRay(float x, float y) const
{
    float ndcX = (2 * (x + 0.5f) / static_cast<float>(width) - 1) * aspectRatio * scale;
    float ndcY = (1 - 2 * (y + 0.5f) / static_cast<float>(height)) * scale;
    Vector3 direction = (forward + ndcX * right + ndcY * up).normalize();
    return Ray(position, direction);
}
