#include "Hallway.h"

#define Z_OFFSET 216
#define WALL_OFFSET_Y 5
#define WALL_SCALE_Y 10
#define WALL_SCALE_X 4

int c = -10;
GLuint createWallMesh() {

    float vertices[] = {
        // positions            // texture coords   //normals
        -0.5f, -0.5f,  0.05f,    0.0f, 0.0f,        0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f,  0.05f,    0.0f, 1.0f,        0.0f, 0.0f, -1.0f,
         0.5f, -0.5f,  0.05f,    1.0f, 0.0f,        0.0f, 0.0f, -1.0f,
         0.5f,  0.5f,  0.05f,    1.0f, 1.0f,        0.0f, 0.0f, -1.0f,
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

GLuint createFloorMesh() {
    float vertices[] = {
        // positions           // tex coords   // normals (upward)
        -0.5f, 0.0f, -0.5f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.0f, -0.5f,     1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f,  0.5f,     0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        1, 3, 2
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}


std::vector<WallSegment> generateWallLayout() {
    std::vector<WallSegment> walls;
    float startPointZ = 64.0f;
    
    walls.push_back(WallSegment(glm::vec3(-4.25f, WALL_OFFSET_Y, startPointZ), 0.0f, glm::vec3(3.5f, WALL_SCALE_Y, 0.1f))); // Behind wall
    walls.push_back(WallSegment(glm::vec3(4.25f, WALL_OFFSET_Y, startPointZ), 0.0f, glm::vec3(3.5f, WALL_SCALE_Y, 0.1f))); // Behind wall

    startPointZ -= WALL_SCALE_X/2;

    for (int i = 0; i < 17; ++i) {
        walls.push_back(WallSegment(glm::vec3(6.0f, WALL_OFFSET_Y, startPointZ), 90.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f))); // Right wall
        walls.push_back(WallSegment(glm::vec3(-6.0f, WALL_OFFSET_Y, startPointZ), -90.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f))); // Left wall
        startPointZ -= 4.0f;
    }
    for (int i = 0;i < 3;++i) {
        walls.push_back(WallSegment(glm::vec3(-6.0f, WALL_OFFSET_Y, startPointZ), -90.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f))); // Left wall
        startPointZ -= 4.0f;
    }
    
    startPointZ -= WALL_SCALE_X / 2;
    // Right turn
    for (int i = 0; i < 7; ++i) {
        walls.push_back(WallSegment(glm::vec3(8.0f + i * 4.0f, WALL_OFFSET_Y, startPointZ+16.0f), 0.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f))); // Inner corner
        walls.push_back(WallSegment(glm::vec3(-4.0f + i * 4.0f, WALL_OFFSET_Y, startPointZ+4.0f), 180.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f)));  // Outer corner
    }

    walls.push_back(WallSegment(glm::vec3(23.75f, WALL_OFFSET_Y, -36.0f), 0.0f, glm::vec3(3.5f, WALL_SCALE_Y, 0.1f))); // Behind wall
    walls.push_back(WallSegment(glm::vec3(32.25f, WALL_OFFSET_Y, -36.0f), 0.0f, glm::vec3(3.5f, WALL_SCALE_Y, 0.1f))); // Behind wall

    startPointZ += 14;
    // New hallway
    for (int i = 0;i < 20;++i) {
        if (i >= 3 && i <= 16) {
            walls.push_back(WallSegment(glm::vec3(22.0f, WALL_OFFSET_Y, startPointZ), -90.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f)));
        }
        walls.push_back(WallSegment(glm::vec3(34.0f, WALL_OFFSET_Y, startPointZ), 90.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f)));
        startPointZ -= 4.0f;
    }

    // Left turn
    startPointZ -= WALL_SCALE_X / 2;
    for (int i = 0; i < 7; ++i) {
        walls.push_back(WallSegment(glm::vec3(20.0f - i * 4.0f, WALL_OFFSET_Y, startPointZ+16.0f), 0.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f))); // Inner corner
        walls.push_back(WallSegment(glm::vec3(32.0f - i * 4.0f, WALL_OFFSET_Y, startPointZ+4.0f), 180.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f)));  // Outer corner
    }

    startPointZ += 14;
    // new hallway
    for (int i = 0;i < 20;++i) {
        if (i >= 3) {
            walls.push_back(WallSegment(glm::vec3(6.0f, WALL_OFFSET_Y, startPointZ), 90.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f)));
        }
        walls.push_back(WallSegment(glm::vec3(-6.0f, WALL_OFFSET_Y, startPointZ), -90.0f, glm::vec3(4.0f, WALL_SCALE_Y, 0.1f)));
        startPointZ -= 4.0f;
    }

    return walls;

}

