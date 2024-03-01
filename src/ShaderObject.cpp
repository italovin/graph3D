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

void ShaderObject::CompileShaderObject(const char *source, bool compileDebug)
{
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
        if(!success){
            glGetShaderInfoLog(handle, 512, NULL, infoLog);
            std::cout << shaderTypeString << ": COMPILATION_FAILED\n" << infoLog << std::endl;
        } else {
            std::cout << shaderTypeString << ": COMPILATION_SUCCESSFUL" << std::endl;
        }
    }
    
    
}
const char * LoadShaderSource(const char * shaderPath){
    std::ifstream shaderFile;
    std::string shaderCodeString;
    try{
        shaderFile.open(shaderPath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCodeString = shaderStream.str();
    } catch (std::ifstream::failure e){
        std::cout << "READ NOT SUCCESSFUL" << std::endl;
    }
    return shaderCodeString.c_str();
}
unsigned int ShaderObject::GetHandle(){
    return handle;
}