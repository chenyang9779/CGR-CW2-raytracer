#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../bvh/aabb.h"
#include "../camera/ray.h"
#include "../camera/vector3.h"
#include "../material/material.h"
#include "../camera/light.h"

// Intersection struct to store hit details
struct Intersection
{
    bool hit;
    float distance;
    Vector3 point;
    Vector3 normal;
    Material material;

    Intersection() : hit(false), distance(0), point(Vector3()), normal(Vector3()), material() {}
};

// Abstract Geometry base class
class Geometry {
public:
    virtual ~Geometry() {}

    // Pure virtual methods for common functionalities
    virtual Intersection intersect(const Ray &ray) const = 0;
    virtual AABB boundingBox() const = 0;
    virtual Vector3 centroid() const = 0;
};

// Sphere class inheriting from Geometry
class Sphere : public Geometry
{
public:
    Vector3 center;
    float radius;
    Material material;

    Sphere(const Vector3 &center, float radius, const Material &material)
        : center(center), radius(radius), material(material) {}

    Intersection intersect(const Ray &ray) const override;
    AABB boundingBox() const override;
    Vector3 centroid() const override { return center; }
};

// Cylinder class inheriting from Geometry
class Cylinder : public Geometry
{
public:
    Vector3 center;
    Vector3 axis;
    float radius;
    float height;
    Material material;

    Cylinder(const Vector3 &center, const Vector3 &axis, float radius, float height, const Material &material)
        : center(center), axis(axis.normalize()), radius(radius), height(height), material(material) {}

    Intersection intersect(const Ray &ray) const override;
    AABB boundingBox() const override;
    Vector3 centroid() const override { return center; } // Update with actual centroid calculation
};

// Triangle class inheriting from Geometry
class Triangle : public Geometry
{
public:
    Vector3 v0, v1, v2;
    Material material;

    Triangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Material &material)
        : v0(v0), v1(v1), v2(v2), material(material) {}

    Intersection intersect(const Ray &ray) const override;
    AABB boundingBox() const override;
    Vector3 centroid() const override { return (v0 + v1 + v2) / 3.0f; }
};

#endif // GEOMETRY_H
