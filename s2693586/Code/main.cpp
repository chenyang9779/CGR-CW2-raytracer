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
#include "shading/blinn_phong_bvh.cpp"
#include <memory>

void writeBinaryImageToPPM(const std::string &outputFileName, int width, int height, const std::vector<unsigned char> &image)
{
    // Open the output file in binary mode
    std::ofstream outFile(outputFileName, std::ios::binary);

    // Check if the file is open
    if (outFile.is_open())
    {
        // Write PPM header (P6 format)
        outFile << "P6\n"
                << width << " " << height << "\n255\n";

        // Write binary pixel data
        outFile.write(reinterpret_cast<const char *>(image.data()), image.size());

        // Close the file
        outFile.close();
    }
    else
    {
        // Output error message if the file cannot be opened
        std::cerr << "Failed to open output file: " << outputFileName << std::endl;
    }
}

// Function to set the color of a pixel in the image buffer
void setPixelColor(int x, int y, int width, const Vector3 &toneMappedColor, std::vector<uint8_t> &image)
{
    // Calculate the index in the 1D image vector
    int flippedX = width - 1 - x;
    int index = (y * width + flippedX) * 3;

    // Set the RGB values, clamping them to [0, 255] and converting to uint8_t
    image[index] = static_cast<uint8_t>(std::min(toneMappedColor.x * 255.0f, 255.0f));     // R
    image[index + 1] = static_cast<uint8_t>(std::min(toneMappedColor.y * 255.0f, 255.0f)); // G
    image[index + 2] = static_cast<uint8_t>(std::min(toneMappedColor.z * 255.0f, 255.0f)); // B
}

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
                 RenderMode renderMode, int width, int height, const Vector3 &backgroundColor, int nbounces,
                 const std::string &outputFileName, bool applyToneMap)
{
    std::vector<uint8_t> image(width * height * 3, 0); // Initialize image buffer
    std::vector<Vector3> hdrColors(width * height);    // Buffer to store HDR colors

    // First pass: calculate HDR colors for each pixel
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
            Vector3 color = backgroundColor; // Default to background color
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
                }
            }

            // Store color in the HDR buffer
            hdrColors[y * width + x] = color;
        }
    }

    // Second pass: Apply ACES tone mapping to each pixel
    Vector3 minColor(std::numeric_limits<float>::max());
    Vector3 maxColor(-std::numeric_limits<float>::max());

    // Calculate min and max color values for tone mapping
    if (applyToneMap)
    {
        for (const auto &color : hdrColors)
        {
            if (color != backgroundColor)
            {
                minColor = Vector3(
                    std::min(minColor.x, color.x),
                    std::min(minColor.y, color.y),
                    std::min(minColor.z, color.z));

                maxColor = Vector3(
                    std::max(maxColor.x, color.x),
                    std::max(maxColor.y, color.y),
                    std::max(maxColor.z, color.z));
            }
        }
    }

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (renderMode == RenderMode::BINARY)
            {
                Vector3 toneMappedColor = hdrColors[y * width + x];
                setPixelColor(x, y, width, toneMappedColor, image);
            }
            else
            {
                Vector3 hdrColor = hdrColors[y * width + x];
                if (applyToneMap)
                {
                    hdrColor = toneMap(hdrColor, camera.exposure, minColor, maxColor, backgroundColor); // Use ACES tone mapping
                }

                setPixelColor(x, y, width, hdrColor, image);
            }
        }
    }

    // Write the binary image to file (PPM format)
    writeBinaryImageToPPM(outputFileName, width, height, image);
}

