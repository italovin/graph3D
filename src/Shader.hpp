#ifndef SHADER_H
#define SHADER_H
#include "ShaderCode.hpp"
#include "Mesh.hpp"
// Base class to model the shaders used in materials
class Shader{
protected:
    // Use material maps (Related to textures)
    std::unordered_map<std::string, bool> maps;
    // Use material flags
    std::unordered_map<std::string, bool> flags;
    // Used mesh layout attributes and their locations
    std::unordered_map<std::string, std::pair<bool, MeshAttribute>> attributes;
    MeshIndexType indexType = MeshIndexType::None;
public:
    virtual ~Shader() = default;
    bool operator==(const Shader& other) const;
    // Calculates hashes for identify uniquely the shaders
    std::size_t Hash() const;
    const std::unordered_map<std::string, bool>& GetMaps();
    const std::unordered_map<std::string, bool>& GetFlags();
    const std::unordered_map<std::string, std::pair<bool, MeshAttribute>>& GetAttributes();
    // Used to build the code
    virtual ShaderCode ProcessCode();
};

struct ShaderHash {
    std::size_t operator()(const Shader& shader) const {
        return shader.Hash();
    }
};

#endif
