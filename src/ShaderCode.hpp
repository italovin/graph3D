#ifndef SHADER_CODE_H
#define SHADER_CODE_H
#include <unordered_map>
#include <string>
#include <optional>
#include <algorithm>
#include "Shader.hpp"
#include "ShaderTypes.hpp"
#include "MaterialTypes.hpp"
#include <chrono>
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
    std::pair<std::string, std::vector<std::pair<std::string, MaterialParameter>>> materialParametersStruct;
    std::pair<std::string, std::string> materialParametersUniformBlock;
    int materialParametersSpaceUsed = 0;
    // This vector are material parameters sorted by larger alignment
    std::vector<std::pair<std::string, MaterialParameter>> materialSortedParameters;
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
    // Each member in elements of array of structs sums even this max value
    const int maxUniformBlockArrayElementSize = 128;
    std::vector<std::string> extensions;
    std::string additionalOutsideString;
    ShaderStageCode vertexShader;
    ShaderStageCode fragmentShader;
    ShaderStageCode tesselationControlShader;
    ShaderStageCode tesselationEvaluationShader;
    ShaderStageCode geometryShader;   
    const std::string GLSLTypeToString(ShaderDataType type);
    const std::string GLSLMatTypeToString(MaterialParameterType type);
    void ProcessVertexShaderCode(const ShaderStageCode &shaderStageCode, std::string &outsideString);
    void ProcessShaderStageCode(const ShaderStageCode &shaderStageCode, std::string &mainString,
    std::string &outsideString, std::string &outsideStringIns);
    void SortMaterialParameters(ShaderStageCode &shaderStageCode);
    void PushMaterialParameter(ShaderStage shaderStage, const std::string &name, const MaterialParameter &parameter, int toAddSize);
public:
    void SetVersion(int version);
    void AddExtension(const std::string &extension);
    void SetAdditionalOutsideString(const std::string &additionalOutsideString);
    void SetStageToPipeline(ShaderStage shaderStage, bool enabled);
    void AddVertexAttribute(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddOutput(ShaderStage shaderStage, const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    void AddUniform(ShaderStage shaderStage, const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt);
    std::string CreateStruct(ShaderStage shaderStage, const std::string &structType, const std::string &name);
    std::string DefineMaterialParametersStruct(ShaderStage shaderStage, const std::string &structType);
    void AddMaterialFloatToStruct(const std::string &structType, ShaderStage shaderStage, const std::string &name, float defaultValue = 0.0f);
    void AddMaterialBoolToStruct(const std::string &structType, ShaderStage shaderStage, const std::string &name, bool defaultValue = false);
    void AddMaterialVec4ToStruct(const std::string &structType, ShaderStage shaderStage, const std::string &name, glm::vec4 defaultValue = glm::vec4(0.0f));
    void UpdateMaterialParameterUniformBlock(ShaderStage shaderStage, const std::string &name, const std::string &body);
    void CreateUniformBlock(ShaderStage shaderStage, const std::string &name, const std::string &body);
    void SetMain(ShaderStage shaderStage, const std::string &main);
    std::unordered_map<std::string, ShaderCodeParameter> GetUniforms(ShaderStage shaderStage) const;
    std::vector<std::pair<std::string, MaterialParameter>> GetMaterialParameters(ShaderStage shaderStage);
    void SetBindingPurpose(ShaderStage shaderStage, const std::string &uniformBlockName, const std::string &purpose);
    std::unordered_map<std::string, std::string> GetBindingsPurposes(ShaderStage shaderStage) const;
    std::optional<Shader> Generate();
};
#endif
