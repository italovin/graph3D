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
void ShaderProgram::SetBool(const std::string &name, bool value) const {
    glProgramUniform1i(handle, glGetUniformLocation(handle, name.c_str()), (int)value);
}
void ShaderProgram::SetInt(const std::string &name, int value) const{ 
    glProgramUniform1i(handle, glGetUniformLocation(handle, name.c_str()), value); 
}
void ShaderProgram::SetFloat(const std::string &name, float value) const{
    glProgramUniform1f(handle, glGetUniformLocation(handle, name.c_str()), value); 
}
void ShaderProgram::SetDouble(const std::string &name, double value) const{
    glProgramUniform1d(handle, glGetUniformLocation(handle, name.c_str()), value);
}
void ShaderProgram::SetMat4Float(const std::string &name, const glm::mat4 &matrix) const{
    glProgramUniformMatrix4fv(handle, glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
void ShaderProgram::DetachShaderObject(ShaderObject shaderObject)
{
    glDetachShader(handle, shaderObject.GetHandle());
}
void ShaderProgram::RemoveShaderObject(ShaderObject shaderObject){
    DetachShaderObject(shaderObject);
    shaderObject.Delete();
}