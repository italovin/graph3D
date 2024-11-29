#include "Material.hpp"
#include "MaterialTypes.hpp"

Material::Material(Ref<Shader> shader)
{
    this->shader = shader;
    if(parameters.size() > 0)
        DeleteParameters();
    
    auto materialMaps = this->shader->GetMaps();
    // auto materialParameters = shader->GetCode().GetMaterialParameters(ShaderStage::Fragment);
    // auto materialTexturesProperties = shader->GetCode().GetMaterialTexturesProperties(ShaderStage::Fragment);
    // auto materialGlobalParameters = shader->GetCode().GetUniforms(ShaderStage::Fragment);
    // auto materialGlobalVertexParameters = shader->GetCode().GetUniforms(ShaderStage::Vertex);

    for(auto &&materialMap : materialMaps){
        // Add every declared shader map. The activated condition matters when processing code
        MaterialParameter matParameter;
        matParameter.type = MaterialParameterType::Map;
        matParameter.data = nullptr;
        AddParameter(materialMap.first, matParameter);
    }
    auto materialFlags = this->shader->GetFlags();
    for(auto &materialFlag : materialFlags){
        flags.push_back(materialFlag);
    }
    // for(auto &&parameter : materialParameters){
    //     AddParameter(parameter.first, parameter.second);
    // }
    // for(auto &&textureProperty : materialTexturesProperties){
    //     MaterialParameter matParameter;
    //     matParameter.type = MaterialParameterType::Map;
    //     matParameter.data = textureProperty.second;
    //     AddParameter(textureProperty.first, matParameter);
    // }
    // for(auto &&parameter : materialGlobalParameters){
    //     AddGlobalParameter(parameter.first, GetParameterType(parameter.second.dataType), true);
    // }
    // for(auto &&parameter : materialGlobalVertexParameters){
    //     AddGlobalParameter(parameter.first, GetParameterType(parameter.second.dataType), false);
    // }
}

Ref<Shader> Material::GetShader(){
    return this->shader;
}

template <typename T>
void Material::SetParameter(const std::string &name, T value){
    auto parameterIt = FindParameter(name);
    if(parameterIt != parameters.end())
        parameterIt->second.data = value;
}

void Material::AddParameter(const std::string &name, const MaterialParameter &parameter)
{
    parameters.emplace_back(name, parameter);
}

void Material::AddGlobalParameter(const std::string &name, MaterialParameterType type, bool isFragOrVert)
{
    MaterialParameter globalParameter;
    globalParameter.type = type;
    switch(globalParameter.type){
        case MaterialParameterType::Float: globalParameter.data = 0.0f; break;
        case MaterialParameterType::Boolean: globalParameter.data = false; break;
        case MaterialParameterType::Vector4: globalParameter.data = glm::vec4(0.0f); break;
        default: return;
    }
    if(isFragOrVert)
        globalShaderParameters[name] = globalParameter;
    else
        globalVertexShaderParameters[name] = globalParameter;
}

void Material::DeleteParameters(){
    parameters.clear();
}

MaterialParameterType Material::GetParameterType(ShaderDataType type){
    switch(type){
        case ShaderDataType::Sampler2DArray : return MaterialParameterType::Map;
        case ShaderDataType::Float : return MaterialParameterType::Float;
        case ShaderDataType::Bool : return MaterialParameterType::Boolean;
        case ShaderDataType::Float4 : return MaterialParameterType::Vector4;
        default: return MaterialParameterType::Vector4;
    }
}

std::vector<std::pair<std::string, MaterialParameter>>::iterator Material::FindParameter(const std::string &name){
    std::vector<std::pair<std::string, MaterialParameter>>::iterator parameterIt = std::find_if(parameters.begin(), parameters.end(), 
    [name](const std::pair<std::string, MaterialParameter> &parameterPair){
        return name == parameterPair.first;
    });
    return parameterIt;
}

void Material::SetParameterMap(const std::string &name, Ref<Texture> value)
{
    SetParameter(name, value);
}

void Material::SetParameterFloat(const std::string &name, float value)
{
    SetParameter(name, value);
}

void Material::SetParameterBoolean(const std::string &name, bool value)
{
    SetParameter(name, value);
}

