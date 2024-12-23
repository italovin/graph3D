#include "RenderCapabilities.hpp"
#include <GL/glew.h>

bool RenderCapabilities::initialized = false;
std::string RenderCapabilities::versionString = std::string();
std::string RenderCapabilities::vendorString = std::string();
int RenderCapabilities::glslVersion = 0;
int RenderCapabilities::majorVersion = 0;
int RenderCapabilities::minorVersion = 0;
GLApiVersion RenderCapabilities::apiVersion = GLApiVersion::V330;
int RenderCapabilities::maxUBOSize = 0;
int RenderCapabilities::maxSSBOSize = 0;
int RenderCapabilities::maxUniformBufferBindings = 0;
int RenderCapabilities::maxShaderStorageBufferBindings = 0;
int RenderCapabilities::maxVertexUniformComponents = 0;
int RenderCapabilities::maxFragmentUniformComponents = 0;
int RenderCapabilities::maxTessControlUniformComponents = 0;
int RenderCapabilities::maxTessEvaluationUniformComponents = 0;
int RenderCapabilities::maxTextureArrayLayers = 0;
int RenderCapabilities::maxTextureSize = 0;
int RenderCapabilities::max3DTextureSize = 0;
int RenderCapabilities::maxCubeMapTextureSize = 0;
int RenderCapabilities::maxArrayTextureLayers = 0;
int RenderCapabilities::maxTextureImageUnits = 0;
int RenderCapabilities::maxCombinedTextureImageUnits = 0;
int RenderCapabilities::maxVertexAttributes = 0;
int RenderCapabilities::maxVertexOutputComponents = 0;
int RenderCapabilities::maxGeometryInputComponents = 0;
int RenderCapabilities::maxGeometryOutputComponents = 0;


void RenderCapabilities::Initialize() {
    if(initialized)
        return;

    versionString = std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    vendorString = std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));

    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    if(majorVersion == 2){
        if(minorVersion == 0){
            glslVersion = 110;
        } else if(minorVersion == 1){
            glslVersion = 120;
        }
    } else if(majorVersion == 3){
        if(minorVersion == 0){
            glslVersion = 130;
        } else if(minorVersion == 1){
            glslVersion = 140;
        } else if(minorVersion == 2){
            glslVersion = 150;
        } else if(minorVersion == 3){
            glslVersion = 330;
        }
    } else {
        glslVersion = majorVersion * 100 + minorVersion * 10;
    }

    switch(glslVersion){
        case 200: apiVersion = GLApiVersion::V200; break;
        case 210: apiVersion = GLApiVersion::V210; break;
        case 300: apiVersion = GLApiVersion::V300; break;
        case 310: apiVersion = GLApiVersion::V310; break;
        case 320: apiVersion = GLApiVersion::V320; break;
        case 330: apiVersion = GLApiVersion::V330; break;
        case 400: apiVersion = GLApiVersion::V400; break;
        case 410: apiVersion = GLApiVersion::V410; break;
        case 420: apiVersion = GLApiVersion::V420; break;
        case 430: apiVersion = GLApiVersion::V430; break;
        case 440: apiVersion = GLApiVersion::V440; break;
        case 450: apiVersion = GLApiVersion::V450; break;
        case 460: apiVersion = GLApiVersion::V460; break;
        default: apiVersion = GLApiVersion::Undefined; break;
    }

    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUBOSize);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSSBOSize);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxShaderStorageBufferBindings);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents);
    glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS, &maxTessControlUniformComponents);
    glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, &maxTessEvaluationUniformComponents);
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxTextureArrayLayers);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3DTextureSize);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);
    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &maxVertexOutputComponents);
    glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &maxGeometryInputComponents);
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &maxGeometryOutputComponents);

    initialized = true;
}

std::string RenderCapabilities::GetVersionString(){
    return versionString;
}

std::string RenderCapabilities::GetVendorString(){
    return vendorString;
}

int RenderCapabilities::GetGLSLVersion(){
    return glslVersion;
}

int RenderCapabilities::GetMajorVersion(){
    return majorVersion;
}

int RenderCapabilities::GetMinorVersion(){
    return minorVersion;
}

GLApiVersion RenderCapabilities::GetAPIVersion(){
    return apiVersion;
}

int RenderCapabilities::GetMaxUBOSize(){
    return maxUBOSize;
}

int RenderCapabilities::GetMaxTextureArrayLayers(){
    return maxTextureArrayLayers;
}

int RenderCapabilities::GetMaxTextureImageUnits(){
    return maxTextureImageUnits;
}

int RenderCapabilities::GetMaxVertexAttributes(){
    return maxVertexAttributes;
}
