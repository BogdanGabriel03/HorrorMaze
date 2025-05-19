#include <GL/glew.h>
#include <stack>
#include <iostream>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include "Hallway.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Time between current frame and last frame
float deltaTime = 0.0f;	
// Time of last frame
float lastFrame = 0.0f;

// Mouse initial pos
float mouseX = 0, mouseY = 0;
enum GAME_STATE {MAIN_MENU, IN_GAME, GAME_PAUSED};
GAME_STATE gameState;
bool firstMouse = true;

// camera 
Camera camera(glm::vec3(0.0f, 0.0f, 15.0f));

// walls
std::vector<WallSegment> hallwayWalls;

float l = 50.0;
float inGame_floorVertices[] = {
	 l,  0.0,  l,	0.0,0.0,
	 l,  0.0, -l,	0.0,15.0,
	-l,  0.0,  l,	15.0,0.0,
	-l,  0.0, -l,	15.0,15.0,
};

float wallVertices[] = {
	// image coord			// texture coord
	-6.0,	0.0,  -10.0,	0.0,0.0,	
	-6.0,	10.0, -10.0,	0.0,1.0,
	-6.0,	0.0,  20.0,		2.0,0.0,
	-6.0,	10.0, 20.0,		2.0,1.0
};

float pausedState_screenVertices[] = {
	-1.0, -1.0, 0.0,	
	-1.0,  1.0, 0.0,
	 1.0, -1.0, 0.0,
	 1.0,  1.0, 0.0
};

unsigned int pausedState_idx[] = {
	0,1,2,
	1,3,2
};

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  3.0f,  0.0f),
	glm::vec3(5.0f,  3.0f, 0.0f),
	glm::vec3(7.5f, 3.0f, 2.5f),
	glm::vec3(5.0f, 3.0f, 5.0f),
	glm::vec3(2.5f, 3.0f, 7.5f),
	glm::vec3(0.0f,  3.0f, 5.0f),
	glm::vec3(-2.5f, 3.0f, 7.5f),
	glm::vec3(-5.0f,  3.0f, 2.5f),
	glm::vec3(-2.5f,  3.0f, -2.5f),
	glm::vec3(0.0f,  3.0f, -5.0f)
};

Shader* inGameStateShaderProgramme = NULL;
Shader* pausedStateShaderProgramme = NULL;

unsigned int pausedState_vao, pausedState_vbo, pausedState_ebo;
unsigned int floor_vao, floor_vbo, floor_ebo;
unsigned int wall_vbo, wall_vao;
int imgW, imgH, nrChannels;
unsigned int floorTexture, wallTexture, ceilingTexture;
unsigned char* floorImg, * wallImg, *ceilingImg;
float mixPercent = 0.2;

// floor offset
float floorOffset[2] = { 0,-100 };

void entryFunc(int state) {
	if (state == GLUT_ENTERED) {
		// When mouse re-enters window, reinitialize tracking
		firstMouse = true;
	}
}

