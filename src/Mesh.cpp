#include "Mesh.h"
#include <iostream>

Mesh::Mesh() : VAO(0), VBO(0), vertexCount(0)
{
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int numOfVertices)
{
    vertexCount = numOfVertices;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // Layout (location = 0)
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindVertexArray(0);             // Unbind VAO
}

void Mesh::RenderMesh()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount / 3); // For triangles, count is vertices / 3 (e.g., 9 vertices / 3 = 3 points for 1 triangle)
    glBindVertexArray(0); // Unbind VAO
}

void Mesh::ClearMesh()
{
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    vertexCount = 0;
}

Mesh::~Mesh()
{
    ClearMesh();
}