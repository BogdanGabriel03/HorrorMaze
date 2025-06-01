#pragma once
#ifndef HALLWAY_H
#define HALLWAY_H

#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include "Collision.h"

enum HALLWAY_SEGMENT_TYPE { WALL, FLOOR, DOOR, LIGHT};

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

class HallwaySegment {
protected: 
    glm::vec3 position;
    float rotation;
    HALLWAY_SEGMENT_TYPE type;
    glm::vec3 scale;
    OBBCollision boundary;
    bool litBy[4];
public:
    bool operator==(const HallwaySegment& other) const {
        return this->position == other.position
            && this->scale == other.scale
            && this->rotation == other.rotation;
    }

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

        //if(i==6) std::cout << "Rotated axis:\n\tx: " << rotatedX << "\n\ty: " << rotatedY << "\n\tz: " << rotatedZ << "\n\n";

        boundary = OBBCollision(position, halfScale, rotatedX, rotatedY, rotatedZ);
    }

    HallwaySegment(glm::vec3 pos, float rot, glm::vec3 scale, HALLWAY_SEGMENT_TYPE type)
        : position(pos), rotation(rot), scale(scale),type(type) {
        for (int i = 0;i < 4;++i) {
            litBy[i] = 0;
        }
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
    HALLWAY_SEGMENT_TYPE getType() {
        return type;
    }
    void setOffset(HALLWAY_SEGMENT_TYPE _type) {
        type = _type;
    }
    void setLitBy(int idx, bool val) {
        if (idx >= 0 && idx <= 3) {
            litBy[idx] = val;
        }
    }
    bool getLitBy(int idx) {
        return litBy[idx];
    }
};

class WallSegment : public HallwaySegment{
public:
    WallSegment(glm::vec3 pos, float rotAngle, glm::vec3 scaleFactor, HALLWAY_SEGMENT_TYPE type=WALL) : HallwaySegment(pos,rotAngle,scaleFactor,type) {
        
    }
};

class FloorSegment : public HallwaySegment {
public:
    FloorSegment(glm::vec3 pos, float rotAngle, glm::vec3 scaleFactor, HALLWAY_SEGMENT_TYPE type = FLOOR) : HallwaySegment(pos, rotAngle, scaleFactor, type) {

    }
};

class DoorSegment : public HallwaySegment {
protected:
    bool _isOpen;
    bool _isAnimating;
public:
    float _angle;
    float _openSpeed;

    DoorSegment(glm::vec3 pos, float rotAngle, glm::vec3 scaleFactor, HALLWAY_SEGMENT_TYPE type=DOOR, float angle=0) : HallwaySegment(pos, rotAngle, scaleFactor, type) {
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

        glm::vec3 pivotPoint = position;
        glm::vec3 centerOffset = glm::vec3(scale.x * (0.5f - 0.5f), 0, 0);
        glm::vec3 rotatedCenterOffset = glm::vec3(rotationMatrix * glm::vec4(pivotPoint, 0.0f));

        glm::vec3 finalCenter = rotatedCenterOffset;
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
    LightSource(glm::vec3 pos, float rotAngle, glm::vec3 scaleFactor, HALLWAY_SEGMENT_TYPE type = LIGHT) : HallwaySegment(pos, rotAngle, scaleFactor, type) {
        lightColor = glm::vec3(0.6, 0.05, 0.05);
        intensity = 0.08;
    }

    glm::vec3 getLightColor() { return lightColor; }
    void setLightColor(glm::vec3 color) { lightColor = color; }

    float getLightIntensity() { return intensity; }
    void setLightIntensity(float _intensity) { intensity = _intensity; }
};

GLuint createWallMesh();
GLuint createFloorMesh();
std::vector<WallSegment> generateWallLayout();
std::vector<DoorSegment> generateDoorsLayout();
std::vector<LightSource> generateLightsLayout();
std::vector<FloorSegment> generateFloorLayout();
void updateHallway(std::vector<HallwaySegment*>& segments, glm::vec3 playerPos, float);
void updateLightView(std::vector<LightSource>&  lights, std::vector<HallwaySegment*>& segments);

#endif
