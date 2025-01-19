#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>  // Para importar cenas
#include <assimp/scene.h>       // Estrutura de cena
#include <assimp/postprocess.h> // Pós-processamento
#include <vector>
#include "ShaderStandard.hpp"
#include "Components.hpp"
class Model{
private:
    Ref<ShaderStandard> defaultShader; // Default shader model
    std::unordered_map<std::string, Ref<Texture>> loadedTextures;
    std::string format;
    std::string directory;
    const aiScene* scene;
    bool useLighting = true;
    std::vector<std::pair<MeshRendererComponent, TransformComponent>> components;
    float scale = 1.0f;
    void processNode(aiNode *node, const aiScene *scene, const aiMatrix4x4& parentTransform);
    Ref<Mesh> processMesh(aiMesh *mesh);
    Ref<Material> processMaterial(aiMaterial *material);
    Ref<Texture> loadMaterialTexture(aiMaterial *material, aiTextureType type);
public:
    bool Load(const std::string &path, Ref<ShaderStandard> defaultShader, bool useLighting = true,
    bool flipUVs = false);
    const std::vector<std::pair<MeshRendererComponent, TransformComponent>> &GetComponents() const;
    // This is used to adjust scaling in some models with dimensions out of proportion for the scene
    void SetScale(float scale);
};
#endif