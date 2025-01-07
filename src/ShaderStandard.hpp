#ifndef SHADER_STANDARD_H
#define SHADER_STANDARD_H
#include "Shader.hpp"

// Standard shader that uses batching and that may implement lighthing, normal mapping, ambient occlusion and so on
class ShaderStandard : public Shader {

public:
    ShaderStandard();
    ~ShaderStandard() override;
    void EnableAttribPosition(const MeshAttribute &attributeInfo);
    void EnableAttribNormal(const MeshAttribute &attributeInfo);
    void EnableAttribTexCoord_0(const MeshAttribute &attributeInfo);
    void EnableAttribTexCoord_1(const MeshAttribute &attributeInfo);
    void EnableAttribTexCoord_2(const MeshAttribute &attributeInfo);
    void EnableAttribTexCoord_3(const MeshAttribute &attributeInfo);
    void EnableAttribTexCoord_4(const MeshAttribute &attributeInfo);
    void EnableAttribTexCoord_5(const MeshAttribute &attributeInfo);
    void EnableAttribTexCoord_6(const MeshAttribute &attributeInfo);
    void EnableAttribTexCoord_7(const MeshAttribute &attributeInfo);
    void EnableAttribTangent(const MeshAttribute &attributeInfo);
    void EnableAttribBiTangent(const MeshAttribute &attributeInfo);
    void EnableAttribColor(const MeshAttribute &attributeInfo);
    // Activate diffuse texturing module
    void ActivateDiffuseMap();
    // Activate specual mapping module
    void ActivateSpecularMap();
    // Activate normal map module
    void ActivateNormalMap();
    // Activate default lighting module. Needs at least Normals enabled. Enables Tangent for tangent space calculation
    void ActivateLighting();
    // This defines if indices are unsigned int or unsigned short
    void SetIndexType(MeshIndexType type);
    ShaderCode ProcessCode() override;
};


#endif
