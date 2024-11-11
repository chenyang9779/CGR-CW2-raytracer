#include "blinn_phong.h"
#include <cmath>
#include <algorithm>

// Function to calculate Fresnel reflection using Schlick's approximation
float fresnelSchlick(float cosTheta, float refractiveIndex)
{
    float r0 = (1.0f - refractiveIndex) / (1.0f + refractiveIndex);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * std::pow(1.0f - cosTheta, 5.0f);
}

// Function to calculate the refracted direction using Snell's Law, and indicate if refraction was possible
bool calculateRefraction(const Vector3 &incident, const Vector3 &normal, float eta, Vector3 &refractionDir)
{
    float cosi = std::clamp(incident.dot(normal), -1.0f, 1.0f);
    float etai = 1, etat = eta;
    Vector3 n = normal;

    if (cosi < 0)
    {
        cosi = -cosi;
    }
    else
    {
        std::swap(etai, etat);
        n = -normal;
    }

    float etaRatio = etai / etat;
    float k = 1 - etaRatio * etaRatio * (1 - cosi * cosi);

    if (k < 0)
    {
        return false; // Total internal reflection occurred
    }
    else
    {
        refractionDir = incident * etaRatio + n * (etaRatio * cosi - sqrtf(k));
        return true;
    }
}

Intersection findClosestIntersection(const Ray &ray, const std::vector<Sphere> &spheres,
                                     const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles)
{
    Intersection closestIntersection;
    closestIntersection.distance = std::numeric_limits<float>::max();

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

    return closestIntersection;
}

Vector3 blinnPhongShading(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights,
                          const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles,
                          int nbounces, Vector3 backgroundColor)
{
    if (nbounces <= 0)
    {
        return backgroundColor; // Return black if max depth is reached
    }

    const Material &material = intersection.material;
    Vector3 normal = intersection.normal;
    Vector3 viewDir = (-ray.direction).normalize();
    Vector3 color(0.0f, 0.0f, 0.0f);
    const float epsilon = 0.001f;

    // Blinn-Phong shading for direct illumination
    for (const auto &light : lights)
    {
        Vector3 lightDir = (light.position - intersection.point).normalize();
        float distanceToLight = (light.position - intersection.point).length();
        Vector3 shadowOrigin = intersection.point + normal * epsilon; // Offset to avoid self-intersection
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
        Vector3 reflectionOrigin = intersection.point + normal * epsilon; // Offset to avoid self-intersection
        Ray reflectionRay(reflectionOrigin, reflectionDir);

        Intersection closestReflectionIntersection = findClosestIntersection(reflectionRay, spheres, cylinders, triangles);

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

    // Refraction component with new logic
    Vector3 refractionColor(0.0f, 0.0f, 0.0f);
    if (material.isRefractive && nbounces > 0)
    {
        Vector3 refractionDir;
        float fresnelReflectance = fresnelSchlick(std::abs(viewDir.dot(normal)), material.refractiveIndex);
        if (calculateRefraction(ray.direction, normal, material.refractiveIndex, refractionDir))
        {
            Vector3 refractionOrigin = refractionDir.dot(normal) < 0 ? intersection.point - normal * epsilon : intersection.point + normal * epsilon; // Offset slightly to avoid self-intersection
            Ray refractionRay(refractionOrigin, refractionDir.normalize());

            Intersection closestRefractionIntersection = findClosestIntersection(refractionRay, spheres, cylinders, triangles);

            if (closestRefractionIntersection.hit)
            {
                refractionColor = blinnPhongShading(closestRefractionIntersection, refractionRay, lights, spheres, cylinders, triangles, nbounces - 1, backgroundColor);
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

        // Blend the reflection and refraction based on the Fresnel effect
        color += (1.0f - fresnelReflectance) * refractionColor + fresnelReflectance * reflectionColor;
    }

    return color.clamp(0.0f, 1.0f);
}
