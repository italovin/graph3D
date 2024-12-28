#ifndef MESH_H
#define MESH_H
#include <vector>
#include <string>
#include <variant>

enum class MeshTopology{
    Triangles,
    Lines,
    LineStrip
};

enum class MeshAttributeFormat{
    Vec1, Vec2, Vec3, Vec4, Mat3, Mat4, None
};

enum class MeshAttributeType{
    Float, Int, UnsignedInt, Byte, UnsignedByte, Short, UnsignedShort, None
};

// Purpose of attribute for standard render operations
enum class MeshAttributeAlias{
    None = 0, Position, TexCoord0, TexCoord1, TexCoord2, TexCoord3, TexCoord4, TexCoord5, TexCoord6, TexCoord7, Normal, Tangent, Bitangent, Color
};


enum class MeshIndexType{
    UnsignedInt, UnsignedShort, None
};

struct MeshAttribute{
    std::string name;
    int location = 0;
    MeshAttributeType type = MeshAttributeType::None;
    MeshAttributeFormat format = MeshAttributeFormat::None;
    MeshAttributeAlias alias = MeshAttributeAlias::None;
    bool normalized = false;
    bool interpretAsInt = false;
    MeshAttribute() = default;

    MeshAttribute(const std::string &name, int location, MeshAttributeType type, MeshAttributeFormat format, bool normalized):
    name(name), location(location), type(type), format(format), normalized(normalized){
    }

    bool operator == (const MeshAttribute &attribute) const{
        return (attribute.type == type) && (attribute.format == format)
        &&(attribute.normalized == normalized) && (attribute.interpretAsInt == interpretAsInt);
    }
    static int LocationsCount(MeshAttributeFormat format){
        switch(format){
            case MeshAttributeFormat::Vec1:
            case MeshAttributeFormat::Vec2:
            case MeshAttributeFormat::Vec3:
            case MeshAttributeFormat::Vec4: return 1;
            case MeshAttributeFormat::Mat3: return 3;
            case MeshAttributeFormat::Mat4: return 4;
            default: return 1;
        }
    }
    int LocationsCount() const{
        return LocationsCount(format);
    }
    static int ScalarElementsCount(MeshAttributeFormat format){
        switch(format){
            case MeshAttributeFormat::Vec1: return 1;
            case MeshAttributeFormat::Vec2: return 2;
            case MeshAttributeFormat::Vec3: return 3;
            case MeshAttributeFormat::Vec4: return 4;
            case MeshAttributeFormat::Mat3: return 3*3;
            case MeshAttributeFormat::Mat4: return 4*4;
            default: return 1;
        }
    }
    int ScalarElementsCount() const{
        return ScalarElementsCount(format);
    }
    static int AttributeDataSize(MeshAttributeType type, MeshAttributeFormat format){
        int scalars = ScalarElementsCount(format);
        switch(type){
            case MeshAttributeType::Float: return scalars * sizeof(float);
            case MeshAttributeType::Int: return scalars * sizeof(int);
            case MeshAttributeType::UnsignedInt: return scalars * sizeof(unsigned int);
            case MeshAttributeType::Byte: return scalars * sizeof(char);
            case MeshAttributeType::UnsignedByte: return scalars * sizeof(unsigned char);
            case MeshAttributeType::Short: return scalars * sizeof(short);
            case MeshAttributeType::UnsignedShort: return scalars * sizeof(unsigned short);
            default: return 0;
        }
    }
    int AttributeDataSize() const{
        return AttributeDataSize(type, format);
    }
};
struct MeshLayout{
    std::vector<MeshAttribute> attributes;
    bool operator == (const MeshLayout &layout) const{
        if(attributes.size() == 0 || attributes.size() != layout.attributes.size()){
            return false;
        }
        bool isEqual = true;
        for (size_t i = 0; i < attributes.size(); i++)
        {
            if(!(attributes[i] == layout.attributes[i])){
                isEqual = false;
                break;
            }
        }
        return isEqual;
    }
};

struct MeshAttributeData{
    std::variant<std::vector<float>, std::vector<int>, std::vector<unsigned int>, std::vector<char>, 
    std::vector<unsigned char>, std::vector<short>, std::vector<unsigned short>> data;
    //Attribute data size in bytes
    int dataSize = 0;
    MeshAttribute attribute;

