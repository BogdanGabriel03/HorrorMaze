#include "Hallway.h"

int c = -10;
GLuint createWallMesh() {
    float vertices[] = {
        // positions            // texture coords
        -0.5f, -1.0f,  0.05f,    0.0f, 0.0f,
        -0.5f,  1.0f,  0.05f,    0.0f, 1.0f,
         0.5f, -1.0f,  0.05f,    1.0f, 0.0f,
         0.5f,  1.0f,  0.05f,    1.0f, 1.0f,
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture Coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));   
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

std::vector<WallSegment> generateWallLayout() {
    std::vector<WallSegment> walls;

    walls.push_back({ glm::vec3(3.1f, 0.0f, 18.0f), 0.0f, glm::vec3(4.0f, 10.0f, 0.1f) }); // Behind wall
    walls.push_back({ glm::vec3(-0.9f, 0.0f, 18.0f), 0.0f, glm::vec3(4.0f, 10.0f, 0.1f) }); // Behind wall
    walls.push_back({ glm::vec3(-4.0f, 0.0f, 18.0f), 0.0f, glm::vec3(2.3f, 10.0f, 0.1f) }); // Behind wall
    
    for (int i = -4; i < 7; ++i) {
        walls.push_back({ glm::vec3(5.0f, 0.0f, -i * 4.0f), 90.0f, glm::vec3(4.0f, 10.0f, 0.1f),20 }); // Right wall
        walls.push_back({ glm::vec3(-5.0f, 0.0f, -i * 4.0f), 90.0f, glm::vec3(4.0f, 10.0f, 0.1f),20 }); // Left wall
    }
    walls.push_back({ glm::vec3(-5.0f, 0.0f, -7 * 4.0f), 90.0f, glm::vec3(4.0f, 10.0f, 0.1f),30 });
    walls.push_back({ glm::vec3(-5.0f, 0.0f, -8 * 4.0f), 90.0f, glm::vec3(4.0f, 10.0f, 0.1f),30 });


    //// Right turn
    for (int i = 0;i < 5;++i) {
        walls.push_back({ glm::vec3(7.0f + i * 4.0f, 0.0f, -26.0f), 0.0f, glm::vec3(4.0f, 10.0f, 0.1f),-10 }); // Inner corner
        walls.push_back({ glm::vec3(-3.0f + i * 4.0f, 0.0f, -34.0f), 0.0f, glm::vec3(4.0f, 10.0f, 0.1f),10 }); // Outer corner
    }

    //// New hallway along X
    for (int i = 0; i < 6; ++i) {
        walls.push_back({ glm::vec3(25.0f, 0.0f, -28.0f - i*4.0f), 90.0f, glm::vec3(4.0f, 10.0f, 0.1f),-30 }); // Far wall
    }
    for (int i = 0; i < 4; ++i) {
        walls.push_back({ glm::vec3(15.0f, 0.0f, -36.0f - i*4.0f), 90.0f, glm::vec3(4.0f, 10.0f, 0.1f),-10 }); // Near wall
    }

    for (auto& wall : walls) {
        wall.computeBoundary();
    }

    return walls;
}

void updateHallway(std::vector<WallSegment>& walls,glm::vec3 playerPos) {
    for (int i = 3;i < 47;++i) {
        if (walls[i].position.z > playerPos.z + 34) {
            walls[i].position.x += walls[i].wallOffset;
            walls[i].wallOffset *= -1;
            walls[i].position.z -= 68;
        }
        else if (walls[i].position.z < playerPos.z - 34) {
            walls[i].position.x += walls[i].wallOffset;
            walls[i].wallOffset *= -1;
            walls[i].position.z += 68;
        }
        walls[i].computeBoundary();
    }
}

void updateFloor(float* floorOff, glm::vec3 playerPos) {
    for (int i = 0; i < 2; ++i) {
        if (floorOff[i] > playerPos.z + 100) {
            floorOff[i] -= 200; // move floor back behind the player
        }
        else if (floorOff[i] < playerPos.z - 100) {
            floorOff[i] += 200; // move floor forward in front of the player
        }
    }
}
