#include <glad/glad.h>
#include <cstddef>

class ShaderObject {
private:
    unsigned int handle;
    GLenum shaderType;
public:
    ShaderObject(GLenum shaderType);
    void CompileShaderObject(const char* source);
    unsigned int GetHandle();
};