#include <vector>
#include <string>
#include "ShaderObject.hpp"
#include "Shader.hpp"

enum class ShaderParameterType{
    
};

// Intermediary representation of shader program. Compound by shader objects
class ShaderCode{
private:
    std::vector<ShaderObject> shaderObjects;
    ShaderObject vertexShader;
    ShaderObject fragmentShader;
    ShaderObject tesselationControlShader;
    ShaderObject tesselationEvaluationShader;
    ShaderObject geometryShader;
public:
    bool AttachObject(const ShaderObject &shaderObject);
    Shader Generate();
};
