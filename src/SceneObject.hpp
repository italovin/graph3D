#include "VertexArray.hpp"
#include "ShaderProgram.hpp"

class SceneObject{
private:
    ShaderProgram shader;
    VertexArray vao;
    unsigned int * vbo;
    unsigned int ibo;
    unsigned int indicesCount;
public:
    SceneObject();
    SceneObject(unsigned int n_vbo);
    void Startup(unsigned int n_vbo);
    void StartImmutableBufferStorage(unsigned int vbo_index, float data[], unsigned int dataSize);
    void StartMutableBufferStorage(unsigned int vbo_index, GLenum bufferType, float data[], unsigned int dataSize);
    void StartElementBufferStorage(unsigned int data[], unsigned int dataSize);
    void UpdateBufferData(unsigned int vbo_index, int offset, float data[], unsigned int dataSize);
    void AttachVertexBuffer(unsigned int vbo_index, unsigned int bindingPoint, int offset, int stride);
    void AttachElementBuffer();
    void SetAttribute(unsigned int attrib, unsigned int bindingPoint, int size, GLenum type, GLboolean normalized, int offset);
    void SetShader(ShaderProgram shaderProgram);
    void Bind();
    void Draw();
};