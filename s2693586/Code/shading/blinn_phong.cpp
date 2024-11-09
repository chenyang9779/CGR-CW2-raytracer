#include "blinn_phong.h"

Vector3 blinnPhongShading(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights)
{
    // Extract material and intersection information
    const Material &material = intersection.material;
    Vector3 normal = intersection.normal;
    Vector3 viewDir = (-ray.direction).normalize();
    Vector3 color(0.0f, 0.0f, 0.0f);

    // Iterate over each light in the scene
    for (const auto &light : lights)
    {
        // Calculate light direction and normalize it
        Vector3 lightDir = (light.position - intersection.point).normalize();

        // Calculate Ambient component
        Vector3 ambient = material.kd * material.diffuseColor * light.color * light.intensity;

        // Calculate Diffuse component (Lambertian reflection)
        float diff = std::max(normal.dot(lightDir), 0.0f);
        Vector3 diffuse = material.kd * diff * material.diffuseColor * light.color * light.intensity;

        // Calculate Specular component (Blinn-Phong reflection model)
        Vector3 halfDir = (viewDir + lightDir).normalize();
        float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularExponent);
        Vector3 specular = material.ks * spec * material.specularColor * light.color * light.intensity;

        // Accumulate the color components
        color += ambient + diffuse + specular;
    }

    // Clamp the color values to be between 0 and 1
    return color.clamp(0.0f, 1.0f);
}
