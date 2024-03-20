#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(){
    debugInfo = false;
    handle = glCreateProgram();
}

ShaderProgram::ShaderProgram(bool debugInfo){
    this->debugInfo = debugInfo;
    handle = glCreateProgram();
}
ShaderProgram::ShaderProgram(const std::vector<ShaderObject> &shaderObjects){
    debugInfo = false;
    handle = glCreateProgram();
    for (auto &&shaderObject : shaderObjects)
    {
        AttachShaderObject(shaderObject);
    }
    Link();
    for (auto &&shaderObject : shaderObjects)
    {
        DetachShaderObject(shaderObject);
    }
}
void ShaderProgram::AttachShaderObject(ShaderObject shaderObject)
{
    glAttachShader(handle, shaderObject.GetHandle());
}
void ShaderProgram::Create(){
    handle = glCreateProgram();
}
void ShaderProgram::Link(){
    glLinkProgram(handle);
    if(debugInfo){
        int  success;
        glGetShaderiv(handle, GL_LINK_STATUS, &success);

        if(!success){
            GLint maxLength = 0;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(handle, maxLength, nullptr, infoLog.data());
            std::cout << "LINK_FAILED:\n" << infoLog.data() << std::endl;
        } else {
            std::cout << "LINK_SUCCESSFUL" << std::endl;
        }
    }
}
void ShaderProgram::Use(){
    glUseProgram(handle);
}
GLuint ShaderProgram::GetHandle(){
    return handle;
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