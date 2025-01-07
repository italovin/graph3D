#include "Mesh.hpp"
#include <numeric>
#include <algorithm>
// Mesh::~Mesh(){ 
// }

template <typename T>
bool Mesh::PushAttributeBase(const std::string &name, int location, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
std::vector<T> &&data, bool interpretAsInt, MeshAttributeAlias alias){
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
    meshAttribute.alias = alias;
    int totalDataSize = MeshAttribute::AttributeDataSize(type, format)*verticesCount;
    attributesData.emplace_back(std::move(data), totalDataSize, meshAttribute);
    layout.attributes.push_back(meshAttribute);
    return true;
}

template <typename T>
bool Mesh::PushAttributeBase(const std::string &name, int location, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
const std::vector<T> &data, bool interpretAsInt, MeshAttributeAlias alias){
    return PushAttributeBase(name, location, format, type, normalized, std::vector<T>(data), interpretAsInt, alias);
}

bool Mesh::CheckIfAliasExists(MeshAttributeAlias alias)
{
    if(alias == MeshAttributeAlias::None)
        return false;
    auto it = std::find_if(layout.attributes.begin(), layout.attributes.end(),
    [alias](const MeshAttribute &attribute){
        return alias == attribute.alias;
    });
    return (it != layout.attributes.end());
}

void Mesh::SetIndices(const std::vector<unsigned short> &indices, MeshTopology topology)
{
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

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<float> &data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::Float, normalized, data, interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<float> &&data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::Float, normalized, std::move(data), interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<int> &data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::Int, normalized, data, interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<int> &&data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::Int, normalized, std::move(data), interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned int> &data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedInt, normalized, data, interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned int> &&data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedInt, normalized, std::move(data), interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<char> &data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::Byte, normalized, data, interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<char> &&data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::Byte, normalized, std::move(data), interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned char> &data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedByte, normalized, data, interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned char> &&data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedByte, normalized, std::move(data), interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<short> &data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::Short, normalized, data, interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<short> &&data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::Short, normalized, std::move(data), interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned short> &data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedShort, normalized, data, interpretAsInt, alias);
}

bool Mesh::PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned short> &&data, bool interpretAsInt, MeshAttributeAlias alias){
    if(CheckIfAliasExists(alias))
        return false;
    return PushAttributeBase(name, location, format, MeshAttributeType::UnsignedShort, normalized, std::move(data), interpretAsInt, alias);
}
bool Mesh::PushAttributePosition(const std::vector<float> &positions){
    return PushAttribute("position", locationCounter++, MeshAttributeFormat::Vec3, false, positions, false, MeshAttributeAlias::Position);
}
bool Mesh::PushAttributeTexCoord0(const std::vector<float> &texCoords0){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, false, texCoords0, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeTexCoord0(const std::vector<unsigned short> &texCoords0){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, true, texCoords0, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeTexCoord1(const std::vector<float> &texCoords1){
    return PushAttribute("texCoord1", locationCounter++, MeshAttributeFormat::Vec2, false, texCoords1, false, MeshAttributeAlias::TexCoord1);
}
bool Mesh::PushAttributeTexCoord1(const std::vector<unsigned short> &texCoords1){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, true, texCoords1, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeTexCoord2(const std::vector<float> &texCoords2){
    return PushAttribute("texCoord2", locationCounter++, MeshAttributeFormat::Vec2, false, texCoords2, false, MeshAttributeAlias::TexCoord2);
}
bool Mesh::PushAttributeTexCoord2(const std::vector<unsigned short> &texCoords2){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, true, texCoords2, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeTexCoord3(const std::vector<float> &texCoords3){
    return PushAttribute("texCoord3", locationCounter++, MeshAttributeFormat::Vec2, false, texCoords3, false, MeshAttributeAlias::TexCoord3);
}
bool Mesh::PushAttributeTexCoord3(const std::vector<unsigned short> &texCoords3){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, true, texCoords3, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeTexCoord4(const std::vector<float> &texCoords4){
    return PushAttribute("texCoord4", locationCounter++, MeshAttributeFormat::Vec2, false, texCoords4, false, MeshAttributeAlias::TexCoord4);
}
bool Mesh::PushAttributeTexCoord4(const std::vector<unsigned short> &texCoords4){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, true, texCoords4, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeTexCoord5(const std::vector<float> &texCoords5){
    return PushAttribute("texCoord5", locationCounter++, MeshAttributeFormat::Vec2, false, texCoords5, false, MeshAttributeAlias::TexCoord5);
}
bool Mesh::PushAttributeTexCoord5(const std::vector<unsigned short> &texCoords5){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, true, texCoords5, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeTexCoord6(const std::vector<float> &texCoords6){
    return PushAttribute("texCoord6", locationCounter++, MeshAttributeFormat::Vec2, false, texCoords6, false, MeshAttributeAlias::TexCoord6);
}
bool Mesh::PushAttributeTexCoord6(const std::vector<unsigned short> &texCoords6){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, true, texCoords6, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeTexCoord7(const std::vector<float> &texCoords7){
    return PushAttribute("texCoord7", locationCounter++, MeshAttributeFormat::Vec2, false, texCoords7, false, MeshAttributeAlias::TexCoord7);
}
bool Mesh::PushAttributeTexCoord7(const std::vector<unsigned short> &texCoords7){
    return PushAttribute("texCoord0", locationCounter++, MeshAttributeFormat::Vec2, true, texCoords7, false, MeshAttributeAlias::TexCoord0);
}
bool Mesh::PushAttributeNormal(const std::vector<float> &normals){
    return PushAttribute("normal", locationCounter++, MeshAttributeFormat::Vec3, false, normals, false, MeshAttributeAlias::Normal);
}
bool Mesh::PushAttributeNormal(const std::vector<short> &normals){
    return PushAttribute("normal", locationCounter++, MeshAttributeFormat::Vec3, true, normals, false, MeshAttributeAlias::Normal);
}
bool Mesh::PushAttributeTangent(const std::vector<float> &tangents){
    return PushAttribute("tangent", locationCounter++, MeshAttributeFormat::Vec3, false, tangents, false, MeshAttributeAlias::Tangent);
}
bool Mesh::PushAttributeTangent(const std::vector<short> &tangents){
    return PushAttribute("tangent", locationCounter++, MeshAttributeFormat::Vec3, true, tangents, false, MeshAttributeAlias::Tangent);
}
bool Mesh::PushAttributeBitangent(const std::vector<float> &bitangents){
    return PushAttribute("bitangent", locationCounter++, MeshAttributeFormat::Vec3, false, bitangents, false, MeshAttributeAlias::Bitangent);
}
bool Mesh::PushAttributeBitangent(const std::vector<short> &bitangents){
    return PushAttribute("bitangent", locationCounter++, MeshAttributeFormat::Vec3, true, bitangents, false, MeshAttributeAlias::Bitangent);
}
bool Mesh::PushAttributeColor(const std::vector<unsigned char> &colors){
    return PushAttribute("color", locationCounter++, MeshAttributeFormat::Vec4, true, colors, false, MeshAttributeAlias::Color);
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
