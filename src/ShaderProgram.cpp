#include "ShaderProgram.hpp"

void ShaderProgram::GetUniformsInfo(){
    GLint uniform_count = 0;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &uniform_count);

    if (uniform_count != 0)
    {
        GLint 	max_name_len = 0;
        GLsizei length = 0;
        GLsizei count = 0;
        GLenum 	type = GL_NONE;
        glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
        
        auto uniform_name = std::make_unique<char[]>(max_name_len);

        for (GLint i = 0; i < uniform_count; ++i)
        {
            glGetActiveUniform(handle, i, max_name_len, &length, &count, &type, uniform_name.get());

            uniform_info uniform_info = {};
            uniform_info.location = glGetUniformLocation(handle, uniform_name.get());
            uniform_info.count = count;
            uniforms.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));
        }
    }
}

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
    GetUniformsInfo();
}
void ShaderProgram::Use(){
    glUseProgram(handle);
}
GLuint ShaderProgram::GetHandle() const{
    return handle;
}
void ShaderProgram::SetBool(const std::string &name, bool value) const {
    if(uniforms.count(name) > 0)
        glProgramUniform1i(handle, uniforms.at(name).location, (int)value);
}
void ShaderProgram::SetInt(const std::string &name, int value) const{
    if(uniforms.count(name) > 0) 
        glProgramUniform1i(handle, uniforms.at(name).location, value); 
}
void ShaderProgram::SetFloat(const std::string &name, float value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform1f(handle, uniforms.at(name).location, value); 
}
void ShaderProgram::SetDouble(const std::string &name, double value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform1d(handle, uniforms.at(name).location, value);
}
void ShaderProgram::SetMat4Float(const std::string &name, const glm::mat4 &matrix) const{
    if(uniforms.count(name) > 0)    
        glProgramUniformMatrix4fv(handle, uniforms.at(name).location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void ShaderProgram::DetachShaderObject(ShaderObject shaderObject)
{
    glDetachShader(handle, shaderObject.GetHandle());
}
void ShaderProgram::RemoveShaderObject(ShaderObject shaderObject){
    DetachShaderObject(shaderObject);
    shaderObject.Delete();
}