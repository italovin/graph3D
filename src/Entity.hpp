#ifndef ENTITY_H
#define ENTITY_H
#include <entt/entt.hpp>
#include "Scene.hpp"
#include "BasicComponents.hpp"

class Entity{
private:
    entt::entity handle { entt::null};
    Scene* scene;
public:
    TransformComponent& transform = this->AddComponent<TransformComponent>();
    Entity() = delete;
    Entity(entt::entity handle, Scene* scene);

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args){
        T& component = scene->registry.emplace_or_replace<T>(handle, std::forward<Args>(args)...);
        return component;
    }

    template<typename T>
    T& GetComponent()
    {
        return scene->registry.get<T>(handle);
    }

    template<typename T>
    bool HasComponent()
    {
        return scene->registry.all_of<T>(handle);
    }
};
#endif