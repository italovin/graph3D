#include "ShaderCode.hpp"
#include <algorithm>
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
const std::string ShaderCode::GLSLMatTypeToString(MaterialParameterType type){
    switch(type){
        case MaterialParameterType::Boolean: return "bool";
        case MaterialParameterType::Float: return "float";
        case MaterialParameterType::Vector4: return "vec4";
        case MaterialParameterType::Map: return "sampler2DArray";
        default: return "undefined";
    }
}

void ShaderCode::SetVersion(int version){
    this->version = version;
}

void ShaderCode::AddExtension(const std::string &extension){
    this->extensions.emplace_back(extension);
}

void ShaderCode::SetAdditionalOutsideString(const std::string &additionalOutsideString){
    this->additionalOutsideString = additionalOutsideString;
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
        case ShaderStage::Vertex : vertexShader.materialProperties.first = structType; return structType;
        case ShaderStage::TesselationControl : tesselationControlShader.materialProperties.first = structType; return structType;
        case ShaderStage::TesselationEvaluation : tesselationEvaluationShader.materialProperties.first = structType; return structType;
        case ShaderStage::Geometry : geometryShader.materialProperties.first = structType; return structType;
        case ShaderStage::Fragment : fragmentShader.materialProperties.first = structType; return structType;
        default: return std::string();
    }
    return structType;
}

void ShaderCode::AddMaterialFloatToStruct(const std::string &structType, ShaderStage shaderStage, const std::string &name, float defaultValue){
    MaterialParameter parameter;
    parameter.type = MaterialParameterType::Float;
    parameter.data = defaultValue;
    int toAddSize = 4;
    PushMaterialParameter(shaderStage, name, parameter, toAddSize);
}

void ShaderCode::AddMaterialBoolToStruct(const std::string &structType, ShaderStage shaderStage, const std::string &name, bool defaultValue){
    MaterialParameter parameter;
    parameter.type = MaterialParameterType::Boolean;
    parameter.data = defaultValue;
    int toAddSize = 4;
    PushMaterialParameter(shaderStage, name, parameter, toAddSize);
}

void ShaderCode::AddMaterialVec4ToStruct(const std::string &structType, ShaderStage shaderStage, const std::string &name, glm::vec4 defaultValue){
    MaterialParameter parameter;
    parameter.type = MaterialParameterType::Vector4;
    parameter.data = defaultValue;
    int toAddSize = 16;
    PushMaterialParameter(shaderStage, name, parameter, toAddSize);
}

