#ifndef JSON_READER_H
#define JSON_READER_H

// Includes
#include <string>
#include <vector>
#include "nlohmann/json.hpp"   // Includes the nlohmann/json library for JSON parsing
#include "camera/camera.h"     // Camera class for view setup
#include "camera/light.h"      // Light definitions
#include "material/material.h" // Material properties for rendering
#include "geometry/geometry.h" // Geometric objects (spheres, cylinders, triangles, etc.)

using json = nlohmann::json; // Alias for easier use of the nlohmann::json namespace

// Enumeration for different render modes
enum class RenderMode
{
    BINARY, // Binary mode: pixels are either hit or miss
    PHONG   // Phong mode: realistic shading based on lighting
};

class SceneData
{
public:
    Camera camera;                   // Camera object for defining the view
    int width, height;               // Dimensions of the render image
    int nbounces;                    // Number of bounces for recursive raytracing
    RenderMode renderMode;           // Current render mode (BINARY or PHONG)
    std::vector<Light> lights;       // List of light sources in the scene
    std::vector<Sphere> spheres;     // List of spheres in the scene
    std::vector<Cylinder> cylinders; // List of cylinders in the scene
    std::vector<Triangle> triangles; // List of triangles in the scene
    Vector3 backgroundColor;         // Background color for the scene

    // Constructor to initialize the scene data
    SceneData(const Camera& cam, int w, int h, RenderMode mode)
        : camera(cam),                    // Initialize the camera
          width(w),                       // Set the image width
          height(h),                      // Set the image height
          renderMode(mode),               // Set the render mode
          backgroundColor(Vector3(0.0f, 0.0f, 0.0f)), // Default background color (black)
          nbounces(nbounces) {}           // Default number of ray bounces
};

#endif // JSON_READER_H
