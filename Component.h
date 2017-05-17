#pragma once

namespace ECS
{
    class EntitySystemManager;
    class Entity;

    /**
    * Every component in the game should inherit from this struct.
    */
    class BaseComponent
    {
    public:
        /**
        * This method sets the parent for the component.
        */
        void setParent(const int parent) { this->parent = parent; }

        /**
        * This method sets the ecs for the component.
        */
        void setECS(EntitySystemManager* ecs) { this->ecs = ecs; }

        /**
        * This operator checks to see if the component has a certain parent.
        */
        bool operator==(const int rhs) { return parent == rhs; }

        /**
        * This operator gets the parent for the component.
        */
        Entity* getParent() const;

    private:
        int parent;
        EntitySystemManager* ecs;
    };
}