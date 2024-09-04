#include "ShaderObject.hpp"

void ShaderObject::ConstructCore(GLenum shaderType){
    this->shaderType = shaderType;
    handle = glCreateShader(shaderType);
}

ShaderObject::ShaderObject(GLenum shaderType){
    debugInfo = false;
    ConstructCore(shaderType);
}

ShaderObject::ShaderObject(GLenum shaderType, const std::string &shaderPath){
    debugInfo = false;
    ConstructCore(shaderType);
    CompileFromPath(shaderPath);
}

ShaderObject::ShaderObject(GLenum shaderType, bool debugInfo){
    this->debugInfo = debugInfo;
    ConstructCore(shaderType);
}

ShaderObject::ShaderObject(GLenum shaderType, const std::string &shaderPath, bool debugInfo){
    this->debugInfo = debugInfo;
    ConstructCore(shaderType);
    CompileFromPath(shaderPath);
}

void ShaderObject::Compile(const std::string &source){
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

std::string ShaderObject::LoadShaderSource(const std::string &shaderPath){
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
    } catch (std::ifstream::failure e){
        std::cout << "FILE READ NOT SUCCESSFUL" << std::endl;
    }
    return shaderCodeString;
}

void ShaderObject::CompileFromPath(const std::string &shaderPath){
    std::string source = LoadShaderSource(shaderPath);
    Compile(source);
}

GLuint ShaderObject::GetHandle() const{
    return handle;
}

GLenum ShaderObject::GetType() const{
    return shaderType;
}

void ShaderObject::Delete() const{
    glDeleteShader(handle);
}