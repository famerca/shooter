#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

class Shader
{
public:
    Shader();
    ~Shader();

    void CreateFromFiles(const char* vertexShaderPath, const char* fragmentShaderPath);
    void UseShader();
    void ClearShader();
    GLuint GetProgramID() const { return shaderID; }

private:
    GLuint shaderID;

    std::string ReadShaderFile(const char* filePath);
    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint program, const char* shaderCode, GLenum shaderType);
};