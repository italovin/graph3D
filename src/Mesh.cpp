#include "Mesh.hpp"

Mesh::~Mesh(){ 
}

void Mesh::SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology){
    this->indices = indices;
    this->topology = topology;
}

void Mesh::PushAttribute(const std::string &name, int index, const std::vector<float> &data, int size, bool normalized){
    MeshAttribute meshAttribute;
    meshAttribute.index = index;
    meshAttribute.size = glm::clamp(size, 1, 4);
    meshAttribute.type = MeshFloat32;
    meshAttribute.normalized = normalized;
    MeshAttributeData meshAttributeData;
    meshAttributeData.name = name;
    //std::copy(data.begin(), data.end(), meshAttributeData.data);
    meshAttributeData.data = data;
    meshAttributeData.dataSize = sizeof(float)*data.size();
    meshAttributeData.attribute = meshAttribute;
    attributesData.push_back(meshAttributeData);
    layout.attributes.push_back(meshAttribute);
}

int Mesh::GetAttributesCount(){
    return attributesData.size();
}

std::vector<int> Mesh::GetAttributeDatasSizes(){
    std::vector<int> datasSizes;
    for(auto &&attriuteData : attributesData){
        datasSizes.push_back(attriuteData.dataSize);
    }
    return datasSizes;
}

int Mesh::GetMeshDataSize(){
    int size = 0;
    for (auto &&attributeData : attributesData)
    {
        switch(attributeData.attribute.type){
            case MeshFloat32:
                size += sizeof(float)*attributeData.dataSize;
                break;
            case MeshInt32:
                size += sizeof(int)*attributeData.dataSize;
                break;
        }
    }
    return size;
}

int Mesh::GetIndicesCount(){
    return indices.size();
}

int Mesh::GetIndicesSize(){
    return sizeof(unsigned int)*GetIndicesCount();
}

MeshLayout Mesh::GetLayout(){
    return layout;
}

MeshTopology Mesh::GetTopology(){
    return topology;
}

const std::vector<MeshAttributeData> &Mesh::GetAttributesDatas(){
    return attributesData;
}

const std::vector<unsigned int> &Mesh::GetIndices(){
    return indices;
}