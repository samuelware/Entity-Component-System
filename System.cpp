#include "System.h"

namespace ECS
{
    GenericSystem::GenericSystem(const std::type_index& component) : component(component) {}
    bool GenericSystem::operator==(const std::type_index& rhs) { return component == rhs; }
}