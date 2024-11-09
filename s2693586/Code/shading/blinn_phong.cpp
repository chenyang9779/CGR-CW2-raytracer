#include "blinn_phong.h"

Vector3 blinnPhongShading(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights, 
                          const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles,
                          int nbounces, Vector3 backgroundColor)
{
    if (nbounces <= 0) {
        return Vector3(0.0f, 0.0f, 0.0f); // Return black if max depth is reached
    }

    const Material &material = intersection.material;
    Vector3 normal = intersection.normal;
    Vector3 viewDir = (-ray.direction).normalize();
    Vector3 color(0.0f, 0.0f, 0.0f);

    // Blinn-Phong shading for direct illumination
    for (const auto &light : lights)
    {
        Vector3 lightDir = (light.position - intersection.point).normalize();
        float distanceToLight = (light.position - intersection.point).length();
        Vector3 shadowOrigin = intersection.point + normal * 0.001f; // Offset to avoid self-intersection
        Ray shadowRay(shadowOrigin, lightDir);

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

        Vector3 ambient = material.kd * material.diffuseColor * light.intensity;

        if (inShadow)
        {
            color += ambient;
            continue;
        }

        float diff = std::max(normal.dot(lightDir), 0.0f);
        Vector3 diffuse = material.kd * diff * material.diffuseColor * light.intensity;

        Vector3 halfDir = (viewDir + lightDir).normalize();
        float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularExponent);
        
        Vector3 specular = material.ks * spec * material.specularColor * light.intensity;

        color += ambient + diffuse + specular;
    }

    // Reflection component
    Vector3 reflectionColor(0.0f, 0.0f, 0.0f);
    if (material.isReflective)
    {
        Vector3 reflectionDir = (ray.direction - normal * 2.0f * ray.direction.dot(normal)).normalize();
        Vector3 reflectionOrigin = intersection.point + normal * 0.001f; // Offset to avoid self-intersection
        Ray reflectionRay(reflectionOrigin, reflectionDir);

        Intersection closestReflectionIntersection;
        closestReflectionIntersection.distance = std::numeric_limits<float>::max();

        for (const auto &sphere : spheres)
        {
            Intersection reflectionIntersection = sphere.intersect(reflectionRay);
            if (reflectionIntersection.hit && reflectionIntersection.distance < closestReflectionIntersection.distance)
            {
                closestReflectionIntersection = reflectionIntersection;
            }
        }

        for (const auto &cylinder : cylinders)
        {
            Intersection reflectionIntersection = cylinder.intersect(reflectionRay);
            if (reflectionIntersection.hit && reflectionIntersection.distance < closestReflectionIntersection.distance)
            {
                closestReflectionIntersection = reflectionIntersection;
            }
        }

        for (const auto &triangle : triangles)
        {
            Intersection reflectionIntersection = triangle.intersect(reflectionRay);
            if (reflectionIntersection.hit && reflectionIntersection.distance < closestReflectionIntersection.distance)
            {
                closestReflectionIntersection = reflectionIntersection;
            }
        }

        if (closestReflectionIntersection.hit)
        {
            reflectionColor = blinnPhongShading(closestReflectionIntersection, reflectionRay, lights, spheres, cylinders, triangles, nbounces - 1, backgroundColor);
        }
        else
        {
            reflectionColor = backgroundColor; // Default background color to simulate environment
        }

        // Tint reflection color by material's diffuse color
        // reflectionColor = reflectionColor * material.diffuseColor;

        // Scale reflection color by reflectivity
        reflectionColor = reflectionColor * material.reflectivity;

        // Blend the base color with the reflection color
        float reflectivity = material.reflectivity;
        color = (1.0f - reflectivity) * color + reflectivity * reflectionColor;
    }



    return color.clamp(0.0f, 1.0f); // Clamp the color between 0 and 1
}





