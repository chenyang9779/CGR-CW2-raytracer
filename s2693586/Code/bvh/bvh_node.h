#ifndef BVH_NODE_H
#define BVH_NODE_H

#include <vector>
#include <memory>
#include "aabb.h"
#include "../geometry/geometry.h"

class BVHNode {
public:
    AABB boundingBox;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    std::vector<std::shared_ptr<const Geometry>> objects; 

    BVHNode() = default;

    static std::unique_ptr<BVHNode> build(std::vector<std::shared_ptr<const Geometry>> objects) {
        auto node = std::make_unique<BVHNode>();

        AABB combinedBox;
        for (const auto &obj : objects) {
            combinedBox.expand(obj->boundingBox());
        }
        node->boundingBox = combinedBox;

        if (objects.size() <= 1) {
            node->objects = objects;
            return node;
        }

        Vector3 boundsSize = node->boundingBox.maxBounds - node->boundingBox.minBounds;
        int splitAxis = boundsSize.x > boundsSize.y ? (boundsSize.x > boundsSize.z ? 0 : 2) : (boundsSize.y > boundsSize.z ? 1 : 2);

        std::sort(objects.begin(), objects.end(), [splitAxis](const std::shared_ptr<const Geometry> &a, const std::shared_ptr<const Geometry> &b) {
            return a->centroid()[splitAxis] < b->centroid()[splitAxis];
        });

        size_t mid = objects.size() / 2;
        std::vector<std::shared_ptr<const Geometry>> leftObjects(objects.begin(), objects.begin() + mid);
        std::vector<std::shared_ptr<const Geometry>> rightObjects(objects.begin() + mid, objects.end());

        node->left = build(leftObjects);
        node->right = build(rightObjects);

        return node;
    }

    bool intersect(const Ray &ray, Intersection &closestIntersection) const {
        float tMin = 0.0f, tMax = std::numeric_limits<float>::max();
        if (!boundingBox.intersect(ray, tMin, tMax)) {
            return false; 
        }

        if (!left && !right) {
            bool hit = false;
            for (const auto &obj : objects) {
                Intersection tempIntersection = obj->intersect(ray);
                if (tempIntersection.hit && tempIntersection.distance < closestIntersection.distance) {
                    closestIntersection = tempIntersection;
                    hit = true;
                }
            }
            return hit;
        }

        bool hitLeft = left ? left->intersect(ray, closestIntersection) : false;
        bool hitRight = right ? right->intersect(ray, closestIntersection) : false;

        return hitLeft || hitRight;
    }
};

#endif // BVH_NODE_H
