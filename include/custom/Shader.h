#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stack>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

enum locationType{
	SHADER_UNIFORM
};

class Shader {
public:
	unsigned int PID;	// program ID

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();			// use/activate shader

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4 matrix) const;
	void setVec4(const std::string& name, glm::vec4 vec);
	void setVec3(const std::string& name, glm::vec3 vec);
	int getLocation(locationType, const std::string& name);
};

#endif