void Material::SetParameterVector4(const std::string &name, glm::vec4 value)
{
    SetParameter(name, value);
}

void Material::SetFlag(const std::string &name, bool value){
    std::vector<std::pair<std::string, bool>>::iterator flagsIt = std::find_if(flags.begin(), flags.end(),
    [name](const std::pair<std::string, bool> &flagPair){
        return name == flagPair.first;
    });
    if(flagsIt != flags.end())
        flagsIt->second = value;
}

void Material::SetOnGlobalFloatChangeCallback(std::function<void(const std::string&, float)> callback){
    floatGlobalChangeCallback = callback;
}

void Material::SetOnGlobalBooleanChangeCallback(std::function<void(const std::string&, bool)> callback){
    booleanGlobalChangeCallback = callback;
}

void Material::SetOnGlobalVector4ChangeCallback(std::function<void(const std::string&, glm::vec4)> callback){
    vector4GlobalChangeCallback = callback;
}

std::optional<Ref<Texture>> Material::GetParameterMap(const std::string &name)
{
    auto parameterIt = FindParameter(name);
    if(parameterIt == parameters.end())
        return std::nullopt;
    if(parameterIt->second.type != MaterialParameterType::Map)
        return std::nullopt;
    return std::get<Ref<Texture>>(parameterIt->second.data);
}

std::optional<float> Material::GetParameterFloat(const std::string &name)
{
    auto parameterIt = FindParameter(name);
    if(parameterIt == parameters.end())
        return std::nullopt;
    if(parameterIt->second.type != MaterialParameterType::Float)
        return std::nullopt;
    return std::get<float>(parameterIt->second.data);
}

std::optional<bool> Material::GetParameterBoolean(const std::string &name)
{
    auto parameterIt = FindParameter(name);
    if(parameterIt == parameters.end())
        return std::nullopt;
    if(parameterIt->second.type != MaterialParameterType::Boolean)
        return std::nullopt;
    return std::get<bool>(parameterIt->second.data);
}

std::optional<glm::vec4> Material::GetParameterVector4(const std::string &name)
{
    auto parameterIt = FindParameter(name);
    if(parameterIt == parameters.end())
        return std::nullopt;
    if(parameterIt->second.type != MaterialParameterType::Vector4)
        return std::nullopt;
    return std::get<glm::vec4>(parameterIt->second.data);
}

std::vector<std::pair<std::string, MaterialParameter>> Material::GetParameters() const
{
    return this->parameters;
}

std::vector<std::pair<std::string, MaterialParameter>> Material::GetMapParameters() const
{
    std::vector<std::pair<std::string, MaterialParameter>> mapParameters;
    for(auto &&parameter : this->parameters){
        if(parameter.second.type == MaterialParameterType::Map){
            mapParameters.emplace_back(parameter.first, parameter.second);
        }
    }
    return mapParameters;
}

const std::vector<std::pair<std::string, bool>> &Material::GetFlags(){
    return flags;
}

void Material::SetGlobalParameterMap(const std::string &name, Ref<Texture> value)
{
    if(globalShaderParameters.count(name) > 0)
        globalShaderParameters[name].data = value;
}

void Material::SetGlobalParameterFloat(const std::string &name, float value)
{
    if(globalShaderParameters.count(name) > 0)
        globalShaderParameters[name].data = value;
    if(floatGlobalChangeCallback)
        floatGlobalChangeCallback(name, value);
}

void Material::SetGlobalParameterBoolean(const std::string &name, bool value)
{
    if(globalShaderParameters.count(name) > 0)
        globalShaderParameters[name].data = value;
    if(booleanGlobalChangeCallback)
        booleanGlobalChangeCallback(name, value);
}

void Material::SetGlobalParameterVector4(const std::string &name, glm::vec4 value)
{
    if(globalShaderParameters.count(name) > 0)
        globalShaderParameters[name].data = value;
    if(vector4GlobalChangeCallback)
        vector4GlobalChangeCallback(name, value);
}

const std::unordered_map<std::string, MaterialParameter> &Material::GetGlobalVertexParameters() const{
    return globalVertexShaderParameters;
}
