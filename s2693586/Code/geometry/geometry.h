#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../bvh/aabb.h"          // Include AABB (Axis-Aligned Bounding Box) definition
#include "../camera/ray.h"        // Include Ray class for ray-object intersection
#include "../camera/vector3.h"    // Include Vector3 class for 3D vector operations
#include "../material/material.h" // Include Material class for material properties
#include "../camera/light.h"      // Include Light class for lighting information

// Struct to store intersection details between a ray and an object
struct Intersection
{
    bool hit;          // Whether the ray intersects the object
    float distance;    // Distance from the ray origin to the intersection point
    Vector3 point;     // The intersection point in world space
    Vector3 normal;    // The surface normal at the intersection point
    Material material; // Material properties of the intersected object

    // Default constructor to initialize default values
    Intersection() : hit(false), distance(0), point(Vector3()), normal(Vector3()), material() {}
};

// Abstract base class for all geometric objects
class Geometry
{
public:
    virtual ~Geometry() {}

    // Pure virtual methods that must be implemented by derived classes

    // Computes the intersection between the ray and the object
    virtual Intersection intersect(const Ray &ray) const = 0;

    // Returns the bounding box of the object for use in acceleration structures
    virtual AABB boundingBox() const = 0;

    // Returns the centroid (geometric center) of the object
    virtual Vector3 centroid() const = 0;
};

// Sphere class, representing a sphere object in the scene
class Sphere : public Geometry
{
public:
    Vector3 center;    // Center position of the sphere
    float radius;      // Radius of the sphere
    Material material; // Material properties of the sphere

    // Constructor to initialize a sphere
    Sphere(const Vector3 &center, float radius, const Material &material)
        : center(center), radius(radius), material(material) {}

    // Override the intersect method to compute ray-sphere intersection
    Intersection intersect(const Ray &ray) const override;

    // Override the boundingBox method to compute the sphere's AABB
    AABB boundingBox() const override;

    // Returns the sphere's center as its centroid
    Vector3 centroid() const override { return center; }
};

// Cylinder class, representing a cylindrical object in the scene
class Cylinder : public Geometry
{
public:
    Vector3 center;    // Base center position of the cylinder
    Vector3 axis;      // Axis vector (direction) of the cylinder
    float radius;      // Radius of the cylinder
    float height;      // Height of the cylinder
    Material material; // Material properties of the cylinder

    // Constructor to initialize a cylinder
    Cylinder(const Vector3 &center, const Vector3 &axis, float radius, float height, const Material &material)
        : center(center), axis(axis.normalize()), radius(radius), height(height), material(material) {}

    // Override the intersect method to compute ray-cylinder intersection
    Intersection intersect(const Ray &ray) const override;

    // Override the boundingBox method to compute the cylinder's AABB
    AABB boundingBox() const override;

    // Compute the centroid of the cylinder
    Vector3 centroid() const override;
};

// Triangle class, representing a triangular object in the scene
class Triangle : public Geometry
{
public:
    Vector3 v0, v1, v2; // Vertices of the triangle
    Material material;  // Material properties of the triangle

    // Constructor to initialize a triangle
    Triangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Material &material)
        : v0(v0), v1(v1), v2(v2), material(material) {}

    // Override the intersect method to compute ray-triangle intersection
    Intersection intersect(const Ray &ray) const override;

    // Override the boundingBox method to compute the triangle's AABB
    AABB boundingBox() const override;

    // Compute the centroid (average of vertices) of the triangle
    Vector3 centroid() const override { return (v0 + v1 + v2) / 3.0f; }
};

#endif // GEOMETRY_H
