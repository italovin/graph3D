#ifndef SCENE_H
#define SCENE_H
#include "entt/entt.hpp"

class Entity;

class Scene{
public:
    entt::registry registry;
    Entity CreateEntity();
};
#endif