void ShaderCode::AddMaterialMapArray(ShaderStage shaderStage, const std::string &name, Ref<Texture> defaultValue){
    switch(shaderStage){
        case ShaderStage::Vertex :
        if(vertexShader.materialTexturesProperties.count(name) == 0){
            vertexShader.materialTexturesProperties.emplace(std::make_pair(name, defaultValue));
            vertexShader.materialTexturesPropertiesOrder.push_back(std::make_pair(name, defaultValue));
        } break;
        case ShaderStage::TesselationControl :
        if(tesselationControlShader.materialTexturesProperties.count(name) == 0){
            tesselationControlShader.materialTexturesProperties.emplace(std::make_pair(name, defaultValue));
            tesselationControlShader.materialTexturesPropertiesOrder.push_back(std::make_pair(name, defaultValue));
        } break;
        case ShaderStage::TesselationEvaluation :
        if(tesselationEvaluationShader.materialTexturesProperties.count(name) == 0){
            tesselationEvaluationShader.materialTexturesProperties.emplace(std::make_pair(name, defaultValue));
            tesselationEvaluationShader.materialTexturesPropertiesOrder.push_back(std::make_pair(name, defaultValue));
        } break;
        case ShaderStage::Geometry :
        if(geometryShader.materialTexturesProperties.count(name) == 0){
            geometryShader.materialTexturesProperties.emplace(std::make_pair(name, defaultValue));
            geometryShader.materialTexturesPropertiesOrder.push_back(std::make_pair(name, defaultValue));
        } break;
        case ShaderStage::Fragment : if(fragmentShader.materialTexturesProperties.count(name) == 0){
            fragmentShader.materialTexturesProperties.emplace(std::make_pair(name, defaultValue));
            fragmentShader.materialTexturesPropertiesOrder.push_back(std::make_pair(name, defaultValue));
        } break;
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

    if(!shaderStageCode.materialPropertiesOrder.empty()){
        outsideString += "struct " + shaderStageCode.materialProperties.first + "{\n";
        for(auto &&matParameter : shaderStageCode.materialPropertiesOrder){
            outsideString += GLSLMatTypeToString(matParameter.second.type) + " " + matParameter.first + ";\n";
        }
        outsideString += "};\n";
    }

    for(auto &&textureProperty : shaderStageCode.materialTexturesProperties){
        outsideString += "uniform sampler2DArray " + textureProperty.first + ";\n";
    }

    if(!shaderStageCode.materialParametersUniformBlock.first.empty()){
        outsideString += "layout (std140) uniform " + shaderStageCode.materialParametersUniformBlock.first + "{\n" + shaderStageCode.materialParametersUniformBlock.second + "\n};\n";
    }

    for(auto &&parameter : shaderStageCode.uniforms){
        outsideString += "uniform " + GLSLTypeToString(parameter.second.dataType) +
        + " " + parameter.first + ";\n";
    }


    for(auto &&uniformBlock : shaderStageCode.uniformBlocks){
        outsideString += "layout (std140) uniform " + uniformBlock.first + "{\n" + uniformBlock.second + "};\n";
    }
}

void ShaderCode::SortMaterialParameters(ShaderStageCode &shaderStageCode){
    auto compLambda = [](const std::pair<std::string, MaterialParameter> &a, const std::pair<std::string, MaterialParameter> &b){
        size_t aAlignment = 0;
        size_t bAlignment = 0;
        switch(a.second.type){
            case MaterialParameterType::Boolean: aAlignment = 4; break;
            case MaterialParameterType::Float: aAlignment = 4; break;
            case MaterialParameterType::Vector4: aAlignment = 16; break;
            default: break;
        }
        switch(b.second.type){
            case MaterialParameterType::Boolean: bAlignment = 4; break;
            case MaterialParameterType::Float: bAlignment = 4; break;
            case MaterialParameterType::Vector4: bAlignment = 16; break;
            default: break;
        }
        return aAlignment > bAlignment;
    };
    std::sort(shaderStageCode.materialPropertiesOrder.begin(), shaderStageCode.materialPropertiesOrder.end(), compLambda);
}

void ShaderCode::PushMaterialParameter(ShaderStage shaderStage, const std::string &name, const MaterialParameter &parameter, int toAddSize){
    auto checkIfNotGreatherMax = [this](int value){
        return value <= maxUniformBlockArrayElementSize;
    };
    switch(shaderStage){
        case ShaderStage::Vertex : if(!checkIfNotGreatherMax(vertexShader.materialParametersSpaceUsed + toAddSize)) return;
        if(vertexShader.materialProperties.second.count(name) == 0){
            vertexShader.materialProperties.second.emplace(std::make_pair(name,parameter));
            vertexShader.materialPropertiesOrder.push_back(std::make_pair(name,parameter));
            vertexShader.materialParametersSpaceUsed += toAddSize;
        } break;
        case ShaderStage::TesselationControl : if(!checkIfNotGreatherMax(tesselationControlShader.materialParametersSpaceUsed + toAddSize)) return;
        if(tesselationControlShader.materialProperties.second.count(name) == 0){
            tesselationControlShader.materialProperties.second.emplace(std::make_pair(name,parameter));
            tesselationControlShader.materialPropertiesOrder.push_back(std::make_pair(name,parameter));
            tesselationControlShader.materialParametersSpaceUsed += toAddSize;
        } break;
        case ShaderStage::TesselationEvaluation :
        if(!checkIfNotGreatherMax(tesselationEvaluationShader.materialParametersSpaceUsed + toAddSize)) return;
        if(tesselationEvaluationShader.materialProperties.second.count(name) == 0){
            tesselationEvaluationShader.materialProperties.second.emplace(std::make_pair(name,parameter));
            tesselationEvaluationShader.materialPropertiesOrder.push_back(std::make_pair(name,parameter));
            tesselationEvaluationShader.materialParametersSpaceUsed += toAddSize;
        }  break;
        case ShaderStage::Geometry : if(!checkIfNotGreatherMax(geometryShader.materialParametersSpaceUsed + toAddSize)) return;
        if(geometryShader.materialProperties.second.count(name) == 0){
            geometryShader.materialProperties.second.emplace(std::make_pair(name,parameter));
            geometryShader.materialPropertiesOrder.push_back(std::make_pair(name,parameter));
            geometryShader.materialParametersSpaceUsed += toAddSize;
        }  break;
        case ShaderStage::Fragment : if(!checkIfNotGreatherMax(fragmentShader.materialParametersSpaceUsed + toAddSize)) return;
       if(fragmentShader.materialProperties.second.count(name) == 0){
            fragmentShader.materialProperties.second.emplace(std::make_pair(name,parameter));
            fragmentShader.materialPropertiesOrder.push_back(std::make_pair(name,parameter));
            fragmentShader.materialParametersSpaceUsed += toAddSize;
        }  break;
        default: return;
    }
}

Ref<GL::ShaderGL> ShaderCode::Generate()
{
    std::vector<GL::ShaderObjectGL> shaderObjects;
    // Reserve for vertex and fragment shader
    shaderObjects.reserve(2);
    
    // Vertex Shader is needed
    if(vertexShader.enabled)
        shaderObjects.emplace_back(GL::ShaderObjectGL(GL_VERTEX_SHADER));
    else
        return nullptr;

    if(tesselationControlShader.enabled && tesselationEvaluationShader.enabled){
        shaderObjects.emplace_back(GL::ShaderObjectGL(GL_TESS_CONTROL_SHADER));
        shaderObjects.emplace_back(GL::ShaderObjectGL(GL_TESS_EVALUATION_SHADER));
    } else if(!tesselationControlShader.enabled && tesselationEvaluationShader.enabled){
        shaderObjects.emplace_back(GL::ShaderObjectGL(GL_TESS_EVALUATION_SHADER));
    } else if(tesselationControlShader.enabled && !tesselationEvaluationShader.enabled){
        // Tess control cannot be used without tess evaluation
        return nullptr;
    }

    if(geometryShader.enabled)
        shaderObjects.emplace_back(GL::ShaderObjectGL(GL_GEOMETRY_SHADER));

    if(fragmentShader.enabled)
        shaderObjects.emplace_back(GL::ShaderObjectGL(GL_FRAGMENT_SHADER));
    std::string outsideStringInsPrevious;
    for(size_t i = 0; i < shaderObjects.size(); i++){
        std::string versionString = "#version " + std::to_string(version) + "\n";
        std::string extensionsString;
        for(auto &&extension : extensions){
            extensionsString += "#extension " + extension + " : enable\n";
        }
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
            shaderSource = versionString + extensionsString + outsideString + additionalOutsideString +
            "void main(){\n" +
            mainString + "\n}";
        } else {
            shaderSource = versionString + extensionsString + outsideString + outsideStringInsPrevious +
            "void main(){\n" +
            mainString + "\n}";
        }
        outsideStringInsPrevious = outsideStringIns;
        shaderObjects[i].Compile(shaderSource);
    }
    Ref<GL::ShaderGL> shader = CreateRef<GL::ShaderGL>(shaderObjects);
    return shader;
}

