#include <GL/glew.h>

class VertexArray {
private:
    unsigned int handle;
public:
    VertexArray();
    unsigned int GetHandle();
    void Bind();
};