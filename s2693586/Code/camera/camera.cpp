#include "camera.h"
#include <cmath>

Camera::Camera(const Vector3 &position, const Vector3 &lookAt, const Vector3 &up, float fov, int width, int height, float exposure, float aperture, float focalDistance)
    : position(position), fov(fov), width(width), height(height), exposure(exposure), aperture(aperture), focalDistance(focalDistance)
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

    if (aperture == 0.0f)
    {
        return Ray(position, direction);
    }

    Vector3 apertureSample = sampleAperture();
    float t = focalDistance / direction.dot(forward); // Scale to focal distance
    Vector3 focalPoint = position + direction * t;
    Vector3 rayOrigin = position + apertureSample;
    Vector3 newDirection = (focalPoint - rayOrigin).normalize();

    return Ray(rayOrigin, newDirection);
}

// Helper function to sample a random point on the lens aperture
Vector3 Camera::sampleAperture() const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    float x, y;
    do
    {
        x = dist(gen);
        y = dist(gen);
    } while (x * x + y * y > 1.0f); // Ensure the sample is inside the unit disk

    return Vector3(x * aperture * 0.5f, y * aperture * 0.5f, 0.0f); // Scale to aperture size
}
