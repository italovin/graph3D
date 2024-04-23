#include "SceneObject.hpp"

SceneObject::SceneObject(){
    indicesCount = 0;
    vao = VertexArray();
    Bind();
}
SceneObject::SceneObject(unsigned int n_buffers){
    indicesCount = 0;
    vao = VertexArray();
    //Bind();
    Startup(n_buffers);
}

void SceneObject::Startup(unsigned int n_buffers)
{
    buffers = std::vector<GLuint>(n_buffers);
    glCreateBuffers(n_buffers, buffers.data());
}

GLuint SceneObject::GetBuffer(int index){
    return buffers[index];
}

void SceneObject::BufferStorageEmpty(GLuint buffers_index, unsigned int dataSize){
    glNamedBufferStorage(buffers[buffers_index], dataSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void SceneObject::BufferStorage(GLuint buffers_index, GLfloat data[], unsigned int dataSize){
    glNamedBufferStorage(buffers[buffers_index], dataSize, data, GL_DYNAMIC_STORAGE_BIT);
}

void SceneObject::BufferStorage(GLuint buffers_index, const std::vector<GLfloat> &data)
{
    unsigned int dataSize = sizeof(float)*data.size();
    glNamedBufferStorage(buffers[buffers_index], dataSize, data.data(), GL_DYNAMIC_STORAGE_BIT);
}

void SceneObject::BufferStorage(GLuint buffers_index, GLuint data[], unsigned int dataSize){
    glNamedBufferStorage(buffers[buffers_index], dataSize, data, GL_DYNAMIC_STORAGE_BIT);
}

void SceneObject::BufferStorage(GLuint buffers_index, const std::vector<GLuint> &data)
{
    unsigned int dataSize = sizeof(GLuint)*data.size();
    glNamedBufferStorage(buffers[buffers_index], dataSize, data.data(), GL_DYNAMIC_STORAGE_BIT);
}

void SceneObject::BufferStorage(GLuint buffers_index, GLushort data[], unsigned int dataSize){
    glNamedBufferStorage(buffers[buffers_index], dataSize, data, GL_DYNAMIC_STORAGE_BIT);
}

void SceneObject::BufferStorage(GLuint buffers_index, const std::vector<GLushort> &data)
{
    unsigned int dataSize = sizeof(GLushort)*data.size();
    glNamedBufferStorage(buffers[buffers_index], dataSize, data.data(), GL_DYNAMIC_STORAGE_BIT);
}

void SceneObject::MutableBufferStorage(GLuint buffers_index, GLenum bufferType, GLfloat data[], unsigned int dataSize)
{
    if(bufferType == GL_ARRAY_BUFFER){
        glNamedBufferData(buffers[buffers_index], dataSize, data, GL_STATIC_DRAW);
    }
}

void SceneObject::MutableBufferStorage(GLuint buffers_index, GLenum bufferType, const std::vector<GLfloat> &data)
{
    unsigned int dataSize = sizeof(float)*data.size();
    if(bufferType == GL_ARRAY_BUFFER){
        glNamedBufferData(buffers[buffers_index], dataSize, data.data(), GL_STATIC_DRAW);
    }
}

void SceneObject::BufferSubData(GLuint buffers_index, int offset, GLfloat data[], unsigned int dataSize){
    glNamedBufferSubData(buffers[buffers_index], offset, dataSize, data);
}

void SceneObject::BufferSubData(GLuint buffers_index, int offset, const std::vector<GLfloat> &data){
    GLsizeiptr dataSize = sizeof(GLfloat)*data.size();
    glNamedBufferSubData(buffers[buffers_index], offset, dataSize, data.data());
}

void SceneObject::BufferSubData(GLuint buffers_index, int offset, GLuint data[], unsigned int dataSize){
    glNamedBufferSubData(buffers[buffers_index], offset, dataSize, data);
}

void SceneObject::BufferSubData(GLuint buffers_index, int offset, const std::vector<GLuint> &data){
    GLsizeiptr dataSize = sizeof(GLuint)*data.size();
    glNamedBufferSubData(buffers[buffers_index], offset, dataSize, data.data());
}

void SceneObject::AttachVertexBuffer(GLuint buffers_index, GLuint bindingPoint, int offset, int stride)
{
    glVertexArrayVertexBuffer(vao.GetHandle(), bindingPoint, buffers[buffers_index], offset, stride);
}
void SceneObject::AttachElementBuffer(GLuint buffers_index){
    glVertexArrayElementBuffer(vao.GetHandle(), buffers[buffers_index]);
}

void SceneObject::SetIndicesInfo(unsigned int indicesCount, unsigned int indicesOffset){
    this->indicesCount = indicesCount;
    this->indicesOffset = indicesOffset;
}

void SceneObject::SetAttribute(GLuint attrib, GLuint bindingPoint, int size, GLenum type, GLboolean normalized, int offset)
{
    glVertexArrayAttribFormat(vao.GetHandle(), attrib, size, type, normalized, offset);
    glEnableVertexArrayAttrib(vao.GetHandle(), attrib);
    glVertexArrayAttribBinding(vao.GetHandle(), attrib, bindingPoint);
}
ShaderProgram SceneObject::Shader() const{
    return shader;
}
void SceneObject::SetShader(ShaderProgram shaderProgram){
    shader = shaderProgram;
}

void SceneObject::UpdateModel(const std::string &modelName){
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 scl = glm::scale(model, transform.scale);
    glm::mat4 rot = glm::mat4_cast(transform.rotation);
    glm::mat4 trn = glm::translate(model, transform.position);
    model = trn*rot*scl;
    shader.SetMat4Float(modelName, model);
}

glm::mat4 SceneObject::GetModelMatrix() const{
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 scl = glm::scale(model, transform.scale);
    glm::mat4 rot = glm::mat4_cast(transform.rotation);
    glm::mat4 trn = glm::translate(model, transform.position);
    model = trn*rot*scl;
    return model;
}

void SceneObject::Bind()
{
    vao.Bind();
}

void SceneObject::Draw(){
    shader.Use();
    Bind();
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 
    reinterpret_cast<GLvoid *>(static_cast<uintptr_t>(indicesOffset)));
}

void SceneObject::DrawLines(){
    shader.Use();
    Bind();
    glDrawElements(GL_LINES, indicesCount, GL_UNSIGNED_INT,
    reinterpret_cast<GLvoid *>(static_cast<uintptr_t>(indicesOffset)));
}

void SceneObject::DrawLines(GLenum type){
    shader.Use();
    Bind();
    glDrawElements(GL_LINES, indicesCount, type, 
    reinterpret_cast<GLvoid *>(static_cast<uintptr_t>(indicesOffset)));
}
