#ifndef SHADER_STANDARD_H
#define SHADER_STANDARD_H
#include "Shader.hpp"

// Standard shader that uses batching and that may implement lighthing, normal mapping, ambient occlusion and so on
class ShaderStandard : public Shader {

public:
    ShaderStandard();
    ~ShaderStandard() override;
    void EnableAttribPosition(int location);
    void EnableAttribNormal(int location);
    void EnableAttribTexCoord_0(int location);
    void EnableAttribTexCoord_1(int location);
    void EnableAttribTexCoord_2(int location);
    void EnableAttribTexCoord_3(int location);
    void EnableAttribTexCoord_4(int location);
    void EnableAttribTexCoord_5(int location);
    void EnableAttribTexCoord_6(int location);
    void EnableAttribTexCoord_7(int location);
    void EnableAttribTangent(int location);
    void EnableAttribBiTangent(int location);
    void EnableAttribColor(int location);
    // Activate diffuse texturing module
    void ActivateDiffuseMap();
    // Activate specual mapping module
    void ActivateSpecularMap();
    // Activate normal map module
    void ActivateNormalMap();
    // Activate default lighting module. Needs at least Normals enabled. Enables Tangent for tangent space calculation
    void ActivateLighting();
    ShaderCode ProcessCode() override;
};


#endif
