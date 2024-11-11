#include <iostream>
#include <vector>
#include <chrono>
#include <limits>
#include <fstream>
#include "json_reader.cpp"
#include "camera/camera.cpp"
#include "camera/ray.h"
#include "material/material.h"
#include "camera/light.h"
#include "shading/blinn_phong.cpp"
#include "geometry/geometry.cpp"
#include "tone/tone_mapping.cpp"
#include "bvh/bvh_node.h"
#include <memory>

// Function to collect all geometry objects from SceneData
std::vector<std::shared_ptr<const Geometry>> collectGeometries(const SceneData &sceneData)
{
    std::vector<std::shared_ptr<const Geometry>> geometries;

    // Add spheres to the geometries list
    for (const auto &sphere : sceneData.spheres)
    {
        geometries.push_back(std::make_shared<Sphere>(sphere));
    }

    // Add cylinders to the geometries list
    for (const auto &cylinder : sceneData.cylinders)
    {
        geometries.push_back(std::make_shared<Cylinder>(cylinder));
    }

    // Add triangles to the geometries list
    for (const auto &triangle : sceneData.triangles)
    {
        geometries.push_back(std::make_shared<Triangle>(triangle));
    }

    return geometries;
}

void renderScene(const Camera &camera, const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders,
                 const std::vector<Triangle> &triangles, const std::vector<Light> &lights,
                 RenderMode renderMode, int width, int height, const Vector3 &backgroundColor, int nbounces, const std::string &outputFileName)
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
            Vector3 color;
            if (closestIntersection.hit)
            {
                if (renderMode == RenderMode::BINARY)
                {
                    // Binary shading: set color to red if there's an intersection
                    color = Vector3(1.0f, 0.0f, 0.0f);
                }
                else if (renderMode == RenderMode::PHONG)
                {
                    // Phong shading: use Blinn-Phong shading to calculate color
                    color = blinnPhongShading(closestIntersection, ray, lights, spheres, cylinders, triangles, nbounces, backgroundColor);
                    color = toneMap(color, camera.exposure);
                }
            }
            else
            {
                if (renderMode == RenderMode::PHONG)
                {
                    color = backgroundColor;
                }
            }
            // Convert color from [0, 1] range to [0, 255] for RGB and store in image buffer
            image[index] = static_cast<uint8_t>(std::min(color.x * 255.0f, 255.0f));     // R
            image[index + 1] = static_cast<uint8_t>(std::min(color.y * 255.0f, 255.0f)); // G
            image[index + 2] = static_cast<uint8_t>(std::min(color.z * 255.0f, 255.0f)); // B
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

void renderSceneBVH(const Camera &camera, const BVHNode *root, const std::vector<Light> &lights,
                    RenderMode renderMode, int width, int height, const Vector3 &backgroundColor,
                    int nbounces, const std::string &outputFileName)
{
    // Implementation of rendering using the BVH acceleration structure
    std::vector<uint8_t> image(width * height * 3, 0); // Image buffer

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int flippedX = width - 1 - x;

            // Generate ray from camera
            Ray ray = camera.generateRay(static_cast<float>(x), static_cast<float>(y));
            Intersection closestIntersection;
            closestIntersection.distance = std::numeric_limits<float>::max();

            // Check for intersection with BVH root
            if (root->intersect(ray, closestIntersection))
            {
                // If intersection occurs, determine color based on render mode
                Vector3 color;
                if (renderMode == RenderMode::BINARY)
                {
                    color = Vector3(1.0f, 0.0f, 0.0f); // Set to red if intersection
                }
                else if (renderMode == RenderMode::PHONG)
                {
                    color = blinnPhongShadingBVH(closestIntersection, ray, lights, root, nbounces - 1, backgroundColor);
                    color = toneMap(color, camera.exposure);
                }

                // Convert color to [0, 255] range for image output
                int index = (y * width + flippedX) * 3;
                image[index] = static_cast<uint8_t>(std::min(color.x * 255.0f, 255.0f));
                image[index + 1] = static_cast<uint8_t>(std::min(color.y * 255.0f, 255.0f));
                image[index + 2] = static_cast<uint8_t>(std::min(color.z * 255.0f, 255.0f));
            }
            else
            {
                // Assign background color
                int index = (y * width + flippedX) * 3;
                image[index] = static_cast<uint8_t>(std::min(backgroundColor.x * 255.0f, 255.0f));
                image[index + 1] = static_cast<uint8_t>(std::min(backgroundColor.y * 255.0f, 255.0f));
                image[index + 2] = static_cast<uint8_t>(std::min(backgroundColor.z * 255.0f, 255.0f));
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

void renderWithoutBVH(const SceneData &sceneData, const std::string &outputFileName)
{
    // Render the scene by checking intersections with all geometries directly
    renderScene(sceneData.camera, sceneData.spheres, sceneData.cylinders, sceneData.triangles,
                sceneData.lights, sceneData.renderMode, sceneData.width, sceneData.height,
                sceneData.backgroundColor, sceneData.nbounces, outputFileName);
}

void renderWithBVH(const SceneData &sceneData, BVHNode *root, const std::string &outputFileName)
{
    // Use BVH to render the scene and find intersections more efficiently
    renderSceneBVH(sceneData.camera, root, sceneData.lights, sceneData.renderMode,
                   sceneData.width, sceneData.height, sceneData.backgroundColor,
                   sceneData.nbounces, outputFileName);
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_json_file> <output_file> <use_bvh (0 or 1)>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    std::string outputFileName = argv[2];
    bool useBVH = (std::stoi(argv[3]) != 0); // Use BVH if the third argument is 1

    // Load the scene from the JSON file
    SceneData sceneData = readSceneFromJson(fileName);

    // Print whether BVH is enabled or not
    std::cout << "BVH enabled: " << (useBVH ? "Yes" : "No") << std::endl;

    // Start timing the render process
    auto start = std::chrono::high_resolution_clock::now();

    if (useBVH)
    {
        // Collect geometries from scene data
        std::vector<std::shared_ptr<const Geometry>> geometries = collectGeometries(sceneData);

        // Build the BVH using collected geometries
        std::unique_ptr<BVHNode> root = BVHNode::build(geometries);
        renderWithBVH(sceneData, root.get(), outputFileName);
    }
    else
    {
        // Render without BVH
        renderWithoutBVH(sceneData, outputFileName);
    }

    // Stop timing the render process
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Render Time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
