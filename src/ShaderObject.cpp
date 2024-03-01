#include "ShaderObject.hpp"

ShaderObject::ShaderObject(GLenum shaderType){
    this->shaderType = shaderType;
    handle = glCreateShader(shaderType);
}

void ShaderObject::CompileShaderObject(const char *source)
{
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);
    int  success;
    char infoLog[512];
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(handle, 512, NULL, infoLog);
    std::cout << success << ": " << infoLog << std::endl;
}

unsigned int ShaderObject::GetHandle(){
    return handle;
}