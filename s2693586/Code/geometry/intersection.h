#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>          // Include the vector library to handle lists of objects
#include "../camera/ray.h" // Include the Ray class for ray definitions
#include "geometry.h"      // Include geometric object definitions (spheres, cylinders, triangles)

// Function to find the closest intersection of a ray with the scene's geometric objects
// Parameters:
// - ray: The ray being cast into the scene
// - spheres: A list of spheres in the scene
// - cylinders: A list of cylinders in the scene
// - triangles: A list of triangles in the scene
// Returns:
// - An Intersection object containing details of the closest intersection (if any) with the ray
//   - If no intersection occurs, the Intersection object will indicate that the ray missed all objects.
Intersection findClosestIntersection(const Ray &ray, const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles);

#endif // INTERSECTION_H
