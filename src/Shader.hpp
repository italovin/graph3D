#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderObject.hpp"
#include "Resource.hpp"

struct uniform_info
{
	GLint location;
	GLsizei count;
    GLenum type;
};

class Shader{
private:
    GLuint handle = 0;
    bool debugInfo;
    std::unordered_map<std::string, uniform_info> uniforms;
    void GetUniformsInfo();

public:
    Shader();
    Shader(const Shader& other);
    Shader& operator=(const Shader& other);
    Shader(bool debugInfo);
    Shader(const std::vector<ShaderObject> &shaderObjects, bool debugInfo = false);
    Shader(std::vector<ShaderObject> &&shaderObjects, bool debugInfo = false);
    void AttachShaderObject(const ShaderObject &shaderObject);
    void AttachShaderObject(ShaderObject &&shaderObject);
    void Create();
    void Link();
    void Use();
    GLuint GetHandle() const;
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetDouble(const std::string &name, double value) const;
    void SetVec4(const std::string &name, glm::vec4 value) const;
    void SetMat4Float(const std::string &name, const glm::mat4 &matrix) const;
    void SetBlockBinding(const std::string &name, unsigned int bindingPoint) const;
    void DetachShaderObject(const ShaderObject &shaderObject);
    void DetachShaderObject(ShaderObject &&shaderObject);
    //This method will detach the shader and delete it, then shader cannot be attached to other program
    void RemoveShaderObject(const ShaderObject &shaderObject);
    std::unordered_map<std::string, uniform_info> GetUniforms();
};
#endif
