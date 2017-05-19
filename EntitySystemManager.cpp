#include "EntitySystemManager.h"
#include "System.h"

namespace ECS
{
    /**
    * This method adds an entity to the manager and returns a pointer
    * to the entity. Pointers will become invalid when another entity is added.
    */
    Entity& EntitySystemManager::addEntity(const int group)
    {
        entities.emplace_back(this, ids++, group);
        return entities.back();
    }

    /**
    * This method gets an entity by its ID. This is used by Components to get entities.
    */
    Entity& EntitySystemManager::getEntityByID(const int id)
    {
        return *std::find(entities.begin(), entities.end(), id);
    }

    /**
    * This method returns a vector of pointers to entities by their group. This
    * lets you perform an operation on multiple entities of a certain type.
    */
    std::vector<Entity*> EntitySystemManager::getEntitiesByGroup(const int group)
    {
        std::vector<Entity*> entity_ptrs;
        for (auto& entity : entities)
            if (entity.inGroup(group))
                entity_ptrs.push_back(&entity);
        return entity_ptrs;
    }

    void EntitySystemManager::handleEvent(const sf::Event& event)
    {
        for (auto& system : systems)
            if (system.second->active)
                system.second->handleEvent(event);
    }

    /**
    * This is where all systems and components are updated, and any dead entities are removed.
    */
    void EntitySystemManager::update(const float dt)
    {
        for (auto& system : systems)
            if (system.second->active)
                system.second->update(dt);
        for (auto& system : systems)
            system.second->removeDeadComponents();

        // Remove any dead entities.
        entities.erase(std::remove_if(entities.begin(), entities.end(),
            [](const Entity& entity)
            {
                return !entity.alive;
            }
        ), entities.end());
    }
}