#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include <glm/gtx/transform.hpp>

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


#endif