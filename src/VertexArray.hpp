#include <glad/glad.h>

class VertexArray {
private:
    unsigned int handle;
public:
    VertexArray();
    unsigned int GetHandle();
    void Bind();
};