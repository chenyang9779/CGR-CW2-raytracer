#include "geometry.h"
#include <cmath>

Intersection Sphere::intersect(const Ray &ray) const
{
    Intersection result;

    // Vector from ray origin to sphere center
    Vector3 oc = ray.origin - center;

    // Compute quadratic coefficients for the ray-sphere intersection equation
    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    // Check if the discriminant indicates an intersection
    if (discriminant < 0)
    {
        return result; // No intersection
    }
    else
    {
        // Compute the two solutions of the quadratic equation
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

        // Choose the nearest positive intersection point
        float t = (t1 > 0 && t2 > 0) ? std::min(t1, t2) : ((t1 > 0) ? t1 : t2);
        if (t > 0)
        {
            // Populate intersection result
            result.hit = true;
            result.distance = t;
            result.point = ray.origin + ray.direction * t;       // Compute intersection point
            result.normal = (result.point - center).normalize(); // Compute surface normal
            result.material = material;                          // Assign material properties
        }
    }
    return result;
}

AABB Sphere::boundingBox() const
{
    Vector3 radiusVec(radius, radius, radius);
    return AABB(center - radiusVec, center + radiusVec);
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
                result.material = material;
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
        result.material = material;
    }

    if (tCapTop > 0 && (tCapTop < result.distance || !result.hit))
    {
        result.hit = true;
        result.distance = tCapTop;
        result.point = ray.origin + ray.direction * tCapTop;
        result.normal = axisNormalized; // Normal pointing outwards for the top cap
        result.material = material;
    }

    return result;
}

AABB Cylinder::boundingBox() const
{
    Vector3 axisNormalized = axis.normalize();
    Vector3 halfAxis = axisNormalized * (height * 0.5f);

    // Two end points of the cylinder
    Vector3 baseCenter = center - halfAxis;
    Vector3 topCenter = center + halfAxis;

    // Generate a set of axis-aligned directions (x, y, z)
    std::vector<Vector3> directions = {
        Vector3(1, 0, 0), // x-axis
        Vector3(0, 1, 0), // y-axis
        Vector3(0, 0, 1)  // z-axis
    };

    // Initialize the min and max bounds for the AABB
    Vector3 minBound(std::numeric_limits<float>::max());
    Vector3 maxBound(std::numeric_limits<float>::lowest());

    // Check all possible extreme points of the cylinder
    std::vector<Vector3> extremePoints = {
        baseCenter + radius * directions[0], baseCenter - radius * directions[0],
        baseCenter + radius * directions[1], baseCenter - radius * directions[1],
        baseCenter + radius * directions[2], baseCenter - radius * directions[2],
        topCenter + radius * directions[0], topCenter - radius * directions[0],
        topCenter + radius * directions[1], topCenter - radius * directions[1],
        topCenter + radius * directions[2], topCenter - radius * directions[2]};

    // Evaluate bounding box by comparing all extreme points
    for (const auto &point : extremePoints)
    {
        minBound.x = std::min(minBound.x, point.x);
        minBound.y = std::min(minBound.y, point.y);
        minBound.z = std::min(minBound.z, point.z);

        maxBound.x = std::max(maxBound.x, point.x);
        maxBound.y = std::max(maxBound.y, point.y);
        maxBound.z = std::max(maxBound.z, point.z);
    }

    // Return the resulting axis-aligned bounding box
    return AABB(minBound, maxBound);
}

Vector3 Cylinder::centroid() const
{
    return center + (axis.normalize() * (height * 0.5f));
}

Intersection Triangle::intersect(const Ray &ray) const
{
    Intersection result;

    // Möller–Trumbore intersection algorithm
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 h = ray.direction.cross(edge2);
    float a = edge1.dot(h);

    if (a > -1e-6 && a < 1e-6)
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
    {
        result.hit = true;
        result.distance = t;
        result.point = ray.origin + ray.direction * t;

        // Ensure normal points in the opposite direction of the ray
        Vector3 computedNormal = edge1.cross(edge2).normalize();
        result.normal = (ray.direction.dot(computedNormal) < 0) ? computedNormal : -computedNormal;

        result.material = material;
    }

    return result;
}

// Triangle bounding box implementation
AABB Triangle::boundingBox() const
{
    Vector3 minBound(
        std::min({v0.x, v1.x, v2.x}),
        std::min({v0.y, v1.y, v2.y}),
        std::min({v0.z, v1.z, v2.z}));

    Vector3 maxBound(
        std::max({v0.x, v1.x, v2.x}),
        std::max({v0.y, v1.y, v2.y}),
        std::max({v0.z, v1.z, v2.z}));

    // Expand the bounds slightly to avoid degenerate AABBs
    const float epsilon = 1e-3f;
    if (minBound == maxBound)
    {
        minBound -= Vector3(epsilon, epsilon, epsilon);
        maxBound += Vector3(epsilon, epsilon, epsilon);
    }

    return AABB(minBound, maxBound);
}
