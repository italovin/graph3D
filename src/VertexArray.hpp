#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
#include <GL/glew.h>

class VertexArray {
private:
    GLuint handle;
public:
    VertexArray();
    GLuint GetHandle();
    void Bind();
};
#endif