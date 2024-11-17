#include "blinn_phong.h"
#include <cmath>
#include <algorithm>

// Function to perform Blinn-Phong shading using a BVH acceleration structure
// Parameters:
// - intersection: The intersection details (point, normal, material, etc.)
// - ray: The incoming ray that hit the object
// - lights: List of light sources in the scene
// - root: The root node of the BVH acceleration structure
// - nbounces: Remaining recursion depth for reflections/refractions
// - backgroundColor: The color to return if no further intersections occur
// Returns: The computed color for the intersection point
Vector3 blinnPhongShadingBVH(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights, const BVHNode *root, int nbounces, const Vector3 &backgroundColor)
{
    // Terminate recursion if the maximum depth is reached
    if (nbounces <= 0)
    {
        return backgroundColor; // Return background color if max depth is reached
    }
    // Material and geometric properties of the intersected object
    const Material &material = intersection.material;
    Vector3 normal = intersection.normal;           // Surface normal at the intersection point
    Vector3 viewDir = (-ray.direction).normalize(); // Direction toward the viewer
    Vector3 color(0.0f, 0.0f, 0.0f);                // Initialize the resulting color to black
    const float epsilon = 0.001f;                   // Offset to avoid self-intersections

    // Step 1: Direct Illumination using Blinn-Phong Model
    for (const auto &light : lights)
    {
        // Compute the direction and distance to the light source
        Vector3 lightDir = (light.position - intersection.point).normalize();
        float distanceToLight = (light.position - intersection.point).length();
        Vector3 shadowOrigin = intersection.point + normal * epsilon; // Offset to prevent self-shadowing
        Ray shadowRay(shadowOrigin, lightDir);

        // Use BVH to check if the point is in shadow
        Intersection shadowIntersection;
        bool inShadow = root->intersectShadowRay(shadowRay, distanceToLight);

        // Calculate attenuation based on distance to light
        float k1 = 0.1f;  // Linear attenuation coefficient
        float k2 = 0.01f; // Quadratic attenuation coefficient
        float attenuation = 1.0f / (1.0f + k1 * distanceToLight + k2 * distanceToLight * distanceToLight);
        Vector3 effectiveLightIntensity = light.intensity * attenuation;

        // Compute ambient lighting
        Vector3 ambient = material.kd * material.diffuseColor * effectiveLightIntensity;

        // If the point is in shadow, only add the ambient light
        if (inShadow)
        {
            color += ambient; // Add ambient light only if in shadow
            continue;
        }

        // Diffuse component
        float diff = std::max(normal.dot(lightDir), 0.0f); // Dot product for diffuse intensity
        Vector3 diffuse = material.kd * diff * material.diffuseColor * effectiveLightIntensity;

        // Specular component
        Vector3 halfDir = (viewDir + lightDir).normalize(); // Halfway vector
        float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularExponent);
        Vector3 specular = material.ks * spec * material.specularColor * effectiveLightIntensity;

        // Accumulate lighting contributions
        color += ambient + diffuse + specular;
    }

    // Step 2: Reflection Component
    Vector3 reflectionColor(0.0f, 0.0f, 0.0f);

    if (material.isReflective)
    {
        // Compute reflection direction using the surface normal
        Vector3 reflectionDir = (ray.direction - normal * 2.0f * ray.direction.dot(normal)).normalize();
        Vector3 reflectionOrigin = intersection.point + normal * epsilon;
        Ray reflectionRay(reflectionOrigin, reflectionDir);

        // Check for the closest intersection along the reflection ray
        Intersection closestReflectionIntersection;
        closestReflectionIntersection.distance = std::numeric_limits<float>::max();
        if (root->intersect(reflectionRay, closestReflectionIntersection))
        {
            // Recursively compute the reflection color
            reflectionColor = blinnPhongShadingBVH(closestReflectionIntersection, reflectionRay, lights, root, nbounces - 1, backgroundColor);
        }
        else
        {
            reflectionColor = backgroundColor;
        }
        // Scale reflection color by the material's reflectivity
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
        // Compute Fresnel reflectance for blending reflection and refraction
        float fresnelReflectance = fresnelSchlick(std::abs(viewDir.dot(normal)), material.refractiveIndex);

        // Calculate refraction direction using Snell's Law
        if (calculateRefraction(ray.direction, normal, material.refractiveIndex, refractionDir))
        {
            // Offset the origin to avoid self-intersections
            Vector3 refractionOrigin = refractionDir.dot(normal) < 0 ? intersection.point - normal * epsilon : intersection.point + normal * epsilon;
            Ray refractionRay(refractionOrigin, refractionDir.normalize());

            // Check for the closest intersection along the refraction ray
            Intersection closestRefractionIntersection;
            closestRefractionIntersection.distance = std::numeric_limits<float>::max();
            if (root->intersect(refractionRay, closestRefractionIntersection))
            {
                // Recursively compute the refraction color
                refractionColor = blinnPhongShadingBVH(closestRefractionIntersection, refractionRay, lights, root, nbounces - 1, backgroundColor);
            }
            else
            {
                refractionColor = backgroundColor;
            }
            // Scale refraction color by (1 - reflectivity)
            refractionColor = refractionColor * (1.0f - material.reflectivity);
            // Combine reflection, refraction, and shading color
            color = (1.0f - fresnelReflectance) * refractionColor + fresnelReflectance * reflectionColor + color;
        }
    }
    // Clamp the resulting color to the range [0, 1] to ensure valid output
    return color.clamp(0.0f, 1.0f);
}