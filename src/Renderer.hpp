#include "MeshRenderer.hpp"
#include "VertexArray.hpp"
#include "algorithm"

class Renderer{
private:
    std::vector<MeshLayout> layouts;
    void PushNewLayout(MeshRenderer &meshRenderer);
    unsigned int batchDefaultSize = 1024*1024;
    struct Buffer{
        GLuint name;
        int bufferSize;
    };
    struct Batch{
        VertexArray vao;
        std::vector<Buffer> buffers;
    };
    std::vector<Batch> batches;
    Batch CreateBatch(const std::vector<int> &buffersSizes);
    void StartBatch(Batch &batch);
public:
    void SetupBatchLayout(Batch &batch, MeshLayout &layout);
    void Prepare(const std::vector<MeshRenderer> &meshRenderers);
};