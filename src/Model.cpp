#include "Model.hpp"
#include "ShaderStandard.hpp"
#include "Constants.hpp"
#include <stb/stb_image.h>
#include <gli/gli.hpp>
#include <variant>
#include <fmt/core.h>

void Model::processNode(aiNode *node, const aiScene *scene, const aiMatrix4x4 &parentTransform)
{
    aiMatrix4x4 nodeTransform = parentTransform * node->mTransformation;
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // Cria Mesh
        auto myMesh = processMesh(mesh);

        // Cria Material
        auto myMaterial = processMaterial(material);

        aiVector3D aiScaling;
        aiQuaternion aiRotation;
        aiVector3D aiPosition;
        nodeTransform.Decompose(aiScaling, aiRotation, aiPosition);
        glm::vec3 position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
        glm::quat rotation = glm::quat(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
        glm::vec3 scale = glm::vec3(aiScaling.x, aiScaling.y, aiScaling.z);
        scale *= this->scale;
        TransformComponent transform(position, rotation, scale);

        components.emplace_back(MeshRendererComponent(myMesh, myMaterial), transform);
    }

    // Processa os filhos recursivamente
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, nodeTransform);
    }
}

Ref<Mesh> Model::processMesh(aiMesh *mesh)
{
    Ref<Mesh> meshData = CreateRef<Mesh>();
    std::vector<float> vertices;
    std::vector<short> normals;
    std::variant<std::vector<float>, std::vector<unsigned short>> texCoords0 = std::vector<unsigned short>();
    std::vector<short> tangents;
    std::vector<short> bitangents;
    std::vector<unsigned char> colors;
    std::variant<std::vector<unsigned int>, std::vector<unsigned short>> indices;

    size_t verticesVec2Size = 2 * mesh->mNumVertices;
    size_t verticesVec3Size = 3 * mesh->mNumVertices;
    size_t verticesVec4Size = 4 * mesh->mNumVertices;

    bool hasNormals = mesh->HasNormals();
    bool hasTexCoords0 = mesh->HasTextureCoords(0);
    bool hasTangentsAndBitangents = mesh->HasTangentsAndBitangents();
    bool hasColors = mesh->HasVertexColors(0);

    // Vectors initilization
    vertices.resize(verticesVec3Size);
    if(hasNormals)
        normals.resize(verticesVec3Size);

    if(hasTangentsAndBitangents){
        tangents.resize(verticesVec3Size);
        bitangents.resize(verticesVec3Size);
    }

    if(hasColors){
        colors.resize(verticesVec4Size);
    }
    // This is loop check if mesh uses tex coords outside [0, 1] boundary
    // If this is true, store tex coords as float for correct interpolating on fragment shader
    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
        if(std::abs(mesh->mTextureCoords[0][i].x) > 1.0f || std::abs(mesh->mTextureCoords[0][i].y) > 1.0f){
            texCoords0 = std::vector<float>();
            break;
        }
    }
    std::visit([verticesVec2Size](auto&& value){
        value.resize(verticesVec2Size);
    }, texCoords0);
    //////

    // This check if maxIndex <= 65535, allowing to unsigned short for indices
    unsigned int maxIndex = 0;
    unsigned int totalNumIndices = 0;
    // Iterar por todas as faces da malha
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        totalNumIndices += face.mNumIndices;
        // Iterar pelos índices da face atual
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            maxIndex = std::max(maxIndex, face.mIndices[j]);
        }
    }
    if(maxIndex <= 65535){
        indices = std::vector<unsigned short>();
    } else {
        indices = std::vector<unsigned int>();
    }
    std::visit([totalNumIndices](auto&& value){
        value.reserve(totalNumIndices);
    }, indices);
    //////

    // Processa os vértices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        size_t vertexIndex = 3 * i;
        size_t normalIndex = 3 * i;
        size_t texCoordIndex = 2 * i;
        size_t tangentBitangentIndex = 3 * i;
        size_t colorIndex = 4 * i;

        vertices[vertexIndex] = mesh->mVertices[i].x;
        vertices[vertexIndex + 1] = mesh->mVertices[i].y;
        vertices[vertexIndex + 2] = mesh->mVertices[i].z;

        if(hasNormals){
            int normalIntX = std::round(mesh->mNormals[i].x * 32768.0f);
            int normalIntY = std::round(mesh->mNormals[i].y * 32768.0f);
            int normalIntZ = std::round(mesh->mNormals[i].z * 32768.0f);
            normals[normalIndex] = static_cast<short>(std::clamp(normalIntX, -32768, 32767));
            normals[normalIndex + 1] = static_cast<short>(std::clamp(normalIntY, -32768, 32767));
            normals[normalIndex + 2] = static_cast<short>(std::clamp(normalIntZ, -32768, 32767));
        }

        if (hasTexCoords0){
            float texCoordX = mesh->mTextureCoords[0][i].x;
            float texCoordY = mesh->mTextureCoords[0][i].y;
            auto updateTexCoords = [texCoordX, texCoordY, texCoordIndex](auto& texCoords) {
                using T = typename std::decay<decltype(texCoords)>::type::value_type;
                if constexpr (std::is_same_v<T, float>) {
                    texCoords[texCoordIndex] = texCoordX;
                    texCoords[texCoordIndex + 1] = texCoordY;
                } else if constexpr (std::is_same_v<T, unsigned short>) {
                    texCoords[texCoordIndex] = static_cast<unsigned short>(std::round(texCoordX * 65535.0f));
                    texCoords[texCoordIndex + 1] = static_cast<unsigned short>(std::round(texCoordY * 65535.0f));
                }
            };
            std::visit(updateTexCoords, texCoords0);

        } else {
            auto updateTexCoords = [texCoordIndex](auto& texCoords) {
                using T = typename std::decay<decltype(texCoords)>::type::value_type;
                if constexpr (std::is_same_v<T, float>) {
                    texCoords[texCoordIndex] = 0.0f;
                    texCoords[texCoordIndex + 1] = 0.0f;
                } else if constexpr (std::is_same_v<T, unsigned short>) {
                    texCoords[texCoordIndex] = 0x00;
                    texCoords[texCoordIndex + 1] = 0x00;
                }
            };
            std::visit(updateTexCoords, texCoords0);
        }
        if(hasTangentsAndBitangents){
            int tangentIntX = std::round(mesh->mTangents[i].x * 32768.0f);
            int tangentIntY = std::round(mesh->mTangents[i].y * 32768.0f);
            int tangentIntZ = std::round(mesh->mTangents[i].z * 32768.0f);
            tangents[tangentBitangentIndex] = static_cast<short>(std::clamp(tangentIntX, -32768, 32767));
            tangents[tangentBitangentIndex + 1] = static_cast<short>(std::clamp(tangentIntY, -32768, 32767));
            tangents[tangentBitangentIndex + 2] = static_cast<short>(std::clamp(tangentIntZ, -32768, 32767));

            int bitangentIntX = std::round(mesh->mBitangents[i].x * 32768.0f);
            int bitangentIntY = std::round(mesh->mBitangents[i].y * 32768.0f);
            int bitangentIntZ = std::round(mesh->mBitangents[i].z * 32768.0f);
            bitangents[tangentBitangentIndex] = static_cast<short>(std::clamp(bitangentIntX, -32768, 32767));
            bitangents[tangentBitangentIndex + 1] = static_cast<short>(std::clamp(bitangentIntY, -32768, 32767));
            bitangents[tangentBitangentIndex + 2] = static_cast<short>(std::clamp(bitangentIntZ, -32768, 32767));
        }
        if(hasColors){
            colors[colorIndex] = mesh->mColors[0][i].r;
            colors[colorIndex + 1] = mesh->mColors[0][i].g;
            colors[colorIndex + 2] = mesh->mColors[0][i].b;
            colors[colorIndex + 3] = mesh->mColors[0][i].a;
        }
    }

    // Processa os índices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            std::visit([face, j](auto&& value){
                value.push_back((face.mIndices[j]));
            }, indices);
        }
    }

    meshData->PushAttributePosition(vertices);
    std::visit([&meshData](auto&& value){
        meshData->PushAttributeTexCoord0(value);
    }, texCoords0);
    if(hasNormals)
        meshData->PushAttributeNormal(normals);
    if(hasTangentsAndBitangents){
        meshData->PushAttributeTangent(tangents);
        meshData->PushAttributeBitangent(bitangents);
    }
    if(hasColors)
        meshData->PushAttributeColor(colors);
    std::visit([&meshData](auto&& value){
        meshData->SetIndices(value, MeshTopology::Triangles);
    }, indices);
    
    return meshData;
}