std::vector<FloorSegment> generateFloorLayout() {
    std::vector<FloorSegment> floorTiles;
    float startPointZ = 60.0f;
    for (int i = 0;i < 10;++i) {
        floorTiles.push_back(FloorSegment(glm::vec3(0.0f, 0.0f, startPointZ), 0.0f, glm::vec3(12.0f, 0.0f, 8.0f)));
        startPointZ -= 8.0f;
    }
    startPointZ += 10.0f;
    for (int i = 0;i < 2;++i) {
        floorTiles.push_back(FloorSegment(glm::vec3(10.0f+i*8.0f, 0.0f, startPointZ), 0.0f, glm::vec3(8.0f, 0.0f, 12.0f)));
    }
    startPointZ += 2.0f;
    for (int i = 0;i < 10;++i) {
        floorTiles.push_back(FloorSegment(glm::vec3(28.0f, 0.0f, startPointZ), 0.0f, glm::vec3(12.0f, 0.0f, 8.0f)));
        startPointZ -= 8.0f;
    }
    startPointZ += 10.0f;
    for (int i = 0;i < 2;++i) {
        floorTiles.push_back(FloorSegment(glm::vec3(18.0f - i * 8.0f, 0.0f, startPointZ), 0.0f, glm::vec3(8.0f, 0.0f, 12.0f)));
    }
    startPointZ += 2.0f;
    for (int i = 0;i < 10;++i) {
        floorTiles.push_back(FloorSegment(glm::vec3(0.0f, 0.0f, startPointZ), 0.0f, glm::vec3(12.0f, 0.0f, 8.0f)));
        startPointZ -= 8.0f;
    }
    return floorTiles;
}


std::vector<DoorSegment> generateDoorsLayout() {
    std::vector<DoorSegment> doors;
    doors.push_back(DoorSegment(glm::vec3(0.0f, WALL_OFFSET_Y, 64.0f),0.0f, glm::vec3(5.0f, WALL_SCALE_Y, 0.05f)));
    doors.push_back(DoorSegment(glm::vec3(28.0f, WALL_OFFSET_Y, -36.0f), 0.0f, glm::vec3(5.0f, WALL_SCALE_Y, 0.05f)));
    return doors;
}

std::vector<LightSource> generateLightsLayout() {
    std::vector<LightSource> lights;
    lights.push_back(LightSource(glm::vec3(4.0f, 9.0f, 34.0f), 0.0f, glm::vec3(0.7)));
    lights.push_back(LightSource(glm::vec3(-4.0f, 9.0f, 34.0f), 0.0f, glm::vec3(0.7)));

    lights.push_back(LightSource(glm::vec3(32.0f, 9.0f, -62.0f), 0.0f, glm::vec3(0.7)));
    lights.push_back(LightSource(glm::vec3(24.0f, 9.0f, -62.0f), 0.0f, glm::vec3(0.7)));
    return lights;
}

