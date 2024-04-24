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
    };
    struct Batch{
        VertexArray vao;
        std::vector<Buffer> buffers;
        Buffer indiceBuffer;
        std::vector<int> indicesCounts;
        std::vector<int> baseVertices;
        MeshTopology topology;
        ShaderProgram shader;
    };
    std::vector<Batch> batches;
    Batch CreateBatch(const std::vector<unsigned int> &buffersSizes, const std::vector<int> &strides,
    unsigned int indicesBufferSize, MeshTopology topology, const ShaderProgram &shader,
    const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
    void StartBatch(Batch &batch);
    void SetupBatchLayout(Batch &batch, MeshLayout &layout);
    void BufferSubData(Batch &batch, const std::vector<unsigned int> &offsets, const std::vector<MeshAttributeData> &attributesDatas);
    void BufferIndices(Batch &batch, unsigned int offset, const std::vector<unsigned int> &indices);
    void SetBatchIndicesInfo(Batch &batch, const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
public:
    void Prepare(std::vector<MeshRenderer> &meshRenderers);
    void Draw();
    int GetBatchesCount();
};
#endif