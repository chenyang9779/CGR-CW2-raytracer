// intersection.h
#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include "../camera/ray.h"
#include "geometry.h"
Intersection findClosestIntersection(const Ray &ray, const std::vector<Sphere> &spheres, const std::vector<Cylinder> &cylinders, const std::vector<Triangle> &triangles);

#endif // INTERSECTION_H
