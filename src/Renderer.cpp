#include "Renderer.hpp"

Renderer::Batch Renderer::CreateBatch(const std::vector<int> &buffersSizes, int indicesBufferSize, MeshTopology topology, const ShaderProgram &shader,
const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices){
    std::vector<GLuint> newBuffers = std::vector<GLuint>(buffersSizes.size());
    glCreateBuffers(buffersSizes.size(), newBuffers.data());
    VertexArray newVao = VertexArray();
    Batch newBatch;
    int bindingPointIt = 0;
    for(int i = 0; i < buffersSizes.size(); i++){
        Buffer buffer = {.name = newBuffers[i], .bufferSize = buffersSizes[i], .bindingPoint = bindingPointIt++};
        newBatch.buffers.push_back(buffer);
    }
    GLuint newIndicesBuffer;
    glCreateBuffers(1, &newIndicesBuffer);
    Buffer indicesBuffer = {.name = newIndicesBuffer, .bufferSize = indicesBufferSize};
    newBatch.indiceBuffer = indicesBuffer;
    newBatch.vao = newVao;
    newBatch.topology = topology;
    newBatch.shader = shader;
    newBatch.indicesCounts = indicesCounts;
    newBatch.baseVertices = baseVertices;
    batches.push_back(newBatch);
    return newBatch;
}

void Renderer::StartBatch(Batch &batch){
    for(auto &&buffer : batch.buffers){
        glNamedBufferStorage(buffer.name, buffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(batch.vao.GetHandle(), buffer.bindingPoint, buffer.name, 0, 0);
    }
    glNamedBufferStorage(batch.indiceBuffer.name, batch.indiceBuffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(batch.vao.GetHandle(), batch.indiceBuffer.name);
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
        glEnableVertexArrayAttrib(batch.vao.GetHandle(), attribute.index);
        glVertexArrayAttribBinding(batch.vao.GetHandle(), attribute.index, bindingPoint);
    }
}

void Renderer::BufferSubData(Batch &batch, const std::vector<int> &offsets, const std::vector<MeshAttributeData> &attributesDatas){
    int index = 0;
    for(auto &&buffer : batch.buffers){
        glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, attributesDatas[index].data.data());
        index++;
    }
}

void Renderer::BufferIndices(Batch &batch, int offset, const std::vector<unsigned int> &indices){
    glNamedBufferSubData(batch.indiceBuffer.name, offset, sizeof(unsigned int)*indices.size(), indices.data());
}

void Renderer::SetBatchIndicesInfo(Batch &batch, const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices){
    batch.indicesCounts = indicesCounts;
    batch.baseVertices = baseVertices;
}

void Renderer::Prepare(std::vector<MeshRenderer> &meshRenderers){
    auto comparationFunction = [](const MeshRenderer &a, const MeshRenderer &b){
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
    for (auto &&group : meshRenderersGroups)
    {
        if(std::all_of(group.begin(), group.end(), [&](MeshRenderer m){
            return (m.GetMesh().GetLayout() == group[0].GetMesh().GetLayout())
            && (m.GetMesh().GetTopology() == group[0].GetMesh().GetTopology());  
        })){
            std::vector<int> buffersSizes = std::vector<int>(group[0].GetMesh().GetAttributesCount());
            std::vector<std::vector<int>> dataOffsetsMatrix = std::vector<std::vector<int>>(group.size());
            std::vector<int> indicesOffsets = std::vector<int>();
            std::vector<int> baseVertices = std::vector<int>();
            std::vector<int> indicesCounts = std::vector<int>();
            int indicesSize = 0;
            int rendererIndex = 0;
            int indicesOffset = 0;
            int baseVertex = 0;
            for(auto &&renderer : group){
                Mesh mesh = renderer.GetMesh();
                indicesSize += mesh.GetIndicesSize();
                indicesOffsets.push_back(indicesOffset);
                indicesOffset += indicesSize;
                indicesCounts.push_back(mesh.GetIndicesCount());
                baseVertices.push_back(baseVertex);
                baseVertex += mesh.GetIndicesCount();

                std::vector<int> attributesDataSizes = mesh.GetAttributeDatasSizes();
                int dataOffset = 0;
                for(int i = 0; i < buffersSizes.size(); i++){
                    buffersSizes[i] += attributesDataSizes[i];
                    dataOffsetsMatrix[rendererIndex].push_back(dataOffset);
                    dataOffset += attributesDataSizes[i];
                    rendererIndex++;
                }
            }
            Batch createdBatch = CreateBatch(buffersSizes, indicesSize, group[0].GetMesh().GetTopology(), group[0].GetShader(),
            indicesCounts, baseVertices);
            StartBatch(createdBatch);
            MeshLayout layout = group[0].GetMesh().GetLayout();
            SetupBatchLayout(createdBatch, layout);
            int indiceOffsetIndex = 0;
            for(auto &&renderer : group){
                for(auto &&offsets : dataOffsetsMatrix){
                    BufferSubData(createdBatch, offsets, renderer.GetMesh().GetAttributesDatas());
                }
                BufferIndices(createdBatch, indicesOffsets[indiceOffsetIndex++], renderer.GetMesh().GetIndices());
            }
        }
    }
}

void Renderer::Draw(){
    for(auto &&batch : batches){
        GLenum mode;
        switch(batch.topology){
            case Line:
                mode = GL_LINE;
                break;
            case LineStrip:
                mode = GL_LINE_STRIP;
                break;
            case Triangle:
                mode = GL_TRIANGLES;
                break;
        }
        batch.shader.Use();
        batch.vao.Bind();
        std::vector<int> indices(batch.indicesCounts.size(), 0);
        glDrawElements(mode, batch.indicesCounts[0], GL_UNSIGNED_INT, 0);
        glMultiDrawElementsBaseVertex(mode, batch.indicesCounts.data(), GL_UNSIGNED_INT, reinterpret_cast<GLvoid **>(indices.data()), batch.indicesCounts.size(), batch.baseVertices.data());
    }
}