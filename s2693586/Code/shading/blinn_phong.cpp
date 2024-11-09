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
        // Calculate the direction towards the light
        Vector3 lightDir = (light.position - intersection.point).normalize();
        float distanceToLight = (light.position - intersection.point).length();

        // Create a shadow ray from the intersection point to the light
        Vector3 shadowOrigin = intersection.point + normal * 0.001f; // Offset to avoid self-intersection
        Ray shadowRay(shadowOrigin, lightDir);

        // Check if the shadow ray intersects any object in the scene
        bool inShadow = false;
        for (const auto &sphere : spheres)
        {
            Intersection shadowIntersection = sphere.intersect(shadowRay);
            if (shadowIntersection.hit && shadowIntersection.distance < distanceToLight)
            {
                inShadow = true;
                break;
            }
        }

        for (const auto &cylinder : cylinders)
        {
            Intersection shadowIntersection = cylinder.intersect(shadowRay);
            if (shadowIntersection.hit && shadowIntersection.distance < distanceToLight)
            {
                inShadow = true;
                break;
            }
        }

        for (const auto &triangle : triangles)
        {
            Intersection shadowIntersection = triangle.intersect(shadowRay);
            if (shadowIntersection.hit && shadowIntersection.distance < distanceToLight)
            {
                inShadow = true;
                break;
            }
        }

        // Ambient component: always present, regardless of shadows
        Vector3 ambient = material.kd * material.diffuseColor * light.intensity;

        // If the point is in shadow, add only the ambient component
        if (inShadow)
        {
            color += ambient;
            continue; // Skip the diffuse and specular components
        }

        // Diffuse component (Lambertian reflection)
        float diff = std::max(normal.dot(lightDir), 0.0f);
        Vector3 diffuse = material.kd * diff * material.diffuseColor * light.intensity;

        // Specular component (Blinn-Phong reflection)
        Vector3 halfDir = (viewDir + lightDir).normalize();
        float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularExponent);
        Vector3 specular = material.ks * spec * material.specularColor * light.intensity;

        // Accumulate the color components
        color += ambient + diffuse + specular;
    }

    return color.clamp(0.0f, 1.0f); // Clamp the color between 0 and 1
}



