#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H
#include <map>
#include <sstream>
#include "ShaderObject.hpp"

enum GLSL_TYPE {
    BOOL,
    INT,
    UINT,
    FLOAT,
    DOUBLE,
    BVEC2,
    IVEC2,
    UVEC2,
    VEC2,
    DVEC2,
    BVEC3,
    IVEC3,
    UVEC3,
    VEC3,
    DVEC3,
    VEC4,
    MAT4
};

class ShaderBuilder {
private:
    bool clearOnBuild = false;
    int version;
    GLenum shaderType;
    std::stringstream attributesStream;
    std::stringstream mainStream;
    std::string GLSLTypeToString(GLSL_TYPE type);
public:
    ShaderBuilder(bool clearOnBuild);
    ShaderBuilder& SetVersion(int version);
    ShaderBuilder& SetShaderType(GLenum shaderType);
    ShaderBuilder& AddInput(GLSL_TYPE type, const std::string &inputName);
    ShaderBuilder& AddInput(int location, GLSL_TYPE type, const std::string &inputName);
    ShaderBuilder& AddUniform(GLSL_TYPE type, const std::string &uniformName);
    ShaderBuilder& AddOutput(GLSL_TYPE type, const std::string &outputName);
    ShaderBuilder& CreateMain(const std::string &body);
    ShaderObject Build();
};
#endif