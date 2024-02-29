#include "SceneObject.hpp"

SceneObject::SceneObject(){
    indicesCount = 0;
    Bind();
}
SceneObject::SceneObject(unsigned int n_vbo){
    indicesCount = 0;
    Bind();
    Startup(n_vbo);
}

void SceneObject::Startup(unsigned int n_vbo)
{
    vbo = new unsigned int[n_vbo];
    glGenBuffers(n_vbo, vbo);
    glGenBuffers(1, &ibo);
}

void SceneObject::SetBufferData(unsigned int vbo_index, GLenum bufferType, float data[], unsigned int dataSize){
    if(bufferType == GL_ARRAY_BUFFER){
        glBindBuffer(GL_ARRAY_BUFFER, vbo[vbo_index]);
        glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
    }
}

void SceneObject::SetElementBufferData(unsigned int data[], unsigned int dataSize)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
    indicesCount = dataSize / sizeof(unsigned int);
}

void SceneObject::SetAttribute(unsigned int attrib, int size, GLenum type, GLboolean normalized, int offset, unsigned int vbo_index)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vbo_index]);
    glVertexAttribPointer(attrib, size, type, normalized, size*sizeof(type), (void *)offset);
    glEnableVertexAttribArray(attrib);
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