void updateHallway(std::vector<HallwaySegment*>& segments, glm::vec3 playerPos, float deltaTime) {
    DoorSegment* dSegment = NULL;
    for (auto& segment : segments) {
        switch (segment->getType()) {
        case WALL:
            if (segment->getPosition().z > playerPos.z + Z_OFFSET / 2) {
                segment->setPosition(glm::vec3(0, 0, -Z_OFFSET));
            }
            else if (segment->getPosition().z < playerPos.z - Z_OFFSET / 2) {
                segment->setPosition(glm::vec3(0, 0, +Z_OFFSET));
            }
            segment->computeBoundary();
            break;
        case DOOR:
            dSegment = static_cast<DoorSegment*>(segment);
            if (dSegment->getPosition().z > playerPos.z + Z_OFFSET / 2) {
                dSegment->setPosition(glm::vec3(0, 0, -Z_OFFSET));
                dSegment->setIsOpen(false);
                dSegment->_angle = 0;
            }
            else if (dSegment->getPosition().z < playerPos.z - Z_OFFSET / 2) {
                dSegment->setPosition(glm::vec3(0, 0, +Z_OFFSET));
                dSegment->setIsOpen(false);
                dSegment->_angle = 0;
            }

            if (dSegment->isAnimating()) {
                bool playerOnLeft = ((playerPos - dSegment->getPosition()).z < 0);

                float openTargetAngle = playerOnLeft ? -90.0f : 90.0f;
                float closedTargetAngle = 0.0f;

                float targetAngle = dSegment->isOpen() ? openTargetAngle : closedTargetAngle;

                float angleDifference = targetAngle - dSegment->_angle;
                float dir = (angleDifference > 0) ? 1.0f : -1.0f;

                float angleStep = dir * 90.0f / dSegment->_openSpeed * deltaTime;

                if (fabs(angleStep) > fabs(angleDifference)) {
                    dSegment->_angle = targetAngle;
                    dSegment->setIsAnimating(false);
                }
                else {
                    dSegment->_angle += angleStep;
                }
            }
            dSegment->computeBoundary();
            break;
        case LIGHT:
            if (segment->getPosition().z > playerPos.z + Z_OFFSET / 2) {
                segment->setPosition(glm::vec3(0, 0, -Z_OFFSET));
                std::cout << "\nDISTANTA: " << segment->getPosition().z - playerPos.z << "\n";
            }
            else if (segment->getPosition().z < playerPos.z - Z_OFFSET / 2) {
                segment->setPosition(glm::vec3(0, 0, +Z_OFFSET));
            }
            break;
        case FLOOR:
            if (segment->getPosition().z > playerPos.z + Z_OFFSET / 2) {
                segment->setPosition(glm::vec3(0, 0, -Z_OFFSET));
                //std::cout << "\nDISTANTA: " << light.getPosition().z - walls[27].getPosition().z << "\n";
            }
            else if (segment->getPosition().z < playerPos.z - Z_OFFSET / 2) {
                segment->setPosition(glm::vec3(0, 0, +Z_OFFSET));
            }
            break;
        default:
            std::cerr << "No compatible type!";
            break;
        }
    }
}

void updateLightView(std::vector<LightSource>& lights, std::vector<HallwaySegment*>& segments) {
    for (int j = 0;j < segments.size();++j) {
        HallwaySegment* destination = segments[j];
        //if (destination->getType() == FLOOR) std::cout << "In functie: " << j << "\t" << destination->getPosition() << "\n";
        for (int i = 0;i < lights.size();++i) {
            destination->setLitBy(i, true);
            glm::vec3 rayDir = destination->getPosition() - lights[i].getPosition();
            Ray ray(lights[i].getPosition(), rayDir);
            if (fabs(lights[i].getPosition().z - destination->getPosition().z) > Z_OFFSET / 2) 
            {
                destination->setLitBy(i, false);
                continue;
            }
            for (auto& collision : segments) {
                if (collision == destination || collision->getType() == LIGHT || collision->getType() == FLOOR) continue;
                float tHit;
                bool hit = rayIntersectsOBB(ray, collision->getBoundary(), tHit);
                if (hit && tHit < glm::length(rayDir)) {
                    destination->setLitBy(i, false);
                    break;
                }
            }
        }
        /*if (j == 142 || j==143 || j==144 || j==145) {
            std::cout << destination->getPosition() << "\n";
            for (int k = 0;k < 4;++k) std::cout << j << ": " << destination->getLitBy(k) << " ";
            std::cout << "\n";
        }*/
    }
}
