#include "Model.hpp"
#include "ShaderStandard.hpp"
#include <stb/stb_image.h>
#include <variant>

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
    enum class TexCoordsType{
        Float, UnsignedShort
    };
    TexCoordsType texCoordsType = TexCoordsType::UnsignedShort;
    std::variant<std::vector<float>, std::vector<unsigned short>> texCoords0;
    std::vector<short> tangents;
    std::vector<short> bitangents;
    std::vector<unsigned char> colors;
    enum class IndicesType{
        UnsignedInt, UnsignedShort
    };
    IndicesType indicesType = IndicesType::UnsignedShort;
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
            texCoordsType = TexCoordsType::Float;
            break;
        }
    }
    if(texCoordsType == TexCoordsType::UnsignedShort){
        texCoords0 = std::vector<unsigned short>();
        std::get<std::vector<unsigned short>>(texCoords0).resize(verticesVec2Size);
    } else if(texCoordsType == TexCoordsType::Float){
        texCoords0 = std::vector<float>();
        std::get<std::vector<float>>(texCoords0).resize(verticesVec2Size);
    }
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
        indicesType = IndicesType::UnsignedShort;
        indices = std::vector<unsigned short>();
        std::get<std::vector<unsigned short>>(indices).reserve(totalNumIndices);
    } else {
        indicesType = IndicesType::UnsignedInt;
        indices = std::vector<unsigned int>();
        std::get<std::vector<unsigned int>>(indices).reserve(totalNumIndices);
    }
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
            if(texCoordsType == TexCoordsType::Float){
                std::get<std::vector<float>>(texCoords0)[texCoordIndex] = texCoordX;
                std::get<std::vector<float>>(texCoords0)[texCoordIndex + 1] = texCoordY;
            } else if(texCoordsType == TexCoordsType::UnsignedShort){
                std::get<std::vector<unsigned short>>(texCoords0)[texCoordIndex] = 
                static_cast<unsigned short>(std::round(texCoordX * 65535.0f));
                std::get<std::vector<unsigned short>>(texCoords0)[texCoordIndex + 1] = 
                static_cast<unsigned short>(std::round(texCoordY * 65535.0f));
            } 

        } else {
            if(texCoordsType == TexCoordsType::Float){
                std::get<std::vector<float>>(texCoords0)[texCoordIndex] = 0.0f;
                std::get<std::vector<float>>(texCoords0)[texCoordIndex + 1] = 0.0f;
            } else if(texCoordsType == TexCoordsType::UnsignedShort){
                std::get<std::vector<unsigned short>>(texCoords0)[texCoordIndex] = 0;
                std::get<std::vector<unsigned short>>(texCoords0)[texCoordIndex + 1] = 0;
            }
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
            if(indicesType == IndicesType::UnsignedInt)
                std::get<std::vector<unsigned int>>(indices).push_back(face.mIndices[j]);
            else if(indicesType == IndicesType::UnsignedShort)
                std::get<std::vector<unsigned short>>(indices).push_back(face.mIndices[j]);
        }
    }

    meshData->PushAttributePosition(vertices);
    if(texCoordsType == TexCoordsType::Float)
        meshData->PushAttributeTexCoord0(std::get<std::vector<float>>(texCoords0));
    else if(texCoordsType == TexCoordsType::UnsignedShort)
        meshData->PushAttributeTexCoord0(std::get<std::vector<unsigned short>>(texCoords0));
    if(hasNormals)
        meshData->PushAttributeNormal(normals);
    if(hasTangentsAndBitangents){
        meshData->PushAttributeTangent(tangents);
        meshData->PushAttributeBitangent(bitangents);
    }
    if(hasColors)
        meshData->PushAttributeColor(colors);
    if(indicesType == IndicesType::UnsignedInt)
        meshData->SetIndices(std::get<std::vector<unsigned int>>(indices), MeshTopology::Triangles);
    else if(indicesType == IndicesType::UnsignedShort)
        meshData->SetIndices(std::get<std::vector<unsigned short>>(indices), MeshTopology::Triangles);
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
    materialData->SetParameterMap("diffuseMap", diffuseMap);
    materialData->SetParameterMap("normalMap", normalMap);
    materialData->SetParameterMap("specularMap", specularMap);
    materialData->SetFlag("lighting", useLighting);
    return materialData;
}

