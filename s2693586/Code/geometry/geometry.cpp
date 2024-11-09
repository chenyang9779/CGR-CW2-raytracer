#include "geometry.h"
#include <cmath>

Intersection Sphere::intersect(const Ray& ray) const {
    Intersection result;
    
    // Vector from ray origin to sphere center
    Vector3 oc = ray.origin - center;
    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return result; // No intersection
    } else {
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

        float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
        if (t > 0) {
            result.hit = true;
            result.distance = t;
            result.point = ray.origin + ray.direction * t;
            result.normal = (result.point - center).normalize();
        }
    }
    return result;
}

Intersection Cylinder::intersect(const Ray& ray) const {
    Intersection result;

    // Implementing a basic cylinder intersection logic (finite cylinder)
    Vector3 oc = ray.origin - center;
    Vector3 d = ray.direction - axis * ray.direction.dot(axis);
    Vector3 o = oc - axis * oc.dot(axis);

    float a = d.dot(d);
    float b = 2.0f * o.dot(d);
    float c = o.dot(o) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    // Check for intersection with the infinite cylinder
    if (discriminant < 0) {
        return result; // No intersection
    }

    float sqrtDiscriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

    // Choose the nearest positive t value
    float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
    if (t < 0) {
        return result; // No valid intersection
    }

    // Check if the intersection is within the height bounds of the finite cylinder
    Vector3 point = ray.origin + ray.direction * t;
    float projectionLength = (point - center).dot(axis.normalize()); // Ensure axis is normalized

    // Add a small tolerance (epsilon) to account for floating-point precision errors
    // const float epsilon = 1e-4f;
    if (projectionLength < - height || projectionLength > height) {
        return result; // Intersection is outside the bounds of the finite cylinder
    }

    // If we have a valid intersection, fill the result
    result.hit = true;
    result.distance = t;
    result.point = point;

    // Calculate the normal at the intersection point
    Vector3 pointOnAxis = center + axis * projectionLength;
    result.normal = (result.point - pointOnAxis).normalize();

    return result;
}




Intersection Triangle::intersect(const Ray& ray) const {
    Intersection result;
    
    // Möller–Trumbore intersection algorithm
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 h = ray.direction.cross(edge2);
    float a = edge1.dot(h);

    if (std::fabs(a) < 1e-6) {
        return result; // Ray is parallel to the triangle
    }

    float f = 1.0 / a;
    Vector3 s = ray.origin - v0;
    float u = f * s.dot(h);
    if (u < 0.0 || u > 1.0) {
        return result; // Intersection is outside of the triangle
    }

    Vector3 q = s.cross(edge1);
    float v = f * ray.direction.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return result; // Intersection is outside of the triangle
    }

    float t = f * edge2.dot(q);
    if (t > 1e-6) { // Intersection point is along the ray direction
        result.hit = true;
        result.distance = t;
        result.point = ray.origin + ray.direction * t;
        result.normal = edge1.cross(edge2).normalize();
    }

    return result;
}

