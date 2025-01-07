#include "Shader.hpp"

bool Shader::operator==(const Shader &other) const{
    return maps == other.maps && flags == other.flags && attributes == other.attributes
    && indexType == other.indexType;
}
std::size_t Shader::Hash() const {
    auto mapHash = [](const std::unordered_map<std::string, bool>& map) {
        std::size_t hash = 0;

        for (const auto& [key, value] : map) {
            std::size_t keyHash = std::hash<std::string>()(key);
            std::size_t valueHash = std::hash<bool>()(value);

            hash ^= keyHash ^ (valueHash << 1);
        }

        return hash;
    };
    auto attributesMapHash = [](const std::unordered_map<std::string, std::pair<bool, MeshAttribute>>& map) {
        std::size_t hash = 0;

        for (const auto& [key, value] : map) {
            std::size_t keyHash = std::hash<std::string>()(key);
            std::size_t valueHash = std::hash<bool>()(value.first) ^ 
            (std::hash<int>()(value.second.location) << 1) ^
            (std::hash<MeshAttributeType>()(value.second.type) << 2) ^
            (std::hash<MeshAttributeFormat>()(value.second.format) << 3) ^ 
            (std::hash<MeshAttributeAlias>()(value.second.alias) << 4) ^ 
            (std::hash<bool>()(value.second.normalized) << 5) ^ 
            (std::hash<bool>()(value.second.interpretAsInt) << 6);

            hash ^= keyHash ^ (valueHash << 1);
        }

        return hash;
    };
    const std::size_t prime = 137;
    std::size_t mapsHash = mapHash(maps);
    std::size_t flagsHash = mapHash(flags);
    std::size_t attributesHash = attributesMapHash(attributes);
    std::size_t indexTypeHash = std::hash<MeshIndexType>()(indexType);

    std::size_t combinedHash = mapsHash;
    combinedHash ^= flagsHash + prime + (combinedHash << 6) + (combinedHash >> 2);
    combinedHash ^= attributesHash + prime + (combinedHash << 6) + (combinedHash >> 2);
    combinedHash ^= indexTypeHash + prime + (combinedHash << 6) + (combinedHash >> 2);
    return combinedHash;
}

const std::unordered_map<std::string, bool>& Shader::GetMaps(){
    return maps;
}
const std::unordered_map<std::string, bool>& Shader::GetFlags(){
    return flags;
}
const std::unordered_map<std::string, std::pair<bool, MeshAttribute>>& Shader::GetAttributes(){
    return attributes;
}

ShaderCode Shader::ProcessCode(){
    return ShaderCode();
}
