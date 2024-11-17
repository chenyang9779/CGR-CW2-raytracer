#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h" // Include Vector3 class for 3D vector operations
#include "ray.h"     // Include Ray class for ray generation

// Class representing a camera in 3D space
// A camera generates rays for ray tracing, simulating the projection of a scene onto an image plane
class Camera
{
public:
    // Constructor
    // Parameters:
    // - position: The position of the camera in world space
    // - lookAt: The point the camera is looking at
    // - up: The up direction for the camera (used to compute the camera's orientation)
    // - fov: Field of view in degrees (controls the camera's perspective)
    // - width: Image width in pixels
    // - height: Image height in pixels
    // - exposure: Exposure setting for brightness adjustment
    // - aperture: Aperture size (default is 0.0f for a pinhole camera)
    // - focalDistance: Distance to the focal plane (default is 1.0f for sharp focus)
    Camera(const Vector3 &position, const Vector3 &lookAt, const Vector3 &up, float fov, int width, int height, float exposure, float aperture = 0.0f, float focalDistance = 1.0f);

    // Generates a ray for a given pixel on the image plane
    // Parameters:
    // - x, y: Pixel coordinates in the image plane
    // Returns: A Ray object starting from the camera and pointing towards the scene
    Ray generateRay(float x, float y) const;

private:
    Vector3 position;    // Camera position in world space
    Vector3 forward;     // Forward direction of the camera (normalized)
    Vector3 right;       // Right direction of the camera (normalized)
    Vector3 up;          // Up direction of the camera (normalized)
    float fov;           // Field of view in degrees
    int width;           // Width of the rendered image in pixels
    int height;          // Height of the rendered image in pixels
    float aspectRatio;   // Aspect ratio of the image (width / height)
    float scale;         // Scaling factor based on FOV (used for ray direction calculation)

    // Depth of field (defocus) properties
    float aperture;      // Size of the camera's aperture (controls blur strength)
    float focalDistance; // Distance to the focal plane (determines the focus depth)

    // Helper function to sample a random point on the aperture (lens)
    // Used for simulating depth of field effects
    Vector3 sampleAperture() const;

public:
    float exposure; // Exposure setting to control image brightness
};

#endif // CAMERA_H
