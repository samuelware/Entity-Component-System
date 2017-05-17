#include <iostream>
#include <SFML/Graphics.hpp>
#include "EntityComponentSystem.h"

class PositionComponent : public ECS::BaseComponent, public sf::Vector2f { using sf::Vector2f::Vector2; };
class PositionSystem : public ECS::BaseSystem<PositionComponent> {};

class VelocityComponent : public ECS::BaseComponent, public sf::Vector2f { using sf::Vector2f::Vector2; };
class VelocitySystem : public ECS::BaseSystem<VelocityComponent> {};

class MovementComponent : public ECS::BaseComponent {};
class MovementSystem : public ECS::BaseSystem<MovementComponent>
{
public:
    void update(const float dt)
    {
        for (auto& c : components)
        {
            PositionComponent& position = *c.getParent()->getComponent<PositionComponent>();
            VelocityComponent& velocity = *c.getParent()->getComponent<VelocityComponent>();

            position += velocity * dt;
        }
    }
};

class ShapeComponent : public ECS::BaseComponent, public sf::RectangleShape { using sf::RectangleShape::RectangleShape; };
class ShapeSystem : public ECS::BaseSystem<ShapeComponent>, public sf::Drawable
{
public:
    void update(const float dt)
    {
        for (auto& c : components)
        {
            PositionComponent& position = *c.getParent()->getComponent<PositionComponent>();
            c.setPosition(position);
        }
    }

    void draw(sf::RenderTarget& target, sf::RenderStates) const
    {
        for (auto& c : components)
            target.draw(c);
    }
};

class PlayerComponent : public ECS::BaseComponent
{
public:
    float speed = 250.0f;
};
class PlayerSystem : public ECS::BaseSystem<PlayerComponent>
{
public:
    void update(const float dt)
    {
        for (auto& c : components)
        {
            ECS::Entity* player = c.getParent();
            VelocityComponent& velocity = *player->getComponent<VelocityComponent>();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                velocity.x = -c.speed;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                velocity.x = c.speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                velocity.y = -c.speed;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                velocity.y = c.speed;
        }
    }
};

enum Groups
{
    Player
};

int main()
{
    ECS::EntitySystemManager manager;
    manager.addSystem<PositionSystem>();
    manager.addSystem<VelocitySystem>();
    manager.addSystem<MovementSystem>();
    ShapeSystem& shapes = *manager.addSystem<ShapeSystem>();
    manager.addSystem<PlayerSystem>();

    ECS::Entity* player = manager.addEntity(Groups::Player);
    player->addComponent<PositionComponent>(sf::Vector2f(100.0f, 100.0f));
    player->addComponent<VelocityComponent>(sf::Vector2f(150.0f, 0.0f));
    player->addComponent<MovementComponent>();
    player->addComponent<ShapeComponent>(sf::Vector2f(100.0f, 100.0f));
    player->addComponent<PlayerComponent>();

    sf::RenderWindow window(sf::VideoMode(640, 480), "ECS");
    window.setFramerateLimit(60);

    sf::Clock clock;
    float dt;

    sf::Event event;
    while (window.isOpen())
    {
        dt = clock.restart().asSeconds();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        manager.update(dt);

        window.clear();
        window.draw(shapes);
        window.display();
    }

    return 0;
}