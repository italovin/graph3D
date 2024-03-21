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
    std::vector<GLuint> vbo;
    GLuint ibo;
    unsigned int indicesCount;
public:
    Transform transform;
    SceneObject();
    SceneObject(unsigned int n_vbo);
    void Startup(unsigned int n_vbo);
    void StartImmutableBufferStorage(GLuint vbo_index, float data[], unsigned int dataSize);
    void StartImmutableBufferStorage(GLuint vbo_index, const std::vector<float> &data);
    void StartMutableBufferStorage(GLuint vbo_index, GLenum bufferType, float data[], unsigned int dataSize);
    void StartMutableBufferStorage(GLuint vbo_index, GLenum bufferType, const std::vector<float> &data);
    void StartElementBufferStorage(unsigned int data[], unsigned int dataSize);
    void StartElementBufferStorage(const std::vector<unsigned int> &data);
    void StartElementBufferStorage(unsigned short data[], unsigned int dataSize);
    void StartElementBufferStorage(const std::vector<unsigned short> &data);
    void UpdateBufferData(GLuint vbo_index, int offset, float data[], unsigned int dataSize);
    void AttachVertexBuffer(GLuint vbo_index, GLuint bindingPoint, int offset, int stride);
    void AttachElementBuffer();
    void SetAttribute(GLuint attrib, GLuint bindingPoint, int size, GLenum type, GLboolean normalized, int offset);
    void SetShader(ShaderProgram shaderProgram);
    void UpdateModel(const std::string &modelName);
    void UpdateView(const std::string &viewName, Camera camera);
    void UpdateProjection(const std::string &projectionName, unsigned int width, unsigned int height);
    void Bind();
    void Draw();
    void DrawLines();
    void DrawLines(GLenum type);
};
