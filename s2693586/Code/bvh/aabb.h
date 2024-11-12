#ifndef AABB_H
#define AABB_H

#include "../camera/vector3.h"
#include "../camera/ray.h"
#include <limits>
#include <cmath>

class AABB {
public:
    Vector3 minBounds;
    Vector3 maxBounds;

    // Constructor initializing bounds to extreme values to allow proper expansion
    AABB() 
        : minBounds(Vector3(std::numeric_limits<float>::max())),
          maxBounds(Vector3(-std::numeric_limits<float>::max())) {}

    AABB(const Vector3 &minBounds, const Vector3 &maxBounds) 
        : minBounds(minBounds), maxBounds(maxBounds) {}

    // Expands the current AABB to include another AABB
    void expand(const AABB &other) {
        minBounds = Vector3(
            std::min(minBounds.x, other.minBounds.x), 
            std::min(minBounds.y, other.minBounds.y), 
            std::min(minBounds.z, other.minBounds.z)
        );
        maxBounds = Vector3(
            std::max(maxBounds.x, other.maxBounds.x), 
            std::max(maxBounds.y, other.maxBounds.y), 
            std::max(maxBounds.z, other.maxBounds.z)
        );
    }

    // Ray-AABB intersection
    bool intersect(const Ray &ray, float &tMin, float &tMax) const {
        constexpr float epsilon = 1e-8f; // Small value to handle precision issues
        tMin = 0.0f;
        tMax = std::numeric_limits<float>::max();

        for (int i = 0; i < 3; ++i) {
            float invD = (std::fabs(ray.direction[i]) > epsilon) ? 1.0f / ray.direction[i] : std::numeric_limits<float>::infinity();
            float t0 = (minBounds[i] - ray.origin[i]) * invD;
            float t1 = (maxBounds[i] - ray.origin[i]) * invD;

            if (invD < 0.0f) std::swap(t0, t1); // Ensure t0 is the near intersection, t1 is the far intersection
            tMin = std::max(tMin, t0);           // Update the entry point
            tMax = std::min(tMax, t1);           // Update the exit point

            // Early exit if there's no intersection
            if (tMax <= tMin + epsilon) {
                return false;
            }
        }
        return true;
    }
};

#endif // AABB_H