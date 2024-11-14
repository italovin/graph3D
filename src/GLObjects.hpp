#ifndef GLOBJECTS_H
#define GLOBJECTS_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Resource.hpp"

namespace GL{
    class ObjectGL{
    protected:
        GLuint handle = 0;
        virtual void Release();
    public:
        ObjectGL() = default;
        ObjectGL(const ObjectGL &) = delete;
        ObjectGL &operator=(const ObjectGL &) = delete;
        ObjectGL(ObjectGL &&other);
        ObjectGL &operator=(ObjectGL &&other);
        ~ObjectGL();
        GLuint GetHandle() const;
    };
    class TextureGL: public ObjectGL {
    private:
        GLenum textureType;
        GLenum format;
        GLenum dataType;
        GLenum internalFormat;
    public:
        TextureGL(GLenum textureType, GLenum format, GLenum dataType);
        void Bind(int texUnit);
        void SetupParameters();
        void SetupStorage2D(size_t width, size_t height);
        void SetupStorage3D(size_t width, size_t height, int layers);
        void PushData2D(size_t width, size_t height, const std::vector<GLubyte> &pixels);
        void PushData2D(size_t width, size_t height, const std::vector<GLfloat> &pixels);
        void PushData3D(size_t width, size_t height, const std::vector<std::vector<GLubyte>> &pixels);
        void PushData3D(size_t width, size_t height, const std::vector<std::vector<GLfloat>> &pixels);
        void PushData3DLayer(size_t width, size_t height, int layer, const std::vector<GLubyte> &pixels);
        void PushData3DLayer(size_t width, size_t height, int layer, const std::vector<GLfloat> &pixels);
        void GenerateMipmap();
        void Release() override;
    };

    class ShaderObjectGL : public ObjectGL {
    private:
        GLenum shaderType;
        std::string LoadShaderSource(const std::string &shaderPath);
        void ConstructCore(GLenum shaderType);
        bool debugInfo;
    public:
        ShaderObjectGL(GLenum shaderType);
        ShaderObjectGL(GLenum shaderType, bool debugInfo);
        ShaderObjectGL(GLenum shaderType, const std::string &shaderPath);
        ShaderObjectGL(GLenum shaderType, const std::string &shaderPath, bool debugInfo);
        void Compile(const std::string &source);
        void CompileFromPath(const std::string &shaderPath);
        GLenum GetType() const;
        void Release() override;
    };

    class ShaderGL : public ObjectGL {
    public:
        struct uniform_info
        {
            GLint location;
            GLsizei count;
            GLenum type;
        };
    private:
        bool debugInfo;
        std::unordered_map<std::string, uniform_info> uniforms;
        void GetUniformsInfo();

    public:
        ShaderGL();
        ShaderGL(bool debugInfo);
        ShaderGL(const std::vector<ShaderObjectGL> &shaderObjects, bool debugInfo = false);
        ShaderGL(std::vector<ShaderObjectGL> &&shaderObjects, bool debugInfo = false);
        void AttachShaderObject(const ShaderObjectGL &shaderObject);
        void AttachShaderObject(ShaderObjectGL &&shaderObject);
        void Create();
        void Link();
        void Use();
        void SetBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, int value) const;
        void SetFloat(const std::string &name, float value) const;
        void SetDouble(const std::string &name, double value) const;
        void SetVec4(const std::string &name, glm::vec4 value) const;
        void SetMat4Float(const std::string &name, const glm::mat4 &matrix) const;
        void SetBlockBinding(const std::string &name, unsigned int bindingPoint) const;
        void DetachShaderObject(const ShaderObjectGL &shaderObject);
        void DetachShaderObject(ShaderObjectGL &&shaderObject);
        std::unordered_map<std::string, uniform_info> GetUniforms();
        void Release() override;
    };

    class VertexArrayGL : public ObjectGL {
    public:
        VertexArrayGL();
        void Bind();
        void Release() override;
    };

    using TextureGLResource = Resource<TextureGL>;
    using ShaderObjectGLResource = Resource<ShaderObjectGL>;
    using ShaderGLResource = Resource<ShaderGL>;
}

#endif
