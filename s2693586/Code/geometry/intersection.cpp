#include "intersection.h"


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