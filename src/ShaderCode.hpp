#ifndef SHADER_CODE_H
#define SHADER_CODE_H
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include "Shader.hpp"
#include "ShaderTypes.hpp"

enum class ShaderStage{
    Vertex,
    TesselationControl,
    TesselationEvaluation,
    Geometry,
    Fragment
};

struct ShaderCodeParameter{
    std::optional<int> location = std::nullopt;
    ShaderDataType dataType;
};

struct UniformBlock{
    std::unordered_map<std::string, ShaderCodeParameter> parameters;
};

struct ShaderStageCode{
    bool enabled = false;
    // This map is used only for vertex shader attributes
    std::unordered_map<std::string, ShaderCodeParameter> inputs;
    // This map denotes the outputs parameters to send to next stage
    std::unordered_map<std::string, ShaderCodeParameter> outputs;
    std::unordered_map<std::string, ShaderCodeParameter> uniforms;
    std::unordered_map<std::string, std::string> uniformBlocks;
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
    void SetStageToPipeline(ShaderStage shaderStage, bool enabled);
    void AddVertexAttribute(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddOutput(ShaderStage shaderStage, const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddUniform(ShaderStage shaderStage, const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddUniformBlock(ShaderStage shaderStage, const std::string &name, const std::string &body);
    void SetMain(ShaderStage shaderStage, const std::string &main);
    std::unordered_map<std::string, ShaderCodeParameter> GetFragmentUniforms() const;
    std::optional<Shader> Generate();
};
#endif
