#include "camera.h"
#include <cmath> // For mathematical operations like tan and M_PI

// Camera constructor
// Parameters:
// - position: The position of the camera in world space
// - lookAt: The point the camera is looking at
// - up: The up direction vector for the camera
// - fov: Field of view in degrees
// - width, height: Dimensions of the image (in pixels)
// - exposure: Camera exposure setting
// - aperture: Size of the aperture for depth of field effects (default 0.0f)
// - focalDistance: Distance to the focal plane for depth of field (default 1.0f)
Camera::Camera(const Vector3 &position, const Vector3 &lookAt, const Vector3 &up, float fov, int width, int height, float exposure, float aperture, float focalDistance)
    : position(position), fov(fov), width(width), height(height), exposure(exposure), aperture(aperture), focalDistance(focalDistance)
{
    // Calculate the forward direction (camera looks from position to lookAt)
    forward = (lookAt - position).normalize();

    // Calculate the right vector (perpendicular to forward and up)
    right = forward.cross(up).normalize();

    // Recalculate the true up vector (orthogonal to forward and right)
    this->up = right.cross(forward).normalize();

    // Calculate aspect ratio of the image
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // Calculate scale factor based on field of view
    scale = std::tan(fov * 0.5 * M_PI / 180.0f); // Convert FOV to radians and calculate tangent
}

// Function to generate a ray for a specific pixel
// Parameters:
// - x, y: Pixel coordinates in the image (0-based indexing)
// Returns: A Ray object originating from the camera and passing through the pixel
Ray Camera::generateRay(float x, float y) const
{
    // Convert pixel coordinates to normalized device coordinates (NDC)
    float ndcX = (2 * (x + 0.5f) / static_cast<float>(width) - 1) * aspectRatio * scale; // Scale by aspect ratio and FOV
    float ndcY = (1 - 2 * (y + 0.5f) / static_cast<float>(height)) * scale;              // Invert y-axis for image plane
    Vector3 direction = (forward + ndcX * right + ndcY * up).normalize();                // Compute ray direction

    // Handle pinhole camera case (aperture = 0)
    if (aperture == 0.0f)
    {
        return Ray(position, direction); // Direct ray without depth of field effects
    }

    // Simulate depth of field by sampling a random point on the aperture
    Vector3 apertureSample = sampleAperture();
    // Compute the focal point (where the ray converges based on focal distance)
    float t = focalDistance / direction.dot(forward); // Scale based on focal distance
    Vector3 focalPoint = position + direction * t;

    // Compute the new ray origin (offset by aperture sample) and direction
    Vector3 rayOrigin = position + apertureSample;
    Vector3 newDirection = (focalPoint - rayOrigin).normalize();

    // Return the ray with modified origin and direction
    return Ray(rayOrigin, newDirection);
}

// Helper function to sample a random point on the aperture (lens)
// Returns: A Vector3 representing the sampled point on the aperture
Vector3 Camera::sampleAperture() const
{

    // Use random number generators for uniform sampling within a unit disk
    static std::random_device rd;                                   // Random device for seeding
    static std::mt19937 gen(rd());                                  // Mersenne Twister RNG
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f); // Uniform distribution in [-1, 1]

    float x, y;
    do
    {
        x = dist(gen);
        y = dist(gen);
    } while (x * x + y * y > 1.0f); // Ensure the sample is inside the unit disk

    return Vector3(x * aperture * 0.5f, y * aperture * 0.5f, 0.0f); // Scale to aperture size
}
