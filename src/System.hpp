#include "entt/entt.hpp"

class System{
public:
    // Executes at system startup in application
    virtual void Start(entt::registry &registry) = 0;
    // Executes for each frame
    virtual void Update(entt::registry &registry, float deltaTime) = 0;
};
