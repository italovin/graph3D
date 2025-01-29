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
