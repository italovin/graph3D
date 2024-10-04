#include "ShaderCode.hpp"
#include "ShaderTypes.hpp"

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

std::string ShaderCode::CreateStruct(ShaderStage shaderStage, const std::string &structType, const std::string &name)
{
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.regularStructs[structType] = std::unordered_map<std::string, ShaderCodeParameter>(); return structType;
        case ShaderStage::TesselationControl : tesselationControlShader.regularStructs[structType] = std::unordered_map<std::string, ShaderCodeParameter>(); return structType;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.regularStructs[structType] = std::unordered_map<std::string, ShaderCodeParameter>(); return structType;
        case ShaderStage::Geometry : geometryShader.regularStructs[structType] = std::unordered_map<std::string, ShaderCodeParameter>(); return structType;
        case ShaderStage::Fragment : fragmentShader.regularStructs[structType] = std::unordered_map<std::string, ShaderCodeParameter>(); return structType;
        default: return std::string();
    }
    return structType;
}

std::string ShaderCode::DefineMaterialParametersStruct(ShaderStage shaderStage, const std::string &structType)
{
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.materialParametersStruct.first = structType; return structType;
        case ShaderStage::TesselationControl : tesselationControlShader.materialParametersStruct.first = structType; return structType;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.materialParametersStruct.first = structType; return structType;
        case ShaderStage::Geometry : geometryShader.materialParametersStruct.first = structType; return structType;
        case ShaderStage::Fragment : fragmentShader.materialParametersStruct.first = structType; return structType;
        default: return std::string();
    }
    return structType;
}

void ShaderCode::AddMaterialParameterToStruct(const std::string &structType, ShaderStage shaderStage, const std::string &name, ShaderDataType dataType)
{
    ShaderCodeParameter parameter;
    parameter.dataType = dataType;
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.materialParametersStruct.second[name] = parameter; break;
        case ShaderStage::TesselationControl : tesselationControlShader.materialParametersStruct.second[name] = parameter; break;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.materialParametersStruct.second[name] = parameter; break;
        case ShaderStage::Geometry : geometryShader.materialParametersStruct.second[name] = parameter; break;
        case ShaderStage::Fragment : fragmentShader.materialParametersStruct.second[name] = parameter; break;
        default: return;
    }
}

void ShaderCode::UpdateMaterialParameterUniformBlock(ShaderStage shaderStage, const std::string &name, const std::string &body)
{
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.materialParametersUniformBlock.first = name; vertexShader.materialParametersUniformBlock.second = body; break;
        case ShaderStage::TesselationControl : tesselationControlShader.materialParametersUniformBlock.first = name; tesselationControlShader.materialParametersUniformBlock.second = body; break;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.materialParametersUniformBlock.first = name; tesselationEvaluationShader.materialParametersUniformBlock.second = body; break;
        case ShaderStage::Geometry : geometryShader.materialParametersUniformBlock.first = name; geometryShader.materialParametersUniformBlock.second = body; break;
        case ShaderStage::Fragment : fragmentShader.materialParametersUniformBlock.first = name; fragmentShader.materialParametersUniformBlock.second = body; break;
        default: return;
    }
}

