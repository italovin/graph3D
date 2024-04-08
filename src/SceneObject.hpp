#include "VertexArray.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

class SceneObject{
private:
    ShaderProgram shader;
    VertexArray vao;
    std::vector<GLuint> buffers;
    unsigned int indicesOffset = 0;
    unsigned int indicesCount = 0;
public:
    Transform transform;
    SceneObject();
    SceneObject(unsigned int n_buffers);
    void Startup(unsigned int n_buffers);
    GLuint GetBuffer(int index);
    //Create empty immutable buffer storage
    void BufferStorageEmpty(GLuint buffers_index, unsigned int dataSize);
    //Create immutable buffer storage
    void BufferStorage(GLuint buffers_index, GLfloat data[], unsigned int dataSize);
    void BufferStorage(GLuint buffers_index, const std::vector<GLfloat> &data);
    void BufferStorage(GLuint buffers_index, GLuint data[], unsigned int dataSize);
    void BufferStorage(GLuint buffers_index, const std::vector<GLuint> &data);
    void BufferStorage(GLuint buffers_index, GLushort data[], unsigned int dataSize);
    void BufferStorage(GLuint buffers_index, const std::vector<GLushort> &data);
    //Create mutable buffer storage
    void MutableBufferStorage(GLuint buffers_index, GLenum bufferType, GLfloat data[], unsigned int dataSize);
    void MutableBufferStorage(GLuint buffers_index, GLenum bufferType, const std::vector<GLfloat> &data);
    void BufferSubData(GLuint buffers_index, int offset, GLfloat data[], unsigned int dataSize);
    void BufferSubData(GLuint buffers_index, int offset, const std::vector<GLfloat> &data);
    void BufferSubData(GLuint buffers_index, int offset, GLuint data[], unsigned int dataSize);
    void BufferSubData(GLuint buffers_index, int offset, const std::vector<GLuint> &data);
    void AttachVertexBuffer(GLuint buffers_index, GLuint bindingPoint, int offset, int stride);
    void AttachElementBuffer(GLuint buffers_index);
    void SetIndicesInfo(unsigned int indicesCount, unsigned int indicesOffset);
    void SetAttribute(GLuint attrib, GLuint bindingPoint, int size, GLenum type, GLboolean normalized, int offset);
    ShaderProgram Shader() const;
    void SetShader(ShaderProgram shaderProgram);
    void UpdateModel(const std::string &modelName);
    glm::mat4 GetModelMatrix() const;
    void Bind();
    void Draw();
    void DrawLines();
    void DrawLines(GLenum type);
};
