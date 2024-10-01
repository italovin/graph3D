#ifndef MATERIAL_H
#define MATERIAL_H
#include <unordered_map>
#include <string>
#include <variant>
#include <optional>
#include <glm/glm.hpp>
#include "Base.hpp"
#include "Texture.hpp"
#include "ShaderCode.hpp"


enum class MaterialParameterType{
    Map,
    Float,
    Boolean,
    Vector4
};

struct MaterialParameter{
    std::variant<Ref<Texture>, float, bool, glm::vec4> data;
    MaterialParameterType type;
};

class Material : public Resource {
private:
    bool changingBuffer = false;
    std::unordered_map<std::string, MaterialParameter> parameters;
    // Global parameters sets a uniform in the shader. The last material to set overrides the value
    std::unordered_map<std::string, MaterialParameter> globalShaderParameters;
    std::unordered_map<std::string, MaterialParameter> globalVertexShaderParameters;
    std::optional<std::reference_wrapper<ShaderCode>> shaderCode;
    void AddParameter(const std::string &name, MaterialParameterType type);
    void AddGlobalParameter(const std::string &name, MaterialParameterType type, bool isFragOrVert);
    void DeleteParameters();
    MaterialParameterType GetParameterType(ShaderDataType type);
public:
    Material() = delete;
    Material(ShaderCode &shaderCode);
    std::optional<std::reference_wrapper<ShaderCode>> GetShaderCode() const;
    void SetParameterMap(const std::string &name, Ref<Texture> value);
    void SetParameterFloat(const std::string &name, float value);
    void SetParameterBoolean(const std::string &name, bool value);
    void SetParameterVector4(const std::string &name, glm::vec4 value);
    std::optional<Ref<Texture>> GetParameterMap(const std::string &name);
    std::optional<float> GetParameterFloat(const std::string &name);
    std::optional<bool> GetParameterBoolean(const std::string &name);
    std::optional<glm::vec4> GetParameterVector4(const std::string &name);
    std::unordered_map<std::string, MaterialParameter> GetParameters() const;
    void SetGlobalParameterMap(const std::string &name, Ref<Texture> value);
    void SetGlobalParameterFloat(const std::string &name, float value);
    void SetGlobalParameterBoolean(const std::string &name, bool value);
    void SetGlobalParameterVector4(const std::string &name, glm::vec4 value);
};
#endif
