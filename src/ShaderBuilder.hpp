#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H
#include <map>
#include <sstream>
#include "ShaderObject.hpp"

enum GLSL_TYPE {
    SH_BOOL,
    SH_INT,
    SH_UINT,
    SH_FLOAT,
    SH_DOUBLE,
    SH_BVEC2,
    SH_IVEC2,
    SH_UVEC2,
    SH_VEC2,
    SH_DVEC2,
    SH_BVEC3,
    SH_IVEC3,
    SH_UVEC3,
    SH_VEC3,
    SH_DVEC3,
    SH_VEC4,
    SH_MAT4
};

class ShaderBuilder {
private:
    bool clearOnBuild = false;
    int version = 330;
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
    ShaderBuilder& SetMain(const std::string &body);
    ShaderObject Build();
};
#endif