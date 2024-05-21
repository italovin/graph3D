#ifndef SCENE_H
#define SCENE_H
#include "entt/entt.hpp"

class Entity;

class Scene{
private:
    entt::registry registry;
public:
    Entity CreateEntity();
};
#endif