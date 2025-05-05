#include "ShaderStandard.hpp"
#include "RenderCapabilities.hpp"
#include "ShaderCode.hpp"
#include "ShaderTypes.hpp"
#include "Constants.hpp"
ShaderStandard::ShaderStandard(){
    // Declare attributes used in shader
    attributes.emplace(Constants::ShaderStandard::positionAttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::normalAttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::tangentAttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::bitangentAttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::colorAttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::texCoord0AttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::texCoord1AttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::texCoord2AttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::texCoord3AttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::texCoord4AttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::texCoord5AttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::texCoord6AttribName, std::make_pair(false, MeshAttribute()));
    attributes.emplace(Constants::ShaderStandard::texCoord7AttribName, std::make_pair(false, MeshAttribute()));
    // Declare parameters and maps used in shader
    maps.emplace(Constants::ShaderStandard::diffuseMapName, false);
    maps.emplace(Constants::ShaderStandard::specularMapName, false);
    maps.emplace(Constants::ShaderStandard::normalMapName, false);
    // Declare uniforms used in shader
    uniforms.emplace_back(Constants::ShaderStandard::diffuseUniformName, false);
    uniforms.emplace_back(Constants::ShaderStandard::specularUniformName, false);
    // Flags only to enable certain shader effects or processings
    flags.emplace(Constants::ShaderStandard::lightingName, false);
}

ShaderStandard::~ShaderStandard(){
}

std::vector<std::pair<std::string, bool>>::iterator ShaderStandard::GetUniform(const std::string &name){
    auto it = std::find_if(uniforms.begin(), uniforms.end(), [&name](const std::pair<std::string, bool> &uniform){
        return uniform.first == name;
    });
    return it;
}

void ShaderStandard::EnableAttribPosition(const MeshAttribute &attributeInfo){
    attributes["position"] = std::make_pair(true, attributeInfo);;
}

