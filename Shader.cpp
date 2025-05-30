#include <filesystem>
#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::cout << "Current Path: " << std::filesystem::current_path() << std::endl;

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    PID = glCreateProgram();
    glAttachShader(PID, vertex);
    glAttachShader(PID, fragment);
    glLinkProgram(PID);
    // print linking errors if any
    glGetProgramiv(PID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(PID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(PID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(PID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(PID, name.c_str()), value);
}
void Shader::setFloat( const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(PID, name.c_str()), value); 
}
void Shader::setMat4(const std::string& name, glm::mat4 matrix) const 
{
    glUniformMatrix4fv(glGetUniformLocation(PID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::setVec4(const std::string& name, glm::vec4 vec) {
    glUniform4fv(glGetUniformLocation(PID, name.c_str()), 1, glm::value_ptr(vec));
}
void Shader::setVec3(const std::string& name, glm::vec3 vec) {
    glUniform3fv(glGetUniformLocation(PID, name.c_str()), 1, glm::value_ptr(vec));
}
int Shader::getLocation(locationType type, const std::string& name) {
    switch (type) {
    case SHADER_UNIFORM:
        return glGetUniformLocation(PID, name.c_str());
        break;
    default:
        return -1;
    }
}