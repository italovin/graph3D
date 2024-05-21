#include "Scene.hpp"
#include "Entity.hpp"

Entity Scene::CreateEntity(){
    Entity entity = {registry.create(), this};
    return entity;
}