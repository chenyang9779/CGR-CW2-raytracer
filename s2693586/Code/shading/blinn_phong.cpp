#include "blinn_phong.h"

Vector3 blinnPhongShading(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights, 
                          const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles)
{
    const Material &material = intersection.material;
    Vector3 normal = intersection.normal;
    Vector3 viewDir = (-ray.direction).normalize();
    Vector3 color(0.0f, 0.0f, 0.0f);

    for (const auto &light : lights)
    {
        // Calculate the light direction and distance to the light
        Vector3 lightDir = (light.position - intersection.point).normalize();
        float maxDistance = (light.position - intersection.point).length();

        // Create a shadow ray from the intersection point to the light
        Vector3 shadowOrigin = intersection.point + normal * 0.001f; // Offset to avoid self-intersection
        Ray shadowRay(shadowOrigin, lightDir);

        // Check if the shadow ray intersects any object in the scene
        bool inShadow = false;
        for (const auto &sphere : spheres)
        {
            Intersection shadowIntersection = sphere.intersect(shadowRay);
            if (shadowIntersection.hit && shadowIntersection.distance < maxDistance)
            {
                inShadow = true;
                break;
            }
        }

        for (const auto &cylinder : cylinders)
        {
            Intersection shadowIntersection = cylinder.intersect(shadowRay);
            if (shadowIntersection.hit && shadowIntersection.distance < maxDistance)
            {
                inShadow = true;
                break;
            }
        }

        for (const auto &triangle : triangles)
        {
            Intersection shadowIntersection = triangle.intersect(shadowRay);
            if (shadowIntersection.hit && shadowIntersection.distance < maxDistance)
            {
                inShadow = true;
                break;
            }
        }

        // If the point is in shadow, only add the ambient component
        Vector3 ambient = material.kd * material.diffuseColor * light.color * light.intensity;
        if (inShadow)
        {
            color += ambient;
            continue;
        }

        // Add ambient, diffuse, and specular components
        float diff = std::max(normal.dot(lightDir), 0.0f);
        Vector3 diffuse = material.kd * diff * material.diffuseColor * light.color * light.intensity;

        Vector3 halfDir = (viewDir + lightDir).normalize();
        float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularExponent);
        Vector3 specular = material.ks * spec * material.specularColor * light.color * light.intensity;

        color += ambient + diffuse + specular;
    }

    return color.clamp(0.0f, 1.0f); // Clamp the color between 0 and 1
}

