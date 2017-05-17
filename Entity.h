#pragma once

#include <vector>
#include <typeindex>
#include <cassert>
#include <bitset>
#include <utility>
#include "EntitySystemManager.h"

namespace ECS
{
    /**
    * This class represents an entity. All this class does is keep track
    * of which components are attached to the entity and interact with systems that handle
    * the components.
    */
    class Entity
    {
    public:
        Entity(EntitySystemManager* ecs, const int id, const int group);
        bool operator==(const int id);
        bool inGroup(const int group);

        /**
        * This method adds a component to the entity. It actually adds a component to the system
        * responsible for that specific component.
        */
        template <typename ComponentType, typename ...Args>
        ComponentType* addComponent(Args&& ...args)
        {
            static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");
            assert(!hasComponent<ComponentType>());
            components.emplace_back(typeid(ComponentType));
            BaseSystem<ComponentType>* system = dynamic_cast<BaseSystem<ComponentType>*>(ecs->getSystemByComponent<ComponentType>());
            return system->addComponent(id, ecs, std::forward<Args>(args)...);
        }

        /**
        * This method removes a component from the entity, and the system responsible for
        * that specific component.
        */
        template <typename ComponentType>
        void removeComponent()
        {
            static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");
            assert(hasComponent<ComponentType>());
            components.erase(std::find(components.begin(), components.end(), std::type_index(typeid(ComponentType))));
            BaseSystem<ComponentType>* system = dynamic_cast<BaseSystem<ComponentType>*>(ecs->getSystemByComponent<ComponentType>());
            system->removeComponent(id);
        }

        /**
        * This method returns a pointer to the component for the entity from the system responsible for that
        * specific component.
        */
        template <typename ComponentType>
        ComponentType* getComponent()
        {
            static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");
            assert(hasComponent<ComponentType>());
            BaseSystem<ComponentType>* system = dynamic_cast<BaseSystem<ComponentType>*>(ecs->getSystemByComponent<ComponentType>());
            return system->getComponent(id);
        }

        /**
        * This method checks if the entity has a specific component attached to it.
        */
        template <typename ComponentType>
        bool hasComponent()
        {
            static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");
            auto it = std::find(components.begin(), components.end(), std::type_index(typeid(ComponentType)));
            return it != components.end();
        }

    public:
        bool alive = true;
        int group;

    private:
        int id;
        EntitySystemManager* ecs;
        std::vector<std::type_index> components;
    };
}