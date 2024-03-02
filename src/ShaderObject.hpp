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
    std::string LoadShaderSourceCore(const char* shaderPath);
    std::string LoadShaderSource(const char *shaderPath);
    std::string LoadShaderSource(std::string shaderPath);
    void ConstructCore(GLenum shaderType);
    bool debugInfo;
public:
    ShaderObject(GLenum shaderType);
    ShaderObject(GLenum shaderType, const char* shaderPath);
    ShaderObject(GLenum shaderType, std::string shaderPath);
    ShaderObject(GLenum shaderType, bool debugInfo);
    ShaderObject(GLenum shaderType, const char* shaderPath, bool debugInfo);
    ShaderObject(GLenum shaderType, std::string shaderPath, bool debugInfo);
    void CompileShaderObject(std::string source);
    void SetupShaderObject(const char *shaderPath);
    void SetupShaderObject(std::string shaderPath);
    unsigned int GetHandle();
    void Delete();
};