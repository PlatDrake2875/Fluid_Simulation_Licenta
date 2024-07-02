#include "Mesh.h"

Mesh::Mesh(const GLfloat* vertices, const GLfloat* colors, const GLuint* indices, size_t vertexCount, size_t indexCount, GLenum drawMode)
    : vertexCount(vertexCount), indexCount(indexCount), drawMode(drawMode) {
    createBuffers(vertices, colors, indices);
}

Mesh::~Mesh() {
    destroyBuffers();
}

void Mesh::createBuffers(const GLfloat* vertices, const GLfloat* colors, const GLuint* indices) {
    // Create and bind a VAO (Vertex Array Object)
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    // Create a new buffer for vertices
    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Create a new buffer for colors (if colors is not null)
    if (colors != nullptr) {
        glGenBuffers(1, &ColorBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    // Create a new buffer for indices
    glGenBuffers(1, &EboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void Mesh::updateVertices(const GLfloat* vertices, size_t vertexCount) {
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * 3 * sizeof(GLfloat), vertices);
}

void Mesh::destroyBuffers() {
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);
    glDeleteBuffers(1, &EboId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

void Mesh::render() const {
    glBindVertexArray(VaoId);
    glDrawElements(drawMode, indexCount, GL_UNSIGNED_INT, 0);
}
