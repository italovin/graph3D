#include "ShaderCode.hpp"

const std::string ShaderCode::GLSLTypeToString(ShaderDataType type){ 
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

void ShaderCode::SetVertexToPipeline(bool enabled)
{
    vertexShader.enabled = enabled;
}

void ShaderCode::SetFragmentToPipeline(bool enabled)
{
    fragmentShader.enabled = enabled;
}

void ShaderCode::SetTesselationControlToPipeline(bool enabled)
{
    tesselationControlShader.enabled = enabled;
}

void ShaderCode::SetTesselationEvaluationToPipeline(bool enabled)
{
    tesselationEvaluationShader.enabled = enabled;
}

void ShaderCode::SetGeometryToPipeline(bool enabled)
{
    geometryShader.enabled = enabled;
}

void ShaderCode::AddVertexParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType)
{
    ShaderCodeParameter parameter;
    parameter.dataType = dataType;
    parameter.qualifierType = qualifierType;
    vertexShader.parameters[name] = parameter; 
}

void ShaderCode::AddFragmentParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType)
{
    ShaderCodeParameter parameter;
    parameter.dataType = dataType;
    parameter.qualifierType = qualifierType;
    fragmentShader.parameters[name] = parameter; 
}

void ShaderCode::AddTesselationControlParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType)
{
    ShaderCodeParameter parameter;
    parameter.dataType = dataType;
    parameter.qualifierType = qualifierType;
    tesselationControlShader.parameters[name] = parameter; 
}

void ShaderCode::AddTesselationEvaluationParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType)
{
    ShaderCodeParameter parameter;
    parameter.dataType = dataType;
    parameter.qualifierType = qualifierType;
    tesselationEvaluationShader.parameters[name] = parameter; 
}

void ShaderCode::AddGeometryParameter(const std::string &name, ShaderDataType dataType, ShaderQualifierType qualifierType)
{
    ShaderCodeParameter parameter;
    parameter.dataType = dataType;
    parameter.qualifierType = qualifierType;
    geometryShader.parameters[name] = parameter; 
}

Shader ShaderCode::Generate()
{
    std::vector<ShaderObject> shaderObjects;
    // Reserve for vertex and fragment shader
    shaderObjects.reserve(2);
    
    if(vertexShader.enabled)
        shaderObjects.emplace_back(ShaderObject(GL_VERTEX_SHADER));

    if(tesselationControlShader.enabled && tesselationEvaluationShader.enabled){
        shaderObjects.emplace_back(ShaderObject(GL_TESS_CONTROL_SHADER));
        shaderObjects.emplace_back(ShaderObject(GL_TESS_EVALUATION_SHADER));
    } else if(!tesselationControlShader.enabled && tesselationEvaluationShader.enabled){
        shaderObjects.emplace_back(ShaderObject(GL_TESS_EVALUATION_SHADER));
    } else {
        return Shader();
    }

    if(geometryShader.enabled)
        shaderObjects.emplace_back(ShaderObject(GL_GEOMETRY_SHADER));

    if(fragmentShader.enabled)
        shaderObjects.emplace_back(ShaderObject(GL_FRAGMENT_SHADER));

    for(int i = 0; i < shaderObjects.size(); i++){
        std::string versionString = "#version " + std::to_string(version);
        std::string outsideString;
        if(shaderObjects[i].GetType() == GL_VERTEX_SHADER){
            for(auto &&parameter : vertexShader.parameters){
                 
            }
        }
    }
    
    Shader shader = Shader(shaderObjects, false);
    return shader;
}
