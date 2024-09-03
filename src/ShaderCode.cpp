#include "ShaderCode.hpp"

void ShaderCode::AttachObject(const ShaderObject &shaderObject)
{
    shaderObjects.emplace_back(shaderObject);
}

Shader ShaderCode::Generate()
{
    Shader shader = Shader(shaderObjects, false);
    return shader;
}