Ref<Material> Model::processMaterial(aiMaterial *material)
{
    Ref<Material> materialData = CreateRef<Material>(this->defaultShader);
    Ref<Texture> diffuseMap = loadMaterialTexture(material, aiTextureType_DIFFUSE);
    aiTextureType normalMapTextureType;
    if(format == "obj")
        normalMapTextureType = aiTextureType_HEIGHT;
    else
        normalMapTextureType = aiTextureType_NORMALS;
    Ref<Texture> normalMap = loadMaterialTexture(material, normalMapTextureType);
    Ref<Texture> specularMap = loadMaterialTexture(material, aiTextureType_SPECULAR);
    materialData->SetParameterMap(Constants::ShaderStandard::diffuseMapName, diffuseMap);
    materialData->SetParameterMap(Constants::ShaderStandard::normalMapName, normalMap);
    materialData->SetParameterMap(Constants::ShaderStandard::specularMapName, specularMap);
    { // Diffuse uniform or base color (albedo)
        // For while, base color and color diffuse are same info
        // Generally, color diffuse is for classic lighting and base color for PBR
        aiColor4D diffuse(1.0f, 1.0f, 1.0f, 1.0f);
        if (AI_SUCCESS == material->Get(AI_MATKEY_BASE_COLOR, diffuse)) {
            materialData->SetParameterVector4(Constants::ShaderStandard::diffuseUniformName, glm::vec4(diffuse.r,diffuse.g,diffuse.b,diffuse.a));
        } else if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) {
            materialData->SetParameterVector4(Constants::ShaderStandard::diffuseUniformName, glm::vec4(diffuse.r,diffuse.g,diffuse.b,diffuse.a));
        } else {
            materialData->SetParameterVector4(Constants::ShaderStandard::diffuseUniformName, glm::vec4(diffuse.r,diffuse.g,diffuse.b,diffuse.a));
        }
    }
    { // Specular uniform
        aiColor4D specularColor(0.0f, 0.0f, 0.0f, 1.0f);
        float specularFactor = 1.0f;
        material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
        material->Get(AI_MATKEY_SPECULAR_FACTOR, specularFactor);
        materialData->SetParameterVector4(Constants::ShaderStandard::specularUniformName, specularFactor*glm::vec4(specularColor.r,specularColor.g,specularColor.b,specularColor.b));
    }
    materialData->SetFlag(Constants::ShaderStandard::lightingName, useLighting);
    return materialData;
}