void ShaderStandard::EnableAttribNormal(const MeshAttribute &attributeInfo){
    attributes["normal"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTexCoord_0(const MeshAttribute &attributeInfo){
    attributes["texCoord0"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTexCoord_1(const MeshAttribute &attributeInfo){
    attributes["texCoord1"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTexCoord_2(const MeshAttribute &attributeInfo){
    attributes["texCoord2"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTexCoord_3(const MeshAttribute &attributeInfo){
    attributes["texCoord3"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTexCoord_4(const MeshAttribute &attributeInfo){
    attributes["texCoord4"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTexCoord_5(const MeshAttribute &attributeInfo){
    attributes["texCoord5"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTexCoord_6(const MeshAttribute &attributeInfo){
    attributes["texCoord6"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTexCoord_7(const MeshAttribute &attributeInfo){
    attributes["texCoord7"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribTangent(const MeshAttribute &attributeInfo){
    attributes["tangent"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribBiTangent(const MeshAttribute &attributeInfo){
    attributes["bitangent"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::EnableAttribColor(const MeshAttribute &attributeInfo){
    attributes["color"] = std::make_pair(true, attributeInfo);
}
void ShaderStandard::UseDiffuseUniform(){
    auto it = GetUniform(Constants::ShaderStandard::diffuseUniformName);
    if(it == uniforms.end())
        return;
    (*it).second = true;
}
void ShaderStandard::UseSpecularUniform()
{
    auto it = GetUniform(Constants::ShaderStandard::specularUniformName);
    if(it == uniforms.end())
        return;
    (*it).second = true;
}
void ShaderStandard::ActivateDiffuseMap()
{
    maps[Constants::ShaderStandard::diffuseMapName] = true;
}
void ShaderStandard::ActivateSpecularMap(){
    maps[Constants::ShaderStandard::specularMapName] = true;
}
void ShaderStandard::ActivateNormalMap(){
    maps[Constants::ShaderStandard::normalMapName] = true;
}
void ShaderStandard::ActivateLighting(){
    flags[Constants::ShaderStandard::lightingName] = true;
}

void ShaderStandard::SetIndexType(MeshIndexType type)
{
    indexType = type;
}

ShaderCode ShaderStandard::ProcessCode()
{
    ShaderCode code; // Shader code to build
    // Attributes
    int positionLocation = Constants::ShaderStandard::positionAttribLocation;
    int normalLocation = Constants::ShaderStandard::normalAttribLocation;
    int tangentLocation = Constants::ShaderStandard::tangentAttribLocation;
    int bitangentLocation = Constants::ShaderStandard::bitangentAttribLocation;
    int colorLocation = Constants::ShaderStandard::colorAttribLocation;
    int texCoord0Location = Constants::ShaderStandard::texCoord0AttribLocation;
    int texCoord1Location = Constants::ShaderStandard::texCoord1AttribLocation;
    int texCoord2Location = Constants::ShaderStandard::texCoord2AttribLocation;
    int texCoord3Location = Constants::ShaderStandard::texCoord3AttribLocation;
    int texCoord4Location = Constants::ShaderStandard::texCoord4AttribLocation;
    int texCoord5Location = Constants::ShaderStandard::texCoord5AttribLocation;
    int texCoord6Location = Constants::ShaderStandard::texCoord6AttribLocation;
    int texCoord7Location = Constants::ShaderStandard::texCoord7AttribLocation;

    bool positionEnabled = attributes[Constants::ShaderStandard::positionAttribName].first;
    bool normalEnabled = attributes[Constants::ShaderStandard::normalAttribName].first;
    bool tangentEnabled = attributes[Constants::ShaderStandard::tangentAttribName].first;
    bool bitangentEnabled = attributes[Constants::ShaderStandard::bitangentAttribName].first;
    bool colorEnabled = attributes[Constants::ShaderStandard::colorAttribName].first;
    bool texCoord0Enabled = attributes[Constants::ShaderStandard::texCoord0AttribName].first;
    bool texCoord1Enabled = attributes[Constants::ShaderStandard::texCoord1AttribName].first;
    bool texCoord2Enabled = attributes[Constants::ShaderStandard::texCoord2AttribName].first;
    bool texCoord3Enabled = attributes[Constants::ShaderStandard::texCoord3AttribName].first;
    bool texCoord4Enabled = attributes[Constants::ShaderStandard::texCoord4AttribName].first;
    bool texCoord5Enabled = attributes[Constants::ShaderStandard::texCoord5AttribName].first;
    bool texCoord6Enabled = attributes[Constants::ShaderStandard::texCoord6AttribName].first;
    bool texCoord7Enabled = attributes[Constants::ShaderStandard::texCoord7AttribName].first;

    // Modules
    bool diffuseMapActivated = maps[Constants::ShaderStandard::diffuseMapName];
    bool specularMapActivated = maps[Constants::ShaderStandard::specularMapName];
    bool normalMapActivated = maps[Constants::ShaderStandard::normalMapName];
    bool diffuseUniformUsed = (*GetUniform(Constants::ShaderStandard::diffuseUniformName)).second;
    bool specularUniformUsed = (*GetUniform(Constants::ShaderStandard::specularUniformName)).second;
    bool lightingActivated = flags[Constants::ShaderStandard::lightingName];
    bool materialsUniformBlockToUse =
    diffuseUniformUsed |
    specularUniformUsed;
    // Defined constant names
    std::string diffuseMapString = Constants::ShaderStandard::diffuseMapName; // Diffuse map variable name (sampler array in this case)
    std::string specularMapString = Constants::ShaderStandard::specularMapName; // Specular map variable name
    std::string normalMapString = Constants::ShaderStandard::normalMapName; // Normal map variable name

    // Vertex strings
    std::string objIDOutSetString;
    std::string texCoord0OutSetting; // Setting output texCoord0Out
    std::string normalMatrixString; // Get normal matrix at objID position from uniform block
    std::string modelMatrixString; // Get model matrix at objID position from uniform block
    std::string mvpMatrixString; // Get mvp matrix at objID position from uniform block
    std::string normalOutSetString; // When used normal output
    std::string fragPosSetting; // Setting world space frag position
    std::string tbnCalcString; // Calculate TBN matrix to transform vector to tangent space
    std::string glPositionString; // Final gl_Position string setting
    // Fragment strings
    std::string albedoString;
    std::string normalString;
    std::string specularColorString;
    std::string fragColorString; // Final color result

    // Used Uniforms:
    // Light color - fragment shader
    // Light position - vertex shader (when used with tangent space version) or fragment shader
    // View position - vertex shader (when used with tangent space version) or fragment shader
    std::string maxObjectsGroupString = std::to_string(
        glm::min(
            static_cast<std::size_t>(RenderCapabilities::GetMaxUBOSize()/sizeof(glm::mat4)),
            static_cast<std::size_t>(Constants::ShaderStandard::maxObjectsToGroup))
    );
    code.SetVersion(RenderCapabilities::GetGLSLVersion());
    // Enable shader stages to pipeline
    code.SetStageToPipeline(ShaderStage::Vertex, true);
    code.SetStageToPipeline(ShaderStage::Fragment, true);

    code.AddOutput(ShaderStage::Fragment, "FragColor", ShaderDataType::Float4); // Final color output in fragment shader

    std::string objIDString;
    if(RenderCapabilities::GetAPIVersion() < GLApiVersion::V460){ // This works with the non indirect drawing version
        code.AddExtension("GL_ARB_shader_draw_parameters");
        objIDString = "gl_DrawIDARB + gl_BaseInstanceARB + gl_InstanceID";
    } else { // This works with the indirect drawing version
        objIDString = "gl_BaseInstance + gl_InstanceID";
    }
    code.AddOutput(ShaderStage::Vertex, "objID", ShaderDataType::Int);
    objIDOutSetString = "objID = " + objIDString + ";\n";
    if(positionEnabled){
        code.AddVertexAttribute("aPosition", ShaderDataType::Float3, positionLocation);
        code.CreateUniformBlock(ShaderStage::Vertex, "mvpsUBO", "mat4 mvps[" + maxObjectsGroupString + "];"); // MVPs uniform block
        code.SetBindingPurpose(ShaderStage::Vertex, "mvpsUBO", Constants::ShaderStandard::mvpsBinding);
        mvpMatrixString = "mat4 mvp = mvps[objID];\n";
        glPositionString = "gl_Position = mvp*vec4(aPosition, 1.0);\n";
    } else {
        glPositionString = "gl_Position = vec4(1.0)\n";
    }

    if(texCoord0Enabled){
        code.AddVertexAttribute("aTexCoord0", ShaderDataType::Float2, texCoord0Location);
        code.AddOutput(ShaderStage::Vertex, "aTexCoord0Out", ShaderDataType::Float2);
        texCoord0OutSetting += "aTexCoord0Out = aTexCoord0;\n";
    }
    if(normalEnabled){
        code.AddVertexAttribute("aNormal", ShaderDataType::Float3, normalLocation);
    }
    if(tangentEnabled){
        code.AddVertexAttribute("aTangent", ShaderDataType::Float3, tangentLocation);
    }
    if(bitangentEnabled){
        code.AddVertexAttribute("aBitangent", ShaderDataType::Float3, bitangentLocation);
    }
    if(colorEnabled){
        code.AddVertexAttribute("aColor", ShaderDataType::Float4, colorLocation);
        code.AddOutput(ShaderStage::Vertex, "aColorOut", ShaderDataType::Float4);
        if(diffuseMapActivated){
            if(diffuseUniformUsed){ // Color Attrib + Diffuse Map + Base Color
                // Added diffuse map sampler array down below
                albedoString = "albedo = texture("+diffuseMapString+", vec3(aTexCoord0Out, diffuseMapIndices[objID].x))*aColorOut*materials[objID].diffuseUniform;\n";
            } else { // Color Attrib + Diffuse Map
                albedoString = "albedo = texture("+diffuseMapString+", vec3(aTexCoord0Out, diffuseMapIndices[objID].x))*aColorOut;\n";
            }

        } else {
            if(diffuseUniformUsed){ // Color Attrib + Base Color
                albedoString = "albedo = aColorOut*materials[objID].diffuseUniform;\n";
            } else { // Only Color Attrib
                albedoString = "albedo = aColorOut;\n";
            }
        }
    } else {
        if(diffuseMapActivated){
            if(diffuseUniformUsed){ // Diffuse Map + Base Color
                // Added diffuse map sampler array down below
                albedoString = "albedo = texture("+diffuseMapString+", vec3(aTexCoord0Out, diffuseMapIndices[objID].x))*materials[objID].diffuseUniform;\n";
            } else{ // Diffuse Map only
                albedoString = "albedo = texture("+diffuseMapString+", vec3(aTexCoord0Out, diffuseMapIndices[objID].x));\n";
            }
        } else {
            if(diffuseUniformUsed){ // Base Color only
                albedoString = "albedo = materials[objID].diffuseUniform;\n";
            } else { // None color information
                albedoString = "albedo = vec4(1.0);\n";
            }
        }
    }
    const std::string materialStructName = "Material";
    if(materialsUniformBlockToUse){
        code.DefineMaterialParametersStruct(ShaderStage::Fragment, materialStructName);
        code.UpdateMaterialParameterUniformBlock(ShaderStage::Fragment, "matUBO", materialStructName + " materials[" + maxObjectsGroupString + "];");
        code.SetBindingPurpose(ShaderStage::Fragment, "matUBO", Constants::ShaderStandard::materialsBinding);
    }
    if(diffuseMapActivated){ // Only diffuse map
        code.AddMaterialMapArray(ShaderStage::Fragment, diffuseMapString);
        code.CreateUniformBlock(ShaderStage::Fragment, "diffuseMapIndicesUBO", "ivec4 diffuseMapIndices[" + maxObjectsGroupString + "];"); // UBO diffuse map indices
        code.SetBindingPurpose(ShaderStage::Fragment, "diffuseMapIndicesUBO", Constants::ShaderStandard::diffuseMapIndicesBinding);
    }
    for(auto &&uniform : uniforms){
        if(uniform.second)
            code.AddMaterialVec4ToStruct(materialStructName, ShaderStage::Fragment, uniform.first);
    }

    code.PushOutsideCode(ShaderStage::Fragment, "vec4 albedo = vec4(0.0);");

    if(lightingActivated){
        if(normalEnabled && tangentEnabled){ // Declare normal attribute to do calculations
            //// Vertex shader

            code.CreateUniformBlock(ShaderStage::Vertex, "normalMatrixUBO", "mat4 normalMatrices[" + maxObjectsGroupString + "];");
            code.SetBindingPurpose(ShaderStage::Vertex, "normalMatrixUBO", Constants::ShaderStandard::normalMatricesBinding); // Transpose of inverse of model

            code.CreateUniformBlock(ShaderStage::Vertex, "modelsUBO", "mat4 models[" + maxObjectsGroupString + "];"); // Models uniform block. Used to world space transformations only
            code.SetBindingPurpose(ShaderStage::Vertex, "modelsUBO", Constants::ShaderStandard::modelsBinding);

            code.AddOutput(ShaderStage::Vertex, "fragPos", ShaderDataType::Float3);
            code.AddOutput(ShaderStage::Vertex, "TBN", ShaderDataType::Mat3);
            code.AddOutput(ShaderStage::Vertex, "aNormalOut", ShaderDataType::Float3);

            normalMatrixString = "mat3 normalMatrix = mat3(normalMatrices[objID]);\n";
            modelMatrixString = "mat4 modelMatrix = models[objID];\n";
            fragPosSetting = "fragPos = vec3(modelMatrix * vec4(aPosition, 1.0));\n";
            tbnCalcString += "vec3 T = normalize(normalMatrix * aTangent);\n"
                             "vec3 N = normalize(normalMatrix * aNormal);\n"
                             "T = normalize(T - dot(T, N) * N);\n";
            if(bitangentEnabled){ // Bitangent only makes sense working with normal and tangent
                // Already declared aBitangent input
                tbnCalcString += "vec3 B = normalize(normalMatrix * aBitangent);\n";
            } else {
                tbnCalcString += "vec3 B = cross(N, T);\n";
            }
            tbnCalcString += "TBN = mat3(T, B, N);\n";
            normalOutSetString = "aNormalOut = N;\n";

            //// Fragment shader
            const std::string pointLightStruct = "PointLight";
            // 48 bytes per light
            code.CreateStruct(ShaderStage::Fragment, pointLightStruct);
            code.AddParameterToStruct(ShaderStage::Fragment, pointLightStruct, "position", ShaderDataType::Float4);
            code.AddParameterToStruct(ShaderStage::Fragment, pointLightStruct, "color", ShaderDataType::Float4);
            code.AddParameterToStruct(ShaderStage::Fragment, pointLightStruct, "intensity", ShaderDataType::Float);
            code.AddParameterToStruct(ShaderStage::Fragment, pointLightStruct, "colorTemperature", ShaderDataType::Float);
            code.AddParameterToStruct(ShaderStage::Fragment, pointLightStruct, "range", ShaderDataType::Float);
            code.AddParameterToStruct(ShaderStage::Fragment, pointLightStruct, "cutoff", ShaderDataType::Float);

            const std::string directionalLightStruct = "DirectionalLight";
            // 32 bytes per light
            code.CreateStruct(ShaderStage::Fragment, directionalLightStruct);
            code.AddParameterToStruct(ShaderStage::Fragment, directionalLightStruct, "direction", ShaderDataType::Float4);
            code.AddParameterToStruct(ShaderStage::Fragment, directionalLightStruct, "color", ShaderDataType::Float4);

            const std::string spotLightStruct = "SpotLight";
            // 64 bytes per light
            code.CreateStruct(ShaderStage::Fragment, spotLightStruct);
            code.AddParameterToStruct(ShaderStage::Fragment, spotLightStruct, "position", ShaderDataType::Float4);
            code.AddParameterToStruct(ShaderStage::Fragment, spotLightStruct, "direction", ShaderDataType::Float4);
            code.AddParameterToStruct(ShaderStage::Fragment, spotLightStruct, "color", ShaderDataType::Float4);
            code.AddParameterToStruct(ShaderStage::Fragment, spotLightStruct, "range", ShaderDataType::Float);
            code.AddParameterToStruct(ShaderStage::Fragment, spotLightStruct, "innerCutoff", ShaderDataType::Float);
            code.AddParameterToStruct(ShaderStage::Fragment, spotLightStruct, "outerCutoff", ShaderDataType::Float);
            code.AddParameterToStruct(ShaderStage::Fragment, spotLightStruct, "cutoff", ShaderDataType::Float);

            const std::string maxPointLightsString = std::to_string(Constants::ShaderStandard::maxPointLights);
            const std::string maxDirectionalLightsString = std::to_string(Constants::ShaderStandard::maxDirectionalLights);
            const std::string maxSpotLightsString = std::to_string(Constants::ShaderStandard::maxSpotLights);
            code.CreateUniformBlock(ShaderStage::Fragment, "pointLightUBO", pointLightStruct+" pointLights[" + maxPointLightsString + "];");
            code.SetBindingPurpose(ShaderStage::Fragment, "pointLightUBO", Constants::ShaderStandard::pointLightsBinding);
            code.CreateUniformBlock(ShaderStage::Fragment, "directionalLightUBO", directionalLightStruct+" directionalLights[" + maxDirectionalLightsString + "];");
            code.SetBindingPurpose(ShaderStage::Fragment, "directionalLightUBO", Constants::ShaderStandard::directionalLightsBinding);
            code.CreateUniformBlock(ShaderStage::Fragment, "spotLightUBO", spotLightStruct+" spotLights[" + maxSpotLightsString + "];");
            code.SetBindingPurpose(ShaderStage::Fragment, "spotLightUBO", Constants::ShaderStandard::spotLightsBinding);
            // Uniform (vertex and fragment) to set the number of lights to use
            const std::string pointLightCountName = Constants::ShaderStandard::pointLightCountName;
            const std::string directionalLightCountName = Constants::ShaderStandard::directionalLightCountName;
            const std::string spotLightCountName = Constants::ShaderStandard::spotLightCountName;
            const std::string viewPosName = Constants::ShaderStandard::viewPosName;
            code.AddUniform(ShaderStage::Fragment, pointLightCountName, ShaderDataType::Int);
            code.AddUniform(ShaderStage::Fragment, directionalLightCountName, ShaderDataType::Int);
            code.AddUniform(ShaderStage::Fragment, spotLightCountName, ShaderDataType::Int);
            code.AddUniform(ShaderStage::Fragment, viewPosName, ShaderDataType::Float3); // Get World space view position

            // Lighting

            if(normalMapActivated){ // Using a normal map to retrieve normals
                if(!texCoord0Enabled)
                    return ShaderCode();
                code.AddMaterialMapArray(ShaderStage::Fragment, normalMapString);
                code.CreateUniformBlock(ShaderStage::Fragment, "normalMapIndicesUBO", "ivec4 normalMapIndices[" + maxObjectsGroupString + "];"); // UBO normal map indices
                code.SetBindingPurpose(ShaderStage::Fragment, "normalMapIndicesUBO", Constants::ShaderStandard::normalMapIndicesBinding);
                normalString += "normal = texture("+normalMapString+", vec3(aTexCoord0Out, normalMapIndices[objID].x)).rgb;\n";
                normalString += "normal = normalize(TBN*(normal * 2.0 - 1.0));\n"; // World space normal
            } else {
                normalString += "normal = normalize(aNormalOut);\n";
            }

            if(specularMapActivated){
                code.AddMaterialMapArray(ShaderStage::Fragment, specularMapString);
                code.CreateUniformBlock(ShaderStage::Fragment, "specularMapIndicesUBO", "ivec4 specularMapIndices[" + maxObjectsGroupString + "];"); // UBO specular map indices
                code.SetBindingPurpose(ShaderStage::Fragment, "specularMapIndicesUBO", Constants::ShaderStandard::specularMapIndicesBinding);
                specularColorString += "specularColor = texture("+specularMapString+", vec3(aTexCoord0Out, specularMapIndices[objID].x)).rgb;\n";
            } else {
                if(specularUniformUsed)
                    specularColorString += "specularColor = materials[objID].specularUniform.rgb;\n";
                else
                    specularColorString += "specularColor = vec3(0.03);\n";
            }

            code.PushOutsideCode(ShaderStage::Fragment, "vec3 normal = vec3(0.0);");
            code.PushOutsideCode(ShaderStage::Fragment, "vec3 specularColor = vec3(0.0);");

            code.PushOutsideCode(ShaderStage::Fragment,
            "vec3 CalcPointLight("+pointLightStruct+" pointLight){\n"
            "  vec3 lightPos = pointLight.position.xyz;\n"
            "  vec3 lightDirNorm = normalize(lightPos - fragPos);\n" // Norm vector
            "  vec3 viewDirNorm = normalize("+viewPosName+" - fragPos);\n"
            "  vec3 halfwayDirNorm = normalize(lightDirNorm + viewDirNorm);\n"
            "  float diff = max(dot(normal, lightDirNorm), 0.0);\n"
            "  float spec = pow(max(dot(normal, halfwayDirNorm), 0.0), 16.0);\n"
            "  float lightRange = pointLight.range;\n"
            "  vec3 lightFragDirection = lightPos - fragPos;\n"
            "  float lightFragDistance = length(lightFragDirection);\n"
            "  float attenuation = clamp(1.0 - (lightFragDistance / lightRange), 0.0, 1.0);\n"
            "  attenuation *= attenuation;\n"
            "  float cutoff = pointLight.cutoff;\n"
            "  float cutoffFactor = (cutoff == 0.0) ? step(cutoff, attenuation) : smoothstep(cutoff, cutoff * 1.2, attenuation);\n" // Smooth transition to cutoff region
            "  attenuation *= cutoffFactor;\n"
            "  vec3 lightColor = pointLight.color.rgb;\n"
            "  vec3 diffuse = diff * albedo.rgb;\n"
            "  vec3 specular = spec * specularColor;\n"
            "  return (diffuse + specular) * lightColor * attenuation;\n" // Final color
            "}"
            );
            code.PushOutsideCode(ShaderStage::Fragment,
            "vec3 CalcDirectionalLight("+directionalLightStruct+" directionalLight){\n"
            "   vec3 lightDirNorm = normalize(-directionalLight.direction.xyz);\n"
            "   vec3 viewDirNorm = normalize("+viewPosName+" - fragPos);\n"
            "   vec3 halfwayDirNorm = normalize(lightDirNorm + viewDirNorm);\n"
            "   float diff = max(dot(normal, lightDirNorm), 0.0);\n"
            "   float spec = pow(max(dot(normal, halfwayDirNorm), 0.0), 16.0);\n"
            "   vec3 lightColor = directionalLight.color.rgb;\n"
            "   vec3 diffuse = diff * albedo.rgb;\n"
            "   vec3 specular = spec * specularColor;\n"
            "   return (diffuse + specular) * lightColor;\n"
            "}");
            code.PushOutsideCode(ShaderStage::Fragment,
            "vec3 CalcSpotLight("+spotLightStruct+" spotLight){\n"
            "  vec3 lightPos = spotLight.position.xyz;\n"
            "  vec3 lightDirNorm = normalize(lightPos - fragPos);\n"
            "  vec3 viewDirNorm = normalize("+viewPosName+" - fragPos);\n"
            "  vec3 halfwayDirNorm = normalize(lightDirNorm + viewDirNorm);\n"
            "  float diff = max(dot(normal, lightDirNorm), 0.0);\n"
            "  float spec = pow(max(dot(normal, halfwayDirNorm), 0.0), 16.0);\n"
            "  float lightRange = spotLight.range;\n"
            "  vec3 lightFragDirection = lightPos - fragPos;\n"
            "  float lightFragDistance = length(lightFragDirection);\n"
            "  float attenuation = clamp(1.0 - (lightFragDistance / lightRange), 0.0, 1.0);\n"
            "  attenuation *= attenuation;\n"
            "  float cutoff = spotLight.cutoff;\n"
            "  float cutoffFactor = (cutoff == 0.0) ? step(cutoff, attenuation) : smoothstep(cutoff, cutoff * 1.2, attenuation);\n" // Smooth transition to cutoff region
            "  attenuation *= cutoffFactor;\n"
            "  float theta = dot(lightDirNorm, normalize(-spotLight.direction.xyz));\n"
            "  float epsilon = spotLight.innerCutoff - spotLight.outerCutoff;\n"
            "  float spotIntensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0, 1.0);\n"
            "  vec3 lightColor = spotLight.color.rgb;\n"
            "  vec3 diffuse = diff * albedo.rgb;\n"
            "  vec3 specular = spec * specularColor;\n"
            "  return (diffuse + specular) * attenuation * spotIntensity;\n" // Final color
            "}"
            );
            fragColorString +=
            "vec3 finalColor = vec3(0.0);\n"
            "vec3 ambientLight = vec3(0.005, 0.005, 0.005);\n"
            "vec3 ambient = ambientLight * albedo.rgb;\n"
            "finalColor += ambient;\n"
            "for(int i = 0; i < "+pointLightCountName+"; i++){\n"
            "   finalColor += CalcPointLight(pointLights[i]);\n"
            "}\n"
            "for(int i = 0; i < "+directionalLightCountName+"; i++){\n"
            "   finalColor += CalcDirectionalLight(directionalLights[i]);\n"
            "}\n"
            "for(int i = 0; i < "+spotLightCountName+"; i++){\n"
            "   finalColor += CalcSpotLight(spotLights[i]);\n"
            "}\n"
            //"finalColor = finalColor / (finalColor + vec3(1.0));\n"
            "float gamma = 2.2;\n"
            "FragColor = vec4(pow(finalColor, vec3(1.0/gamma)), albedo.a);\n";
        } else { // Cannot do lighting without normal and tangent attribute
            return ShaderCode();
        }
    } else {
        fragColorString += "float gamma = 2.2;\n";
        // The albedo vector4 is set of combination of using color attribute and diffuse map
        fragColorString += "FragColor = vec4(pow(albedo.rgb, vec3(1.0/gamma)), albedo.a);\n";
    }
    std::string vertexMainString;
    vertexMainString += objIDOutSetString;
    vertexMainString += normalMatrixString;
    vertexMainString += modelMatrixString;
    vertexMainString += mvpMatrixString;
    vertexMainString += fragPosSetting;
    vertexMainString += tbnCalcString;
    vertexMainString += normalOutSetString;
    vertexMainString += texCoord0OutSetting;
    vertexMainString += glPositionString;

    std::string fragmentMainString;
    // Attribution phase
    fragmentMainString += albedoString;
    fragmentMainString += normalString;
    fragmentMainString += specularColorString;
    ///
    fragmentMainString += fragColorString;

    code.SetMain(ShaderStage::Vertex, vertexMainString);
    code.SetMain(ShaderStage::Fragment, fragmentMainString);

    return code;
}
