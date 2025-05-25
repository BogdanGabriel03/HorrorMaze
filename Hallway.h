#pragma once
#ifndef HALLWAY_H
#define HALLWAY_H

#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include "Collision.h"

class HallwaySegment {
protected: 
    glm::vec3 position;
    float rotation;
    float offset;
    glm::vec3 scale;
    OBBCollision boundary;
public:
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

    HallwaySegment(glm::vec3 pos, float rot, glm::vec3 scale, float offset)
        : position(pos), rotation(rot), scale(scale),offset(offset) {
        computeBoundary();
    }

    glm::vec3 getPosition() {
        return position;
    }

    void setPosition(glm::vec3 vec) {
        position += vec;
    }

    OBBCollision getBoundary() {
        return boundary;
    }

    float getRotation() {
        return rotation;
    }

    glm::vec3 getScale() {
        return scale;
    }
    float getOffset() {
        return offset;
    }
    void setOffset(float _off) {
        offset = _off;
    }
};
class WallSegment : public HallwaySegment{
public:
    WallSegment(glm::vec3 pos, float rotAngle, glm::vec3 scaleFactor, float wallOff=0) : HallwaySegment(pos,rotAngle,scaleFactor,wallOff) {
        
    }
};

class DoorSegment : public HallwaySegment {
protected:
    bool _isOpen;
    bool _isAnimating;
public:
    float _angle;
    float _openSpeed;

    DoorSegment(glm::vec3 pos, float rotAngle, glm::vec3 scaleFactor, float DoorOffset=0, float angle=0) : HallwaySegment(pos, rotAngle, scaleFactor, DoorOffset) {
        _angle = angle;
        _isOpen = false;
        _isAnimating = false;
        _openSpeed = 1.5f;
    }
    bool isAnimating() {
        return _isAnimating;
    }
    void setIsAnimating(bool val) {
        _isAnimating = val;
    }
    bool isOpen() {
        return _isOpen;
    }
    void setIsOpen(bool val) {
        _isOpen = val;
    }
    void computeBoundary() {
        // The pivot is at the left edge of the door (local space: (-scale.x/2, 0, 0))
        //glm::vec3 pivotOffsetLocal = glm::vec3(-scale.x * 0.5f, 0.0f, 0.0f);

        // Create rotation matrix
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(_angle), glm::vec3(0, 1, 0));

        glm::vec3 pivotPoint = position + glm::vec3(scale.x * 0.5f, 0, 0);
        glm::vec3 centerOffset = glm::vec3(scale.x * (0.5f - 0.5f), 0, 0);
        glm::vec3 rotatedCenterOffset = glm::vec3(rotationMatrix * glm::vec4(centerOffset, 0.0f));

        glm::vec3 finalCenter = pivotPoint + rotatedCenterOffset;

        // Calculate axes
        glm::vec3 right = glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0));
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 forward = glm::vec3(rotationMatrix * glm::vec4(0, 0, 1, 0));

        // Update collision box
        boundary.center = finalCenter;
        boundary.axis[0] = glm::normalize(right);
        boundary.axis[1] = glm::normalize(up);
        boundary.axis[2] = glm::normalize(forward);
        boundary.halfSize = scale * 0.5f;
    }
};

class LightSource : public HallwaySegment {
private:
    glm::vec3 lightColor;
    float intensity;
public:
    LightSource(glm::vec3 pos, float rotAngle, glm::vec3 scaleFactor, float lightOff = 0) : HallwaySegment(pos, rotAngle, scaleFactor, lightOff) {
        lightColor = glm::vec3(0.6, 0.05, 0.05);
        intensity = 0.1;
    }

    glm::vec3 getLightColor() { return lightColor; }
    void setLightColor(glm::vec3 color) { lightColor = color; }

    float getLightIntensity() { return intensity; }
    void setLightIntensity(float _intensity) { intensity = _intensity; }
};

GLuint createWallMesh();
std::vector<WallSegment> generateWallLayout();
std::vector<DoorSegment> generateDoorsLayout();
std::vector<LightSource> generateLightsLayout();
void updateHallway(std::vector<WallSegment>& walls, std::vector<DoorSegment>& doors, std::vector<LightSource>& lights, glm::vec3 playerPos, float);
void updateFloor(float* floorOff, glm::vec3 playerPos);

#endif
