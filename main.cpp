#include <GL/glew.h>
#include <stack>
#include <iostream>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "custom/Shader.h"
#include "custom/Camera.h"
#include "stb_image.h"
#include "custom/Hallway.h"
#include "custom/Light.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// lightning constants
const float constant = 1.0f;
const float linear = 0.010f;
const float quadratic = 0.00045f;

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

// walls, doors, lights, floor
std::vector<WallSegment> hallwayWalls;
std::vector<DoorSegment> hallwayDoors;
std::vector<LightSource> hallwayLights;
std::vector<FloorSegment> hallwayFloor;
std::vector<HallwaySegment*> hallwaySegments;

// flashlight Flag
bool flashlightOn = false;

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

Shader* inGameStateShaderProgramme = NULL;
Shader* pausedStateShaderProgramme = NULL;
Shader* lightsourceShaderProgramme = NULL;

// VAOs VBOs EBOs
unsigned int pausedState_vao, pausedState_vbo, pausedState_ebo;
unsigned int floor_vao; //floor_vbo, floor_ebo;
unsigned int wall_vao;
unsigned int door_vao;
unsigned int light_vao, flashLight_vao;

// TEXTURES 
int imgW, imgH, nrChannels;
unsigned int floorTexture, wallTexture, ceilingTexture, doorTexture;
unsigned char* floorImg, * wallImg, *ceilingImg, *doorImg;
float mixPercent = 0.2;

// floor offset
//float floorOffset[2] = { 0,-216 };

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

void renderBitmapString(float x, float y, void* font, const char* str) {
	// Save current state
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, SCR_WIDTH, 0, SCR_HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Set raster position with proper y-inversion
	glRasterPos2f(x, SCR_HEIGHT - y - 10);  // Adjust for font height

	// Render each character
	for (const char* c = str; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}

	// Restore state
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
}

void renderMainMenu() {
	// Set up blending for text
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Render text
	glColor3f(1.0f, 0.0f, 0.0f);
	renderBitmapString(SCR_WIDTH / 2 - 150, SCR_HEIGHT / 2,
		GLUT_BITMAP_HELVETICA_18,
		"Press any key to start game");

	glDisable(GL_BLEND);
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
	if (gameState == MAIN_MENU) {
		gameState = IN_GAME;
		return;
	}
	else if(gameState == IN_GAME) {
		const float cameraSpeed = 2.5f * deltaTime;
		glm::vec3 oldPos = camera.Position;

		switch (key) {
		case 'q':
			flashlightOn = !flashlightOn;
			break;
		case 'f':
			for (auto& door : hallwayDoors) {
				float distance = glm::distance(camera.Position, door.getPosition());
				//std::cout << distance << "\n";
				if (distance < 10.0f && !door.isAnimating()) {
					door.setIsAnimating(!door.isAnimating());
					door.setIsOpen(!door.isOpen());
				}
			}
			break;
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
			gameState = GAME_PAUSED;
			break;
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

			if (checkOBBCollision(cameraBox, wall.getBoundary())) {
				collided = true;
				std::cout << "Collided with wall!\n";
				break;
			}
		}
		for (auto& door : hallwayDoors) {
			if (checkOBBCollision(cameraBox, door.getBoundary())) {
				collided = true;
				std::cout << "Collided with door!\n";
				break;
			}
		}
		if (collided) {
			camera.Position = oldPos;
		}
	}
	else if (gameState == GAME_PAUSED) {
		switch (key) {
		case 'p':
			gameState = IN_GAME;
			break;
		default:
			break;
		}
	}
	glutPostRedisplay();
}

void loadImage(const char* path, unsigned char** img) {
	stbi_set_flip_vertically_on_load(true);
	*img = stbi_load(path, &imgW, &imgH, &nrChannels, 0);
}

