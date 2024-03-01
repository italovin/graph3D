#include "VertexArray.hpp"

VertexArray::VertexArray(){
    glCreateVertexArrays(1, &handle);
}
unsigned int VertexArray::GetHandle()
{
    return handle;
}
void VertexArray::Bind()
{
    glBindVertexArray(handle);
}