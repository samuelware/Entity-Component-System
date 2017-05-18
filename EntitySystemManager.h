#pragma once

#include <vector>
#include <cassert>
#include <memory>
#include <algorithm>
#include <list>
#include <map>
#include <typeindex>
#include <SFML/Window/Event.hpp>

namespace ECS
{
    class GenericSystem;
    class Entity;

    /**
    * This class manages all of the entities and systems in the game. It makes
    * sure that entities know about the systems and that all systems are updated.
    */
    class EntitySystemManager
    {
    public:
        /**
        * This method adds an entity to the manager and returns a pointer
        * to the entity. Pointers will become invalid when another entity is added.
        */
        Entity* addEntity(const int group);

        /**
        * This method gets an entity by its ID. This is used by Components to get entities.
        */
        Entity* getEntityByID(const int id);

        /**
        * This method returns a vector of pointers to entities by their group. This
        * lets you perform an operation on multiple entities of a certain type.
        */
        std::vector<Entity*> getEntitiesByGroup(const int group);

        void handleEvent(const sf::Event& event);

        /**
        * This is where all systems and components are updated, and any dead entities are removed.
        */
        void update(const float dt);

        /**
        * This method adds a system to the manager. It also returns a pointer to the system.
        */
        template <typename SystemType>
        SystemType* addSystem()
        {
            static_assert(std::is_base_of<GenericSystem, SystemType>::value, "'SystemType' must inherit 'GenericSystem'.");
            assert(!hasSystem<SystemType>());
            SystemType* system = new SystemType();
            systems[std::type_index(typeid(SystemType))] = std::unique_ptr<SystemType>(system);
            return system;
        }

        /**
        * This method returns a pointer to a system by its component type. The pointer
        * must by dynamically cast to the desired system.
        */
        template <typename ComponentType>
        GenericSystem* getSystemByComponent()
        {
            for (auto& system : systems)
                if (*system.second == std::type_index(typeid(ComponentType)))
                    return system.second.get();
            return nullptr;
        }

        /**
        * This method checks if the manager has a certain system or not.
        */
        template <typename SystemType>
        bool hasSystem()
        {
            static_assert(std::is_base_of<GenericSystem, SystemType>::value, "'SystemType' must inherit 'GenericSystem'.");
            return systems.find(std::type_index(typeid(SystemType))) != systems.end();
        }

        /**
        * This method sets wether a system is active (updates) or not.
        */
        template <typename SystemType>
        void setSystemActive(const bool active)
        {
            static_assert(std::is_base_of<GenericSystem, SystemType>::value, "'SystemType' must inherit 'GenericSystem'.");
            assert(hasSystem<SystemType>());
            systems[std::type_index(typeid(SystemType))]->active = active;
        }

    private:
        int ids = 0;
        std::vector<Entity> entities;
        std::map<std::type_index, std::unique_ptr<GenericSystem>> systems;
    };
}