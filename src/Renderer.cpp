#include "Renderer.hpp"

void Renderer::PushNewLayout(MeshRenderer &meshRenderer){
    MeshLayout rendererLayout = meshRenderer.GetMesh().GetLayout();
    auto it = std::find(layouts.begin(), layouts.end(), rendererLayout);
    if(it != layouts.end()){
        layouts.push_back(rendererLayout);
        vaos.push_back(VertexArray());
    }
}

Renderer::Batch Renderer::CreateBatch(const std::vector<int> &buffersSizes){
    std::vector<GLuint> newBuffers = std::vector<GLuint>(buffersSizes.size());
    glCreateBuffers(buffersSizes.size(), newBuffers.data());
    VertexArray newVao = VertexArray();
    Batch newBatch;
    for(int i = 0; i < buffersSizes.size(); i++){
        Buffer buffer = {.name = newBuffers[i], .bufferSize = buffersSizes[i]};
        newBatch.buffers.push_back(buffer);
    }
    newBatch.vao = newVao;
    batches.push_back(newBatch);
    return newBatch;
}

void Renderer::StartBatch(Batch &batch){
    int bindingPoint = 0;
    for(auto &&buffer : batch.buffers){
        glNamedBufferStorage(buffer.name, buffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(batch.vao.GetHandle(), bindingPoint++, buffer.name, 0, 0);
    }
    
}

void Renderer::SetupBatchLayout(Batch &batch, MeshLayout &layout){
    for(auto &&attribute : layout.attributes){
        int bindingPoint = attribute.index;
        GLenum type;
        GLboolean normalized;
        switch(attribute.type){
            case MeshFloat32:
                type = GL_FLOAT;
                break;
            case MeshInt32:
                type = GL_INT;
                break;
        }
        normalized = attribute.normalized ? GL_TRUE : GL_FALSE;
        glVertexArrayAttribFormat(batch.vao.GetHandle(), attribute.index, attribute.size, type, normalized, 0);
        glEnableVertexAttribArray(attribute.index);
        glVertexArrayAttribBinding(batch.vao.GetHandle(), attribute.index, bindingPoint);
    }
}

void Renderer::Prepare(const std::vector<MeshRenderer> &meshRenderers){
    auto comparationFunction = [](MeshRenderer a, MeshRenderer b){
        return a.GetShader().GetHandle() < b.GetShader().GetHandle();
    };
    std::sort(meshRenderers.begin(), meshRenderers.end(), comparationFunction);
    std::vector<std::vector<MeshRenderer>> meshRenderersGroups;

    decltype(meshRenderers.end()) upper;

    for(auto lower = meshRenderers.begin(); lower != meshRenderers.end(); lower = upper)
    {
        // get the upper position of all elements with the same ID
        upper = std::upper_bound(meshRenderers.begin(), meshRenderers.end(), *lower, comparationFunction);

        // add those elements as a group to the output vector
        meshRenderersGroups.emplace_back(lower, upper);
    }
    int currentBatch = 0;
    for (auto &&group : meshRenderersGroups)
    {
        if(std::all_of(group.begin(), group.end(), [&](MeshRenderer m){
            return m.GetMesh().GetLayout() == group[0].GetMesh().GetLayout();  
        })){
            int batchSize = 0;
            for(auto &&renderer : group){
                Mesh mesh = renderer.GetMesh();
                batchSize += mesh.GetMeshDataSize() + mesh.GetIndicesSize();
            }
            Batch createdBatch = CreateBatch(batchSize);
            MeshLayout layout = group[0].GetMesh().GetLayout();
            StartBatch(createdBatch, layout);
            SetupBatchLayout(createdBatch, layout);
            glMultiDrawElementsBaseVertex()
        }
    }
}