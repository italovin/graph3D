#ifndef RENDERER_H
#define RENDERER_H
#include "MeshRenderer.hpp"
#include "VertexArray.hpp"
#include "algorithm"

class Renderer{
private:
    unsigned int batchDefaultSize = 1024*1024;
    struct Buffer{
        GLuint name;
        int bufferSize;
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
    Batch CreateBatch(const std::vector<int> &buffersSizes, int indicesBufferSize, MeshTopology topology, const ShaderProgram &shader,
    const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
    void StartBatch(Batch &batch);
    void SetupBatchLayout(Batch &batch, MeshLayout &layout);
    void BufferSubData(Batch &batch, const std::vector<int> &offsets, const std::vector<MeshAttributeData> &attributesDatas);
    void BufferIndices(Batch &batch, int offset, const std::vector<unsigned int> &indices);
    void SetBatchIndicesInfo(Batch &batch, const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices);
public:
    void Prepare(std::vector<MeshRenderer> &meshRenderers);
    void Draw();
};
#endif