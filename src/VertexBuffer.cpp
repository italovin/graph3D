#include <glad/glad.h>

class VertexBuffer{
private:
    unsigned int handle;
public:
    VertexBuffer(){
        glGenBuffers(1, &handle);
    }
    void Bind(){
        glBindBuffer(GL_ARRAY_BUFFER, handle);
    }
    void BufferData(float data[], unsigned int size){
        Bind();
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }
};