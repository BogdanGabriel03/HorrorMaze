#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include <glm/gtx/transform.hpp>

typedef struct CollisionBoundary {
	glm::vec3 min;
	glm::vec3 max;
}Collision;

inline bool checkCollision(const Collision& a, const Collision& b) {
	return (a.max.x > b.min.x && a.min.x < b.max.x) &&
		(a.max.y > b.min.y && a.min.y < b.max.y) &&
		(a.max.z > b.min.z && a.min.z < b.max.z);
}

#endif