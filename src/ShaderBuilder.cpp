#include "ShaderBuilder.hpp"

const std::string ShaderBuilder::GLSLTypeToString(ShaderDataType type){ 
    switch (type)
    {
        case ShaderDataType::Bool: return "bool";
        case ShaderDataType::Int: return "int";
        //case ShaderDataType::UInt: return "uint";
        case ShaderDataType::Float: return "float";
        //case ShaderDataType::Double: return "double";
        //case ShaderDataType::Bool2: return "bvec2";
        case ShaderDataType::Int2: return "ivec2";
        //case ShaderDataType::UInt2: return "uvec2";
        case ShaderDataType::Float2: return "vec2";
        //case ShaderDataType::Double2: return "dvec2";
        //case ShaderDataType::Bool3: return "bvec3";
        case ShaderDataType::Int3: return "ivec3";
        //case ShaderDataType::UInt3: return "uvec3";
        case ShaderDataType::Float3: return "vec3";
        //case ShaderDataType::Double3: return "dvec3";
        case ShaderDataType::Float4: return "vec4";
        case ShaderDataType::Mat4: return "mat4";
        default: return "undefined"; //It will cause shader compilation error
    }
}
ShaderBuilder::ShaderBuilder(bool clearOnBuild){
    this->clearOnBuild = clearOnBuild;
}
ShaderBuilder& ShaderBuilder::SetVersion(int version){
    if(version == 0)
        version = 330;
    this->version = version;
    versionStream << "#version " << version << "\n";
    return *this;
}

ShaderBuilder& ShaderBuilder::SetShaderType(GLenum shaderType){
    this->shaderType = shaderType;
    return *this;
}

ShaderBuilder& ShaderBuilder::AddInput(ShaderDataType type, const std::string & inputName)
{
    std::string typeString = GLSLTypeToString(type);
    attributesStream << "in " << typeString << " " << inputName << ";\n";
    return *this;
}

ShaderBuilder& ShaderBuilder::AddInput(int location, ShaderDataType type, const std::string & inputName)
{
    std::string typeString = GLSLTypeToString(type);
    attributesStream << "layout (location = " << location << ") in " << typeString << " " << inputName << ";\n";
    return *this;
}
ShaderBuilder& ShaderBuilder::AddUniform(ShaderDataType type, const std::string &uniformName){
    std::string typeString = GLSLTypeToString(type);
    attributesStream << "uniform " << typeString << " " << uniformName << ";\n";
    return *this;
}
ShaderBuilder& ShaderBuilder::AddOutput(ShaderDataType type, const std::string & outputName)
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
    std::string source = versionStream.str() + attributesStream.str() + mainStream.str();
    shaderObject.CompileShaderObject(source);
    if(clearOnBuild){
        versionStream = std::stringstream();
        attributesStream = std::stringstream();
        mainStream = std::stringstream();
    }
    return shaderObject;
}
