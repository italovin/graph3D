#include "VertexArray.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SceneObject{
private:
    ShaderProgram shader;
    VertexArray vao;
    std::vector<unsigned int> vbo;
    unsigned int ibo;
    unsigned int indicesCount;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
public:
    SceneObject();
    SceneObject(unsigned int n_vbo);
    void Startup(unsigned int n_vbo);
    void StartImmutableBufferStorage(unsigned int vbo_index, float data[], unsigned int dataSize);
    void StartMutableBufferStorage(unsigned int vbo_index, GLenum bufferType, float data[], unsigned int dataSize);
    void StartElementBufferStorage(unsigned int data[], unsigned int dataSize);
    void StartElementBufferStorage(unsigned short data[], unsigned int dataSize);
    void UpdateBufferData(unsigned int vbo_index, int offset, float data[], unsigned int dataSize);
    void AttachVertexBuffer(unsigned int vbo_index, unsigned int bindingPoint, int offset, int stride);
    void AttachElementBuffer();
    void SetAttribute(unsigned int attrib, unsigned int bindingPoint, int size, GLenum type, GLboolean normalized, int offset);
    void SetShader(ShaderProgram shaderProgram);
    void UpdateModel(const std::string &modelName);
    void UpdateView(const std::string &viewName, Camera camera);
    void UpdateProjection(const std::string &projectionName, unsigned int width, unsigned int height);
    void Bind();
    void Draw();
    void DrawLines();
    void DrawLines(GLenum type);
};
