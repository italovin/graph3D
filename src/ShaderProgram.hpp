#include <GL/glew.h>
#include "ShaderObject.hpp"

class ShaderProgram {
private:
    unsigned int handle;

public:
    ShaderProgram();
    void AttachShaderObject(ShaderObject shaderObject);
    void Link();
    void Use();
};