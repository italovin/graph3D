#ifndef MATERIAL_H
#define MATERIAL_H
#include <functional>
#include <unordered_map>
#include <string>
#include <optional>
#include <glm/glm.hpp>
#include "Base.hpp"
#include "ShaderCode.hpp"

class Material{
private:
    std::function<void(const std::string&, float)> floatGlobalChangeCallback;
    std::function<void(const std::string&, bool)> booleanGlobalChangeCallback;
    std::function<void(const std::string&, glm::vec4)> vector4GlobalChangeCallback;
    bool changingBuffer = false;
    std::vector<std::pair<std::string, MaterialParameter>> parameters;
    // Global parameters sets a uniform in the shader. The last material to set overrides the value
    std::unordered_map<std::string, MaterialParameter> globalShaderParameters;
    std::unordered_map<std::string, MaterialParameter> globalVertexShaderParameters;
    Ref<ShaderCode> shaderCode;
    void AddParameter(const std::string &name, const MaterialParameter &parameter);
    void AddGlobalParameter(const std::string &name, MaterialParameterType type, bool isFragOrVert);
    void DeleteParameters();
    MaterialParameterType GetParameterType(ShaderDataType type);
    std::vector<std::pair<std::string, MaterialParameter>>::iterator FindParameter(const std::string &name);
public:
    Material() = delete;
    Material(Ref<ShaderCode> shaderCode);
    Ref<ShaderCode> GetShaderCode() const;
    void SetParameterMap(const std::string &name, Ref<Texture> value);
    void SetParameterFloat(const std::string &name, float value);
    void SetParameterBoolean(const std::string &name, bool value);
    void SetParameterVector4(const std::string &name, glm::vec4 value);
    void SetOnGlobalFloatChangeCallback(std::function<void(const std::string&, float)> callback);
    void SetOnGlobalBooleanChangeCallback(std::function<void(const std::string&, bool)> callback);
    void SetOnGlobalVector4ChangeCallback(std::function<void(const std::string&, glm::vec4)> callback);
    std::optional<Ref<Texture>> GetParameterMap(const std::string &name);
    std::optional<float> GetParameterFloat(const std::string &name);
    std::optional<bool> GetParameterBoolean(const std::string &name);
    std::optional<glm::vec4> GetParameterVector4(const std::string &name);
    std::vector<std::pair<std::string, MaterialParameter>> GetParameters() const;
    std::vector<std::pair<std::string, MaterialParameter>> GetMapParameters() const;
    std::vector<std::pair<std::string, MaterialParameter>> GetFloatParameters() const;
    std::vector<std::pair<std::string, MaterialParameter>> GetBooleanParameters() const;
    std::vector<std::pair<std::string, MaterialParameter>> GetVector4Parameters() const;
    void SetGlobalParameterMap(const std::string &name, Ref<Texture> value);
    void SetGlobalParameterFloat(const std::string &name, float value);
    void SetGlobalParameterBoolean(const std::string &name, bool value);
    void SetGlobalParameterVector4(const std::string &name, glm::vec4 value);
    std::unordered_map<std::string, MaterialParameter> GetGlobalVertexParameters() const;
};
#endif
