#include "Shader.hpp"

void Shader::GetUniformsInfo(){
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
            uniform_info.type = type;
            uniforms.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));
        }
    }
}

Shader::Shader(){
    debugInfo = false;
    handle = glCreateProgram();
}

Shader::Shader(bool debugInfo){
    this->debugInfo = debugInfo;
    handle = glCreateProgram();
}
Shader::Shader(const std::vector<ShaderObject> &shaderObjects){
    Shader(std::vector<ShaderObject>(shaderObjects));
}
Shader::Shader(std::vector<ShaderObject> &&shaderObjects){
    debugInfo = false;
    handle = glCreateProgram();
    for (auto &&shaderObject : shaderObjects)
    {
        AttachShaderObject(std::move(shaderObject));
    }
    Link();
    for (auto &&shaderObject : shaderObjects)
    {
        DetachShaderObject(std::move(shaderObject));
    }
}
Shader::Shader(const std::vector<ShaderObject> &shaderObjects, bool debugInfo){
    Shader(std::vector<ShaderObject>(shaderObjects), debugInfo);
}
Shader::Shader(std::vector<ShaderObject> &&shaderObjects, bool debugInfo){
    this->debugInfo = debugInfo;
    handle = glCreateProgram();
    for (auto &&shaderObject : shaderObjects)
    {
        AttachShaderObject(std::move(shaderObject));
    }
    Link();
    for (auto &&shaderObject : shaderObjects)
    {
        DetachShaderObject(std::move(shaderObject));
    }
}
void Shader::AttachShaderObject(const ShaderObject &shaderObject)
{
    glAttachShader(handle, shaderObject.GetHandle());
}
void Shader::AttachShaderObject(ShaderObject &&shaderObject)
{
    glAttachShader(handle, shaderObject.GetHandle());
}
void Shader::Create(){
    handle = glCreateProgram();
}
void Shader::Link(){
    glLinkProgram(handle);
    if(debugInfo){
        GLint success;
        glGetProgramiv(handle, GL_LINK_STATUS, &success);

        if(success == GL_FALSE){
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
void Shader::Use(){
    glUseProgram(handle);
}
GLuint Shader::GetHandle() const{
    return handle;
}
void Shader::SetBool(const std::string &name, bool value) const {
    if(uniforms.count(name) > 0)
        glProgramUniform1i(handle, uniforms.at(name).location, (int)value);
}
void Shader::SetInt(const std::string &name, int value) const{
    if(uniforms.count(name) > 0) 
        glProgramUniform1i(handle, uniforms.at(name).location, value); 
}
void Shader::SetFloat(const std::string &name, float value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform1f(handle, uniforms.at(name).location, value); 
}
void Shader::SetDouble(const std::string &name, double value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform1d(handle, uniforms.at(name).location, value);
}
void Shader::SetMat4Float(const std::string &name, const glm::mat4 &matrix) const{
    if(uniforms.count(name) > 0)    
        glProgramUniformMatrix4fv(handle, uniforms.at(name).location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::SetBlockBinding(const std::string &name, unsigned int bindingPoint) const{
    unsigned int index = glGetUniformBlockIndex(handle, name.c_str());   
    glUniformBlockBinding(handle, index, bindingPoint);
}
void Shader::DetachShaderObject(const ShaderObject &shaderObject)
{
    glDetachShader(handle, shaderObject.GetHandle());
}
void Shader::DetachShaderObject(ShaderObject &&shaderObject)
{
    glDetachShader(handle, shaderObject.GetHandle());
}
void Shader::RemoveShaderObject(const ShaderObject &shaderObject){
    DetachShaderObject(shaderObject);
    shaderObject.Delete();
}

std::unordered_map<std::string, uniform_info> Shader::GetUniforms()
{
    return this->uniforms;
}