Ref<Texture> Model::loadMaterialTexture(aiMaterial *material, aiTextureType type)
{
    std::string textureFileName;
    bool isSRGB = (type == aiTextureType_DIFFUSE) || (type == aiTextureType_EMISSIVE);
     if (material->GetTextureCount(type) > 0) {
        aiString str;
        material->GetTexture(type, 0, &str);
        textureFileName = std::string(str.C_Str());
    }
    if(textureFileName.empty()){
        return Ref<Texture>(nullptr);
    }
    if(loadedTextures.find(textureFileName) != loadedTextures.end()){
        return loadedTextures[textureFileName]; // Texture already loaded
    } else { // Texture not loaded
        auto pixelsDataToFourChannels = [](const unsigned char *data, int width, int height,
        int nrComponents){
            std::vector<GLubyte> newData;
            if(nrComponents == 4){
                newData = std::vector<GLubyte>(data, data + width * height * nrComponents);
            } else {
                newData = std::vector<GLubyte>(4 * width * height);
            }
            for(int i = 0; i < width * height; i++){
                int j = 0;
                for(j = 0; j < nrComponents; j++){
                    newData[4*i + j] = data[nrComponents * i + j];
                }
                for(; j < 4; j++){
                    newData[4 * i + j] = 0xFF;
                }
            }
            return newData;
        };
        {
            const aiTexture* textureEmbedded = scene->GetEmbeddedTexture(textureFileName.c_str());
            if(textureEmbedded){ // Embedded texture
                // A textura é embutida, use os dados da textura
                int width = textureEmbedded->mWidth;   // largura
                int height = textureEmbedded->mHeight;  // altura
                int nrComponents = 0; // você pode precisar de mais lógica para obter os canais

                // Verifica se os dados da textura estão disponíveis
                if (textureEmbedded->mHeight == 0) { // Indica que a textura é compactada
                    const void* data = textureEmbedded->pcData;
                    int dataSize = textureEmbedded->mWidth; // Verifique se essa é a forma correta de obter o tamanho

                    unsigned char* imageData = stbi_load_from_memory(
                        reinterpret_cast<const unsigned char*>(data),
                        dataSize, // Isso deve ser o tamanho real dos dados binários
                        &width, &height, &nrComponents, 0 // channels será preenchido por stb_image
                    );

                    if (imageData) {
                        Ref<Texture> texture = CreateRef<Texture>();
                        auto newData = pixelsDataToFourChannels(imageData, width, height, nrComponents);
                        gli::format format = isSRGB ? gli::format::FORMAT_RGBA8_SRGB_PACK8 : gli::format::FORMAT_RGBA8_UNORM_PACK8;
                        stbi_image_free(imageData);
                        if(texture->LoadFromMemory(newData, format, width, height)){
                            loadedTextures[textureFileName] = texture;
                            return texture;
                        }
                        return Ref<Texture>(nullptr);
                        
                    } else {
                        std::cerr << "Erro ao carregar textura embutida.\n";
                        return Ref<Texture>(nullptr);
                    }
                } else { // A textura não é compactada
                    nrComponents = 4; // pcData has 4 channels
                    const unsigned char* data = reinterpret_cast<const unsigned char*>(textureEmbedded->pcData);

                    Ref<Texture> texture = CreateRef<Texture>();
                    gli::format format = isSRGB ? gli::format::FORMAT_RGBA8_SRGB_PACK8 : gli::format::FORMAT_RGBA8_UNORM_PACK8;
                    if(texture->LoadFromMemory(std::vector<GLubyte>(data, data + width * height * nrComponents),
                    format, width, height)){
                        loadedTextures[textureFileName] = texture;
                        return texture;
                    }
                    return Ref<Texture>(nullptr);
                }
            }
        }
        // Not embedded texture
        std::vector<std::string> possibleTexturePaths = 
        {
            directory + '/' + textureFileName,
            directory + "/textures/" + textureFileName
        };
        Ref<Texture> texture = CreateRef<Texture>();
        for(auto &&path : possibleTexturePaths){
            if(texture->Load(path, isSRGB)){
                loadedTextures[textureFileName] = texture;
                return texture;
            }
        }
        // If loading failed to all possible paths, return null texture
        return Ref<Texture>(nullptr);
    }
}

bool Model::Load(const std::string &path, Ref<ShaderStandard> defaultShader, bool useLighting, bool flipUVs)
{
    this->defaultShader = defaultShader;
    this->useLighting = useLighting;
    Assimp::Importer importer;
    
    unsigned int flags = aiProcess_Triangulate |
        aiProcess_MakeLeftHanded |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices;
    if(flipUVs)
        flags |= aiProcess_FlipUVs;
    // Carrega o modelo com pós-processamento
    const aiScene* scene = importer.ReadFile(
        path,
        flags
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Erro ao carregar modelo: " << importer.GetErrorString() << std::endl;
        return false;
    }
    this->scene = scene;
    format = path.substr(path.find_last_of('.') + 1);
    directory = path.substr(0, path.find_last_of('/'));
    // Processa o nó raiz da cena
    processNode(scene->mRootNode, scene, aiMatrix4x4());

    return true;
}

const std::vector<std::pair<MeshRendererComponent, TransformComponent>> &Model::GetComponents() const
{
    return components;
}

void Model::SetScale(float scale)
{
    this->scale = scale;
}
