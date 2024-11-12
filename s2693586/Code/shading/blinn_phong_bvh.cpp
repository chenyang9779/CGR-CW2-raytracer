#include "blinn_phong.h"
// #include "blinn_phong.cpp"
#include <cmath>
#include <algorithm>

Vector3 blinnPhongShadingBVH(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights, const BVHNode *root, int nbounces, const Vector3 &backgroundColor)
{
    if (nbounces <= 0)
    {
        return backgroundColor; // Return background color if max depth is reached
    }

    const Material &material = intersection.material;
    Vector3 normal = intersection.normal;
    Vector3 viewDir = (-ray.direction).normalize();
    Vector3 color(0.0f, 0.0f, 0.0f);
    const float epsilon = 0.001f;

    // Step 1: Direct Illumination using Blinn-Phong Model
    for (const auto &light : lights)
    {
        Vector3 lightDir = (light.position - intersection.point).normalize();
        float distanceToLight = (light.position - intersection.point).length();
        Vector3 shadowOrigin = intersection.point + normal * epsilon;
        Ray shadowRay(shadowOrigin, lightDir);

        // Use BVH to check if the point is in shadow
        Intersection shadowIntersection;
        bool inShadow = root->intersectShadowRay(shadowRay, distanceToLight);

        // Ambient component
        Vector3 ambient = material.kd * material.diffuseColor * light.intensity;

        if (inShadow)
        {
            color += ambient; // Add ambient light only if in shadow
            continue;
        }

        // Diffuse component
        float diff = std::max(normal.dot(lightDir), 0.0f);
        Vector3 diffuse = material.kd * diff * material.diffuseColor * light.intensity;

        // Specular component
        Vector3 halfDir = (viewDir + lightDir).normalize();
        float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularExponent);
        Vector3 specular = material.ks * spec * material.specularColor * light.intensity;

        color += ambient + diffuse + specular;
    }

    // Step 2: Reflection Component
    Vector3 reflectionColor(0.0f, 0.0f, 0.0f);

    if (material.isReflective)
    {
        Vector3 reflectionDir = (ray.direction - normal * 2.0f * ray.direction.dot(normal)).normalize();
        Vector3 reflectionOrigin = intersection.point + normal * epsilon;
        Ray reflectionRay(reflectionOrigin, reflectionDir);

        Intersection closestReflectionIntersection;
        closestReflectionIntersection.distance = std::numeric_limits<float>::max();
        if (root->intersect(reflectionRay, closestReflectionIntersection))
        {
            reflectionColor = blinnPhongShadingBVH(closestReflectionIntersection, reflectionRay, lights, root, nbounces - 1, backgroundColor);
        }
        else
        {
            reflectionColor = backgroundColor;
        }
        reflectionColor = reflectionColor * material.reflectivity;

        // Blend the base color with the reflection color
        float reflectivity = material.reflectivity;
        color = (1.0f - reflectivity) * color + reflectivity * reflectionColor;
    }

    // Step 3: Refraction Component with Fresnel Blending
    Vector3 refractionColor(0.0f, 0.0f, 0.0f);
    if (material.isRefractive)
    {
        Vector3 refractionDir;
        float fresnelReflectance = fresnelSchlick(std::abs(viewDir.dot(normal)), material.refractiveIndex);

        if (calculateRefraction(ray.direction, normal, material.refractiveIndex, refractionDir))
        {
            Vector3 refractionOrigin = refractionDir.dot(normal) < 0 ? intersection.point - normal * epsilon : intersection.point + normal * epsilon;
            Ray refractionRay(refractionOrigin, refractionDir.normalize());

            Intersection closestRefractionIntersection;
            closestRefractionIntersection.distance = std::numeric_limits<float>::max();
            if (root->intersect(refractionRay, closestRefractionIntersection))
            {
                refractionColor = blinnPhongShadingBVH(closestRefractionIntersection, refractionRay, lights, root, nbounces - 1, backgroundColor);
            }
            else
            {
                refractionColor = backgroundColor;
            }
            refractionColor = refractionColor * (1.0f - material.reflectivity);
            // Combine reflection, refraction, and shading color
            color = (1.0f - fresnelReflectance) * refractionColor + fresnelReflectance * reflectionColor + color;
        }

    }

    return color.clamp(0.0f, 1.0f);
}