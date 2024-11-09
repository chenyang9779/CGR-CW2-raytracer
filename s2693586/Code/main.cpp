#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include "json_reader.cpp"
#include "camera/camera.cpp"
#include "camera/ray.h"
#include "geometry/geometry.cpp"

void renderScene(const Camera &camera, const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles, int width, int height, const std::string &outputFileName)
{
    std::vector<uint8_t> image(width * height * 3, 0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int flippedX = width - 1 - x;

            // Generate the ray as usual
            Ray ray = camera.generateRay(static_cast<float>(x), static_cast<float>(y));
            Intersection closestIntersection;
            closestIntersection.distance = std::numeric_limits<float>::max();

            // Check intersections with spheres
            for (const auto &sphere : spheres)
            {
                Intersection intersection = sphere.intersect(ray);
                if (intersection.hit && intersection.distance < closestIntersection.distance)
                {
                    closestIntersection = intersection;
                }
            }

            // Check intersections with cylinders
            for (const auto &cylinder : cylinders)
            {
                Intersection intersection = cylinder.intersect(ray);
                if (intersection.hit && intersection.distance < closestIntersection.distance)
                {
                    closestIntersection = intersection;
                }
            }

            // Check intersections with triangles
            for (const auto &triangle : triangles)
            {
                Intersection intersection = triangle.intersect(ray);
                if (intersection.hit && intersection.distance < closestIntersection.distance)
                {
                    closestIntersection = intersection;
                }
            }

            // Set pixel value (white for hit, black for no hit)
            int index = (y * width + flippedX) * 3;
            if (closestIntersection.hit)
            {
                image[index] = 255;   // R
                image[index + 1] = 0; // G
                image[index + 2] = 0; // B
            }
        }
    }

    // Write the binary image to file (PPM format)
    std::ofstream outFile(outputFileName, std::ios::binary);
    if (outFile.is_open())
    {
        outFile << "P6\n"
                << width << " " << height << "\n255\n";
        outFile.write(reinterpret_cast<char *>(image.data()), image.size());
        outFile.close();
    }
    else
    {
        std::cerr << "Failed to open output file." << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_json_file> <output_file>" << std::endl;
        return 1;
    }

    // Load JSON configuration
    std::string fileName = argv[1];
    std::string outputFileName = argv[2];
    json config = readJsonFile(fileName);
    if (config.is_null())
    {
        std::cerr << "Failed to read JSON data." << std::endl;
        return 1;
    }

    try
    {
        // Extract camera data from JSON
        Vector3 cameraPosition = Vector3(config["camera"]["position"][0], config["camera"]["position"][1], config["camera"]["position"][2]);
        Vector3 lookAt = Vector3(config["camera"]["lookAt"][0], config["camera"]["lookAt"][1], config["camera"]["lookAt"][2]);
        Vector3 upVector = Vector3(config["camera"]["upVector"][0], config["camera"]["upVector"][1], config["camera"]["upVector"][2]);
        float fov = config["camera"]["fov"];
        int width = config["camera"]["width"];
        int height = config["camera"]["height"];

        // Initialize the camera
        Camera camera(cameraPosition, lookAt, upVector, fov, width, height);

        // Extract shapes from JSON
        std::vector<Sphere> spheres;
        std::vector<Cylinder> cylinders;
        std::vector<Triangle> triangles;

        if (config.contains("scene") && config["scene"].contains("shapes"))
        {
            for (const auto &shape : config["scene"]["shapes"])
            {
                if (shape["type"] == "sphere")
                {
                    Vector3 center = Vector3(shape["center"][0], shape["center"][1], shape["center"][2]);
                    float radius = shape["radius"];
                    spheres.emplace_back(center, radius);
                }
                else if (shape["type"] == "cylinder")
                {
                    Vector3 center = Vector3(shape["center"][0], shape["center"][1], shape["center"][2]);
                    Vector3 axis = Vector3(shape["axis"][0], shape["axis"][1], shape["axis"][2]).normalize();
                    float radius = shape["radius"];
                    float height = shape["height"];
                    cylinders.emplace_back(center, axis, radius, height);
                }
                else if (shape["type"] == "triangle")
                {
                    Vector3 v0 = Vector3(shape["v0"][0], shape["v0"][1], shape["v0"][2]);
                    Vector3 v1 = Vector3(shape["v1"][0], shape["v1"][1], shape["v1"][2]);
                    Vector3 v2 = Vector3(shape["v2"][0], shape["v2"][1], shape["v2"][2]);
                    triangles.emplace_back(v0, v1, v2);
                }
            }
        }

        // Render the scene and write the output to a binary image file
        renderScene(camera, spheres, cylinders, triangles, width, height, outputFileName);
    }
    catch (const json::exception &e)
    {
        std::cerr << "Error parsing scene configuration: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
