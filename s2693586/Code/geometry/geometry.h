#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../camera/ray.h"
#include "../camera/vector3.h"

struct Intersection {
    bool hit;
    float distance;
    Vector3 point;
    Vector3 normal;

    Intersection() : hit(false), distance(0), point(Vector3()), normal(Vector3()) {}
};

class Sphere {
public:
    Vector3 center;
    float radius;

    Sphere(const Vector3& center, float radius) : center(center), radius(radius) {}

    Intersection intersect(const Ray& ray) const;
};

class Cylinder {
public:
    Vector3 center;
    Vector3 axis;
    float radius;
    float height;

    Cylinder(const Vector3& center, const Vector3& axis, float radius, float height) : center(center), axis(axis.normalize()), radius(radius), height(height) {}

    Intersection intersect(const Ray& ray) const;
};

class Triangle {
public:
    Vector3 v0, v1, v2;

    Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) : v0(v0), v1(v1), v2(v2) {}

    Intersection intersect(const Ray& ray) const;
};

#endif // GEOMETRY_H
