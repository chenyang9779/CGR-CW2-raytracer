#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../camera/ray.h"
#include "../camera/vector3.h"
#include "../camera/material.h" // Include material and light definitions
#include "../camera/light.h"

struct Intersection
{
    bool hit;
    float distance;
    Vector3 point;
    Vector3 normal;
    Material material;  // Add material to store material properties of the intersected shape

    Intersection() : hit(false), distance(0), point(Vector3()), normal(Vector3()), material() {}
};

class Sphere
{
public:
    Vector3 center;
    float radius;
    Material material;

    Sphere(const Vector3 &center, float radius, const Material &material)
        : center(center), radius(radius), material(material) {}

    Intersection intersect(const Ray &ray) const;
};

class Cylinder
{
public:
    Vector3 center;
    Vector3 axis;
    float radius;
    float height;
    Material material;

    Cylinder(const Vector3 &center, const Vector3 &axis, float radius, float height, const Material &material)
        : center(center), axis(axis.normalize()), radius(radius), height(height), material(material) {}

    Intersection intersect(const Ray &ray) const;
};

class Triangle
{
public:
    Vector3 v0, v1, v2;
    Material material;

    Triangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Material &material)
        : v0(v0), v1(v1), v2(v2), material(material) {}

    Intersection intersect(const Ray &ray) const;
};

#endif // GEOMETRY_H
