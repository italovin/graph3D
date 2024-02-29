#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(){
    handle = glCreateProgram();
}

void ShaderProgram::AttachShaderObject(ShaderObject shaderObject)
{
    glAttachShader(handle, shaderObject.GetHandle());
}
void ShaderProgram::Link(){
    glLinkProgram(handle);
}
void ShaderProgram::Use(){
    glUseProgram(handle);
}