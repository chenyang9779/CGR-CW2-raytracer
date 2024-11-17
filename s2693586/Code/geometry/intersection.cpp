#include "intersection.h"

// Function to find the closest intersection of a ray with geometric objects in the scene
// Parameters:
// - ray: The ray being cast into the scene
// - spheres: A list of spheres in the scene
// - cylinders: A list of cylinders in the scene
// - triangles: A list of triangles in the scene
// Returns:
// - The closest Intersection object containing details of the intersection (if any)

Intersection findClosestIntersection(const Ray &ray, const std::vector<Sphere> &spheres,
                                     const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles)
{
    // Initialize the closest intersection with no hit and maximum distance
    Intersection closestIntersection;
    closestIntersection.distance = std::numeric_limits<float>::max();

    // Check intersection with each sphere in the scene
    for (const auto &sphere : spheres)
    {
        // Test for intersection between the ray and the sphere
        Intersection intersection = sphere.intersect(ray);

        // Update the closest intersection if this sphere is closer
        if (intersection.hit && intersection.distance < closestIntersection.distance)
        {
            closestIntersection = intersection;
        }
    }

    // Check intersection with each cylinder in the scene
    for (const auto &cylinder : cylinders)
    {
        // Test for intersection between the ray and the cylinder
        Intersection intersection = cylinder.intersect(ray);

        // Update the closest intersection if this cylinder is closer
        if (intersection.hit && intersection.distance < closestIntersection.distance)
        {
            closestIntersection = intersection;
        }
    }

    // Check intersection with each triangle in the scene
    for (const auto &triangle : triangles)
    {
        // Test for intersection between the ray and the triangle
        Intersection intersection = triangle.intersect(ray);

        // Update the closest intersection if this triangle is closer
        if (intersection.hit && intersection.distance < closestIntersection.distance)
        {
            closestIntersection = intersection;
        }
    }

    // Return the closest intersection found
    return closestIntersection;
}
