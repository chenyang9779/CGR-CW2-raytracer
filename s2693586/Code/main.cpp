#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include "json_reader.cpp"
#include "camera/camera.cpp"
#include "camera/ray.h"
#include "camera/material.h"
#include "camera/light.h"
#include "shading/blinn_phong.cpp"
#include "geometry/geometry.cpp"
#include "tone/tone_mapping.cpp"

void renderScene(const Camera &camera, const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders,
                 const std::vector<Triangle> &triangles, const std::vector<Light> &lights,
                 RenderMode renderMode, int width, int height, const std::string &outputFileName)
{
    std::vector<uint8_t> image(width * height * 3, 0); // Initialize image buffer

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int flippedX = width - 1 - x;

            // Generate the ray from the camera
            Ray ray = camera.generateRay(static_cast<float>(x), static_cast<float>(y));
            Intersection closestIntersection;
            closestIntersection.distance = std::numeric_limits<float>::max();

            // Check intersections with all shapes
            for (const auto &sphere : spheres)
            {
                Intersection intersection = sphere.intersect(ray);
                if (intersection.hit && intersection.distance < closestIntersection.distance)
                {
                    closestIntersection = intersection;
                }
            }
            for (const auto &cylinder : cylinders)
            {
                Intersection intersection = cylinder.intersect(ray);
                if (intersection.hit && intersection.distance < closestIntersection.distance)
                {
                    closestIntersection = intersection;
                }
            }
            for (const auto &triangle : triangles)
            {
                Intersection intersection = triangle.intersect(ray);
                if (intersection.hit && intersection.distance < closestIntersection.distance)
                {
                    closestIntersection = intersection;
                }
            }

            // Set pixel value based on render mode
            int index = (y * width + flippedX) * 3;
            if (closestIntersection.hit)
            {
                Vector3 color;

                if (renderMode == RenderMode::BINARY)
                {
                    // Binary shading: set color to red if there's an intersection
                    color = Vector3(1.0f, 0.0f, 0.0f);
                }
                else if (renderMode == RenderMode::PHONG)
                {
                    // Phong shading: use Blinn-Phong shading to calculate color
                    color = blinnPhongShading(closestIntersection, ray, lights, spheres, cylinders, triangles);
                }

                // Apply tone mapping to the color using the camera's exposure value
                Vector3 mappedColor = toneMap(color, camera.exposure);

                // Convert color from [0, 1] range to [0, 255] for RGB and store in image buffer
                image[index] = static_cast<uint8_t>(std::min(mappedColor.x * 255.0f, 255.0f)); // R
                image[index + 1] = static_cast<uint8_t>(std::min(mappedColor.y * 255.0f, 255.0f)); // G
                image[index + 2] = static_cast<uint8_t>(std::min(mappedColor.z * 255.0f, 255.0f)); // B
            }
        }
    }

    // Write the binary image to file (PPM format)
    std::ofstream outFile(outputFileName, std::ios::binary);
    if (outFile.is_open())
    {
        outFile << "P6\n" << width << " " << height << "\n255\n";
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

    try
    {
        // Load the scene from the JSON file
        std::string fileName = argv[1];
        std::string outputFileName = argv[2];
        SceneData sceneData = readSceneFromJson(fileName);

        // Render the scene and write the output to a binary image file
        renderScene(sceneData.camera, sceneData.spheres, sceneData.cylinders, sceneData.triangles,
                    sceneData.lights, sceneData.renderMode, sceneData.width, sceneData.height, outputFileName);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

