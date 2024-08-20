#include <unordered_map>
#include <string>
#include <variant>
#include <optional>
#include <glm/glm.hpp>
#include "Base.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

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

class Material{
private:
    std::unordered_map<std::string, MaterialParameter> parameters;
    void AddParameter(const std::string &name, MaterialParameterType type);
    void DeleteParameters();
    MaterialParameterType GetParameterType(GLenum type);
public:
    Material(const Ref<Shader> &shader);
    void SetShader(const Ref<Shader> &shader);
    void SetParameterMap(const std::string &name, Ref<Texture> value);
    void SetParameterFloat(const std::string &name, float value);
    void SetParameterBoolean(const std::string &name, bool value);
    void SetParameterVector4(const std::string &name, glm::vec4 value);
    std::optional<Ref<Texture>> GetParameterMap(const std::string &name);
    std::optional<float> GetParameterFloat(const std::string &name);
    std::optional<bool> GetParameterBoolean(const std::string &name);
    std::optional<glm::vec4> GetParameterVector4(const std::string &name);
};