void generateTexture(unsigned char* img, unsigned int *texture) {
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	GLenum format = GL_RGB;
	if (nrChannels == 4)
		format = GL_RGBA;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (img) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, imgW, imgH, 0, format, GL_UNSIGNED_BYTE, img);
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

	// loading images and generating textures 

	loadImage("imagini/floor.png", &floorImg);
	generateTexture(floorImg,&floorTexture);
	loadImage("imagini/ceiling.jpg", &ceilingImg);
	generateTexture(ceilingImg, &ceilingTexture);
	loadImage("imagini/wall.jpg", &wallImg);
	generateTexture(wallImg, &wallTexture);
	loadImage("imagini/door.png", &doorImg);
	generateTexture(doorImg, &doorTexture);


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
	inGameStateShaderProgramme = new Shader("shaders/vertex.vert", "shaders/fragment.frag");
	pausedStateShaderProgramme = new Shader("shaders/vertex_paused.vert", "shaders/fragment_paused.frag");
	lightsourceShaderProgramme = new Shader("shaders/light_source.vert", "shaders/light_source.frag");

	// INITILAIZE UNIFORMS
	inGameStateShaderProgramme->use();
	inGameStateShaderProgramme->setInt("floorTexture", 0);
	inGameStateShaderProgramme->setInt("ceilingTexture", 1);
	inGameStateShaderProgramme->setInt("wallTexture", 2);
	inGameStateShaderProgramme->setInt("doorTexture", 3);

	pausedStateShaderProgramme->use();
	pausedStateShaderProgramme->setVec4("overlayColor", glm::vec4(0, 0, 0, 0.55));

	// UNCOMMENT THIS TO DRAW IN WIREFRME POLYGONS
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	wall_vao = createWallMesh();
	hallwayWalls = generateWallLayout();
	door_vao = createWallMesh();
	hallwayDoors = generateDoorsLayout();
	light_vao = letThereBeLight();
	hallwayLights = generateLightsLayout();
	floor_vao = createFloorMesh();
	hallwayFloor = generateFloorLayout();
	flashLight_vao = letThereBeLight();

	for (auto& wall : hallwayWalls) {
		hallwaySegments.push_back(&wall);
	};
	for (auto& door : hallwayDoors) {
		hallwaySegments.push_back(&door);
	}
	for (auto& floorTile : hallwayFloor) {
		hallwaySegments.push_back(&floorTile);
	}
	for (auto& light : hallwayLights) {
		hallwaySegments.push_back(&light);
	}
}

