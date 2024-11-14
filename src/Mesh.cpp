#include "Mesh.hpp"
#include <numeric>

Mesh::~Mesh(){ 
}

template <typename T>
bool Mesh::PushAttributeBase(const std::string &name, int location, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
std::vector<T> &&data, bool interpretAsInt){
    std::div_t verticesDiv = std::div(data.size(), MeshAttribute::ScalarElementsCount(format));
    if(verticesDiv.rem > 0 || (verticesCount > 0 && verticesCount != verticesDiv.quot)){
        return false;
    } else if(verticesCount == 0){
        verticesCount = verticesDiv.quot;
    }
    MeshAttribute meshAttribute;
    meshAttribute.name = name;
    meshAttribute.location = location;
    meshAttribute.type = type;
    meshAttribute.format = format;
    meshAttribute.normalized = normalized;
    meshAttribute.interpretAsInt = interpretAsInt;
    int totalDataSize = MeshAttribute::AttributeDataSize(type, format)*verticesCount;
    attributesData.emplace_back(std::move(data), totalDataSize, meshAttribute);
    layout.attributes.push_back(meshAttribute);
    return true;
}

template <typename T>
bool Mesh::PushAttributeBase(const std::string &name, int location, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
const std::vector<T> &data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, type, normalized, std::vector<T>(data), interpretAsInt);
}

void Mesh::SetIndices(const std::vector<unsigned short> &indices, MeshTopology topology){
    SetIndices(std::vector<unsigned short>(indices), topology);
}

void Mesh::SetIndices(std::vector<unsigned short> &&indices, MeshTopology topology){
    this->indicesData = MeshIndexData(std::move(indices), sizeof(unsigned short)*indices.size(), MeshIndexType::UnsignedShort);
    this->topology = topology;
}

void Mesh::SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology){
    SetIndices(std::vector<unsigned int>(indices), topology);
}

void Mesh::SetIndices(std::vector<unsigned int> &&indices, MeshTopology topology){
    this->indicesData = MeshIndexData(std::move(indices), sizeof(unsigned int)*indices.size(), MeshIndexType::UnsignedInt);
    this->topology = topology;
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<float> &data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::Float, normalized, data, interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<float> &&data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::Float, normalized, std::move(data), interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<int> &data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::Int, normalized, data, interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<int> &&data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::Int, normalized, std::move(data), interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned int> &data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedInt, normalized, data, interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned int> &&data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedInt, normalized, std::move(data), interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<char> &data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::Byte, normalized, data, interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<char> &&data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::Byte, normalized, std::move(data), interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned char> &data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedByte, normalized, data, interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned char> &&data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedByte, normalized, std::move(data), interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<short> &data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::Short, normalized, data, interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<short> &&data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::Short, normalized, std::move(data), interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned short> &data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedShort, normalized, data, interpretAsInt);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned short> &&data, bool interpretAsInt){
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedShort, normalized, std::move(data), interpretAsInt);
}

int Mesh::GetAttributesCount() const{
    return attributesData.size();
}

std::vector<int> Mesh::GetAttributesSizes() const{
    std::vector<int> sizes(attributesData.size());
    std::transform(attributesData.begin(), attributesData.end(), sizes.begin(),
    [](const MeshAttributeData& attributeData) {
        return attributeData.attribute.AttributeDataSize();
    });
    return sizes;
}

std::vector<int> Mesh::GetAttributesDatasSizes() const{
    std::vector<int> datasSizes(attributesData.size());
    std::transform(attributesData.begin(), attributesData.end(), datasSizes.begin(),
    [](const MeshAttributeData& attributeData) {
        return attributeData.dataSize;
    });
    return datasSizes;
}

const std::vector<MeshAttributeData> &Mesh::GetAttributesDatas(){
    return attributesData;
}

int Mesh::GetTotalAttributesDataSize(){
    return std::accumulate(attributesData.begin(), attributesData.end(), 0, [](int acc, const MeshAttributeData &attribData){
        return acc + attribData.dataSize;
    });
}

const MeshIndexData &Mesh::GetIndices(){
    return indicesData;
}

unsigned int Mesh::GetIndicesCount() const{
    auto type = GetIndicesType();
    switch(type){
        case MeshIndexType::UnsignedInt: return std::get<std::vector<unsigned int>>(indicesData.indices).size();
        case MeshIndexType::UnsignedShort: return std::get<std::vector<unsigned short>>(indicesData.indices).size();
        default: return 0;
    }
}

int Mesh::GetIndicesSize() const{
    auto type = GetIndicesType();
    switch(type){
        case MeshIndexType::UnsignedInt: return sizeof(unsigned int)*GetIndicesCount();
        case MeshIndexType::UnsignedShort: return sizeof(unsigned short)*GetIndicesCount();
        default: return 0;
    }
}

int Mesh::GetIndicesTypeSize(MeshIndexType type)
{
    switch(type){
        case MeshIndexType::UnsignedInt: return sizeof(unsigned int);
        case MeshIndexType::UnsignedShort: return sizeof(unsigned short);
        default: return 0;
    }
}

const MeshLayout &Mesh::GetLayout() const{
    return layout;
}

MeshTopology Mesh::GetTopology() const{
    return topology;
}

MeshIndexType Mesh::GetIndicesType() const{
    return indicesData.type;
}

int Mesh::GetVerticesCount() const{
    return verticesCount;
}