void ShaderCode::CreateUniformBlock(ShaderStage shaderStage, const std::string &name, const std::string &body)
{
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
            locationString = "layout (location=" + std::to_string(input.second.location.value()) + ") ";
        
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
            locationString = "layout (location=" + std::to_string(parameter.second.location.value()) + ") ";
        
        // flat string receives 'flat' when the output is int or uint
        std::string flatString;
        if(parameter.second.dataType == ShaderDataType::Int)
            flatString = "flat ";
        outsideString += locationString + flatString + "out " + GLSLTypeToString(parameter.second.dataType) +
        + " " + parameter.first + ";\n";
        outsideStringIns += locationString + flatString  + "in " + GLSLTypeToString(parameter.second.dataType)
        + " " + parameter.first + ";\n";
    }

    if(!shaderStageCode.materialParametersStruct.first.empty()){
        outsideString += "struct " + shaderStageCode.materialParametersStruct.first + "{\n";
        for(auto &&matParameter : shaderStageCode.materialParametersStruct.second){
            outsideString += GLSLTypeToString(matParameter.second.dataType) + " " + matParameter.first + ";\n";
        }
        outsideString += "};\n";
    }

    if(!shaderStageCode.materialParametersUniformBlock.first.empty()){
        outsideString += "layout (std140) uniform " + shaderStageCode.materialParametersUniformBlock.first + "{\n" + shaderStageCode.materialParametersUniformBlock.second + "};\n";
    }

    for(auto &&parameter : shaderStageCode.uniforms){
        outsideString += "uniform " + GLSLTypeToString(parameter.second.dataType) +
        + " " + parameter.first + ";\n";
    }


    for(auto &&uniformBlock : shaderStageCode.uniformBlocks){
        outsideString += "layout (std140) uniform " + uniformBlock.first + "{\n" + uniformBlock.second + "};\n";
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
        // If Vertex Shader
        if(i == 0){
            shaderSource = versionString + outsideString + 
            "void main(){\n" +
            mainString + "\n}";
        } else {
            shaderSource = versionString + outsideString + outsideStringInsPrevious + 
            "void main(){\n" +
            mainString + "\n}";
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

std::unordered_map<std::string, ShaderCodeParameter> ShaderCode::GetUniforms(ShaderStage shaderStage) const{
    switch(shaderStage){
        case ShaderStage::Vertex : return vertexShader.uniforms;
        case ShaderStage::TesselationControl : return tesselationControlShader.uniforms;
        case ShaderStage::TesselationEvaluation : return tesselationEvaluationShader.uniforms;
        case ShaderStage::Geometry : return geometryShader.uniforms;
        case ShaderStage::Fragment : return fragmentShader.uniforms;
        default: return std::unordered_map<std::string, ShaderCodeParameter>();
    }
}

std::pair<std::string, std::unordered_map<std::string, ShaderCodeParameter>> ShaderCode::GetMaterialParametersStruct(ShaderStage shaderStage) const
{
    switch(shaderStage){
        case ShaderStage::Vertex : return vertexShader.materialParametersStruct;
        case ShaderStage::TesselationControl : return tesselationControlShader.materialParametersStruct;
        case ShaderStage::TesselationEvaluation : return tesselationEvaluationShader.materialParametersStruct;
        case ShaderStage::Geometry : return geometryShader.materialParametersStruct;
        case ShaderStage::Fragment : return fragmentShader.materialParametersStruct;
        default: return std::pair<std::string, std::unordered_map<std::string, ShaderCodeParameter>>();
    }
}

void ShaderCode::SetBindingPurpose(ShaderStage shaderStage, const std::string &uniformBlockName, const std::string &purpose){
    switch(shaderStage){
        case ShaderStage::Vertex : vertexShader.uniformBlockBindingPurposes[uniformBlockName] = purpose; break;
        case ShaderStage::TesselationControl : tesselationControlShader.uniformBlockBindingPurposes[uniformBlockName] = purpose; break;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.uniformBlockBindingPurposes[uniformBlockName] = purpose; break;
        case ShaderStage::Geometry : geometryShader.uniformBlockBindingPurposes[uniformBlockName] = purpose; break;
        case ShaderStage::Fragment : fragmentShader.uniformBlockBindingPurposes[uniformBlockName] = purpose; break;
        default: break;
    }
}

std::unordered_map<std::string, std::string> ShaderCode::GetBindingsPurposes(ShaderStage shaderStage) const{
    switch(shaderStage){
        case ShaderStage::Vertex : return vertexShader.uniformBlockBindingPurposes;
        case ShaderStage::TesselationControl : return tesselationControlShader.uniformBlockBindingPurposes;
        case ShaderStage::TesselationEvaluation : return tesselationEvaluationShader.uniformBlockBindingPurposes;
        case ShaderStage::Geometry : return geometryShader.uniformBlockBindingPurposes;
        case ShaderStage::Fragment : return fragmentShader.uniformBlockBindingPurposes;
        default: return std::unordered_map<std::string, std::string>();
    }
}
