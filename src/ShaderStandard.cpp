#include "ShaderStandard.hpp"
#include "RenderCapabilities.hpp"
#include "ShaderCode.hpp"
#include "ShaderTypes.hpp"

ShaderStandard::ShaderStandard(){
    // Declare attributes used in shader
    attributes.emplace(std::make_pair("position", std::make_pair(false, 0)));
    attributes.emplace(std::make_pair("normal", std::make_pair(false, 1)));
    attributes.emplace(std::make_pair("texCoord0", std::make_pair(false, 2)));
    attributes.emplace(std::make_pair("texCoord1", std::make_pair(false, 3)));
    attributes.emplace(std::make_pair("texCoord2", std::make_pair(false, 4)));
    attributes.emplace(std::make_pair("texCoord3", std::make_pair(false, 5)));
    attributes.emplace(std::make_pair("texCoord4", std::make_pair(false, 6)));
    attributes.emplace(std::make_pair("texCoord5", std::make_pair(false, 7)));
    attributes.emplace(std::make_pair("texCoord6", std::make_pair(false, 8)));
    attributes.emplace(std::make_pair("texCoord7", std::make_pair(false, 9)));
    attributes.emplace(std::make_pair("tangent", std::make_pair(false, 10)));
    attributes.emplace(std::make_pair("bitangent", std::make_pair(false, 11)));
    attributes.emplace(std::make_pair("color", std::make_pair(false, 12)));
    // Declare parameters and maps used in shader
    maps.emplace(std::make_pair("diffuseMap", false));
    maps.emplace(std::make_pair("specularMap", false));
    maps.emplace(std::make_pair("normalMap", false));
    // Flags only to enable certain shader effects or processings
    flags.emplace(std::make_pair("lighting", false));
}

ShaderStandard::~ShaderStandard(){
}

void ShaderStandard::EnableAttribPosition(int location){
    attributes["position"] = std::make_pair(true, location);;
}

