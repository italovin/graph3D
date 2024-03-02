#include "ShaderObject.hpp"

ShaderObject::ShaderObject(GLenum shaderType){
    this->shaderType = shaderType;
    handle = glCreateShader(shaderType);
}

ShaderObject::ShaderObject(GLenum shaderType, const char *shaderPath){
    this->shaderType = shaderType;
    handle = glCreateShader(shaderType);
    SetupShaderObject(shaderPath);
}

ShaderObject::ShaderObject(GLenum shaderType, std::string shaderPath){
    this->shaderType = shaderType;
    handle = glCreateShader(shaderType);
    SetupShaderObject(shaderPath);
}
void ShaderObject::CompileShaderObject(const char *source){
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);
    
}

void ShaderObject::CompileShaderObject(const char *source, bool compileDebug){
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);
    if(compileDebug){
        int  success;
        char infoLog[512];
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
        const char * shaderTypeString;
        if(shaderType == GL_VERTEX_SHADER)
            shaderTypeString = "VERTEX_SHADER";
        else if(shaderType == GL_FRAGMENT_SHADER)
            shaderTypeString = "FRAGMENT_SHADER";
        else
            shaderTypeString = "UNDEFINED_SHADER_TYPE";
        if(!success){
            glGetShaderInfoLog(handle, 512, NULL, infoLog);
            std::cout << shaderTypeString << ": COMPILATION_FAILED\n" << infoLog << std::endl;
        } else {
            std::cout << shaderTypeString << ": COMPILATION_SUCCESSFUL" << std::endl;
        }
    }
    
    
}

const char *ShaderObject::LoadShaderSourceCore(const char *shaderPath){
    std::ifstream shaderFile;
    std::string shaderCodeString;
    shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try{
        shaderFile.open(shaderPath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCodeString = shaderStream.str();
    } catch (std::ifstream::failure e){
        std::cout << "FILE READ NOT SUCCESSFUL" << std::endl;
    }
    return shaderCodeString.c_str();
}

const char *ShaderObject::LoadShaderSource(const char *shaderPath){
    return LoadShaderSourceCore(shaderPath);
}
const char *ShaderObject::LoadShaderSource(std::string shaderPath){
    return LoadShaderSourceCore(shaderPath.c_str());
}
void ShaderObject::SetupShaderObject(const char *shaderPath){
    const char * source = LoadShaderSource(shaderPath);
    CompileShaderObject(source);
}
void ShaderObject::SetupShaderObject(std::string shaderPath){
    const char * source = LoadShaderSource(shaderPath);
    CompileShaderObject(source);
}

unsigned int ShaderObject::GetHandle(){
    return handle;
}

void ShaderObject::Delete(){
    glDeleteShader(handle);
}