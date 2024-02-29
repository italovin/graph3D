#include "VertexArray.hpp"

VertexArray::VertexArray(){
    glGenVertexArrays(1, &handle);
}
unsigned int VertexArray::GetHandle()
{
    return handle;
}
void VertexArray::Bind()
{
    glBindVertexArray(handle);
}