const std::unordered_map<std::string, ShaderCodeParameter> &ShaderCode::GetUniforms(ShaderStage shaderStage){
    switch(shaderStage){
        case ShaderStage::Vertex : return vertexShader.uniforms;
        case ShaderStage::TesselationControl : return tesselationControlShader.uniforms;
        case ShaderStage::TesselationEvaluation : return tesselationEvaluationShader.uniforms;
        case ShaderStage::Geometry : return geometryShader.uniforms;
        case ShaderStage::Fragment : return fragmentShader.uniforms;
        default: return vertexShader.uniforms;
    }
}

std::vector<std::pair<std::string, MaterialParameter>>
ShaderCode::GetMaterialParameters(ShaderStage shaderStage)
{
    switch(shaderStage){
        case ShaderStage::Vertex : SortMaterialParameters(vertexShader); return vertexShader.materialPropertiesOrder;
        case ShaderStage::TesselationControl : SortMaterialParameters(tesselationControlShader); return tesselationControlShader.materialPropertiesOrder;
        case ShaderStage::TesselationEvaluation : SortMaterialParameters(tesselationEvaluationShader); return tesselationEvaluationShader.materialPropertiesOrder;
        case ShaderStage::Geometry : SortMaterialParameters(geometryShader); return geometryShader.materialPropertiesOrder;
        case ShaderStage::Fragment : SortMaterialParameters(fragmentShader); return fragmentShader.materialPropertiesOrder;
        default: return std::vector<std::pair<std::string, MaterialParameter>>();
    }
}

std::vector<std::pair<std::string, Ref<Texture>>>
ShaderCode::GetMaterialTexturesProperties(ShaderStage shaderStage) const
{
    switch(shaderStage){
        case ShaderStage::Vertex : return vertexShader.materialTexturesPropertiesOrder;
        case ShaderStage::TesselationControl : return tesselationControlShader.materialTexturesPropertiesOrder;
        case ShaderStage::TesselationEvaluation : return tesselationEvaluationShader.materialTexturesPropertiesOrder;
        case ShaderStage::Geometry : return geometryShader.materialTexturesPropertiesOrder;
        case ShaderStage::Fragment : return fragmentShader.materialTexturesPropertiesOrder;
        default: return std::vector<std::pair<std::string, Ref<Texture>>>();
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
