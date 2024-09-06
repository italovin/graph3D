#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include "Shader.hpp"
#include "ShaderTypes.hpp"

struct ShaderCodeParameter{
    ShaderDataType dataType;
    ShaderQualifierType qualifierType;
};

struct ShaderStageCode{
    bool enabled = false;
    std::unordered_map<std::string, ShaderCodeParameter> parameters;
    std::string main;
};

// Intermediary representation of shader program. Compound by shader objects
class ShaderCode{
private:
    int version = 330;
    ShaderStageCode vertexShader;
    ShaderStageCode fragmentShader;
    ShaderStageCode tesselationControlShader;
    ShaderStageCode tesselationEvaluationShader;
    ShaderStageCode geometryShader;   
    const std::string GLSLTypeToString(ShaderDataType type);
    void ProcessShaderStageCode(const ShaderStageCode &shaderStageCode, std::string &mainString,
    std::string &outsideString, std::string &outsideStringOuts);
public:
    void SetVertexToPipeline(bool enabled);
    void SetFragmentToPipeline(bool enabled);
    void SetTesselationControlToPipeline(bool enabled);
    void SetTesselationEvaluationToPipeline(bool enabled);
    void SetGeometryToPipeline(bool enabled);
    void AddVertexParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType);
    void AddFragmentParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType);
    void AddTesselationControlParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType);
    void AddTesselationEvaluationParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType);
    void AddGeometryParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType);
    void SetVertexMain(const std::string &main);
    void SetFragmentMain(const std::string &main);
    void SetTesselationControlMain(const std::string &main);
    void SetTesselationEvaluationMain(const std::string &main);
    void SetGeometryMain(const std::string &main);
    std::optional<Shader> Generate();
};
