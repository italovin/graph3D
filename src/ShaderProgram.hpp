#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderObject.hpp"

class ShaderProgram {
private:
    unsigned int handle;

public:
    ShaderProgram();
    void AttachShaderObject(ShaderObject shaderObject);
    void Link();
    void Use();
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetDouble(const std::string &name, double value) const;
    void SetMat4Float(const std::string &name, const glm::mat4 &matrix) const;
    void DetachShaderObject(ShaderObject shaderObject);
    void RemoveShaderObject(ShaderObject shaderObject);
};