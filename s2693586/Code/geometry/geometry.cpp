#include "geometry.h"
#include <cmath>

Intersection Sphere::intersect(const Ray &ray) const
{
    Intersection result;

    // Vector from ray origin to sphere center
    Vector3 oc = ray.origin - center;
    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
    {
        return result; // No intersection
    }
    else
    {
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

        float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
        if (t > 0)
        {
            result.hit = true;
            result.distance = t;
            result.point = ray.origin + ray.direction * t;
            result.normal = (result.point - center).normalize();
            result.material = material;
        }
    }
    return result;
}

Intersection Cylinder::intersect(const Ray &ray) const
{
    Intersection result;

    // Implementing a basic cylinder intersection logic (finite cylinder)
    Vector3 axisNormalized = axis.normalize();
    Vector3 oc = ray.origin - center;

    // Decompose the ray direction and origin components relative to the cylinder axis
    Vector3 d = ray.direction - axisNormalized * ray.direction.dot(axisNormalized);
    Vector3 o = oc - axisNormalized * oc.dot(axisNormalized);

    float a = d.dot(d);
    float b = 2.0f * o.dot(d);
    float c = o.dot(o) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    // Check for intersection with the infinite cylinder
    float tCylinder = -1;
    if (discriminant >= 0)
    {
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

        // Choose the nearest positive t value
        tCylinder = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
        if (tCylinder > 0)
        {
            // Check if the intersection is within the height bounds of the finite cylinder
            Vector3 point = ray.origin + ray.direction * tCylinder;
            float projectionLength = (point - center).dot(axisNormalized);

            if (projectionLength >= -height && projectionLength <= height)
            {
                // If we have a valid intersection, fill the result
                result.hit = true;
                result.distance = tCylinder;
                result.point = point;

                // Calculate the normal at the intersection point
                Vector3 pointOnAxis = center + axisNormalized * projectionLength;
                result.normal = (result.point - pointOnAxis).normalize();
            }
        }
    }

    // Adding checks for intersections with the top and bottom caps of the cylinder
    float tCapTop = -1;
    float tCapBottom = -1;

    Vector3 bottomCenter = center - axisNormalized * (height);
    Vector3 topCenter = center + axisNormalized * (height);

    // Ray-plane intersection for the bottom cap
    float denomBottom = ray.direction.dot(axisNormalized);
    if (std::abs(denomBottom) > 1e-6) // Avoid division by zero for rays parallel to the plane
    {
        float t = (bottomCenter - ray.origin).dot(axisNormalized) / denomBottom;
        if (t > 0)
        {
            Vector3 point = ray.origin + ray.direction * t;
            if ((point - bottomCenter).length() <= radius)
            {
                tCapBottom = t;
            }
        }
    }

    // Ray-plane intersection for the top cap
    float denomTop = ray.direction.dot(axisNormalized);
    if (std::abs(denomTop) > 1e-6) // Avoid division by zero for rays parallel to the plane
    {
        float t = (topCenter - ray.origin).dot(axisNormalized) / denomTop;
        if (t > 0)
        {
            Vector3 point = ray.origin + ray.direction * t;
            if ((point - topCenter).length() <= radius)
            {
                tCapTop = t;
            }
        }
    }

    // Determine if any cap intersections are closer than the cylinder side intersection
    if (tCapBottom > 0 && (tCapBottom < result.distance || !result.hit))
    {
        result.hit = true;
        result.distance = tCapBottom;
        result.point = ray.origin + ray.direction * tCapBottom;
        result.normal = -axisNormalized; // Normal pointing outwards for the bottom cap
    }

    if (tCapTop > 0 && (tCapTop < result.distance || !result.hit))
    {
        result.hit = true;
        result.distance = tCapTop;
        result.point = ray.origin + ray.direction * tCapTop;
        result.normal = axisNormalized; // Normal pointing outwards for the top cap
    }

    result.material = material;
    return result;
}


Intersection Triangle::intersect(const Ray &ray) const
{
    Intersection result;

    // Möller–Trumbore intersection algorithm
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 h = ray.direction.cross(edge2);
    float a = edge1.dot(h);

    if (std::fabs(a) < 1e-6)
    {
        return result; // Ray is parallel to the triangle
    }

    float f = 1.0 / a;
    Vector3 s = ray.origin - v0;
    float u = f * s.dot(h);
    if (u < 0.0 || u > 1.0)
    {
        return result; // Intersection is outside of the triangle
    }

    Vector3 q = s.cross(edge1);
    float v = f * ray.direction.dot(q);
    if (v < 0.0 || u + v > 1.0)
    {
        return result; // Intersection is outside of the triangle
    }

    float t = f * edge2.dot(q);
    if (t > 1e-6)
    { // Intersection point is along the ray direction
        result.hit = true;
        result.distance = t;
        result.point = ray.origin + ray.direction * t;
        result.normal = edge1.cross(edge2).normalize();
    }

    result.material = material;
    return result;
}
