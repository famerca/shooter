#include "Shader.h"

Shader::Shader() : shaderID(0)
{
}

Shader::~Shader()
{
    ClearShader();
}

std::string Shader::ReadShaderFile(const char* filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open())
    {
        std::cerr << "Failed to read " << filePath << "! File doesn't exist.\n";
        return "";
    }

    std::stringstream ss;
    ss << fileStream.rdbuf();
    content = ss.str();
    fileStream.close();
    return content;
}

void Shader::CreateFromFiles(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    std::string vertexString = ReadShaderFile(vertexShaderPath);
    std::string fragmentString = ReadShaderFile(fragmentShaderPath);

    if (vertexString.empty() || fragmentString.empty())
    {
        std::cerr << "One or more shader files could not be read. Aborting shader creation.\n";
        return;
    }

    CompileShader(vertexString.c_str(), fragmentString.c_str());
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
    shaderID = glCreateProgram();

    if (!shaderID)
    {
        std::cerr << "Error creating shader program!\n";
        return;
    }

    AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cerr << "Error linking program: " << eLog << "\n";
        return;
    }

    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cerr << "Error validating program: " << eLog << "\n";
        return;
    }
}

void Shader::AddShader(GLuint program, const char* shaderCode, GLenum shaderType)
{
    GLuint currentShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = (GLint)strlen(shaderCode); // Use strlen as shaderCode is C-string

    glShaderSource(currentShader, 1, theCode, codeLength);
    glCompileShader(currentShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(currentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(currentShader, sizeof(eLog), NULL, eLog);
        std::cerr << "Error compiling the " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << eLog << "\n";
        return;
    }

    glAttachShader(program, currentShader);
}

void Shader::UseShader()
{
    glUseProgram(shaderID);
}

void Shader::ClearShader()
{
    if (shaderID != 0)
    {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }
}