#pragma once
#ifndef HALLWAY_H
#define HALLWAY_H

#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include "Collision.h"

struct WallSegment {
    glm::vec3 position;
    float rotation;
    glm::vec3 scale;
    float wallOffset;

    OBBCollision boundary;

    void computeBoundary() {
        glm::vec3 halfScale = scale * 0.5f;

        // Axele inițiale (locale)
        glm::vec3 xAxis(1, 0, 0);
        glm::vec3 yAxis(0, 1, 0);
        glm::vec3 zAxis(0, 0, 1);

        // Rotire în funcție de rotația peretelui
        glm::mat4 rotMat = glm::rotate(glm::radians(rotation), glm::vec3(0, 1, 0));
        glm::vec3 rotatedX = glm::vec3(rotMat * glm::vec4(xAxis, 0.0));
        glm::vec3 rotatedY = glm::vec3(rotMat * glm::vec4(yAxis, 0.0));
        glm::vec3 rotatedZ = glm::vec3(rotMat * glm::vec4(zAxis, 0.0));

        boundary = OBBCollision(position, halfScale, rotatedX, rotatedY, rotatedZ);
    }
};

GLuint createWallMesh();
std::vector<WallSegment> generateWallLayout();
void updateHallway(std::vector<WallSegment>& walls,glm::vec3 playerPos);
void updateFloor(float* floorOff, glm::vec3 playerPos);

#endif
