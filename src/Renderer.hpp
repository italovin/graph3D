#ifndef RENDERER_H
#define RENDERER_H
#include "Base.hpp"
#include "MeshRenderer.hpp"
#include "VertexArray.hpp"
#include "WorldObject.hpp"
#include "Camera.hpp"
#include "algorithm"

class Renderer{
private:
    
    Camera* camera;
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    int mvpDefaultBindingPoint;
    struct Buffer{
        GLuint name;
        unsigned int bufferSize;
        int stride;
        int bindingPoint;

        Buffer() = default;
        Buffer(GLuint name, unsigned int bufferSize, int stride, int bindingPoint):
        name(name), bufferSize(bufferSize), stride(stride), bindingPoint(bindingPoint){}
    };
    struct Batch{
        VertexArray vao;
        std::vector<Buffer> buffers;
        Buffer indiceBuffer;
        std::vector<int> indicesCounts;
        std::vector<int> baseVertices;
        MeshTopology topology;
        ShaderProgram shader;
        GLenum mode; //must be equivalent to topology
        std::vector<int> indicesOffsetInBuffer; //must be a vector with zeros
        GLenum indicesType;
        Buffer mvpUniformBuffer;
        std::vector<Ref<Transform>> transforms;
        std::vector<glm::mat4> mvps;
    };
    std::vector<Batch> batches;
    Batch &CreateBatch(const std::vector<unsigned int> &buffersSizes, const std::vector<int> &strides,
    unsigned int indicesBufferSize, MeshTopology topology, MeshIndexType type, const ShaderProgram &shader,
    const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
    Batch &CreateBatchWithMVP(const std::vector<unsigned int> &buffersSizes, const std::vector<int> &strides,
    unsigned int indicesBufferSize, unsigned int mvpBufferSize, const std::vector<Ref<Transform>> &transforms, 
    MeshTopology topology, MeshIndexType type, const ShaderProgram &shader,
    const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
    GLenum GetDrawMode(MeshTopology topology);
    GLenum GetIndicesType(MeshIndexType type);
    void StartBatch(Batch &batch);
    void StartMVPBuffer(Batch &batch);
    void SetupBatchLayout(Batch &batch, MeshLayout &layout);
    void BufferSubData(Batch &batch, const std::vector<unsigned int> &offsets, const std::vector<MeshAttributeData> &attributesDatas);
    void BufferIndices(Batch &batch, unsigned int offset, const MeshIndexData &indicesData);
    void BufferSubDataMVPs(Batch &batch);
    void SetBatchIndicesInfo(Batch &batch, const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
public:
    void SetMVPBindingPoint(int bindingPoint);
    void SetProjection(const glm::mat4 &projectionMatrix);
    void SetMainCamera(Camera *mainCamera);
    void Prepare(std::vector<MeshRenderer> &&meshRenderers);
    void Prepare(std::vector<WorldObject> &&worldObjects);
    void Draw();
    int GetBatchesCount();
};
#endif