#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H
#include <map>
#include <sstream>
#include "ShaderObject.hpp"
#include "ShaderTypes.hpp"

enum class ShaderLayoutStandard{
    std140, std430
};

class ShaderBuilder {
private:
    bool clearOnBuild = false;
    int version = 330;
    GLenum shaderType;
    std::stringstream versionStream;
    std::stringstream attributesStream;
    std::stringstream mainStream;
    const std::string GLSLTypeToString(ShaderDataType type);
public:
    ShaderBuilder(bool clearOnBuild);
    ShaderBuilder& SetVersion(int version);
    ShaderBuilder& SetShaderType(GLenum shaderType);
    ShaderBuilder& AddInput(ShaderDataType type, const std::string &inputName);
    ShaderBuilder& AddInput(int location, ShaderDataType type, const std::string &inputName);
    ShaderBuilder& AddUniform(ShaderDataType type, const std::string &uniformName);
    ShaderBuilder& AddUniformBlock(const std::string &body, const std::string &uniformName);
    ShaderBuilder& AddOutput(ShaderDataType type, const std::string &outputName);
    ShaderBuilder& SetMain(const std::string &body);
    ShaderObject Build();
};
#endif