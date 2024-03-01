#include "SceneObject.hpp"

SceneObject::SceneObject(){
    indicesCount = 0;
    vao = VertexArray();
    Bind();
}
SceneObject::SceneObject(unsigned int n_vbo){
    indicesCount = 0;
    vao = VertexArray();
    Bind();
    Startup(n_vbo);
}

void SceneObject::Startup(unsigned int n_vbo)
{
    vbo = new unsigned int[n_vbo];
    glCreateBuffers(n_vbo, vbo);
    glCreateBuffers(1, &ibo);
}

void SceneObject::SetBufferData(unsigned int vbo_index, GLenum bufferType, float data[], unsigned int dataSize){
    if(bufferType == GL_ARRAY_BUFFER){
        glNamedBufferStorage(vbo[vbo_index], dataSize, data, GL_DYNAMIC_STORAGE_BIT);
    }
}

void SceneObject::SetElementBufferData(unsigned int data[], unsigned int dataSize)
{
    glNamedBufferStorage(ibo, dataSize, data, GL_DYNAMIC_STORAGE_BIT);
    indicesCount = dataSize / sizeof(unsigned int);
}

void SceneObject::AttachVertexBuffer(unsigned int vbo_index, unsigned int bindingPoint, int offset, int stride)
{
    glVertexArrayVertexBuffer(vao.GetHandle(), bindingPoint, vbo[vbo_index], offset, stride);
}
void SceneObject::AttachElementBuffer(){
    glVertexArrayElementBuffer(vao.GetHandle(), ibo);
}

void SceneObject::SetAttribute(unsigned int attrib, unsigned int bindingPoint, int size, GLenum type, GLboolean normalized, int offset)
{
    glVertexArrayAttribFormat(vao.GetHandle(), attrib, size, type, normalized, offset);
    glEnableVertexAttribArray(attrib);
    glVertexArrayAttribBinding(vao.GetHandle(), attrib, bindingPoint);
}

void SceneObject::SetShader(ShaderProgram shaderProgram){
    shader = shaderProgram;
}

void SceneObject::Bind()
{
    vao.Bind();
}

void SceneObject::Draw(){
    shader.Use();
    Bind();
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}