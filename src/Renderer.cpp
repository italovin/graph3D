#include "Renderer.hpp"

GLenum Renderer::GetDrawMode(MeshTopology topology){
    switch(topology){
        case MeshTopology::Lines: return GL_LINES;
        case MeshTopology::LineStrip: return GL_LINE_STRIP;
        case MeshTopology::Triangles: return GL_TRIANGLES;
        default: return GL_TRIANGLES;
    }
}

Renderer::Batch Renderer::CreateBatch(const std::vector<unsigned int> &buffersSizes, const std::vector<int> &strides, 
unsigned int indicesBufferSize, MeshTopology topology, const ShaderProgram &shader,
const std::vector<int> &indicesCounts, const std::vector<int> &baseVertices){
    VertexArray newVao = VertexArray();
    std::vector<GLuint> newBuffers = std::vector<GLuint>(buffersSizes.size());
    glCreateBuffers(buffersSizes.size(), newBuffers.data());
    Batch newBatch;
    for(int i = 0; i < buffersSizes.size(); i++){
        Buffer buffer = {.name = newBuffers[i], .bufferSize = buffersSizes[i], .stride = strides[i],
        .bindingPoint = i};
        newBatch.buffers.push_back(buffer);
    }
    GLuint newIndicesBuffer;
    glCreateBuffers(1, &newIndicesBuffer);
    Buffer indicesBuffer = {.name = newIndicesBuffer, .bufferSize = indicesBufferSize};
    newBatch.vao = newVao;
    newBatch.indiceBuffer = indicesBuffer;
    newBatch.indicesCounts = indicesCounts;
    newBatch.baseVertices = baseVertices;
    newBatch.topology = topology;
    newBatch.shader = shader;
    newBatch.mode = GetDrawMode(topology);
    newBatch.indicesOffsetInBuffer = std::vector<int>(indicesCounts.size(), 0);
    batches.push_back(newBatch);
    return newBatch;
}

void Renderer::StartBatch(Batch &batch){
    for(auto &&buffer : batch.buffers){
        glNamedBufferStorage(buffer.name, buffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(batch.vao.GetHandle(), buffer.bindingPoint, buffer.name, 0, buffer.stride);
    }
    glNamedBufferStorage(batch.indiceBuffer.name, batch.indiceBuffer.bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(batch.vao.GetHandle(), batch.indiceBuffer.name);
}

void Renderer::SetupBatchLayout(Batch &batch, MeshLayout &layout){
    int bindingPoint = 0;
    int locationsOffset = 0;
    for(auto &&attribute : layout.attributes){
        GLenum type;
        GLboolean normalized;
        int locations = attribute.LocationsCount();
        switch(attribute.type){
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4: type = GL_FLOAT; break;
            case ShaderDataType::Int: type = GL_INT; break;
            default: type = GL_FLOAT;
        }
        normalized = attribute.normalized ? GL_TRUE : GL_FALSE;
        for(int i = 0; i < locations; i++){
            glVertexArrayAttribFormat(batch.vao.GetHandle(), locationsOffset + i, attribute.ScalarElementsCount(), type, normalized, 0);
            glEnableVertexArrayAttrib(batch.vao.GetHandle(), locationsOffset + i);
            glVertexArrayAttribBinding(batch.vao.GetHandle(), locationsOffset + i, bindingPoint);
        }
        locationsOffset += locations;
        bindingPoint++;
    }
}

void Renderer::BufferSubData(Batch &batch, const std::vector<unsigned int> &offsets, const std::vector<MeshAttributeData> &attributesDatas){
    int index = 0;
    for(auto &&buffer : batch.buffers){
        glNamedBufferSubData(buffer.name, offsets[index], attributesDatas[index].dataSize, attributesDatas[index].data.data());
        index++;
    }
}

void Renderer::BufferIndices(Batch &batch, unsigned int offset, const std::vector<unsigned int> &indices){
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
            std::vector<unsigned int> buffersSizes = std::vector<unsigned int>(group[0].GetMesh().GetAttributesCount());
            std::vector<int> strides = std::vector<int>(group[0].GetMesh().GetAttributesCount());
            std::vector<std::vector<unsigned int>> dataOffsetsMatrix = std::vector<std::vector<unsigned int>>(group.size());
            std::vector<unsigned int> indicesOffsets = std::vector<unsigned int>();
            std::vector<int> baseVertices = std::vector<int>();
            std::vector<int> indicesCounts = std::vector<int>();
            int indicesSize = 0;
            int rendererIndex = 0;
            int indicesOffset = 0;
            int baseVertex = 0;
            std::vector<int> dataOffsets = std::vector<int>(group[0].GetMesh().GetAttributesCount());
            for(auto &&renderer : group){
                Mesh mesh = renderer.GetMesh();
                indicesSize += mesh.GetIndicesSize();
                indicesOffsets.push_back(indicesOffset);
                indicesOffset += indicesSize;
                indicesCounts.push_back(mesh.GetIndicesCount());
                baseVertices.push_back(baseVertex);
                baseVertex += mesh.GetIndicesCount();

                std::vector<int> attributesDataSizes = mesh.GetAttributeDatasSizes();
                
                for(int i = 0; i < buffersSizes.size(); i++){
                    if(rendererIndex == 0){
                        strides[i] = mesh.GetLayout().attributes[i].AttributeDataSize();
                    }
                    buffersSizes[i] += attributesDataSizes[i];
                    dataOffsetsMatrix[rendererIndex].push_back(dataOffsets[i]);
                    dataOffsets[i] += attributesDataSizes[i];
                }
                rendererIndex++;
            }
            Batch createdBatch = CreateBatch(buffersSizes, strides, indicesSize, group[0].GetMesh().GetTopology(), group[0].GetShader(),
            indicesCounts, baseVertices);
            StartBatch(createdBatch);
            
            for(int i = 0; i < group.size(); i++){
                auto offsets = dataOffsetsMatrix[i];
                Mesh mesh = group[i].GetMesh();
                BufferSubData(createdBatch, offsets, mesh.GetAttributesDatas());
                BufferIndices(createdBatch, indicesOffsets[i], mesh.GetIndices());
            }
            MeshLayout layout = group[0].GetMesh().GetLayout();
            SetupBatchLayout(createdBatch, layout);
        }
    }
}

void Renderer::Draw(){
    for(auto &&batch : batches){
        batch.shader.Use();
        batch.vao.Bind();
        glMultiDrawElementsBaseVertex(batch.mode, batch.indicesCounts.data(), 
        GL_UNSIGNED_INT, reinterpret_cast<GLvoid **>(batch.indicesOffsetInBuffer.data()), 
        batch.indicesCounts.size(), batch.baseVertices.data());
    }
}

int Renderer::GetBatchesCount(){
    return batches.size();
}