#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <string>

// Universal constants to be used in the engine
namespace Constants
{
    namespace ShaderStandard
    {
        /* Maximum number of objects to group in a single render group
           This also defines the size of the uniform buffers in standard shaders*/
        const unsigned long maxObjectsToGroup = 8192;
        // Default position attribute location
        const int positionAttribLocation = 0;
        // Default normal attribute location
        const int normalAttribLocation = 1;
        // Default tangent attribute location
        const int tangentAttribLocation = 2;
        // Default bitangent attribute location
        const int bitangentAttribLocation = 3;
        // Default color attribute location
        const int colorAttribLocation = 4;
        // Default tex coordinate 0 attribute location
        const int texCoord0AttribLocation = 5;
        // Default tex coordinate 1 attribute location
        const int texCoord1AttribLocation = 6;
        // Default tex coordinate 2 attribute location
        const int texCoord2AttribLocation = 7;
        // Default tex coordinate 3 attribute location
        const int texCoord3AttribLocation = 8;
        // Default tex coordinate 4 attribute location
        const int texCoord4AttribLocation = 9;
        // Default tex coordinate 5 attribute location
        const int texCoord5AttribLocation = 10;
        // Default tex coordinate 6 attribute location
        const int texCoord6AttribLocation = 11;
        // Default tex coordinate 7 attribute location
        const int texCoord7AttribLocation = 12;
        // Default position attribute name
        const std::string positionAttribName = "position";
        // Default normal attribute name
        const std::string normalAttribName = "normal";
        // Default tangent attribute name
        const std::string tangentAttribName = "tangent";
        // Default bitangent attribute name
        const std::string bitangentAttribName = "bitangent";
        // Default color attribute name
        const std::string colorAttribName = "color";
        // Default tex coordinate 0 attribute name
        const std::string texCoord0AttribName = "texCoord0";
        // Default tex coordinate 1 attribute name
        const std::string texCoord1AttribName = "texCoord1";
        // Default tex coordinate 2 attribute name
        const std::string texCoord2AttribName = "texCoord2";
        // Default tex coordinate 3 attribute name
        const std::string texCoord3AttribName = "texCoord3";
        // Default tex coordinate 4 attribute name
        const std::string texCoord4AttribName = "texCoord4";
        // Default tex coordinate 5 attribute name
        const std::string texCoord5AttribName = "texCoord5";
        // Default tex coordinate 6 attribute name
        const std::string texCoord6AttribName = "texCoord6";
        // Default tex coordinate 7 attribute name
        const std::string texCoord7AttribName = "texCoord7";
        // Uniform name for the view/camera/eye world position
        const std::string viewPosName = "viewPos";
        // Default diffuse map key name
        const std::string diffuseMapName = "diffuseMap";
        // Default normal map key name
        const std::string normalMapName = "normalMap";
        // Default specular map key name
        const std::string specularMapName = "specularMap";
        // Default diffuse/base color propertie key name
        const std::string diffuseUniformName = "diffuseUniform";
        // Default specular propertie key name
        const std::string specularUniformName = "specularUniform";
        // Default lighting use flag key name
        const std::string lightingName = "lighting";
        // Default diffuse map indices uniform block binding name
        const std::string diffuseMapIndicesBinding = "diffuseMapIndices";
        // Default normal map indices uniform block binding name
        const std::string normalMapIndicesBinding = "normalMapIndices";
        // Default specular map indices uniform block binding name
        const std::string specularMapIndicesBinding = "specularMapIndices";
        // Default model-view-projection uniform block binding name
        const std::string mvpsBinding = "mvps";
        // Default models uniform block binding name
        const std::string modelsBinding = "models";
        // Default normal matrices uniform block binding name
        const std::string normalMatricesBinding = "normalMatrices";
        // Default materials properties uniform block binding name
        const std::string materialsBinding = "materials";
        // Maximum number of point lights to use in a scene
        const unsigned long maxPointLights = 16;
        // Maximum number of directional lights to use in a scene
        const unsigned long maxDirectionalLights = 4;
        // Maximum number of spot lights to use in a scene
        const unsigned long maxSpotLights = 16;
        // Maximum number of lights to use in a scene
        const unsigned long maxLights = maxPointLights + maxDirectionalLights + maxSpotLights;
        // Default point light uniform block binding name
        const std::string pointLightsBinding = "pointLights";
        // Default directional light uniform block binding name
        const std::string directionalLightsBinding = "directionalLights";
        // Default spot light uniform block binding name
        const std::string spotLightsBinding = "spotLights";
        // Uniform name for point light counter
        const std::string pointLightCountName = "pointLightCount";
        // Uniform name for directional light counter
        const std::string directionalLightCountName = "directionalLightCount";
        // Uniform name for spot light counter
        const std::string spotLightCountName = "spotLightCount";
    }
}

#endif
