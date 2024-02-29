#include "ShaderObject.hpp"

ShaderObject::ShaderObject(GLenum shaderType){
    this->shaderType = shaderType;
    handle = glCreateShader(shaderType);
}

void ShaderObject::CompileShaderObject(const char *source)
{
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);
}

unsigned int ShaderObject::GetHandle(){
    return handle;
}