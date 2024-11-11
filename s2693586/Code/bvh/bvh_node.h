#ifndef BVH_NODE_H
#define BVH_NODE_H

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include "aabb.h"
#include "../geometry/geometry.h"

class BVHNode
{
public:
    AABB boundingBox;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    std::vector<std::shared_ptr<const Geometry>> objects;

    BVHNode() = default;

    // Depth limit to prevent infinite recursion
    static const int MAX_DEPTH = 20;

    // Static function to build the BVH with consideration for objects that span the split plane

    // Helper function to partition objects around a pivot
    std::vector<std::shared_ptr<const Geometry>> partitionAroundMedian(std::vector<std::shared_ptr<const Geometry>> &objects, int splitAxis)
    {
        auto median_it = objects.begin() + objects.size() / 2;
        std::nth_element(objects.begin(), median_it, objects.end(),
                         [splitAxis](const std::shared_ptr<const Geometry> &a, const std::shared_ptr<const Geometry> &b)
                         {
                             return a->centroid()[splitAxis] < b->centroid()[splitAxis];
                         });
        return std::vector<std::shared_ptr<const Geometry>>(objects.begin(), median_it);
    }

    static std::unique_ptr<BVHNode> build(std::vector<std::shared_ptr<const Geometry>> objects, int depth = 0)
    {
        auto node = std::make_unique<BVHNode>();

        // Base cases: leaf node if depth limit exceeded or not enough objects to split
        static constexpr int MAX_LEAF_SIZE = 4;
        if (depth > MAX_DEPTH || objects.size() <= MAX_LEAF_SIZE)
        {
            node->objects = objects;
            return node;
        }
        
        // Compute bounding box for all objects
        AABB combinedBox;
        for (const auto &obj : objects)
        {
            combinedBox.expand(obj->boundingBox());
        }
        node->boundingBox = combinedBox;

        // Calculate axis to split on based on the largest bounding box extent
        Vector3 boundsSize = node->boundingBox.maxBounds - node->boundingBox.minBounds;
        int splitAxis = (boundsSize.x > boundsSize.y) ? ((boundsSize.x > boundsSize.z) ? 0 : 2) : ((boundsSize.y > boundsSize.z) ? 1 : 2);

        // Sort objects along the chosen axis based on their centroids
        std::sort(objects.begin(), objects.end(), [splitAxis](const std::shared_ptr<const Geometry> &a, const std::shared_ptr<const Geometry> &b)
                  { return a->centroid()[splitAxis] < b->centroid()[splitAxis]; });

        // Midpoint split along the sorted axis
        size_t mid = objects.size() / 2;
        float splitPosition = objects[mid]->centroid()[splitAxis];

        // Split objects into left and right lists, considering objects that span the split plane
        std::vector<std::shared_ptr<const Geometry>> leftObjects;
        std::vector<std::shared_ptr<const Geometry>> rightObjects;

        for (const auto &obj : objects)
        {
            float minBound = obj->boundingBox().minBounds[splitAxis];
            float maxBound = obj->boundingBox().maxBounds[splitAxis];

            if (maxBound <= splitPosition)
            {
                // Fully in the left partition
                leftObjects.push_back(obj);
            }
            else if (minBound >= splitPosition)
            {
                // Fully in the right partition
                rightObjects.push_back(obj);
            }
            else
            {
                // Spans both partitions, include in both
                leftObjects.push_back(obj);
                rightObjects.push_back(obj);
            }
        }

        // Prevent infinite recursion by checking if split was effective
        if (leftObjects.size() == objects.size() || rightObjects.size() == objects.size())
        {
            node->objects = objects; // Treat as a leaf node if splitting is ineffective
            return node;
        }

        // Recursively build child nodes with increased depth
        node->left = build(leftObjects, depth + 1);
        node->right = build(rightObjects, depth + 1);

        return node;
    }

    // Ray intersection method for the BVH
    bool intersect(const Ray &ray, Intersection &closestIntersection) const
    {
        float tMin = 0.0f, tMax = std::numeric_limits<float>::max();

        // Step 1: Check for intersection with bounding box
        if (!boundingBox.intersect(ray, tMin, tMax))
        {
            return false; // No intersection with this node's bounding box
        }

        // Step 2: Leaf node - directly test stored objects for intersections
        if (!left && !right)
        {
            bool hit = false;
            for (const auto &obj : objects)
            {
                Intersection tempIntersection = obj->intersect(ray);
                if (tempIntersection.hit && tempIntersection.distance < closestIntersection.distance)
                {
                    closestIntersection = tempIntersection; // Update to the closest intersection
                    hit = true;
                    // std::cout << "Hit object in leaf node at distance " << tempIntersection.distance << std::endl;
                }
            }
            return hit; // Return true if any object was hit in this leaf node
        }

        // Step 3: Non-leaf node - recursively check child nodes for intersection
        bool hitLeft = left && left->intersect(ray, closestIntersection);
        bool hitRight = right && right->intersect(ray, closestIntersection);

        // Return true if either child node was hit
        return hitLeft || hitRight;
    }

    bool intersectShadowRay(const Ray &ray, float maxDistance) const
    {
        float tMin = 0.0f, tMax = maxDistance;

        if (!boundingBox.intersect(ray, tMin, tMax))
        {
            return false;
        }

        if (!left && !right) // Leaf node
        {
            for (const auto &obj : objects)
            {
                Intersection tempIntersection = obj->intersect(ray);
                if (tempIntersection.hit && tempIntersection.distance < maxDistance)
                {
                    return true; // Early exit for shadow
                }
            }
            return false;
        }

        // Recursively check left and right with early exit
        return (left && left->intersectShadowRay(ray, maxDistance)) ||
               (right && right->intersectShadowRay(ray, maxDistance));
    }
};

#endif // BVH_NODE_H
