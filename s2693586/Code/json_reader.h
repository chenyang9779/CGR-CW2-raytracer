#ifndef JSON_READER_H
#define JSON_READER_H

#include <string>
#include <vector>
#include "nlohmann/json.hpp" // Assuming you're using the nlohmann/json library
#include "camera/camera.h"
#include "camera/light.h"
#include "camera/material.h"
#include "geometry/geometry.h"

using json = nlohmann::json;

enum class RenderMode {
    BINARY,
    PHONG
};

class SceneData {
public:
    Camera camera;
    int width, height;
    RenderMode renderMode;
    std::vector<Light> lights;           // List of lights in the scene
    std::vector<Sphere> spheres;         // List of spheres in the scene
    std::vector<Cylinder> cylinders;     // List of cylinders in the scene
    std::vector<Triangle> triangles;     // List of triangles in the scene

    SceneData(const Camera& cam, int w, int h, RenderMode mode)
        : camera(cam), width(w), height(h), renderMode(mode) {}
};

#endif // JSON_READER_H
