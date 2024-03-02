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
    const char* LoadShaderSourceCore(const char* shaderPath);
public:
    ShaderObject(GLenum shaderType);
    ShaderObject(GLenum shaderType, const char* shaderPath);
    ShaderObject(GLenum shaderType, std::string shaderPath);
    void CompileShaderObject(const char *source);
    void CompileShaderObject(const char *source, bool compileDebug);
    const char* LoadShaderSource(const char *shaderPath);
    const char* LoadShaderSource(std::string shaderPath);
    void SetupShaderObject(const char *shaderPath);
    void SetupShaderObject(std::string shaderPath);
    unsigned int GetHandle();
    void Delete();
};