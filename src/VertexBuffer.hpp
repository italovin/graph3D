#include <glad/glad.h>

class VertexBuffer {
private:
    unsigned int handle;
public:
    VertexBuffer();
    unsigned int GetHandle();
    void Bind();
    void BufferData(float data[], unsigned int size);
};