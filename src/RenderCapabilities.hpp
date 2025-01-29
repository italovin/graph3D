#ifndef RENDER_CAPABILITIES_H
#define RENDER_CAPABILITIES_H
#include <string>
#include "GLApiVersions.hpp"

class RenderCapabilities {
public:

    static void Initialize();
    static std::string GetVersionString();
    static std::string GetVendorString();
    static int GetGLSLVersion();
    static int GetMajorVersion();
    static int GetMinorVersion();
    static GLApiVersion GetAPIVersion();
    static int GetMaxUBOSize();
    static int GetMaxUBOBindings();
    static int GetMaxTextureArrayLayers();
    static int GetMaxTextureImageUnits();
    static int GetMaxVertexAttributes();
    static int GetMaxVertexOutputComponents();

private:
    RenderCapabilities();

    static bool initialized;
    // Strings
    static std::string versionString;
    static std::string vendorString;

    // Version
    static int glslVersion;
    static int majorVersion;
    static int minorVersion;
    static GLApiVersion apiVersion;

    // Max attributes informations
    //Buffers
    static int maxUBOSize;
    static int maxSSBOSize;
    static int maxUniformBufferBindings;
    static int maxShaderStorageBufferBindings;
    //Shader
    static int maxVertexUniformComponents;
    static int maxFragmentUniformComponents;
    static int maxTessControlUniformComponents;
    static int maxTessEvaluationUniformComponents;
    //Texture
    static int maxTextureArrayLayers;
    static int maxTextureSize;
    static int max3DTextureSize;
    static int maxCubeMapTextureSize;
    static int maxArrayTextureLayers;
    static int maxTextureImageUnits;
    static int maxCombinedTextureImageUnits;
    //Vertex
    static int maxVertexAttributes;
    static int maxVertexOutputComponents;
    static int maxGeometryInputComponents;
    static int maxGeometryOutputComponents;
};

#endif