    MeshAttributeData() = default;
    MeshAttributeData(const std::vector<float> &&data, int dataSize, const MeshAttribute &attribute):
    data(data), dataSize(dataSize), attribute(attribute){}
    MeshAttributeData(const std::vector<int> &&data, int dataSize, const MeshAttribute &attribute):
    data(data), dataSize(dataSize), attribute(attribute){}
    MeshAttributeData(const std::vector<unsigned int> &&data, int dataSize, const MeshAttribute &attribute):
    data(data), dataSize(dataSize), attribute(attribute){}
    MeshAttributeData(const std::vector<char> &&data, int dataSize, const MeshAttribute &attribute):
    data(data), dataSize(dataSize), attribute(attribute){}
    MeshAttributeData(const std::vector<unsigned char> &&data, int dataSize, const MeshAttribute &attribute):
    data(data), dataSize(dataSize), attribute(attribute){}
    MeshAttributeData(const std::vector<short> &&data, int dataSize, const MeshAttribute &attribute):
    data(data), dataSize(dataSize), attribute(attribute){}
    MeshAttributeData(const std::vector<unsigned short> &&data, int dataSize, const MeshAttribute &attribute):
    data(data), dataSize(dataSize), attribute(attribute){}
};

struct MeshIndexData{
    std::variant<std::vector<unsigned int>, std::vector<unsigned short>> indices;
    //Indices data size in bytes
    int indicesSize = 0;
    MeshIndexType type = MeshIndexType::None;
    MeshIndexData() = default;
    MeshIndexData(const std::vector<unsigned int> &&indices, int indicesSize, MeshIndexType type):
    indices(indices), indicesSize(indicesSize), type(type){}
    MeshIndexData(const std::vector<unsigned short> &&indices, int indicesSize, MeshIndexType type):
    indices(indices), indicesSize(indicesSize), type(type){}
};

class Mesh{
private:
    MeshIndexData indicesData;
    MeshTopology topology = MeshTopology::Triangles;
    std::vector<MeshAttributeData> attributesData;
    int verticesCount = 0;
    MeshLayout layout;
    int locationCounter = 0;
    template <typename T>
    bool PushAttributeBase(const std::string &name, int location, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
    std::vector<T> &&data, bool interpretAsInt, MeshAttributeAlias alias);
    template <typename T>
    bool PushAttributeBase(const std::string &name, int location, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
    const std::vector<T> &data, bool interpretAsInt, MeshAttributeAlias alias);
    
public:
    //~Mesh();
    
    void SetIndices(const std::vector<unsigned short> &indices, MeshTopology topology);
    void SetIndices(std::vector<unsigned short> &&indices, MeshTopology toplogy);
    void SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology);
    void SetIndices(std::vector<unsigned int> &&indices, MeshTopology topology);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<float> &data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<float> &&data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<int> &data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<int> &&data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned int> &data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned int> &&data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<char> &data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<char> &&data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned char> &data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned char> &&data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<short> &data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<short> &&data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, const std::vector<unsigned short> &data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    bool PushAttribute(const std::string &name, int location, MeshAttributeFormat format, bool normalized, std::vector<unsigned short> &&data, bool interpretAsInt = false, MeshAttributeAlias alias = MeshAttributeAlias::None);
    // Standard specific attributes push functions
    bool PushAttributePosition(const std::vector<float> &positions);
    bool PushAttributeTexCoord0(const std::vector<float> &texCoords0);
    bool PushAttributeTexCoord1(const std::vector<float> &texCoords1);
    bool PushAttributeTexCoord2(const std::vector<float> &texCoords2);
    bool PushAttributeTexCoord3(const std::vector<float> &texCoords3);
    bool PushAttributeTexCoord4(const std::vector<float> &texCoords4);
    bool PushAttributeTexCoord5(const std::vector<float> &texCoords5);
    bool PushAttributeTexCoord6(const std::vector<float> &texCoords6);
    bool PushAttributeTexCoord7(const std::vector<float> &texCoords7);
    bool PushAttributeNormal(const std::vector<float> &normals);
    bool PushAttributeTangent(const std::vector<float> &tangents);
    bool PushAttributeBitangent(const std::vector<float> &bitangents);
    bool PushAttributeColor(const std::vector<unsigned char> &colors);
    int GetAttributesCount() const;
    std::vector<int> GetAttributesSizes() const;
    std::vector<int> GetAttributesDatasSizes() const;
    const std::vector<MeshAttributeData> &GetAttributesDatas();
    int GetTotalAttributesDataSize();
    const MeshIndexData &GetIndices();
    unsigned int GetIndicesCount() const;
    int GetIndicesSize() const;
    static int GetIndicesTypeSize(MeshIndexType type);
    const MeshLayout &GetLayout() const;
    MeshTopology GetTopology() const;
    MeshIndexType GetIndicesType() const;
    int GetVerticesCount() const;
};
#endif
