#ifndef RENDERER_H
#define RENDERER_H
#include "Base.hpp"
#include "VertexArray.hpp"
#include "Camera.hpp"
#include "Components.hpp"
#include "Entity.hpp"
#include <algorithm>
#include <boolinq/boolinq.h>

class Renderer{
private:
    std::function<void(GLuint)> auxBindEBOFunction = [](GLuint ebo){}; //This helps fix the DSA EBO binding for older GL versions
    void AuxBindEboFunction(GLuint ebo);
    Camera* camera;
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    int mvpDefaultBindingPoint;
    int modelsIndicesDefaultLocation;
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
        std::vector<Buffer> attributesBuffers; //For while, each buffer is a attribute
        Buffer indiceBuffer;
        std::vector<int> indicesCounts;
        std::vector<int> baseVertices;
        MeshTopology topology;
        Shader shader;
        GLenum mode; //must be equivalent to topology
        std::vector<int> indicesOffsetInBuffer; //must be a vector with zeros
        GLenum indicesType;
        Buffer modelsIndicesBuffer;
        Buffer mvpUniformBuffer;
        std::vector<std::reference_wrapper<TransformComponent>> transforms;
        std::vector<glm::mat4> mvps;
    };
    std::vector<Batch> batches;
    Batch &CreateBatch(const std::vector<unsigned int> &buffersSizes, const std::vector<int> &strides,
    unsigned int indicesBufferSize, MeshTopology topology, MeshIndexType type, const Shader &shader,
    const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
    Batch &CreateBatchWithMVP(const std::vector<unsigned int> &attributesBuffersSizes, const std::vector<int> &strides,
    unsigned int indicesBufferSize, unsigned int mvpBufferSize, unsigned int modelsIndicesBufferSize,
    const std::vector<std::reference_wrapper<TransformComponent>> &transforms, 
    MeshTopology topology, MeshIndexType type, const Shader &shader,
    const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
    GLenum GetDrawMode(MeshTopology topology);
    GLenum GetIndicesType(MeshIndexType type);
    void StartBatch(Batch &batch);
    void StartModelsIndicesBuffer(Batch &batch);
    void StartMVPBuffer(Batch &batch);
    void SetupBatchLayout(Batch &batch, MeshLayout &layout);
    void SetupModelsIndicesAttribute(Batch &batch);
    void BufferSubData(Batch &batch, const std::vector<unsigned int> &offsets, const std::vector<MeshAttributeData> &attributesDatas);
    void BufferIndices(Batch &batch, unsigned int offset, const MeshIndexData &indicesData);
    void BufferModelsIndices(Batch &batch, unsigned int offset, int index, unsigned int count);
    void BufferSubDataMVPs(Batch &batch);
    void SetBatchIndicesInfo(Batch &batch, const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
public:
    //GLSL version integer compatible
    void SetupDrawEnvironment(int version);
    void SetMVPBindingPoint(int bindingPoint);
    void SetModelsIndicesLocation(int location);
    void SetProjection(const glm::mat4 &projectionMatrix);
    void SetMainCamera(Camera *mainCamera);
    void Prepare(entt::registry &registry);
    void Draw();
    int GetBatchesCount();
};
#endif