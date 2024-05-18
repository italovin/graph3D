#include "WorldObject.hpp"

WorldObject::WorldObject(){
    transform = CreateRef<Transform>();
}

const MeshRenderer &WorldObject::GetMeshRenderer() const{
    return this->meshRenderer;
}

const Ref<Transform> &WorldObject::GetTransform() const{
    return this->transform;
}

void WorldObject::SetMeshRenderer(MeshRenderer &&meshRenderer){
    this->meshRenderer = meshRenderer;
}