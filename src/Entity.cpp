#include "Entity.hpp"

Entity::Entity(entt::entity handle, Scene * scene)
{
    this->handle = handle;
    this->scene = scene;
}