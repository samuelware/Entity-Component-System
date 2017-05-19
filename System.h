#pragma once

#include <typeindex>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>
#include <SFML/Window/Event.hpp>
#include "Component.h"
#include "Entity.h"

namespace ECS
{
    /**
    * This class exists so that we don't have to provide templates
    * when storing systems in the manager.
    */
    class GenericSystem
    {
    public:
        GenericSystem(const std::type_index& component);
        bool operator==(const std::type_index& component);

        virtual void handleEvent(const sf::Event& event) {}
        virtual void update(const float dt) {}
        virtual void removeDeadComponents() {}

    public:
        bool active = true;

    protected:
        std::type_index component;
    };

    /**
    * Every system in the game should inherit from this class. This class
    * contains the vector of the components it processes to reduce cache misses and improve performance.
    */
    template <typename ComponentType>
    class BaseSystem : public GenericSystem
    {
    public:
        BaseSystem() : GenericSystem(std::type_index(typeid(ComponentType)))
        {
            static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");
        }

        /**
        * This method adds a component to the system and provide arguments for the component's constructor.
        */
        template <typename ...Args>
        ComponentType& addComponent(const int parent, EntitySystemManager* ecs, Args&& ...args)
        {
            components.emplace_back(std::forward<Args>(args)...);
            components.back().setParent(parent);
            components.back().setECS(ecs);
            return components.back();
        }

        /**
        * This method returns a pointer to the component belonging to the parent.
        */
        ComponentType& getComponent(const int parent)
        {
            return *std::find(components.begin(), components.end(), parent);
        }

        /**
        * This method removes the component belonging to the parent.
        */
        void removeComponent(const int parent)
        {
            auto it = std::find(components.begin(), components.end(), parent);
            if (it == components.end()) return;
            components.erase(it);
        }

        /**
        * This method removes components from entities which are dead, as to not leave
        * lingering components lying in memory.
        */
        void removeDeadComponents()
        {
            components.erase(std::remove_if(components.begin(), components.end(),
                [](const ComponentType& component)
                {
                    return !component.getParent().alive;
                }
            ), components.end());
        }

    protected:
        std::vector<ComponentType> components;
    };
}