#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

#include <vector>                 // Include vector header to use std::vector
#include "../geometry/geometry.h" // Include geometry.h to use Intersection struct
#include "../camera/ray.h"        // Include ray definition
#include "../camera/light.h"      // Include light definitions
#include "../material/material.h" // Include material definitions
#include "../bvh/bvh_node.h"      // Include BVHNode definitions
#include "../geometry/intersection.cpp"

float fresnelSchlick(float cosTheta, float refractiveIndex);
bool calculateRefraction(const Vector3 &incident, const Vector3 &normal, float eta, Vector3 &refractionDir);
Intersection findClosestIntersection(const Ray &ray, const std::vector<Sphere> &spheres,
                                     const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles);

// Function for Blinn-Phong shading without BVH
Vector3 blinnPhongShading(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights,
                          const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders,
                          const std::vector<Triangle> &triangles, int nbounces, Vector3 backgroundColor);

// Function for Blinn-Phong shading with BVH
Vector3 blinnPhongShadingBVH(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights,
                             const BVHNode *root, int nbounces, const Vector3 &backgroundColor);
#endif // BLINN_PHONG_H
