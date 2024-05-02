#ifndef RENDERER_H
#define RENDERER_H
#include "MeshRenderer.hpp"
#include "VertexArray.hpp"
#include "algorithm"

class Renderer{
private:
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
    };
    std::vector<Batch> batches;
    Batch CreateBatch(const std::vector<unsigned int> &buffersSizes, const std::vector<int> &strides,
    unsigned int indicesBufferSize, MeshTopology topology, const ShaderProgram &shader,
    const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
    GLenum GetDrawMode(MeshTopology topology);
    void StartBatch(Batch &batch);
    void SetupBatchLayout(Batch &batch, MeshLayout &layout);
    void BufferSubData(Batch &batch, const std::vector<unsigned int> &offsets, const std::vector<MeshAttributeData> &attributesDatas);
    void BufferIndices(Batch &batch, unsigned int offset, const std::vector<unsigned int> &indices);
    void SetBatchIndicesInfo(Batch &batch, const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
public:
    void Prepare(std::vector<MeshRenderer> &&meshRenderers);
    void Draw();
    int GetBatchesCount();
};
#endif