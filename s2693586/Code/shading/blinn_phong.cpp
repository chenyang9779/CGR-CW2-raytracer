#include "blinn_phong.h"
#include <cmath>

// // Function to calculate refraction direction
// Vector3 calculateRefraction(const Vector3& incident, const Vector3& normal, float eta)
// {
//     float cosi = std::clamp(incident.dot(normal), -1.0f, 1.0f);
//     float etai = 1, etat = eta;
//     Vector3 n = normal;

//     if (cosi < 0) {
//         cosi = -cosi; // The ray is outside the material
//     } else {
//         std::swap(etai, etat); // The ray is inside the material
//         n = -normal;
//     }

//     float etaRatio = etai / etat;
//     float k = 1 - etaRatio * etaRatio * (1 - cosi * cosi);
//     if (k < 0) {
//         return Vector3(0, 0, 0); // Total internal reflection
//     }
//     return incident * etaRatio + n * (etaRatio * cosi - sqrtf(k));
// }

Vector3 blinnPhongShading(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights,
                          const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles,
                          int nbounces, Vector3 backgroundColor)
{
    if (nbounces <= 0)
    {
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

        // Normalize the vectors for correct Blinn-Phong calculations
        Vector3 viewDirNormalized = viewDir.normalize();
        Vector3 lightDirNormalized = lightDir.normalize();

        // Calculate the diffuse component
        float diff = std::max(normal.dot(lightDirNormalized), 0.0f);
        Vector3 diffuse = material.kd * diff * material.diffuseColor * light.intensity;

        // Calculate the halfway vector for Blinn-Phong specular highlights
        Vector3 halfDir = (viewDirNormalized + lightDirNormalized).normalize();
        float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularExponent);
        Vector3 specular = material.ks * spec * material.specularColor * light.intensity;

        // Accumulate the lighting components
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

    // Refraction component
    // Vector3 refractionColor(0.0f, 0.0f, 0.0f);
    // if (material.isRefractive && nbounces > 0)
    // {
    //     float eta = material.refractiveIndex;
    //     Vector3 refractionDir = calculateRefraction(ray.direction, normal, eta);
    //     if (refractionDir != Vector3(0.0f, 0.0f, 0.0f)) // Only proceed if not total internal reflection
    //     {
    //         Vector3 refractionOrigin = intersection.point - normal * 0.001f; // Offset to avoid self-intersection
    //         Ray refractionRay(refractionOrigin, refractionDir.normalize());

    //         Intersection closestRefractionIntersection;
    //         closestRefractionIntersection.distance = std::numeric_limits<float>::max();

    //         for (const auto& sphere : spheres)
    //         {
    //             Intersection refractionIntersection = sphere.intersect(refractionRay);
    //             if (refractionIntersection.hit && refractionIntersection.distance < closestRefractionIntersection.distance)
    //             {
    //                 closestRefractionIntersection = refractionIntersection;
    //             }
    //         }

    //         for (const auto& cylinder : cylinders)
    //         {
    //             Intersection refractionIntersection = cylinder.intersect(refractionRay);
    //             if (refractionIntersection.hit && refractionIntersection.distance < closestRefractionIntersection.distance)
    //             {
    //                 closestRefractionIntersection = refractionIntersection;
    //             }
    //         }

    //         for (const auto& triangle : triangles)
    //         {
    //             Intersection refractionIntersection = triangle.intersect(refractionRay);
    //             if (refractionIntersection.hit && refractionIntersection.distance < closestRefractionIntersection.distance)
    //             {
    //                 closestRefractionIntersection = refractionIntersection;
    //             }
    //         }

    //         if (closestRefractionIntersection.hit)
    //         {
    //             refractionColor = blinnPhongShading(closestRefractionIntersection, refractionRay, lights, spheres, cylinders, triangles, nbounces - 1, backgroundColor);
    //         }

    //         refractionColor = refractionColor * (1.0f - fresnelEffect);
    //         color = refractionColor;
    //     }
    // }

    return color.clamp(0.0f, 1.0f);
}
