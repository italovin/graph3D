#include "Material.hpp"

Material::Material(ShaderCode &shaderCode)
{
    this->shaderCode = shaderCode;
    if(parameters.size() > 0)
        DeleteParameters();
    
    auto materialParameters = shaderCode.GetMaterialParametersStruct(ShaderStage::Fragment);
    auto materialGlobalParameters = shaderCode.GetUniforms(ShaderStage::Fragment);
    auto materialGlobalVertexParameters = shaderCode.GetUniforms(ShaderStage::Vertex);
    for(auto &&parameter : materialParameters.second){
        AddParameter(parameter.first, GetParameterType(parameter.second.dataType));
    }
    for(auto &&parameter : materialGlobalParameters){
        AddGlobalParameter(parameter.first, GetParameterType(parameter.second.dataType), true);
    }
    for(auto &&parameter : materialGlobalVertexParameters){
        AddGlobalParameter(parameter.first, GetParameterType(parameter.second.dataType), false);
    }
}

std::optional<std::reference_wrapper<ShaderCode>> Material::GetShaderCode() const{
    return this->shaderCode;
}

void Material::AddParameter(const std::string &name, MaterialParameterType type)
{
    MaterialParameter parameter;
    parameter.data = std::variant<Ref<Texture>, float, bool, glm::vec4>();
    parameter.type = type;
    parameters[name] = parameter;
}

void Material::AddGlobalParameter(const std::string &name, MaterialParameterType type, bool isFragOrVert)
{
    MaterialParameter globalParameter;
    globalParameter.data = std::variant<Ref<Texture>, float, bool, glm::vec4>();
    globalParameter.type = type;
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

void Material::SetParameterMap(const std::string &name, Ref<Texture> value)
{
    if(parameters.count(name) > 0)
        parameters[name].data = value;
}

void Material::SetParameterFloat(const std::string &name, float value)
{
    if(parameters.count(name) > 0)
        parameters[name].data = value;
}

void Material::SetParameterBoolean(const std::string &name, bool value)
{
    if(parameters.count(name) > 0)
        parameters[name].data = value;
}

void Material::SetParameterVector4(const std::string &name, glm::vec4 value)
{
    if(parameters.count(name) > 0)
        parameters[name].data = value;
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
    if(parameters.count(name) == 0)
        return std::nullopt;
    return std::get<Ref<Texture>>(parameters[name].data);
}

std::optional<float> Material::GetParameterFloat(const std::string &name)
{
    if(parameters.count(name) == 0)
        return std::nullopt;
    return std::get<float>(parameters[name].data);
}

std::optional<bool> Material::GetParameterBoolean(const std::string &name)
{
    if(parameters.count(name) == 0)
        return std::nullopt;
    return std::get<bool>(parameters[name].data);
}

std::optional<glm::vec4> Material::GetParameterVector4(const std::string &name)
{
    if(parameters.count(name) == 0)
        return std::nullopt;
    return std::get<glm::vec4>(parameters[name].data);
}

std::unordered_map<std::string, MaterialParameter> Material::GetParameters() const
{
    return this->parameters;
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

std::unordered_map<std::string, MaterialParameter> Material::GetGlobalVertexParameters() const{
    return globalVertexShaderParameters;
}
