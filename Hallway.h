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

    Collision boundary;

    void computeBoundary() {
        glm::vec3 halfScale = scale * 0.5f;
        //printf("%f - %f - %f\t %f\t Pos: %f,%f,%f\n",halfScale.x,halfScale.y,halfScale.z, rotation, position.x, position.y, position.z);

        // Fără rotație (0.0f): perete frontal/spate
        if (rotation == 0.0f) {
            boundary.min = position - glm::vec3(halfScale.x, halfScale.y, halfScale.z);
            boundary.max = position + glm::vec3(halfScale.x, halfScale.y, halfScale.z);
        }
        // Rotație 90°: perete lateral
        else if (rotation == 90.0f) {
            boundary.min = position - glm::vec3(halfScale.z, halfScale.y, halfScale.x);
            boundary.max = position + glm::vec3(halfScale.z, halfScale.y, halfScale.x);
        }

        //printf("Min: %f,%f,%f  -  Max: %f,%f,%f\n", boundary.min.x, boundary.min.y, boundary.min.z, boundary.max.x, boundary.max.y, boundary.max.z);
    }
};

GLuint createWallMesh();
std::vector<WallSegment> generateWallLayout();
void updateHallway(std::vector<WallSegment>& walls,glm::vec3 playerPos);
void updateFloor(float* floorOff, glm::vec3 playerPos);

#endif
