#include "Material.hpp"

Material::Material(ShaderCode &shaderCode)
{
    this->shaderCode = shaderCode;
    if(parameters.size() > 0)
        DeleteParameters();
    
    auto uniforms = shaderCode.GetFragmentUniforms();
    for(auto &&uniform : uniforms){
        AddParameter(uniform.first, GetParameterType(uniform.second.dataType));
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
