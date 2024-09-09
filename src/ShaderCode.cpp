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

void ShaderCode::SetVersion(int version){
    this->version = version;
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

void ShaderCode::AddVertexAttribute(const std::string &name, ShaderDataType dataType, std::optional<int> location)
{
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    vertexShader.inputs[name] = parameter; 
}

void ShaderCode::AddVertexOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt)
{
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    vertexShader.outputs[name] = parameter; 
}

void ShaderCode::AddFragmentOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt)
{
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    fragmentShader.outputs[name] = parameter; 
}

void ShaderCode::AddTesselationControlOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt)
{
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    tesselationControlShader.outputs[name] = parameter; 
}

void ShaderCode::AddTesselationEvaluationOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt)
{
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    tesselationEvaluationShader.outputs[name] = parameter; 
}

void ShaderCode::AddGeometryOutput(const std::string &name, ShaderDataType dataType, std::optional<int> location = std::nullopt)
{
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    geometryShader.outputs[name] = parameter; 
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

void ShaderCode::ProcessVertexShaderCode(const ShaderStageCode &shaderStageCode, std::string &outsideString)
{
    for(auto &&input : shaderStageCode.inputs){
        std::string locationString;
        if(input.second.location.has_value())
            locationString = "layout (location=" + std::to_string(input.second.location.value()) + ")";
        
        outsideString += locationString + "in " + GLSLTypeToString(input.second.dataType) 
        + " " + input.first + ";\n";
    }
}

void ShaderCode::ProcessShaderStageCode(const ShaderStageCode &shaderStageCode, std::string &mainString,
std::string &outsideString, std::string &outsideStringIns){
    mainString = shaderStageCode.main;
    for(auto &&parameter : shaderStageCode.outputs){
        std::string locationString;
        if(parameter.second.location.has_value())
            locationString = "layout (location=" + std::to_string(parameter.second.location.value()) + ")";
        
        outsideString += locationString + "out " + GLSLTypeToString(parameter.second.dataType) +
        + " " + parameter.first + ";\n";
        outsideStringIns += locationString + "in " + GLSLTypeToString(parameter.second.dataType) 
        + " " + parameter.first + ";\n";
    }
    for(auto &&parameter : shaderStageCode.uniforms){
        outsideString += "uniform " + GLSLTypeToString(parameter.second.dataType) +
        + " " + parameter.first + ";\n";
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

    std::string outsideStringInsPrevious;
    for(int i = 0; i < shaderObjects.size(); i++){
        std::string versionString = "#version " + std::to_string(version) + "\n";
        std::string mainString;
        std::string outsideString;
        std::string outsideStringIns;
        std::string shaderSource;
        if(shaderObjects[i].GetType() == GL_VERTEX_SHADER){
            ProcessVertexShaderCode(vertexShader, outsideString);
            ProcessShaderStageCode(vertexShader, mainString, outsideString, outsideStringIns);
        } else if(shaderObjects[i].GetType() == GL_TESS_CONTROL_SHADER){
            ProcessShaderStageCode(tesselationControlShader, mainString, outsideString, outsideStringIns);
        } else if(shaderObjects[i].GetType() == GL_TESS_EVALUATION_SHADER){
            ProcessShaderStageCode(tesselationEvaluationShader, mainString, outsideString, outsideStringIns);
        } else if(shaderObjects[i].GetType() == GL_GEOMETRY_SHADER){
            ProcessShaderStageCode(geometryShader, mainString, outsideString, outsideStringIns);
        } else if(shaderObjects[i].GetType() == GL_FRAGMENT_SHADER){
            ProcessShaderStageCode(fragmentShader, mainString, outsideString, outsideStringIns);
        }
        if(i == 0){
            shaderSource = versionString + outsideString + 
            "void main(){" +
            mainString + "}";
        } else {
            shaderSource = versionString + outsideString + outsideStringInsPrevious + 
            "void main(){" +
            mainString + "}";
        }
        outsideStringInsPrevious = outsideStringIns;

        shaderObjects[i].Compile(shaderSource);
    }
    
    Shader shader = Shader(shaderObjects, false);
    return shader;
}

std::unordered_map<std::string, ShaderCodeParameter> ShaderCode::GetFragmentUniforms() const{
    return fragmentShader.uniforms;
}