void mouse(int x, int y) {
	if (gameState == IN_GAME) {
		int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
		int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

		if (firstMouse) // initially set to true
		{
			mouseX = x;
			mouseY = y;
			firstMouse = false;
			glutWarpPointer(centerX, centerY);
			return;
		}

		float xoffset = x - centerX;
		float yoffset = centerY - y; // reversed since y-coordinates range from bottom to top

		glutWarpPointer(centerX, centerY);

		mouseX = centerX;
		mouseY = centerY;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void mouseButton(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (button == 3) { // Scroll up
			camera.ProcessMouseScroll(-1.0f);
		}
		else if (button == 4) { // Scroll down
			camera.ProcessMouseScroll(1.0f);
		}
	}
}

void keyboard(unsigned char key, int x, int y)
{
	const float cameraSpeed =2.5f * deltaTime;
	glm::vec3 oldPos = camera.Position;

	switch (key) {
	case 'x':
		if (inGameStateShaderProgramme != NULL) {
			if (mixPercent <= 0.9) {
				mixPercent += 0.1;
				inGameStateShaderProgramme->setFloat("mixPercent", mixPercent);
			}
		}
		break;
	case 'c':
		if (inGameStateShaderProgramme != NULL) {
			if (mixPercent >= 0.1) {
				mixPercent -= 0.1;
				inGameStateShaderProgramme->setFloat("mixPercent", mixPercent);
			}
		}
		break;
	case 'w':
		camera.ProcessKeyboard(FORWARD, deltaTime);
		break;
	case 's':
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		break;
	case 'a':
		camera.ProcessKeyboard(LEFT, deltaTime);
		break;
	case 'd':
		camera.ProcessKeyboard(RIGHT, deltaTime);
		break;
	case 'p':
		if (gameState == GAME_PAUSED) gameState = IN_GAME;
		else if (gameState == IN_GAME) gameState = GAME_PAUSED;
	default:
		break;
	};

	OBBCollision cameraBox(
		camera.Position,
		glm::vec3(0.5f, 1.0f, 0.5f),             // dimensiuni (ajustează după nevoie)
		camera.Right,
		camera.Up,
		glm::normalize(glm::cross(camera.Right, camera.Up))
	);

	bool collided = false;
	for (auto& wall : hallwayWalls) {

		if (checkOBBCollision(cameraBox, wall.boundary)) {
			collided = true;
			break;
		}
	}
	if (collided) {
		camera.Position = oldPos;
	}

	glutPostRedisplay();
}

void loadImage(const char* path, unsigned char** img) {
	stbi_set_flip_vertically_on_load(true);
	*img = stbi_load(path, &imgW, &imgH, &nrChannels, 0);
}

void generateTexture(unsigned char* img, unsigned int *texture,int opt=0) {
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (img) {
		if (opt == 0) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgW, imgH, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(img);
}

void createProgram() {

	// get version info

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	std::cout << "FreeGLUT version: " << glutGet(GLUT_VERSION) << "\n";

	glewInit();

	// IN_GAME STATE

	glGenVertexArrays(1, &floor_vao);
	glGenBuffers(1, &floor_vbo);
	glGenBuffers(1, &floor_ebo);

	glGenVertexArrays(1, &wall_vao);
	glGenBuffers(1, &wall_vbo);

	glBindVertexArray(floor_vao);

	glBindBuffer(GL_ARRAY_BUFFER, floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(inGame_floorVertices), inGame_floorVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pausedState_idx), pausedState_idx, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(wall_vao);

	glBindBuffer(GL_ARRAY_BUFFER, wall_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// loading images and generating textures 

	loadImage("floor.png", &floorImg);
	generateTexture(floorImg,&floorTexture,1);
	loadImage("ceiling.jpg", &ceilingImg);
	generateTexture(ceilingImg, &ceilingTexture);
	loadImage("wall.jpg", &wallImg);
	generateTexture(wallImg, &wallTexture);


	// GAME_PAUSED STATE

	glGenVertexArrays(1, &pausedState_vao);
	glGenBuffers(1, &pausedState_vbo);
	glGenBuffers(1, &pausedState_ebo);

	glBindVertexArray(pausedState_vao);

	glBindBuffer(GL_ARRAY_BUFFER, pausedState_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pausedState_screenVertices), pausedState_screenVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pausedState_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pausedState_idx), pausedState_idx, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	// CREATE THE SHADERS 

	inGameStateShaderProgramme = new Shader("vertex.vert", "fragment.frag");
	pausedStateShaderProgramme = new Shader("vertex_paused.vert", "fragment_paused.frag");

	// INITILAIZE UNIFORMS

	inGameStateShaderProgramme->use();
	inGameStateShaderProgramme->setInt("floorTexture", 0);
	inGameStateShaderProgramme->setInt("ceilingTexture", 1);
	inGameStateShaderProgramme->setInt("wallTexture", 2);

	pausedStateShaderProgramme->use();
	pausedStateShaderProgramme->setVec4("overlayColor", glm::vec4(0, 0, 0, 0.75));

	// UNCOMMENT THIS TO DRAW IN WIREFRME POLYGONS
	
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	wall_vao = createWallMesh();
	hallwayWalls = generateWallLayout();
}

void Display() {
	// Clear screen
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Always calculate time/deltaTime
	float timeValue = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	float currentFrame = timeValue;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Handle cursor visibility
	if (gameState == GAME_PAUSED) {
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}
	else {
		glutSetCursor(GLUT_CURSOR_NONE);
	}

	updateHallway(hallwayWalls, camera.Position);
	updateFloor(floorOffset, camera.Position);

	// ALWAYS RENDER THE 3D SCENE (whether paused or not)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ceilingTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, wallTexture);

	inGameStateShaderProgramme->use();

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
		(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	inGameStateShaderProgramme->setMat4("projection", projection);

	glm::mat4 view = camera.GetViewMatrix();
	inGameStateShaderProgramme->setMat4("view", view);

	inGameStateShaderProgramme->setFloat("xOffset", 0.0f);
	inGameStateShaderProgramme->setFloat("mixPercent", mixPercent);

	// floor and ceiling
	glBindVertexArray(floor_vao);

	// first floor texture
	inGameStateShaderProgramme->setInt("useTextures", 0);
	glm::mat4 floorModel = glm::mat4(1.0f);
	if(floorOffset[0]) floorModel = glm::translate(floorModel, glm::vec3(0, 0, floorOffset[0]));
	inGameStateShaderProgramme->setMat4("model", floorModel);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// first ceiling texture
	inGameStateShaderProgramme->setInt("useTextures", 1);
	floorModel = glm::translate(floorModel, glm::vec3(0.0, 10.0, 0.0));
	inGameStateShaderProgramme->setMat4("model", floorModel);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// second floor texture
	inGameStateShaderProgramme->setInt("useTextures", 0);
	floorModel = glm::mat4(1.0f);
	if (floorOffset[1]) floorModel = glm::translate(floorModel, glm::vec3(0, 0, floorOffset[1]));
	inGameStateShaderProgramme->setMat4("model", floorModel);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// second ceiling texture
	inGameStateShaderProgramme->setInt("useTextures", 1);
	floorModel = glm::translate(floorModel, glm::vec3(0.0, 10.0, 0.0));
	inGameStateShaderProgramme->setMat4("model", floorModel);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(wall_vao);
	inGameStateShaderProgramme->setInt("useTextures", 2);

	for (auto& wall : hallwayWalls) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, wall.position);
		model = glm::rotate(model, glm::radians(wall.rotation), glm::vec3(0, 1, 0));
		model = glm::scale(model, wall.scale);
		inGameStateShaderProgramme->setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}


	// Only render pause overlay if paused
	if (gameState == GAME_PAUSED) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		pausedStateShaderProgramme->use();
		glBindVertexArray(pausedState_vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("SPG");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	createProgram();

	glutDisplayFunc(Display);
	glutIdleFunc(glutPostRedisplay);
	glutKeyboardFunc(keyboard);
	glutEntryFunc(entryFunc);

	glutSetCursor(GLUT_CURSOR_NONE);  // Ascunde cursorul
	//glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

	glutPassiveMotionFunc(mouse);  // Capturează cursorul
	glutMouseFunc(mouseButton);

	gameState = IN_GAME;
	glutMainLoop();

	delete inGameStateShaderProgramme;
	return 0;
}
