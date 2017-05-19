#include "Component.h"
#include "EntitySystemManager.h"

namespace ECS
{
    /**
    * This operator gets the parent for the component.
    */
    Entity& BaseComponent::getParent() const
    {
        return ecs->getEntityByID(parent);
    }
}