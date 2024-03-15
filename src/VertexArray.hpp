#include <GL/glew.h>

class VertexArray {
private:
    GLuint handle;
public:
    VertexArray();
    GLuint GetHandle();
    void Bind();
};