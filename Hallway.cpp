#include "Hallway.h"

int c = -10;
GLuint createWallMesh() {
    float vertices[] = {
        // positions            // texture coords   //normals
        -0.5f, -0.5f,  0.05f,    0.0f, 0.0f,        0.0f,0.0f,-1.0f,
        -0.5f,  0.5f,  0.05f,    0.0f, 1.0f,        0.0f,0.0f,-1.0f,
         0.5f, -0.5f,  0.05f,    1.0f, 0.0f,        0.0f,0.0f,-1.0f,
         0.5f,  0.5f,  0.05f,    1.0f, 1.0f,        0.0f,0.0f,-1.0f,
    };

    unsigned int indexes[] = {
        0,1,2,
        1,2,3
    };

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture Coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));   
    glEnableVertexAttribArray(2);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

std::vector<WallSegment> generateWallLayout() {
    std::vector<WallSegment> walls;

    //walls.push_back(WallSegment(glm::vec3(-0.9f, 0.0f, 18.0f), 0.0f, glm::vec3(4.0f, 10.0f, 0.1f))); // Behind wall
    
    walls.push_back(WallSegment(glm::vec3(4.35f, 0.0f, 18.0f), 0.0f, glm::vec3(4.0f, 20.0f, 0.1f),20)); // Behind wall
    walls.push_back(WallSegment(glm::vec3(-3.9f, 0.0f, 18.0f), 0.0f, glm::vec3(2.5f, 20.0f, 0.1f),20)); // Behind wall

    for (int i = -4; i < 7; ++i) {
        walls.push_back(WallSegment(glm::vec3(5.0f, 0.0f, -i * 4.0f), 90.0f, glm::vec3(4.0f, 20.0f, 0.1f), 20)); // Right wall
        walls.push_back(WallSegment(glm::vec3(-5.0f, 0.0f, -i * 4.0f), 90.0f, glm::vec3(4.0f, 20.0f, 0.1f), 20)); // Left wall
    }

    walls.push_back(WallSegment(glm::vec3(-5.0f, 0.0f, -7 * 4.0f), 90.0f, glm::vec3(4.0f, 20.0f, 0.1f), 30));
    walls.push_back(WallSegment(glm::vec3(-5.0f, 0.0f, -8 * 4.0f), 90.0f, glm::vec3(4.0f, 20.0f, 0.1f), 30));

    // Right turn
    for (int i = 0; i < 5; ++i) {
        walls.push_back(WallSegment(glm::vec3(7.0f + i * 4.0f, 0.0f, -26.0f), 0.0f, glm::vec3(4.0f, 20.0f, 0.1f), -10)); // Inner corner
        walls.push_back(WallSegment(glm::vec3(-3.0f + i * 4.0f, 0.0f, -34.0f), 0.0f, glm::vec3(4.0f, 20.0f, 0.1f), 10));  // Outer corner
    }

    // New hallway along X
    for (int i = 0; i < 6; ++i) {
        walls.push_back(WallSegment(glm::vec3(25.0f, 0.0f, -28.0f - i * 4.0f), 90.0f, glm::vec3(4.0f, 20.0f, 0.1f), -30)); // Far wall
    }

    for (int i = 0; i < 4; ++i) {
        walls.push_back(WallSegment(glm::vec3(15.0f, 0.0f, -36.0f - i * 4.0f), 90.0f, glm::vec3(4.0f, 20.0f, 0.1f), -10)); // Near wall
    }
    return walls;
}


std::vector<DoorSegment> generateDoorsLayout() {
    std::vector<DoorSegment> doors;
    doors.push_back(DoorSegment(glm::vec3(-0.15f, 0.0f, 18.0f),0.0f, glm::vec3(5.0f, 20.0f, 0.05f),20));
    return doors;
}

std::vector<LightSource> generateLightsLayout() {
    std::vector<LightSource> lights;
    lights.push_back(LightSource(glm::vec3(5.0f, 9.0f, 2.0f), 45.0f, glm::vec3(0.7),20.0f));
    lights.push_back(LightSource(glm::vec3(-5.0f, 9.0f, 2.0f), 45.0f, glm::vec3(0.7),20.0f));
    return lights;
}

void updateHallway(std::vector<WallSegment>& walls, std::vector<DoorSegment>& doors, std::vector<LightSource>& lights, glm::vec3 playerPos, float deltaTime) {
    for (int i = 0;i < 46;++i) {
        if (walls[i].getPosition().z > playerPos.z + 34) {
            walls[i].setPosition(glm::vec3(walls[i].getOffset(),0, -68));
            walls[i].setOffset(walls[i].getOffset() * -1);
        }
        else if (walls[i].getPosition().z < playerPos.z - 34) {
            walls[i].setPosition(glm::vec3(walls[i].getOffset(), 0, +68));
            walls[i].setOffset(walls[i].getOffset() * -1);
        }
        walls[i].computeBoundary();
    }

    for (auto& door : doors) {
        if (door.getPosition().z > playerPos.z + 34) {
            door.setPosition(glm::vec3(door.getOffset(), 0, -68));
            door.setOffset(door.getOffset() * -1);
            door.setIsOpen(false);
            door._angle = 0;
        }
        else if (door.getPosition().z < playerPos.z - 34) {
            door.setPosition(glm::vec3(door.getOffset(), 0, +68));
            door.setOffset(door.getOffset() * -1);
            door.setIsOpen(false);
            door._angle = 0;
        }

        if (door.isAnimating()) {
            bool playerOnLeft = ((playerPos - door.getPosition()).z < 0);

            float openTargetAngle = playerOnLeft ? -90.0f : 90.0f;
            float closedTargetAngle = 0.0f;

            float targetAngle = door.isOpen() ? openTargetAngle : closedTargetAngle;

            float angleDifference = targetAngle - door._angle;
            float dir = (angleDifference > 0) ? 1.0f : -1.0f;

            float angleStep = dir * 90.0f / door._openSpeed * deltaTime;

            if (fabs(angleStep) > fabs(angleDifference)) {
                door._angle = targetAngle;
                door.setIsAnimating(false);
            }
            else {
                door._angle += angleStep;
            }
        }
        door.computeBoundary();
    }

    for (auto& light : lights) {
        if (light.getPosition().z > playerPos.z + 34) {
            light.setPosition(glm::vec3(light.getOffset(), 0, -68));
            light.setOffset(light.getOffset() * -1);
        }
        else if (light.getPosition().z < playerPos.z - 34) {
            light.setPosition(glm::vec3(light.getOffset(), 0, +68));
            light.setOffset(light.getOffset() * -1);
        }
    }
}

void updateFloor(float* floorOff, glm::vec3 playerPos) {
    for (int i = 0; i < 2; ++i) {
        if (floorOff[i] > playerPos.z + 100) {
            floorOff[i] -= 200;
        }
        else if (floorOff[i] < playerPos.z - 100) {
            floorOff[i] += 200;
        }
    }
}
