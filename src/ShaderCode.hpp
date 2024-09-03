#include <vector>
#include "ShaderObject.hpp"
#include "Shader.hpp"

// Intermediary representation of shader program. Compound by shader objects
class ShaderCode{
private:
    std::vector<ShaderObject> shaderObjects;

public:
    void AttachObject(const ShaderObject &shaderObject);
    Shader Generate();
};