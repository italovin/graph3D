#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer(){
    glGenBuffers(1, &handle);
}
void VertexBuffer::Bind(){
    glBindBuffer(GL_ARRAY_BUFFER, handle);
}
void VertexBuffer::BufferData(float data[], unsigned int size){
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
