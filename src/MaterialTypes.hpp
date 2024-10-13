#ifndef MATERIAL_TYPES_H
#define MATERIAL_TYPES_H
#include <variant>
#include <glm/glm.hpp>
#include "Base.hpp"
#include "Texture.hpp"

enum class MaterialParameterType{
    Map,
    Float,
    Boolean,
    Vector4
};

struct MaterialParameter{
    std::variant<Ref<Texture>, float, bool, glm::vec4> data;
    MaterialParameterType type;
};

#endif
