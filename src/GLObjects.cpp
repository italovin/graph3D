#include "GLObjects.hpp"

void GL::ObjectGL::Release(){
    // GL call to delete resource
}

GL::ObjectGL::~ObjectGL(){
    if (this->handle != 0) {
        Release();
        this->handle = 0;
    }
}

GL::ObjectGL::ObjectGL(GL::ObjectGL &&other) : handle(other.handle)
{
    other.handle = 0; //Use the "null" texture for the old object.
}

GL::ObjectGL &GL::ObjectGL::operator=(GL::ObjectGL &&other)
{
    //ALWAYS check for self-assignment.
    if(this != &other)
    {
        if (this->handle != 0) {
            Release();
            this->handle = 0;
        }
        //handle is now 0.
        std::swap(handle, other.handle);
    }
    return *this;
}

GLuint GL::ObjectGL::GetHandle() const{
    return this->handle;
}

// TextureGL

GL::TextureGL::TextureGL(GLenum textureType, GLenum format, GLenum dataType){
    this->textureType = textureType;
    this->format = format;
    this->dataType = dataType;
    this->internalFormat = GL_RGB8;
    if(format == GL_RGB){
        if(dataType == GL_UNSIGNED_BYTE){
            internalFormat = GL_RGB8;
        } else if(dataType == GL_FLOAT){
            internalFormat = GL_RGB32F;
        }
    } else if(format == GL_RGBA){
        if(dataType == GL_UNSIGNED_BYTE){
            internalFormat = GL_RGBA8;
        } else if(dataType == GL_FLOAT){
            internalFormat = GL_RGBA32F;
        }
    }
    glCreateTextures(textureType, 1, &this->handle);
}

void GL::TextureGL::Bind(int texUnit){
    glBindTextureUnit(texUnit, this->handle);
}

