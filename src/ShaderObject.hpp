#include <GL/glew.h>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

class ShaderObject {
private:
    unsigned int handle;
    GLenum shaderType;
public:
    ShaderObject(GLenum shaderType);
    void CompileShaderObject(const char* source);
    void CompileShaderObject(const char* source, bool compileDebug);
    const char * LoadShaderSource(const char * shaderPath);
    unsigned int GetHandle();
};