#pragma once
#ifndef RAY_H_
#define RAY_H_

#include <glm/gtx/transform.hpp>

class Ray {
public:
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(const glm::vec3& origin = glm::vec3(0.0f), const glm::vec3& direction = glm::vec3(0.0f, 0.0f, -1.0f))
        : origin(origin), direction(glm::normalize(direction)) {}

    // Get a point along the ray at distance t
    glm::vec3 getPoint(float t) const {
        return origin + direction * t;
    }
};

#endif
