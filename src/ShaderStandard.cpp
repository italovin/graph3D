#include "ShaderStandard.hpp"
#include "RenderCapabilities.hpp"
#include "ShaderCode.hpp"
#include "ShaderTypes.hpp"
#include "Constants.hpp"
ShaderStandard::ShaderStandard(){
    // Declare attributes used in shader
    attributes.emplace("position", std::make_pair(false, MeshAttribute()));
    attributes.emplace("normal", std::make_pair(false, MeshAttribute()));
    attributes.emplace("texCoord0", std::make_pair(false, MeshAttribute()));
    attributes.emplace("texCoord1", std::make_pair(false, MeshAttribute()));
    attributes.emplace("texCoord2", std::make_pair(false, MeshAttribute()));
    attributes.emplace("texCoord3", std::make_pair(false, MeshAttribute()));
    attributes.emplace("texCoord4", std::make_pair(false, MeshAttribute()));
    attributes.emplace("texCoord5", std::make_pair(false, MeshAttribute()));
    attributes.emplace("texCoord6", std::make_pair(false, MeshAttribute()));
    attributes.emplace("texCoord7", std::make_pair(false, MeshAttribute()));
    attributes.emplace("tangent", std::make_pair(false, MeshAttribute()));
    attributes.emplace("bitangent", std::make_pair(false, MeshAttribute()));
    attributes.emplace("color", std::make_pair(false, MeshAttribute()));
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
    int positionLocation = attributes["position"].second.location;
    int normalLocation = attributes["normal"].second.location;
    int texCoord0Location = attributes["texCoord0"].second.location;
    int texCoord1Location = attributes["texCoord1"].second.location;
    int texCoord2Location = attributes["texCoord2"].second.location;
    int texCoord3Location = attributes["texCoord3"].second.location;
    int texCoord4Location = attributes["texCoord4"].second.location;
    int texCoord5Location = attributes["texCoord5"].second.location;
    int texCoord6Location = attributes["texCoord6"].second.location;
    int texCoord7Location = attributes["texCoord7"].second.location;
    int tangentLocation = attributes["tangent"].second.location;
    int bitangentLocation = attributes["bitangent"].second.location;
    int colorLocation = attributes["color"].second.location;
    bool positionEnabled = attributes["position"].first;
    bool normalEnabled = attributes["normal"].first;
    bool texCoord0Enabled = attributes["texCoord0"].first;
    bool texCoord1Enabled = attributes["texCoord1"].first;
    bool texCoord2Enabled = attributes["texCoord2"].first;
    bool texCoord3Enabled = attributes["texCoord3"].first;
    bool texCoord4Enabled = attributes["texCoord4"].first;
    bool texCoord5Enabled = attributes["texCoord5"].first;
    bool texCoord6Enabled = attributes["texCoord6"].first;
    bool texCoord7Enabled = attributes["texCoord7"].first;
    bool tangentEnabled = attributes["tangent"].first;
    bool bitangentEnabled = attributes["bitangent"].first;
    bool colorEnabled = attributes["color"].first;
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
    std::string normalOutSetString; // When used normal output
    std::string fragPosSetting; // Setting world space frag position
    std::string tbnCalcString; // Calculate TBN matrix to transform vector to tangent space
    std::string lightOutSetString; // Setting lighting relevant vectors to tangent space
    std::string glPositionString; // Final gl_Position string setting
    // Fragment strings
    std::string albedoString;
    std::string normalString;
    std::string lightSetupString; // Setup view dir and light dir
    std::string ambientString; // Ambient light
    std::string diffuseString; // Diffuse light calculation
    std::string specularString; // Specular light calculation
    std::string attenuationString; // Attenuation of light source for the distance
    std::string fragColorString; // Final color result

    // Used Uniforms:
    // Light color - fragment shader
    // Light position - vertex shader (when used with tangent space version) or fragment shader
    // View position - vertex shader (when used with tangent space version) or fragment shader
    std::string maxObjectsGroupString = std::to_string(
        glm::min(RenderCapabilities::GetMaxUBOSize()/sizeof(glm::mat4), Constants::ShaderStandard::maxObjectsToGroup)
    );
    code.SetVersion(RenderCapabilities::GetGLSLVersion());
    // Enable shader stages to pipeline
    code.SetStageToPipeline(ShaderStage::Vertex, true);
    code.SetStageToPipeline(ShaderStage::Fragment, true);

    code.AddOutput(ShaderStage::Fragment, "FragColor", ShaderDataType::Float4); // Final color output in fragment shader

    std::string objIDString;
    if(RenderCapabilities::GetAPIVersion() < GLApiVersion::V460){
        code.AddExtension("GL_ARB_shader_draw_parameters");
        objIDString = "gl_DrawIDARB + gl_BaseInstanceARB + gl_InstanceID";
    } else {
        objIDString = "gl_BaseInstance + gl_InstanceID";
    }
    code.AddOutput(ShaderStage::Vertex, "objID", ShaderDataType::Int);
    objIDOutSetString = "objID = " + objIDString + ";\n";
    if(positionEnabled){
        code.AddVertexAttribute("aPosition", ShaderDataType::Float3, positionLocation);
        glPositionString = "gl_Position = mvps[objID]*vec4(aPosition, 1.0);\n";
        code.CreateUniformBlock(ShaderStage::Vertex, "mvpsUBO", "mat4 mvps[" + maxObjectsGroupString + "];"); // MVPs uniform block
        code.SetBindingPurpose(ShaderStage::Vertex, "mvpsUBO", Constants::ShaderStandard::mvpsBinding);
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
                albedoString = "vec4 albedo = texture("+diffuseMapString+", vec3(aTexCoord0Out, diffuseMapIndices[objID].x))*aColorOut*materials[objID].diffuseUniform;\n";
            } else { // Color Attrib + Diffuse Map
                albedoString = "vec4 albedo = texture("+diffuseMapString+", vec3(aTexCoord0Out, diffuseMapIndices[objID].x))*aColorOut;\n";
            }
            
        } else {
            if(diffuseUniformUsed){ // Color Attrib + Base Color
                albedoString = "vec4 albedo = aColorOut*materials[objID].diffuseUniform;\n";
            } else { // Only Color Attrib
                albedoString = "vec4 albedo = aColorOut;\n";
            }
        }
    } else {
        if(diffuseMapActivated){
            if(diffuseUniformUsed){ // Diffuse Map + Base Color
                // Added diffuse map sampler array down below
                albedoString = "vec4 albedo = texture("+diffuseMapString+", vec3(aTexCoord0Out, diffuseMapIndices[objID].x))*materials[objID].diffuseUniform;\n";
            } else{ // Diffuse Map only
                albedoString = "vec4 albedo = texture("+diffuseMapString+", vec3(aTexCoord0Out, diffuseMapIndices[objID].x));\n";
            }
        } else {
            if(diffuseUniformUsed){ // Base Color only
                albedoString = "vec4 albedo = materials[objID].diffuseUniform;\n";
            } else { // None color information
                albedoString = "vec4 albedo = vec4(1.0);\n";
            }
        }
    }
    if(materialsUniformBlockToUse){
        code.DefineMaterialParametersStruct(ShaderStage::Fragment, "Material");
        code.UpdateMaterialParameterUniformBlock(ShaderStage::Fragment, "matUBO", "Material materials[" + maxObjectsGroupString + "];");
        code.SetBindingPurpose(ShaderStage::Fragment, "matUBO", Constants::ShaderStandard::materialsBinding);
    }
    if(diffuseMapActivated){ // Only diffuse map
        code.AddMaterialMapArray(ShaderStage::Fragment, diffuseMapString);
        code.CreateUniformBlock(ShaderStage::Fragment, "diffuseMapIndicesUBO", "ivec4 diffuseMapIndices[" + maxObjectsGroupString + "];"); // UBO diffuse map indices
        code.SetBindingPurpose(ShaderStage::Fragment, "diffuseMapIndicesUBO", Constants::ShaderStandard::diffuseMapIndicesBinding);
    }
    for(auto &&uniform : uniforms){
        if(uniform.second)
            code.AddMaterialVec4ToStruct("Material", ShaderStage::Fragment, uniform.first);
    }

    if(lightingActivated){
        if(normalEnabled){ // Declare normal attribute to do calculations
            // Already declared aNormal input
            code.AddOutput(ShaderStage::Vertex, "aNormalOut", ShaderDataType::Float3);
            code.CreateUniformBlock(ShaderStage::Vertex, "normalMatrixUBO", "mat4 normalMatrices[" + maxObjectsGroupString + "];");
            code.SetBindingPurpose(ShaderStage::Vertex, "normalMatrixUBO", Constants::ShaderStandard::normalMatricesBinding); // Transpose of inverse of model
            normalMatrixString += "mat3 normalMatrix = mat3(normalMatrices[objID]);\n";
            normalOutSetString = "aNormalOut = normalMatrix * aNormal;\n";

            code.CreateUniformBlock(ShaderStage::Vertex, "modelsUBO", "mat4 models[" + maxObjectsGroupString + "];"); // Models uniform block. Used to world space transformations only
            code.SetBindingPurpose(ShaderStage::Vertex, "modelsUBO", Constants::ShaderStandard::modelsBinding);
            code.AddOutput(ShaderStage::Vertex, "fragPos", ShaderDataType::Float3);
            fragPosSetting += "fragPos = vec3(models[objID] * vec4(aPosition, 1.0));\n";

            code.AddUniform(ShaderStage::Fragment, "lightColor", ShaderDataType::Float3); // Setting light color
            normalString = "vec3 normal = normalize(aNormalOut);\n";

            if(tangentEnabled){ // Use tangent space to do lighting calculations
                // Already declared aTangent input
                tbnCalcString += "vec3 T = normalize(normalMatrix * aTangent);\n";
                tbnCalcString += "vec3 N = normalize(normalMatrix * aNormal);\n";
                tbnCalcString += "T = normalize(T - dot(T, N) * N);\n";
                if(bitangentEnabled){ // Bitangent only makes sense working with normal and tangent
                    // Already declared aBitangent input
                    tbnCalcString += "vec3 B = normalize(normalMatrix * aBitangent);\n";
                } else {
                    tbnCalcString += "vec3 B = cross(N, T);\n";
                }
                tbnCalcString += "mat3 TBN = transpose(mat3(T, B, N));\n";
                // Lighting
                code.AddUniform(ShaderStage::Vertex, "lightPos", ShaderDataType::Float3); // Get World space light position
                code.AddUniform(ShaderStage::Vertex, "viewPos", ShaderDataType::Float3); // Get World space view position
                code.AddOutput(ShaderStage::Vertex, "TangentLightPos", ShaderDataType::Float3);
                code.AddOutput(ShaderStage::Vertex, "TangentViewPos", ShaderDataType::Float3);
                code.AddOutput(ShaderStage::Vertex, "TangentFragPos", ShaderDataType::Float3);
                lightOutSetString += "TangentLightPos = TBN * lightPos;\n";
                lightOutSetString += "TangentViewPos = TBN * viewPos;\n";
                lightOutSetString += "TangentFragPos  = TBN * fragPos;\n";
                // Fragment
                lightSetupString += "vec3 lightDir = normalize(TangentLightPos - TangentFragPos);\n";
                lightSetupString += "vec3 viewDir = normalize(TangentViewPos - TangentFragPos);\n";
                attenuationString += "float lightDistance = length(TangentLightPos - TangentFragPos);\n";
                if(normalMapActivated){ // Using a normal map to retrieve normals
                    if(!texCoord0Enabled)
                        return ShaderCode();
                    code.AddMaterialMapArray(ShaderStage::Fragment, normalMapString);
                    code.CreateUniformBlock(ShaderStage::Fragment, "normalMapIndicesUBO", "ivec4 normalMapIndices[" + maxObjectsGroupString + "];"); // UBO normal map indices
                    code.SetBindingPurpose(ShaderStage::Fragment, "normalMapIndicesUBO", Constants::ShaderStandard::normalMapIndicesBinding);
                    normalString = "vec3 normal = texture("+normalMapString+", vec3(aTexCoord0Out, normalMapIndices[objID].x)).rgb;\n";
                    normalString += "normal = normalize(normal * 2.0 - 1.0);\n"; // Tangent space normal
                }
            } else { // Use world space to do lighting calculations
                code.AddUniform(ShaderStage::Fragment, "lightPos", ShaderDataType::Float3);
                code.AddUniform(ShaderStage::Fragment, "viewPos", ShaderDataType::Float3);
                lightSetupString += "vec3 lightDir = normalize(lightPos - fragPos);\n";
                lightSetupString += "vec3 viewDir = normalize(viewPos - fragPos);\n";
                attenuationString += "float lightDistance = length(lightPos - fragPos);\n";
            }

            // Ambient
            ambientString += "vec3 ambient = vec3(0.01, 0.01, 0.01) * albedo.rgb;\n";

            // Diffuse
            diffuseString += "float diff = max(dot(normal, lightDir), 0.0);\n";
            diffuseString += "vec3 diffuse = diff * albedo.rgb * lightColor;\n";

            // Specular
            specularString += "vec3 halfwayDir = normalize(lightDir + viewDir);\n";
            specularString += "float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);\n";
            if(specularMapActivated){
                code.AddMaterialMapArray(ShaderStage::Fragment, specularMapString);
                code.CreateUniformBlock(ShaderStage::Fragment, "specularMapIndicesUBO", "ivec4 specularMapIndices[" + maxObjectsGroupString + "];"); // UBO specular map indices
                code.SetBindingPurpose(ShaderStage::Fragment, "specularMapIndicesUBO", Constants::ShaderStandard::specularMapIndicesBinding);
                specularString += "vec3 specular = texture("+specularMapString+", vec3(aTexCoord0Out, specularMapIndices[objID].x)).rgb * spec * lightColor;\n";
            } else {
                if(specularUniformUsed)
                    specularString += "vec3 specular = materials[objID].specularUniform.rgb * spec * lightColor;\n";
                else
                    specularString += "vec3 specular = vec3(0.03) * spec * lightColor;\n";
            }
            attenuationString += "float att_kc = 1.0;\n";
            attenuationString += "float att_kl = 0.2;\n";
            attenuationString += "float att_kq = 0.5;\n";
            attenuationString += "float attenuation = 1.0 / (att_kc + att_kl*lightDistance + att_kq*lightDistance*lightDistance);\n";
            attenuationString += "diffuse *= attenuation;\n";
            attenuationString += "specular *= attenuation;\n";
            fragColorString += "vec3 finalColor = ambient + diffuse + specular;\n";
            fragColorString += "FragColor = vec4(pow(finalColor, vec3(1.0/2.2)), albedo.a);\n";

        } else { // Cannot do lighting without normal attribute
            return ShaderCode();
        }
    } else {
        // The albedo vector4 is set of combination of using color attribute and diffuse map
        fragColorString += "FragColor = vec4(pow(albedo.rgb, vec3(1.0/2.2)), albedo.a);\n";
    }
    std::string vertexMainString;
    vertexMainString += objIDOutSetString;
    vertexMainString += fragPosSetting;
    vertexMainString += normalMatrixString;
    vertexMainString += normalOutSetString;
    vertexMainString += texCoord0OutSetting;
    vertexMainString += tbnCalcString;
    vertexMainString += lightOutSetString;
    vertexMainString += glPositionString;

    std::string fragmentMainString;
    fragmentMainString += albedoString;
    fragmentMainString += normalString;
    fragmentMainString += lightSetupString;
    fragmentMainString += ambientString;
    fragmentMainString += diffuseString;
    fragmentMainString += specularString;
    fragmentMainString += attenuationString;
    fragmentMainString += fragColorString;

    code.SetMain(ShaderStage::Vertex, vertexMainString);
    code.SetMain(ShaderStage::Fragment, fragmentMainString);

    return code;
}
