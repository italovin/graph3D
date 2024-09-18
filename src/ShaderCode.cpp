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

void ShaderCode::SetStageToPipeline(ShaderStage shaderStage, bool enabled){
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.enabled = enabled; break;
        case ShaderStage::TesselationControl : tesselationControlShader.enabled = enabled; break;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.enabled = enabled; break;
        case ShaderStage::Geometry : geometryShader.enabled = enabled; break;
        case ShaderStage::Fragment : fragmentShader.enabled = enabled; break;
        default: return;
    }
}

void ShaderCode::AddVertexAttribute(const std::string &name, ShaderDataType dataType, std::optional<int> location)
{
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    vertexShader.inputs[name] = parameter; 
}

void ShaderCode::AddOutput(ShaderStage shaderStage, const std::string &name, ShaderDataType dataType, std::optional<int> location){
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.outputs[name] = parameter; break;
        case ShaderStage::TesselationControl : tesselationControlShader.outputs[name] = parameter; break;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.outputs[name] = parameter; break;
        case ShaderStage::Geometry : geometryShader.outputs[name] = parameter; break;
        case ShaderStage::Fragment : fragmentShader.outputs[name] = parameter; break;
        default: return;
    }
}

void ShaderCode::AddUniform(ShaderStage shaderStage, const std::string &name, ShaderDataType dataType, std::optional<int> location){
    ShaderCodeParameter parameter;
    parameter.location = location;
    parameter.dataType = dataType;
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.uniforms[name] = parameter; break;
        case ShaderStage::TesselationControl : tesselationControlShader.uniforms[name] = parameter; break;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.uniforms[name] = parameter; break;
        case ShaderStage::Geometry : geometryShader.uniforms[name] = parameter; break;
        case ShaderStage::Fragment : fragmentShader.uniforms[name] = parameter; break;
        default: return;
    }
}

void ShaderCode::AddUniformBlock(ShaderStage shaderStage, const std::string &name, const std::string &body){
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.uniformBlocks[name] = body; break;
        case ShaderStage::TesselationControl : tesselationControlShader.uniformBlocks[name] = body; break;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.uniformBlocks[name] = body; break;
        case ShaderStage::Geometry : geometryShader.uniformBlocks[name] = body; break;
        case ShaderStage::Fragment : fragmentShader.uniformBlocks[name] = body; break;
        default: return;
    }
}

void ShaderCode::SetMain(ShaderStage shaderStage, const std::string &main){
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.main = main; break;
        case ShaderStage::TesselationControl : tesselationControlShader.main = main; break;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.main = main; break;
        case ShaderStage::Geometry : geometryShader.main = main; break;
        case ShaderStage::Fragment : fragmentShader.main = main; break;
        default: return;
    }
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
    for(auto &&body : shaderStageCode.uniformBlocks){
        outsideString += "layout (std140) uniform " + body.first + "{\n" + body.second + "};\n";
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
    Shader shader(shaderObjects);
    for(auto &&shaderObject : shaderObjects){
        shaderObject.Delete();
    }
    return shader;
}

std::unordered_map<std::string, ShaderCodeParameter> ShaderCode::GetFragmentUniforms() const{
    return fragmentShader.uniforms;
}
