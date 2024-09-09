#ifndef SHADER_CODE_H
#define SHADER_CODE_H
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include "Shader.hpp"
#include "ShaderTypes.hpp"

struct ShaderCodeParameter{
    std::optional<int> location = std::nullopt;
    ShaderDataType dataType;
};

struct ShaderStageCode{
    bool enabled = false;
    // This map is used only for vertex shader attributes
    std::unordered_map<std::string, ShaderCodeParameter> inputs;
    // This map denotes the outputs parameters to send to next stage
    std::unordered_map<std::string, ShaderCodeParameter> outputs;
    std::unordered_map<std::string, ShaderCodeParameter> uniforms;
    std::string main;
};

// Intermediary representation of shader program. Compound by shader objects
class ShaderCode : public Resource {
private:
    int version = 330;
    ShaderStageCode vertexShader;
    ShaderStageCode fragmentShader;
    ShaderStageCode tesselationControlShader;
    ShaderStageCode tesselationEvaluationShader;
    ShaderStageCode geometryShader;   
    const std::string GLSLTypeToString(ShaderDataType type);
    void ProcessVertexShaderCode(const ShaderStageCode &shaderStageCode, std::string &outsideString);
    void ProcessShaderStageCode(const ShaderStageCode &shaderStageCode, std::string &mainString,
    std::string &outsideString, std::string &outsideStringIns);
public:
    void SetVersion(int version);
    void SetVertexToPipeline(bool enabled);
    void SetFragmentToPipeline(bool enabled);
    void SetTesselationControlToPipeline(bool enabled);
    void SetTesselationEvaluationToPipeline(bool enabled);
    void SetGeometryToPipeline(bool enabled);
    void AddVertexAttribute(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddVertexOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddFragmentOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddTesselationControlOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddTesselationEvaluationOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddGeometryOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void SetVertexMain(const std::string &main);
    void SetFragmentMain(const std::string &main);
    void SetTesselationControlMain(const std::string &main);
    void SetTesselationEvaluationMain(const std::string &main);
    void SetGeometryMain(const std::string &main);
    std::unordered_map<std::string, ShaderCodeParameter> GetFragmentUniforms() const;
    std::optional<Shader> Generate();
};
#endif
