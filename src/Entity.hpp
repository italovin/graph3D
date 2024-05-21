#ifndef ENTITY_H
#define ENTITY_H
#include <entt/entt.hpp>
#include "Scene.hpp"

class Entity{
private:
    entt::entity handle { entt::null};
    Scene* scene;
public:
    Entity(entt::entity handle, Scene* scene);

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args){
        T& component = scene->registry.emplace<T>(handle, std::forward<Args>(args)...);
        return component;
    }
};
#endif