void Display() {
	// Clear screen
	glClearColor(0, 0, 0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Always calculate time/deltaTime
	float timeValue = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	float currentFrame = timeValue;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Handle cursor visibility
	if (gameState == IN_GAME) {
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else {
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	if (gameState == MAIN_MENU) {
		renderMainMenu();
	}
	else {
		updateHallway(hallwaySegments, camera.Position, deltaTime);
		updateLightView(hallwayLights, hallwaySegments);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ceilingTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, doorTexture);
	
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
			(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// ligth sources
		lightsourceShaderProgramme->use();
		lightsourceShaderProgramme->setMat4("projection", projection);
		lightsourceShaderProgramme->setMat4("view", view);
		lightsourceShaderProgramme->setFloat("xOffset", 0.0f);

		glBindVertexArray(light_vao);

		for (auto& light : hallwayLights) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light.getPosition());
			model = glm::rotate(model, light.getRotation(),glm::vec3(0,1,0));
			lightsourceShaderProgramme->setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		inGameStateShaderProgramme->use();
		for (int i = 0; i < hallwayLights.size(); ++i) {
			std::string base = "lights[" + std::to_string(i) + "]";
			inGameStateShaderProgramme->setVec3(base + ".position", hallwayLights[i].getPosition());
			inGameStateShaderProgramme->setVec3(base + ".color", hallwayLights[i].getLightColor());
			inGameStateShaderProgramme->setFloat(base + ".intensity", hallwayLights[i].getLightIntensity());
			inGameStateShaderProgramme->setFloat(base + ".constant", constant);
			inGameStateShaderProgramme->setFloat(base + ".linear", linear);
			inGameStateShaderProgramme->setFloat(base + ".quadratic", quadratic);
		}
		// the flashlight
		inGameStateShaderProgramme->setBool("flashlightOn", flashlightOn);
		inGameStateShaderProgramme->setVec3("flashlight.position", camera.Position);
		inGameStateShaderProgramme->setVec3("flashlight.direction", camera.Front);
		inGameStateShaderProgramme->setVec3("flashlight.color", glm::vec3(0.7,0.6,0.1));
		inGameStateShaderProgramme->setFloat("flashlight.intensity", 0.6);
		inGameStateShaderProgramme->setFloat("flashlight.cutOff", glm::cos(glm::radians(12.5f)));
		inGameStateShaderProgramme->setFloat("flashlight.constant", 1.0f);
		inGameStateShaderProgramme->setFloat("flashlight.linear", 0.09f);
		inGameStateShaderProgramme->setFloat("flashlight.quadric", 0.032f);
		inGameStateShaderProgramme->setFloat("flashlight.outerCutOff", glm::cos(glm::radians(17.5f)));

		inGameStateShaderProgramme->setMat4("projection", projection);
		inGameStateShaderProgramme->setMat4("view", view);

		inGameStateShaderProgramme->setFloat("xOffset", 0.0f);
		inGameStateShaderProgramme->setFloat("mixPercent", mixPercent);
		inGameStateShaderProgramme->setVec3("viewPos", camera.Position);
		//inGameStateShaderProgramme->setVec3("lightColor", glm::vec3(0.6, 0.05, 0.05));

		// floor and ceiling
		glBindVertexArray(floor_vao);
		inGameStateShaderProgramme->setInt("useTextures", 0);

		for (auto& floorTile : hallwayFloor) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, floorTile.getPosition());
			model = glm::rotate(model, glm::radians(floorTile.getRotation()), glm::vec3(1, 0, 0));
			model = glm::scale(model, floorTile.getScale());
			inGameStateShaderProgramme->setMat4("model", model);
			for (int i = 0;i < hallwayLights.size();++i) {
				std::string base = "hitByLight[" + std::to_string(i) + "]";
				inGameStateShaderProgramme->setBool(base, floorTile.getLitBy(i));
			}
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		// walls 
		glBindVertexArray(wall_vao);
		inGameStateShaderProgramme->setInt("useTextures", 2);

		for (auto& wall : hallwayWalls) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, wall.getPosition());
			model = glm::rotate(model, glm::radians(wall.getRotation()), glm::vec3(0, 1, 0));
			model = glm::scale(model, wall.getScale());
			inGameStateShaderProgramme->setMat4("model", model);
			for (int i = 0;i < hallwayLights.size();++i) {
				std::string base = "hitByLight[" + std::to_string(i) + "]";
				inGameStateShaderProgramme->setBool(base, wall.getLitBy(i));
			}
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		// doors
		glBindVertexArray(door_vao);
		inGameStateShaderProgramme->setInt("useTextures", 3);

		for (auto& door : hallwayDoors) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, door.getPosition());
			model = glm::translate(model, glm::vec3(-0.5f * 5.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(door._angle), glm::vec3(0, 1, 0));
			model = glm::translate(model, glm::vec3(0.5f * 5.0f, 0.0f, 0.0f));
			model = glm::scale(model, door.getScale());
			inGameStateShaderProgramme->setMat4("model", model);
			for (int i = 0;i < hallwayLights.size();++i) {
				std::string base = "hitByLight[" + std::to_string(i) + "]";
				inGameStateShaderProgramme->setBool(base, door.getLitBy(i));
			}
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glm::mat4 model = glm::mat4(1.0f);
		inGameStateShaderProgramme->setMat4("model", model);

		inGameStateShaderProgramme->setInt("useTextures", -1);
		inGameStateShaderProgramme->setVec4("objColor", glm::vec4(0.0, 1.0, 1.0, 1.0));

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
	}
	

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("Horror Maze");

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

	gameState = MAIN_MENU;
	glutMainLoop();

	delete inGameStateShaderProgramme;
	return 0;
}
