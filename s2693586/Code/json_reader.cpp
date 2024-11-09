#include "json_reader.h"
#include <iostream>
#include <fstream>

SceneData readSceneFromJson(const std::string &fileName)
{
    // Open the JSON file
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open())
    {
        std::cerr << "Could not open the file!" << std::endl;
        throw std::runtime_error("Could not open the JSON file.");
    }

    // Parse the JSON content
    json config;
    try
    {
        inputFile >> config;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "Parse error: " << e.what() << std::endl;
        throw std::runtime_error("Failed to parse JSON file.");
    }

    // Close the file
    inputFile.close();

    try
    {
        // Extract camera data from JSON
        Vector3 cameraPosition = Vector3(config["camera"]["position"][0], config["camera"]["position"][1], config["camera"]["position"][2]);
        Vector3 lookAt = Vector3(config["camera"]["lookAt"][0], config["camera"]["lookAt"][1], config["camera"]["lookAt"][2]);
        Vector3 upVector = Vector3(config["camera"]["upVector"][0], config["camera"]["upVector"][1], config["camera"]["upVector"][2]);
        float fov = config["camera"]["fov"];
        int width = config["camera"]["width"];
        int height = config["camera"]["height"];
        float exposure = config["camera"]["exposure"];

        // Determine the render mode
        RenderMode renderMode = RenderMode::BINARY;
        if (config.contains("rendermode"))
        {
            std::string modeStr = config["rendermode"];
            if (modeStr == "phong")
            {
                renderMode = RenderMode::PHONG;
            }
        }

        // Initialize the camera
        Camera camera(cameraPosition, lookAt, upVector, fov, width, height, exposure);

        // Create SceneData object with render mode
        SceneData sceneData(camera, width, height, renderMode);

        if (config.contains("nbounces"))
        {
            sceneData.nbounces = config["nbounces"];
        }

        if (config.contains("scene") && config["scene"].contains("backgroundcolor"))
        {
            sceneData.backgroundColor = Vector3(
                config["scene"]["backgroundcolor"][0],
                config["scene"]["backgroundcolor"][1],
                config["scene"]["backgroundcolor"][2]);
        }
        
        // Extract lights from JSON
        if (config.contains("scene") && config["scene"].contains("lightsources"))
        {
            for (const auto &light : config["scene"]["lightsources"])
            {
                Vector3 position = Vector3(light["position"][0], light["position"][1], light["position"][2]);
                Vector3 intensity = Vector3(light["intensity"][0], light["intensity"][1], light["intensity"][2]);
                sceneData.lights.emplace_back(position, intensity);
            }
        }

        // Extract shapes from JSON (similar as before)
        if (config.contains("scene") && config["scene"].contains("shapes"))
        {
            for (const auto &shape : config["scene"]["shapes"])
            {
                // Extract material properties if available
                Material material(Vector3(0.8, 0.8, 0.8), Vector3(1.0, 1.0, 1.0), 0.9, 0.1, 20.0, false, 1.0);
                if (shape.contains("material"))
                {
                    material.diffuseColor = Vector3(shape["material"]["diffusecolor"][0], shape["material"]["diffusecolor"][1], shape["material"]["diffusecolor"][2]);
                    material.specularColor = Vector3(shape["material"]["specularcolor"][0], shape["material"]["specularcolor"][1], shape["material"]["specularcolor"][2]);
                    material.kd = shape["material"]["kd"];
                    material.ks = shape["material"]["ks"];
                    material.specularExponent = shape["material"]["specularexponent"];
                    material.isReflective = shape["material"]["isreflective"];
                    material.reflectivity = shape["material"]["reflectivity"];
                }

                if (shape["type"] == "sphere")
                {
                    Vector3 center = Vector3(shape["center"][0], shape["center"][1], shape["center"][2]);
                    float radius = shape["radius"];
                    sceneData.spheres.emplace_back(center, radius, material);
                }
                else if (shape["type"] == "cylinder")
                {
                    Vector3 center = Vector3(shape["center"][0], shape["center"][1], shape["center"][2]);
                    Vector3 axis = Vector3(shape["axis"][0], shape["axis"][1], shape["axis"][2]).normalize();
                    float radius = shape["radius"];
                    float height = shape["height"];
                    sceneData.cylinders.emplace_back(center, axis, radius, height, material);
                }
                else if (shape["type"] == "triangle")
                {
                    Vector3 v0 = Vector3(shape["v0"][0], shape["v0"][1], shape["v0"][2]);
                    Vector3 v1 = Vector3(shape["v1"][0], shape["v1"][1], shape["v1"][2]);
                    Vector3 v2 = Vector3(shape["v2"][0], shape["v2"][1], shape["v2"][2]);
                    sceneData.triangles.emplace_back(v0, v1, v2, material);
                }
            }
        }

        return sceneData;
    }
    catch (const json::exception &e)
    {
        std::cerr << "Error parsing scene configuration: " << e.what() << std::endl;
        throw std::runtime_error("Failed to parse scene configuration.");
    }
}