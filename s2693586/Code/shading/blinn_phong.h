#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

#include <vector>                   // Include vector header to use std::vector
#include "../geometry/geometry.h"   // Include geometry.h to use Intersection struct
#include "../camera/ray.h"          // Include ray definition
#include "../camera/light.h"        // Include light definitions
#include "../camera/material.h"     // Include material definitions

Vector3 blinnPhongShading(const Intersection &intersection, const Ray &ray, const std::vector<Light> &lights);

#endif // BLINN_PHONG_H