void GL::TextureGL::SetupParameters(){
    glTextureParameteri(this->handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTextureParameteri(this->handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTextureParameteri(this->handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(this->handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GL::TextureGL::SetupStorage2D(size_t width, size_t height){
    glTextureStorage2D(this->handle, 1, internalFormat, width, height);
}

void GL::TextureGL::SetupStorage3D(size_t width, size_t height, int layers){
    glTextureStorage3D(this->handle, 1, internalFormat, width, height, layers);
}

void GL::TextureGL::PushData2D(size_t width, size_t height, const std::vector<GLubyte> &pixels){
    glTextureSubImage2D(this->handle, 0, 0, 0, width, height, format, dataType, pixels.data());
}

void GL::TextureGL::PushData2D(size_t width, size_t height, const std::vector<GLfloat> &pixels){
    glTextureSubImage2D(this->handle, 0, 0, 0, width, height, format, dataType, pixels.data());
}

void GL::TextureGL::PushData3D(size_t width, size_t height, const std::vector<std::vector<GLubyte>> &pixels){
    int layers = pixels.size();
    for(int i = 0; i < layers; i++){
        glTextureSubImage3D(this->handle, 0, 0, 0, i, width, height, 1, format, dataType, pixels[i].data());
    }
}

void GL::TextureGL::PushData3D(size_t width, size_t height, const std::vector<std::vector<GLfloat>> &pixels){
    int layers = pixels.size();
    for(int i = 0; i < layers; i++){
        glTextureSubImage3D(this->handle, 0, 0, 0, i, width, height, 1, format, dataType, pixels[i].data());
    }
}

void GL::TextureGL::PushData3DLayer(size_t width, size_t height, int layer, const std::vector<GLubyte> &pixels){
    glTextureSubImage3D(this->handle, 0, 0, 0, layer, width, height, 1, format, dataType, pixels.data());
}

void GL::TextureGL::PushData3DLayer(size_t width, size_t height, int layer, const std::vector<GLfloat> &pixels){
    glTextureSubImage3D(this->handle, 0, 0, 0, layer, width, height, 1, format, dataType, pixels.data());
}

void GL::TextureGL::GenerateMipmap(){
    glGenerateTextureMipmap(this->handle);
}

void GL::TextureGL::Release(){
    glDeleteTextures(1, &this->handle);
}

////

// ShaderObjectGL

void GL::ShaderObjectGL::ConstructCore(GLenum shaderType){
    this->shaderType = shaderType;
    handle = glCreateShader(shaderType);
}

GL::ShaderObjectGL::ShaderObjectGL(GLenum shaderType){
    debugInfo = false;
    ConstructCore(shaderType);
}

GL::ShaderObjectGL::ShaderObjectGL(GLenum shaderType, const std::string &shaderPath){
    debugInfo = false;
    ConstructCore(shaderType);
    CompileFromPath(shaderPath);
}

GL::ShaderObjectGL::ShaderObjectGL(GLenum shaderType, bool debugInfo){
    this->debugInfo = debugInfo;
    ConstructCore(shaderType);
}

GL::ShaderObjectGL::ShaderObjectGL(GLenum shaderType, const std::string &shaderPath, bool debugInfo){
    this->debugInfo = debugInfo;
    ConstructCore(shaderType);
    CompileFromPath(shaderPath);
}

void GL::ShaderObjectGL::Compile(const std::string &source){
    const GLchar * sourceData = source.data();
    glShaderSource(handle, 1, &sourceData, nullptr);
    glCompileShader(handle);
    if(debugInfo){
        int  success;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
        std::string shaderTypeString;
        if(shaderType == GL_VERTEX_SHADER)
            shaderTypeString = "VERTEX_SHADER";
        else if(shaderType == GL_FRAGMENT_SHADER)
            shaderTypeString = "FRAGMENT_SHADER";
        else
            shaderTypeString = "UNDEFINED_SHADER_TYPE";

        if(!success){
            int infoMaxLength = 0;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoMaxLength);
            std::vector<char> infoLog(infoMaxLength);
            glGetShaderInfoLog(handle, infoMaxLength, nullptr, infoLog.data());
            std::cout << shaderTypeString << ": COMPILATION_FAILED\n" << infoLog.data() << std::endl;
        } else {
            std::cout << shaderTypeString << ": COMPILATION_SUCCESSFUL" << std::endl;
        }
    }

}

std::string GL::ShaderObjectGL::LoadShaderSource(const std::string &shaderPath){
    std::ifstream shaderFile;
    std::string shaderCodeString;
    shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try{
        shaderFile.open(shaderPath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderCodeString = shaderStream.str();

        if(debugInfo)
            std::cout << "Shader size in bytes: " << shaderCodeString.size() << std::endl;

        shaderFile.close();
    } catch (std::ifstream::failure &e){
        std::cout << "FILE READ NOT SUCCESSFUL" << std::endl;
    }
    return shaderCodeString;
}

void GL::ShaderObjectGL::CompileFromPath(const std::string &shaderPath){
    std::string source = LoadShaderSource(shaderPath);
    Compile(source);
}

GLenum GL::ShaderObjectGL::GetType() const{
    return shaderType;
}

void GL::ShaderObjectGL::Release(){
    glDeleteShader(this->handle);
}

////

// ShaderGL

void GL::ShaderGL::GetUniformsInfo(){
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

GL::ShaderGL::ShaderGL(){
    debugInfo = false;
    handle = glCreateProgram();
}

GL::ShaderGL::ShaderGL(bool debugInfo){
    this->debugInfo = debugInfo;
    handle = glCreateProgram();
}
GL::ShaderGL::ShaderGL(const std::vector<GL::ShaderObjectGL> &shaderObjects, bool debugInfo){
    this->debugInfo = debugInfo;
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
GL::ShaderGL::ShaderGL(std::vector<GL::ShaderObjectGL> &&shaderObjects, bool debugInfo){
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
void GL::ShaderGL::AttachShaderObject(const GL::ShaderObjectGL &shaderObject)
{
    glAttachShader(handle, shaderObject.GetHandle());
}
void GL::ShaderGL::AttachShaderObject(GL::ShaderObjectGL &&shaderObject)
{
    glAttachShader(handle, shaderObject.GetHandle());
}
void GL::ShaderGL::Create(){
    handle = glCreateProgram();
}
void GL::ShaderGL::Link(){
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
void GL::ShaderGL::Use(){
    glUseProgram(handle);
}
void GL::ShaderGL::SetBool(const std::string &name, bool value) const {
    if(uniforms.count(name) > 0)
        glProgramUniform1i(handle, uniforms.at(name).location, (int)value);
}
void GL::ShaderGL::SetInt(const std::string &name, int value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform1i(handle, uniforms.at(name).location, value);
}
void GL::ShaderGL::SetFloat(const std::string &name, float value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform1f(handle, uniforms.at(name).location, value);
}
void GL::ShaderGL::SetDouble(const std::string &name, double value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform1d(handle, uniforms.at(name).location, value);
}
void GL::ShaderGL::SetVec3(const std::string &name, glm::vec3 value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform3f(handle, uniforms.at(name).location, value.x, value.y, value.z);
}
void GL::ShaderGL::SetVec4(const std::string &name, glm::vec4 value) const{
    if(uniforms.count(name) > 0)
        glProgramUniform4f(handle, uniforms.at(name).location, value.x, value.y, value.z, value.w);
}

void GL::ShaderGL::SetMat4Float(const std::string &name, const glm::mat4 &matrix) const{
    if(uniforms.count(name) > 0)
        glProgramUniformMatrix4fv(handle, uniforms.at(name).location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void GL::ShaderGL::SetBlockBinding(const std::string &name, unsigned int bindingPoint) const{
    unsigned int index = glGetUniformBlockIndex(handle, name.c_str());
    glUniformBlockBinding(handle, index, bindingPoint);
}
void GL::ShaderGL::DetachShaderObject(const GL::ShaderObjectGL &shaderObject)
{
    glDetachShader(handle, shaderObject.GetHandle());
}
void GL::ShaderGL::DetachShaderObject(GL::ShaderObjectGL &&shaderObject)
{
    glDetachShader(handle, shaderObject.GetHandle());
}

std::unordered_map<std::string, GL::ShaderGL::uniform_info> GL::ShaderGL::GetUniforms()
{
    return this->uniforms;
}

void GL::ShaderGL::Release(){
    glDeleteProgram(this->handle);
}

/////

// VertexArrayGL

GL::VertexArrayGL::VertexArrayGL(){
    glCreateVertexArrays(1, &this->handle);
}

void GL::VertexArrayGL::Bind(){
    glBindVertexArray(this->handle);
}

void GL::VertexArrayGL::Release(){
    glDeleteVertexArrays(1, &handle);
}

/////