void renderSceneBVH(const Camera &camera, const BVHNode *root, const std::vector<Light> &lights,
                    RenderMode renderMode, int width, int height, const Vector3 &backgroundColor,
                    int nbounces, const std::string &outputFileName, bool applyToneMap)
{
    // Implementation of rendering using the BVH acceleration structure
    std::vector<uint8_t> image(width * height * 3, 0); // Image buffer
    std::vector<Vector3> hdrColors(width * height);    // Buffer to store HDR colors

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
            Vector3 color = backgroundColor; // Default to background color
            if (root->intersect(ray, closestIntersection))
            {
                // If intersection occurs, determine color based on render mode
                if (renderMode == RenderMode::BINARY)
                {
                    color = Vector3(1.0f, 0.0f, 0.0f); // Set to red if intersection
                }
                else if (renderMode == RenderMode::PHONG)
                {
                    color = blinnPhongShadingBVH(closestIntersection, ray, lights, root, nbounces - 1, backgroundColor);
                }
            }
            hdrColors[y * width + x] = color;
        }
    }
    // Second pass: Apply ACES tone mapping to each pixel
    Vector3 minColor(std::numeric_limits<float>::max());
    Vector3 maxColor(-std::numeric_limits<float>::max());

    // Calculate min and max color values for tone mapping
    if (applyToneMap)
    {
        for (const auto &color : hdrColors)
        {
            if (color != backgroundColor)
            {
                minColor = Vector3(
                    std::min(minColor.x, color.x),
                    std::min(minColor.y, color.y),
                    std::min(minColor.z, color.z));

                maxColor = Vector3(
                    std::max(maxColor.x, color.x),
                    std::max(maxColor.y, color.y),
                    std::max(maxColor.z, color.z));
            }
        }
    }

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (renderMode == RenderMode::BINARY)
            {
                Vector3 hdrColor = hdrColors[y * width + x];
                setPixelColor(x, y, width, hdrColor, image);
            }
            else
            {
                Vector3 hdrColor = hdrColors[y * width + x];
                if (applyToneMap)
                {
                    hdrColor = toneMap(hdrColor, camera.exposure, minColor, maxColor, backgroundColor); // Use ACES tone mapping
                }
                setPixelColor(x, y, width, hdrColor, image);
            }
        }
    }

    // Write the binary image to file (PPM format)
    writeBinaryImageToPPM(outputFileName, width, height, image);
}

void renderWithoutBVH(const SceneData &sceneData, const std::string &outputFileName, bool applyToneMap)
{
    renderScene(sceneData.camera, sceneData.spheres, sceneData.cylinders, sceneData.triangles,
                sceneData.lights, sceneData.renderMode, sceneData.width, sceneData.height,
                sceneData.backgroundColor, sceneData.nbounces, outputFileName, applyToneMap);
}

void renderWithBVH(const SceneData &sceneData, BVHNode *root, const std::string &outputFileName, bool applyToneMap)
{
    renderSceneBVH(sceneData.camera, root, sceneData.lights, sceneData.renderMode,
                   sceneData.width, sceneData.height, sceneData.backgroundColor,
                   sceneData.nbounces, outputFileName, applyToneMap);
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_json_file> <output_file> <use_bvh (0 or 1)> <apply_tone_map (0 or 1)>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    std::string outputFileName = argv[2];
    bool useBVH = (std::stoi(argv[3]) != 0);       // Use BVH if the third argument is 1
    bool applyToneMap = (std::stoi(argv[4]) != 0); // Apply tone mapping if the fourth argument is 1

    // Load the scene from the JSON file
    SceneData sceneData = readSceneFromJson(fileName);

    // Print whether BVH and tone mapping are enabled
    std::cout << "BVH enabled: " << (useBVH ? "Yes" : "No") << std::endl;
    std::cout << "Tone Mapping enabled: " << (applyToneMap ? "Yes" : "No") << std::endl;

    // Start timing the render process
    auto start = std::chrono::high_resolution_clock::now();

    if (useBVH)
    {
        // Collect geometries from scene data
        std::vector<std::shared_ptr<const Geometry>> geometries = collectGeometries(sceneData);

        // Build the BVH using collected geometries
        std::unique_ptr<BVHNode> root = BVHNode::build(geometries);
        renderWithBVH(sceneData, root.get(), outputFileName, applyToneMap);
    }
    else
    {
        // Render without BVH
        renderWithoutBVH(sceneData, outputFileName, applyToneMap);
    }

    // Stop timing the render process
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Render Time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
