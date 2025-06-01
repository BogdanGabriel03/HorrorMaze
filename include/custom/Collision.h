#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include <limits>
#include "Ray.h"

// using Oriented Bounding Box Collision
struct OBBCollision {
	glm::vec3 center;
	glm::vec3 axis[3];
	glm::vec3 halfSize;

	OBBCollision() = default;
	OBBCollision(const glm::vec3& c, const glm::vec3& hs, const glm::vec3& x, const glm::vec3& y, const glm::vec3& z)
		: center(c), halfSize(hs) {
		axis[0] = glm::normalize(x);
		axis[1] = glm::normalize(y);
		axis[2] = glm::normalize(z);
	}
};

inline bool checkOBBCollision(const OBBCollision& a, const OBBCollision& b) {
    const float EPSILON = 0.0001f;

    // Matrice 3x3 cu axele
    glm::vec3 axesToTest[15];
    int index = 0;

    // Axele proprii
    for (int i = 0; i < 3; ++i) {
        axesToTest[index++] = a.axis[i];
        axesToTest[index++] = b.axis[i];
    }

    // Produse încrucișate între axele celor două OBB-uri
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            axesToTest[index++] = glm::cross(a.axis[i], b.axis[j]);

    for (int i = 0; i < 15; ++i) {
        glm::vec3 axis = glm::normalize(axesToTest[i]);

        float aProj =
            a.halfSize.x * fabs(glm::dot(a.axis[0], axis)) +
            a.halfSize.y * fabs(glm::dot(a.axis[1], axis)) +
            a.halfSize.z * fabs(glm::dot(a.axis[2], axis));

        float bProj =
            b.halfSize.x * fabs(glm::dot(b.axis[0], axis)) +
            b.halfSize.y * fabs(glm::dot(b.axis[1], axis)) +
            b.halfSize.z * fabs(glm::dot(b.axis[2], axis));

        float dist = fabs(glm::dot(b.center - a.center, axis));

        if (dist > aProj + bProj + EPSILON)
            return false;
    }

    return true;
}

inline bool rayIntersectsOBB(const Ray& ray, const OBBCollision& obb, float& tMinOut) {
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();

    glm::vec3 p = obb.center - ray.origin;

    for (int i = 0;i < 3;++i) {
        const glm::vec3& axis = obb.axis[i];
        float e = glm::dot(axis, p);
        float f = glm::dot(axis, ray.direction);

        if (fabs(f) > 0.0001f) { // Avoid division by 0
            float t1 = (e + obb.halfSize[i]) / f;
            float t2 = (e - obb.halfSize[i]) / f;

            if (t1 > t2) std::swap(t1, t2);

            if (t1 > tMax || t2 < tMin) return false;

            tMin = glm::max(tMin, t1);
            tMax = glm::min(tMax, t2);

            if (tMin > tMax) return false;
        }
        else {
            // Ray is parallel to slab
            if (-e - obb.halfSize[i] > 0.0f || -e + obb.halfSize[i] < 0.0f)
                return false;
        }
    }
    if (tMin > 0.0f) {
        tMinOut = tMin;
        return true;
    }
    return false;
}

#endif