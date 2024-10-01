#ifndef SHADER_CODE_H
#define SHADER_CODE_H
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
    int arraySize = 0;
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
    std::unordered_map<std::string, std::unordered_map<std::string, ShaderCodeParameter>> regularStructs;
    // Designed for material parameters batching
    std::pair<std::string, std::unordered_map<std::string, ShaderCodeParameter>> materialParametersStruct;
    std::pair<std::string, std::string> materialParametersUniformBlock;
    ////
    std::unordered_map<std::string, std::string> uniformBlocks;
    ///
    std::unordered_map<std::string, std::string> uniformBlockBindingPurposes;
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
    std::string CreateStruct(ShaderStage shaderStage, const std::string &structType, const std::string &name);
    std::string DefineMaterialParametersStruct(ShaderStage shaderStage, const std::string &structType);
    void AddMaterialParameterToStruct(const std::string &structType, ShaderStage shaderStage, const std::string &name, ShaderDataType dataType);
    void CreateUniformBlock(ShaderStage shaderStage, const std::string &name, const std::string &body);
    void UpdateMaterialParameterUniformBlock(ShaderStage shaderStage, const std::string &name, const std::string &body);
    void SetMain(ShaderStage shaderStage, const std::string &main);
    std::unordered_map<std::string, ShaderCodeParameter> GetUniforms(ShaderStage shaderStage) const;
    std::pair<std::string, std::unordered_map<std::string, ShaderCodeParameter>> GetMaterialParametersStruct(ShaderStage shaderStage) const;
    void SetBindingPurpose(ShaderStage shaderStage, const std::string &uniformBlockName, const std::string &purpose);
    std::unordered_map<std::string, std::string> GetBindingsPurposes(ShaderStage shaderStage) const;
    std::optional<Shader> Generate();
};
#endif