void ShaderStandard::EnableAttribNormal(int location){
    attributes["normal"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTexCoord_0(int location){
    attributes["texCoord0"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTexCoord_1(int location){
    attributes["texCoord1"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTexCoord_2(int location){
    attributes["texCoord2"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTexCoord_3(int location){
    attributes["texCoord3"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTexCoord_4(int location){
    attributes["texCoord4"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTexCoord_5(int location){
    attributes["texCoord5"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTexCoord_6(int location){
    attributes["texCoord6"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTexCoord_7(int location){
    attributes["texCoord7"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribTangent(int location){
    attributes["tangent"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribBiTangent(int location){
    attributes["bitangent"] = std::make_pair(true, location);
}
void ShaderStandard::EnableAttribColor(int location){
    attributes["color"] = std::make_pair(true, location);
}
void ShaderStandard::ActivateDiffuseMap(){
    maps["diffuseMap"] = true;
}
void ShaderStandard::ActivateSpecularMap(){
    maps["specularMap"] = true;
}
void ShaderStandard::ActivateNormalMap(){
    maps["normalMap"] = true;
}
void ShaderStandard::ActivateLighting(){
    flags["lighting"] = true;
}

ShaderCode ShaderStandard::ProcessCode(){
    ShaderCode code; // Shader code to build
    // Attributes
    int positionLocation = attributes["position"].second;
    int normalLocation = attributes["normal"].second;
    int texCoord0Location = attributes["texCoord0"].second;
    int texCoord1Location = attributes["texCoord1"].second;
    int texCoord2Location = attributes["texCoord2"].second;
    int texCoord3Location = attributes["texCoord3"].second;
    int texCoord4Location = attributes["texCoord4"].second;
    int texCoord5Location = attributes["texCoord5"].second;
    int texCoord6Location = attributes["texCoord6"].second;
    int texCoord7Location = attributes["texCoord7"].second;
    int tangentLocation = attributes["tangent"].second;
    int bitangentLocation = attributes["bitangent"].second;
    int colorLocation = attributes["color"].second;
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
    bool diffuseMapActivated = maps["diffuseMap"];
    bool specularMapActivated = maps["specularMap"];
    bool normalMapActivated = maps["normalMap"];
    bool lightingActivated = flags["lighting"];
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
    std::string fragColorString; // Final color result

    // Used Uniforms:
    // Light color - fragment shader
    // Light position - vertex shader (when used with tangent space version) or fragment shader
    // View position - vertex shader (when used with tangent space version) or fragment shader
    std::string maxObjectsGroupString = std::to_string(glm::min(RenderCapabilities::GetMaxTextureArrayLayers(), RenderCapabilities::GetMaxUBOSize()/64));
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
        code.SetBindingPurpose(ShaderStage::Vertex, "mvpsUBO", "mvps");
    } else {
        glPositionString = "gl_Position = vec4(1.0)\n";
    }


    if(texCoord0Enabled){
        code.AddVertexAttribute("aTexCoord0", ShaderDataType::Float2, texCoord0Location);
        code.AddOutput(ShaderStage::Vertex, "aTexCoord0Out", ShaderDataType::Float2);
        texCoord0OutSetting += "aTexCoord0Out = aTexCoord0 * texCoord0Scales[objID].xy;\n";
        code.CreateUniformBlock(ShaderStage::Vertex, "texCoord0ScalesUBO", "vec4 texCoord0Scales[" + maxObjectsGroupString + "];"); // UBO Scalings for tex coords 0
        code.SetBindingPurpose(ShaderStage::Vertex, "texCoord0ScalesUBO", "texCoord0Scales");
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
        if(diffuseMapActivated){ // Color Attrib + Diffuse Map
            // Added diffuse map sampler array down below
            albedoString = "vec4 albedo = texture(diffuseMap, vec3(aTexCoord0Out, objID))*aColorOut;\n";
        } else { // Only color attrib
            albedoString = "vec4 albedo = aColorOut;\n";
        }
    } else {
        if(diffuseMapActivated){
            // Added diffuse map sampler array down below
            albedoString = "vec4 albedo = texture(diffuseMap, vec3(aTexCoord0Out, objID));\n";
        } else {
            albedoString = "vec4 albedo = vec4(1.0);\n";
        }
    }
    if(diffuseMapActivated){ // Only diffuse map
        code.AddMaterialMapArray(ShaderStage::Fragment, "diffuseMap");
    }
    if(lightingActivated){
        if(normalEnabled){ // Declare normal attribute to do calculations
            // Already declared aNormal input
            code.AddOutput(ShaderStage::Vertex, "aNormalOut", ShaderDataType::Float3);
            code.CreateUniformBlock(ShaderStage::Vertex, "normalMatrixUBO", "mat4 normalMatrices[" + maxObjectsGroupString + "];");
            code.SetBindingPurpose(ShaderStage::Vertex, "normalMatrixUBO", "normalMatrices"); // Transpose of inverse of model
            normalMatrixString += "mat3 normalMatrix = mat3(normalMatrices[objID]);\n";
            normalOutSetString = "aNormalOut = normalMatrix * aNormal;\n";

            code.CreateUniformBlock(ShaderStage::Vertex, "modelsUBO", "mat4 models[" + maxObjectsGroupString + "];"); // Models uniform block. Used to world space transformations only
            code.SetBindingPurpose(ShaderStage::Vertex, "modelsUBO", "models");
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

                if(normalMapActivated){ // Using a normal map to retrieve normals
                    if(!texCoord0Enabled)
                        return ShaderCode();
                    code.AddMaterialMapArray(ShaderStage::Fragment, "normalMap");
                    normalString = "vec3 normal = texture(normalMap, vec3(aTexCoord0Out, objID)).rgb;\n";
                    normalString += "normal = normalize(normal * 2.0 - 1.0);\n"; // Tangent space normal
                }
            } else { // Use world space to do lighting calculations
                code.AddUniform(ShaderStage::Fragment, "lightPos", ShaderDataType::Float3);
                code.AddUniform(ShaderStage::Fragment, "viewPos", ShaderDataType::Float3);
                lightSetupString += "vec3 lightDir = normalize(lightPos - fragPos);\n";
                lightSetupString += "vec3 viewDir = normalize(viewPos - fragPos);\n";
            }

            // Ambient
            ambientString += "vec3 ambient = 0.1 * albedo.rgb;\n";

            // Diffuse
            diffuseString += "float diff = max(dot(normal, lightDir), 0.0);\n";
            diffuseString += "vec3 diffuse = diff * albedo.rgb * lightColor;\n";

            // Specular
            specularString += "vec3 halfwayDir = normalize(lightDir + viewDir);\n";
            specularString += "float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);\n";
            if(specularMapActivated){
                code.AddMaterialMapArray(ShaderStage::Fragment, "specularMap");
                specularString += "vec3 specular = spec * texture(specularMap, vec3(aTexCoord0Out, objID)).rgb * lightColor;\n";
            } else {
                specularString += "vec3 specular = vec3(0.2) * spec;\n";
            }
            fragColorString += "vec3 finalColor = ambient + diffuse + specular;\n";
            fragColorString += "FragColor = vec4(finalColor, albedo.a);\n";

        } else { // Cannot do lighting without normal attribute
            return ShaderCode();
        }
    } else {
        // The albedo vector4 is set of combination of using color attribute and diffuse map
        fragColorString += "FragColor = albedo;\n";
    }
    std::string vertexMainString;
    vertexMainString += objIDOutSetString;
    vertexMainString += texCoord0OutSetting;
    vertexMainString += normalMatrixString;
    vertexMainString += normalOutSetString;
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
    fragmentMainString += fragColorString;

    code.SetMain(ShaderStage::Vertex, vertexMainString);
    code.SetMain(ShaderStage::Fragment, fragmentMainString);

    return code;
}
