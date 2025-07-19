#pragma once

#include <GL/glew.h>

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void CreateMesh(GLfloat* vertices, unsigned int numOfVertices);
    void RenderMesh();
    void ClearMesh();

private:
    GLuint VAO, VBO;
    unsigned int vertexCount;
};