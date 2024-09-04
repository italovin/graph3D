#ifndef SHADER_OBJECT_H
#define SHADER_OBJECT_H
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

class ShaderObject {
private:
    GLuint handle;
    GLenum shaderType;
    std::string LoadShaderSource(const std::string &shaderPath);
    void ConstructCore(GLenum shaderType);
    bool debugInfo;
public:
    ShaderObject(GLenum shaderType);
    ShaderObject(GLenum shaderType, bool debugInfo);
    ShaderObject(GLenum shaderType, const std::string &shaderPath);
    ShaderObject(GLenum shaderType, const std::string &shaderPath, bool debugInfo);
    void Compile(const std::string &source);
    void CompileFromPath(const std::string &shaderPath);
    GLuint GetHandle() const;
    GLenum GetType() const;
    //This method will leave the handle useless
    void Delete() const;
};
#endif