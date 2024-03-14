#include "ShaderBuilder.hpp"

std::string ShaderBuilder::GLSLTypeToString(GLSL_TYPE type){ 
    std::string string;
    switch (type)
    {
    case BOOL:
        string = "bool";
        break;
    case INT:
        string = "int";
        break;
    case UINT:
        string = "uint";
        break;
    case FLOAT:
        string = "float";
        break;
    case DOUBLE:
        string = "double";
        break;
    case BVEC2:
        string = "bvec2";
        break;
    case IVEC2:
        string = "ivec2";
        break;
    case UVEC2:
        string = "uvec2";
        break;
    case VEC2:
        string = "vec2";
        break;
    case DVEC2:
        string = "dvec2";
        break;
    case BVEC3:
        string = "bvec3";
        break;
    case IVEC3:
        string = "ivec3";
        break;
    case UVEC3:
        string = "uvec3";
        break;
    case VEC3:
        string = "vec3";
        break;
    case DVEC3:
        string = "dvec3";
        break;
    case VEC4:
        string = "vec4";
        break;
    case MAT4:
        string = "mat4";
        break;
    }
    return string;
}
ShaderBuilder::ShaderBuilder(bool clearOnBuild){
    this->clearOnBuild = clearOnBuild;
}
ShaderBuilder& ShaderBuilder::SetVersion(int version){
    if(version == 0)
        version = 330;
    this->version = version;
    attributesStream << "#version " << version << "\n";
    return *this;
}

ShaderBuilder& ShaderBuilder::SetShaderType(GLenum shaderType){
    this->shaderType = shaderType;
    return *this;
}

ShaderBuilder& ShaderBuilder::AddInput(GLSL_TYPE type, const std::string & inputName)
{
    std::string typeString = GLSLTypeToString(type);
    attributesStream << "in " << typeString << " " << inputName << ";\n";
    return *this;
}

ShaderBuilder& ShaderBuilder::AddInput(int location, GLSL_TYPE type, const std::string & inputName)
{
    std::string typeString = GLSLTypeToString(type);
    attributesStream << "layout (location = " << location << ") in " << typeString << " " << inputName << ";\n";
    return *this;
}
ShaderBuilder& ShaderBuilder::AddUniform(GLSL_TYPE type, const std::string &uniformName){
    std::string typeString = GLSLTypeToString(type);
    attributesStream << "uniform " << typeString << " " << uniformName << ";\n";
    return *this;
}
ShaderBuilder& ShaderBuilder::AddOutput(GLSL_TYPE type, const std::string & outputName)
{
    std::string typeString = GLSLTypeToString(type);
    attributesStream << "out " << typeString << " " << outputName << ";\n";
    return *this;
}

ShaderBuilder& ShaderBuilder::SetMain(const std::string &body){
    mainStream = std::stringstream("void main(){\n" + body + "}\n");
    return *this;
}


ShaderObject ShaderBuilder::Build(){
    ShaderObject shaderObject(shaderType);
    std::string source = attributesStream.str() + mainStream.str();
    shaderObject.CompileShaderObject(source);
    if(clearOnBuild){
        attributesStream = std::stringstream();
        mainStream = std::stringstream();
    }
    return shaderObject;
}
