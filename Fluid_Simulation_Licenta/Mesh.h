#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <iostream>

class Mesh {
public:
    Mesh(const GLfloat* vertices, const GLfloat* colors, const GLuint* indices, size_t vertexCount, size_t indexCount, GLenum drawMode = GL_LINES);
    ~Mesh();
    void updateVertices(const GLfloat* vertices, size_t vertexCount);
    void render() const;

private:
    GLuint VaoId;
    GLuint VboId;
    GLuint ColorBufferId;
    GLuint EboId;
    size_t vertexCount;
    size_t indexCount;
    GLenum drawMode;

    void createBuffers(const GLfloat* vertices, const GLfloat* colors, const GLuint* indices);
    void destroyBuffers();
};

#endif // MESH_H
