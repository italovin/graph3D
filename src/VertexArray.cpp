#include "VertexArray.hpp"

VertexArray::VertexArray(){
    glCreateVertexArrays(1, &handle);
}
GLuint VertexArray::GetHandle()
{
    return handle;
}
void VertexArray::Bind()
{
    glBindVertexArray(handle);
}