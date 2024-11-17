#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

// Include necessary libraries and headers
#include <vector>                       // For using std::vector to store lists of objects
#include "../geometry/geometry.h"       // Provides geometric object definitions and the Intersection struct
#include "../camera/ray.h"              // Defines the Ray structure
#include "../camera/light.h"            // Defines the Light structure
#include "../material/material.h"       // Material properties such as diffuse, specular, and reflectivity
#include "../bvh/bvh_node.h"            // BVHNode definitions for acceleration structures
#include "../geometry/intersection.cpp" // For calculating intersections between rays and objects

// Calculates the Fresnel effect using Schlick's approximation
// Parameters:
// - cosTheta: The cosine of the angle between the ray and the normal
// - refractiveIndex: The index of refraction of the material
// Returns: The reflectance value based on the Fresnel effect
float fresnelSchlick(float cosTheta, float refractiveIndex);

// Computes the refraction direction for a ray passing through a surface
// Parameters:
// - incident: The incoming ray direction
// - normal: The surface normal vector
// - eta: The ratio of refractive indices (n1/n2)
// - refractionDir: Output vector for the refracted direction
// Returns: True if refraction is successful, false otherwise
bool calculateRefraction(const Vector3 &incident, const Vector3 &normal, float eta, Vector3 &refractionDir);

// Finds the closest intersection of a ray with geometric objects in the scene
// Parameters:
// - ray: The input ray
// - spheres, cylinders, triangles: Lists of geometric objects in the scene
// Returns: The closest intersection point or a default intersection if no hit occurs
Intersection findClosestIntersection(const Ray &ray, const std::vector<Sphere> &spheres,
                                     const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles);

// Implements the Blinn-Phong shading model without BVH acceleration
// Parameters:
// - intersection: The intersection point details
// - ray: The incoming ray
// - lights: List of lights in the scene
// - spheres, cylinders, triangles: Lists of geometric objects
// - nbounces: Number of allowed recursive bounces for reflection/refraction
// - backgroundColor: The color of the background for unhit rays
// Returns: The computed color for the given intersection point
Vector3 blinnPhongShading(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights,
                          const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders,
                          const std::vector<Triangle> &triangles, int nbounces, Vector3 backgroundColor);

// Implements the Blinn-Phong shading model using BVH acceleration
// Parameters:
// - intersection: The intersection point details
// - ray: The incoming ray
// - lights: List of lights in the scene
// - root: Pointer to the root of the BVH tree
// - nbounces: Number of allowed recursive bounces for reflection/refraction
// - backgroundColor: The color of the background for unhit rays
// Returns: The computed color for the given intersection point
Vector3 blinnPhongShadingBVH(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights,
                             const BVHNode *root, int nbounces, const Vector3 &backgroundColor);

#endif // BLINN_PHONG_H
