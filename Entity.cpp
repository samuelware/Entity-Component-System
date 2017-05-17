#include "Entity.h"

namespace ECS
{
    Entity::Entity(EntitySystemManager* ecs, const int id, const int group) : ecs(ecs), id(id), group(group) {}
    bool Entity::operator==(const int rhs) { return id == rhs; }
    bool Entity::inGroup(const int group) { return this->group == group; }
}