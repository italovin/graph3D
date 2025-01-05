#ifndef GLOBJECTS_H
#define GLOBJECTS_H
#include <GL/glew.h>
#include <glm/glm.hpp>
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
        virtual ~ObjectGL();
        GLuint GetHandle() const;
    };
    class TextureGL: public ObjectGL {
    private:
        GLenum textureType;
        GLenum internalFormat;
        GLsizei width = 0;
        GLsizei height = 0;
        GLsizei levels = 1;
    public:
        TextureGL(GLenum textureType);
        void Bind(int texUnit);
        void SetupParameters();
        void SetupStorage2D(GLsizei width, GLsizei height);
        void SetupStorage3D(GLsizei width, GLsizei height, int layers);
        void PushData2D(GLsizei width, GLsizei height, GLenum format, const std::vector<GLubyte> &pixels);
        void PushData2D(GLsizei width, GLsizei height, GLenum format, const std::vector<GLfloat> &pixels);
        void PushData3D(GLsizei width, GLsizei height, GLenum format, const std::vector<std::vector<GLubyte>> &pixels);
        void PushData3D(GLsizei width, GLsizei height, GLenum format, const std::vector<std::vector<GLfloat>> &pixels);
        // Single upload all layers for a 3D texture or texture array
        void PushData3D(GLsizei width, GLsizei height, int layers, GLenum format, const std::vector<GLubyte> &pixels);
        void PushData3DLayer(GLsizei width, GLsizei height, int layer, GLenum format, const std::vector<GLubyte> &pixels);
        void PushData3DLayer(GLsizei width, GLsizei height, int layer, GLenum format, const std::vector<GLfloat> &pixels);
        void GenerateMipmaps();
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
        void SetVec3(const std::string &name, glm::vec3 value) const;
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
