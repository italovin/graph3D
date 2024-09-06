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

void ShaderCode::SetVertexMain(const std::string &main){
    vertexShader.main = main;
}
void ShaderCode::SetFragmentMain(const std::string &main){
    fragmentShader.main = main;
}
void ShaderCode::SetTesselationControlMain(const std::string &main){
    tesselationControlShader.main = main;
}
void ShaderCode::SetTesselationEvaluationMain(const std::string &main){
    tesselationEvaluationShader.main = main;
}
void ShaderCode::SetGeometryMain(const std::string &main){
    geometryShader.main = main;
}

void ShaderCode::ProcessShaderStageCode(const ShaderStageCode &shaderStageCode, std::string &mainString,
std::string &outsideString, std::string &outsideStringOuts){
    mainString = shaderStageCode.main;
    for(auto &&parameter : shaderStageCode.parameters){
        std::string qualifierString;
        bool isInOut = false;
        if(parameter.second.qualifierType == ShaderQualifierType::In_Out){
            qualifierString = "in";
            isInOut = true;
        } else if(parameter.second.qualifierType == ShaderQualifierType::Uniform)
            qualifierString = "uniform";
        
        outsideString += qualifierString + " " + GLSLTypeToString(parameter.second.dataType) +
        + " " + parameter.first + ";";
        if(isInOut){
            outsideStringOuts += "out " + GLSLTypeToString(parameter.second.dataType) 
            + " " + parameter.first + ";";
        }
    }
}

std::optional<Shader> ShaderCode::Generate()
{
    std::vector<ShaderObject> shaderObjects;
    // Reserve for vertex and fragment shader
    shaderObjects.reserve(2);
    
    // Vertex Shader is needed
    if(vertexShader.enabled)
        shaderObjects.emplace_back(ShaderObject(GL_VERTEX_SHADER));
    else
        return std::nullopt;

    if(tesselationControlShader.enabled && tesselationEvaluationShader.enabled){
        shaderObjects.emplace_back(ShaderObject(GL_TESS_CONTROL_SHADER));
        shaderObjects.emplace_back(ShaderObject(GL_TESS_EVALUATION_SHADER));
    } else if(!tesselationControlShader.enabled && tesselationEvaluationShader.enabled){
        shaderObjects.emplace_back(ShaderObject(GL_TESS_EVALUATION_SHADER));
    } else if(tesselationControlShader.enabled && !tesselationEvaluationShader.enabled){
        // Tess control cannot be used without tess evaluation
        return std::nullopt;
    }

    if(geometryShader.enabled)
        shaderObjects.emplace_back(ShaderObject(GL_GEOMETRY_SHADER));

    if(fragmentShader.enabled)
        shaderObjects.emplace_back(ShaderObject(GL_FRAGMENT_SHADER));

    std::string outsideStringOutsPrevious;
    for(int i = 0; i < shaderObjects.size(); i++){
        std::string versionString = "#version " + std::to_string(version) + "\n";
        std::string mainString;
        std::string outsideString;
        std::string outsideStringOuts;
        if(shaderObjects[i].GetType() == GL_VERTEX_SHADER){
            ProcessShaderStageCode(vertexShader, mainString, outsideString, outsideStringOuts);
        } else if(shaderObjects[i].GetType() == GL_TESS_CONTROL_SHADER){
            ProcessShaderStageCode(tesselationControlShader, mainString, outsideString, outsideStringOuts);
        } else if(shaderObjects[i].GetType() == GL_TESS_EVALUATION_SHADER){
            ProcessShaderStageCode(tesselationEvaluationShader, mainString, outsideString, outsideStringOuts);
        } else if(shaderObjects[i].GetType() == GL_GEOMETRY_SHADER){
            ProcessShaderStageCode(geometryShader, mainString, outsideString, outsideStringOuts);
        } else if(shaderObjects[i].GetType() == GL_FRAGMENT_SHADER){
            ProcessShaderStageCode(fragmentShader, mainString, outsideString, outsideStringOuts);
        }
        if(i == 0){
            std::string shaderSource = versionString + outsideString + 
            "void main(){" +
            mainString + "}";
        } else {
            std::string shaderSource = versionString + outsideString + outsideStringOutsPrevious + 
            "void main(){" +
            mainString + "}";
        }
        outsideStringOutsPrevious = outsideStringOuts;
    }
    
    Shader shader = Shader(shaderObjects, false);
    return shader;
}
