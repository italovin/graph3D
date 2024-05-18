#include "Base.hpp"
#include "Transform.hpp"
#include "MeshRenderer.hpp"

class WorldObject{
private:
    MeshRenderer meshRenderer;
    Ref<Transform> transform;
public:
    WorldObject();
    const MeshRenderer &GetMeshRenderer() const;
    const Ref<Transform> &GetTransform() const;
    void SetMeshRenderer(MeshRenderer &&meshRenderer);
};