#include <GL/glew.h>
#include <cstddef>
#include <iostream>

class ShaderObject {
private:
    unsigned int handle;
    GLenum shaderType;
public:
    ShaderObject(GLenum shaderType);
    void CompileShaderObject(const char* source);
    void CompileShaderObject(const char* source, bool compileDebug);
    unsigned int GetHandle();
};