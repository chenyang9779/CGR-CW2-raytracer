#ifndef AABB_H
#define AABB_H

#include "../camera/vector3.h"
#include "../camera/ray.h"

class AABB {
public:
    Vector3 minBounds;
    Vector3 maxBounds;

    AABB() : minBounds(Vector3::zero()), maxBounds(Vector3::zero()) {}
    AABB(const Vector3 &minBounds, const Vector3 &maxBounds) : minBounds(minBounds), maxBounds(maxBounds) {}

    // Expands the current AABB to include another AABB
    void expand(const AABB &other) {
        minBounds = Vector3(std::min(minBounds.x, other.minBounds.x), std::min(minBounds.y, other.minBounds.y), std::min(minBounds.z, other.minBounds.z));
        maxBounds = Vector3(std::max(maxBounds.x, other.maxBounds.x), std::max(maxBounds.y, other.maxBounds.y), std::max(maxBounds.z, other.maxBounds.z));
    }

    // Ray-AABB intersection
    bool intersect(const Ray &ray, float &tMin, float &tMax) const {
        for (int i = 0; i < 3; ++i) {
            float invD = 1.0f / ray.direction[i];
            float t0 = (minBounds[i] - ray.origin[i]) * invD;
            float t1 = (maxBounds[i] - ray.origin[i]) * invD;

            if (invD < 0.0f) std::swap(t0, t1);
            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            if (tMax <= tMin) {
                return false;
            }
        }
        return true;
    }
};

#endif // AABB_H
