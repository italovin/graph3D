#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
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
    void SetBufferData(unsigned int vbo_index, GLenum bufferType, float data[], unsigned int dataSize);
    void SetElementBufferData(unsigned int data[], unsigned int dataSize);
    void SetAttribute(unsigned int attrib, int size, GLenum type, GLboolean normalized, int stride, unsigned int vbo_index);
    void SetShader(ShaderProgram shaderProgram);
    void Bind();
    void Draw();
};