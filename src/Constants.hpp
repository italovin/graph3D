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
    }
}

#endif