Ref<Texture> Model::loadMaterialTexture(aiMaterial *material, aiTextureType type)
{
    std::string textureFileName;
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
        {
            const aiTexture* texture = scene->GetEmbeddedTexture(textureFileName.c_str());
            if(texture){ // Embedded texture
                // A textura é embutida, use os dados da textura
                int width = texture->mWidth;   // largura
                int height = texture->mHeight;  // altura
                int nrComponents = 0; // você pode precisar de mais lógica para obter os canais

                // Verifica se os dados da textura estão disponíveis
                if (texture->mHeight == 0) { // Indica que a textura é compactada
                    const void* data = texture->pcData;
                    int dataSize = texture->mWidth; // Verifique se essa é a forma correta de obter o tamanho

                    unsigned char* imageData = stbi_load_from_memory(
                        reinterpret_cast<const unsigned char*>(data),
                        dataSize, // Isso deve ser o tamanho real dos dados binários
                        &width, &height, &nrComponents, 0 // channels será preenchido por stb_image
                    );

                    if (imageData) {
                        Ref<Texture> textureRef = SetupTexture(imageData, width, height, nrComponents);
                        loadedTextures[textureFileName] = textureRef;
                        stbi_image_free(imageData);
                        return textureRef;
                    } else {
                        std::cerr << "Erro ao carregar textura embutida.\n";
                        return Ref<Texture>(nullptr);
                    }
                } else { // A textura não é compactada
                    nrComponents = 4; // pcData has 4 channels
                    const unsigned char* data = reinterpret_cast<const unsigned char*>(texture->pcData);
                    std::vector<unsigned char> rgbaData(width * height * 4);

                    for (int i = 0; i < width * height; ++i) {
                        rgbaData[i * 4 + 0] = data[i * 4 + 1]; // Red
                        rgbaData[i * 4 + 1] = data[i * 4 + 2]; // Green
                        rgbaData[i * 4 + 2] = data[i * 4 + 3]; // Blue
                        rgbaData[i * 4 + 3] = data[i * 4 + 0]; // Alpha
                    }
                    Ref<Texture> textureRef = SetupTexture(rgbaData.data(), width, height, nrComponents);
                    loadedTextures[textureFileName] = textureRef;
                    return textureRef;
                }
            }
        }
        // Not embedded texture
        std::string mainTexturePath = directory + '/' + textureFileName;
        std::string secondaryTexturePath = directory + "/textures/" + textureFileName;

        int width, height, nrComponents;
        unsigned char *data = stbi_load(mainTexturePath.c_str(), &width, &height, &nrComponents, 0);
        if(!data){
            data = stbi_load(secondaryTexturePath.c_str(), &width, &height, &nrComponents, 0);
        }
        if(!data){
            std::cerr << "Erro ao carregar textura: " << mainTexturePath << "\n";
            return Ref<Texture>(nullptr);
        }

        Ref<Texture> texture = SetupTexture(data, width, height, nrComponents);
        loadedTextures[textureFileName] = texture;
        stbi_image_free(data);
        return texture;
    }
}

Ref<Texture> Model::SetupTexture(unsigned char *data, int width, int height, int nrComponents)
{
    TextureFormat format = TextureFormat::RGB;
    if (nrComponents == 3)
        format = TextureFormat::RGB;
    else if (nrComponents == 4)
        format = TextureFormat::RGBA;
    else if (nrComponents == 1)
        format = TextureFormat::RED;

    Ref<Texture> texture = CreateRef<Texture>(width, height);
    texture->SetPixelsData(std::vector<GLubyte>(data, data + width * height * nrComponents), format);
    return texture;
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
