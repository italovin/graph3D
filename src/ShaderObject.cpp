#include "ShaderObject.hpp"

void ShaderObject::ConstructCore(GLenum shaderType){
    this->shaderType = shaderType;
    handle = glCreateShader(shaderType);
}

ShaderObject::ShaderObject(GLenum shaderType){
    debugInfo = false;
    ConstructCore(shaderType);
}

ShaderObject::ShaderObject(GLenum shaderType, const char *shaderPath){
    debugInfo = false;
    ConstructCore(shaderType);
    SetupShaderObject(shaderPath);
}

ShaderObject::ShaderObject(GLenum shaderType, const std::string &shaderPath){
    debugInfo = false;
    ConstructCore(shaderType);
    SetupShaderObject(shaderPath);
}

ShaderObject::ShaderObject(GLenum shaderType, bool debugInfo){
    this->debugInfo = debugInfo;
    ConstructCore(shaderType);
}

ShaderObject::ShaderObject(GLenum shaderType, const char *shaderPath, bool debugInfo){
    this->debugInfo = debugInfo;
    ConstructCore(shaderType);
    SetupShaderObject(shaderPath);
}

ShaderObject::ShaderObject(GLenum shaderType, const std::string &shaderPath, bool debugInfo){
    this->debugInfo = debugInfo;
    ConstructCore(shaderType);
    SetupShaderObject(shaderPath);
}

void ShaderObject::CompileShaderObject(const std::string source){
    const char * sourceData = source.data();
    glShaderSource(handle, 1, &sourceData, NULL);
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

std::string ShaderObject::LoadShaderSourceCore(const char *shaderPath){
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

std::string ShaderObject::LoadShaderSource(const char *shaderPath){
    return LoadShaderSourceCore(shaderPath);
}
std::string ShaderObject::LoadShaderSource(const std::string shaderPath){
    return LoadShaderSourceCore(shaderPath.c_str());
}
void ShaderObject::SetupShaderObject(const char *shaderPath){
    std::string source = LoadShaderSource(shaderPath);
    CompileShaderObject(source);
}
void ShaderObject::SetupShaderObject(const std::string &shaderPath){
    std::string source = LoadShaderSource(shaderPath);
    CompileShaderObject(source);
}

unsigned int ShaderObject::GetHandle(){
    return handle;
}

void ShaderObject::Delete(){
    glDeleteShader(handle);
}