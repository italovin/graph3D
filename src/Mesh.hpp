#ifndef MESH_H
#define MESH_H
#include <vector>
#include <string>
#include <variant>
#include "ShaderTypes.hpp"

enum class MeshTopology{
    Triangles,
    Lines,
    LineStrip
};

enum class MeshAttributeFormat{
    Vec1, Vec2, Vec3, Vec4, Mat3, Mat4
};

enum class MeshAttributeType{
    Float, Int, UnsignedInt, Byte, UnsignedByte, Short, UnsignedShort
};

struct MeshAttribute{
    std::string name;
    MeshAttributeType type;
    MeshAttributeFormat format;
    bool normalized;
    bool operator == (const MeshAttribute &attribute) const{
        return (attribute.type == type) && (attribute.normalized == normalized);
    }
    int LocationsCount() const{
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
    int ScalarElementsCount() const{
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
    int AttributeDataSize() const{
        int scalars = ScalarElementsCount();
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
};
struct MeshLayout{
    std::vector<MeshAttribute> attributes;
    bool operator == (const MeshLayout &layout) const{
        if(attributes.size() == 0 || attributes.size() != layout.attributes.size()){
            return false;
        }
        bool isEqual = true;
        for (int i = 0; i < attributes.size(); i++)
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
    int dataSize;
    MeshAttribute attribute;
};

class Mesh{
private:
    std::vector<unsigned int> indices;
    MeshTopology topology;
    std::vector<MeshAttributeData> attributesData;
    int verticesCount = 0;
    MeshLayout layout;
    template <typename T>
    bool PushAttributeBase(const std::string &name, MeshAttributeFormat format, MeshAttributeType type, bool normalized, 
    const std::vector<T> &data);
public:
    ~Mesh();
    
    void SetIndices(const std::vector<unsigned int> &indices, MeshTopology topology);
    bool PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<float> &data);
    bool PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<int> &data);
    bool PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned int> &data);
    bool PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<char> &data);
    bool PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned char> &data);
    bool PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<short> &data);
    bool PushAttribute(const std::string &name, MeshAttributeFormat format, bool normalized, const std::vector<unsigned short> &data);
    int GetAttributesCount() const;
    std::vector<int> GetAttributesDatasSizes();
    const std::vector<MeshAttributeData> &GetAttributesDatas();
    int GetTotalAttributesDataSize();
    const std::vector<unsigned int> &GetIndices();
    int GetIndicesCount() const;
    int GetIndicesSize() const;
    const MeshLayout &GetLayout() const;
    MeshTopology GetTopology() const;
};
#endif