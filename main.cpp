#include <iostream>
#include <SFML/Graphics.hpp>
#include "EntityComponentSystem.h"

class TransformComponent : public ECS::BaseComponent { public: sf::Transformable transform; };
class TransformSystem : public ECS::BaseSystem<TransformComponent> {};

class SpriteComponent : public ECS::BaseComponent { public: sf::Sprite sprite; };
class SpriteSystem : public ECS::BaseSystem<SpriteComponent> {};

class ShapeComponent : public ECS::BaseComponent { public: sf::RectangleShape shape; };
class ShapeSystem : public ECS::BaseSystem<ShapeComponent> {};

class DrawableComponent : public ECS::BaseComponent {};
class DrawableSystem : public ECS::BaseSystem<DrawableComponent>, public sf::Drawable
{
public:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (auto& c : components)
        {
            ECS::Entity& entity = c.getParent();
            if (entity.alive)
            {
                TransformComponent& transform = entity.getComponent<TransformComponent>();
                sf::Drawable* drawable = nullptr;

                if (entity.hasComponent<SpriteComponent>())
                    drawable = &entity.getComponent<SpriteComponent>().sprite;
                else if (entity.hasComponent<ShapeComponent>())
                    drawable = &entity.getComponent<ShapeComponent>().shape;

                if (drawable)
                {
                    sf::RenderStates own_states = states.transform * transform.transform.getTransform();
                    target.draw(*drawable, own_states);
                }
            }
        }
    }
};

class VelocityComponent : public ECS::BaseComponent { public: sf::Vector2f velocity; };
class VelocitySystem : public ECS::BaseSystem<VelocityComponent>
{
public:
    void update(const float dt)
    {
        for (auto& c : components)
        {
            ECS::Entity& entity = c.getParent();
            if (entity.alive)
            {
                TransformComponent& transform = entity.getComponent<TransformComponent>();
                transform.transform.move(c.velocity * dt);
            }
        }
    }
};

class InputComponent : public ECS::BaseComponent { public: float speed = 200.0f; };
class InputSystem : public ECS::BaseSystem<InputComponent>
{
public:
    void handleEvent(const sf::Event& event)
    {
        for (auto& c : components)
        {
            ECS::Entity& entity = c.getParent();
            if (entity.alive)
            {
                VelocityComponent& velocity = entity.getComponent<VelocityComponent>();

                if (event.type == sf::Event::KeyPressed)
                {
                    switch (event.key.code)
                    {
                    case sf::Keyboard::Left: velocity.velocity.x = -c.speed; break;
                    case sf::Keyboard::Right: velocity.velocity.x = c.speed; break;
                    case sf::Keyboard::Up: velocity.velocity.y = -c.speed; break;
                    case sf::Keyboard::Down: velocity.velocity.y = c.speed; break;
                    default: break;
                    }
                }
                else if (event.type == sf::Event::KeyReleased)
                {
                    switch (event.key.code)
                    {
                    case sf::Keyboard::Left:
                    case sf::Keyboard::Right: velocity.velocity.x = 0.0f; break;
                    case sf::Keyboard::Up:
                    case sf::Keyboard::Down: velocity.velocity.y = 0.0f; break;
                    default: break;
                    }
                }
            }
        }
    }
};

class KillTimerComponent : public ECS::BaseComponent
{
public:
    float time, timer = 0.0f;
};
class KillTimerSystem : public ECS::BaseSystem<KillTimerComponent>
{
public:
    void update(const float dt)
    {
        for (auto& c : components)
        {
            ECS::Entity& entity = c.getParent();
            if (entity.alive)
            {
                c.timer += dt;
                if (c.timer >= c.time)
                    entity.alive = false;
            }
        }
    }
};

enum Groups
{
    Player,
    Bullet
};

void createPlayer(ECS::EntitySystemManager& manager)
{
    ECS::Entity& entity = manager.addEntity(Groups::Player);
    entity.addComponent<TransformComponent>();
    entity.addComponent<SpriteComponent>();
    entity.addComponent<DrawableComponent>();
    entity.addComponent<InputComponent>();
    entity.addComponent<VelocityComponent>().velocity = { 20.0f, 20.0f };
}

void createBullet(ECS::EntitySystemManager& manager, const sf::Vector2f& position)
{
    ECS::Entity& entity = manager.addEntity(Groups::Bullet);
    entity.addComponent<VelocityComponent>().velocity = { 50.0f, 0.0f };
    entity.addComponent<DrawableComponent>();
    entity.addComponent<KillTimerComponent>().time = 2.5f;
    TransformComponent& transform = entity.addComponent<TransformComponent>();
    ShapeComponent& shape = entity.addComponent<ShapeComponent>();
    
    transform.transform.setOrigin(10.0f, 10.0f);
    transform.transform.setPosition(position);

    shape.shape.setSize(sf::Vector2f(20.0f, 20.0f));
    shape.shape.setFillColor(sf::Color::Red);
}

int main()
{
    ECS::EntitySystemManager manager;
    manager.addSystem<TransformSystem>();
    manager.addSystem<SpriteSystem>();
    manager.addSystem<ShapeSystem>();
    manager.addSystem<VelocitySystem>();
    manager.addSystem<InputSystem>();
    manager.addSystem<KillTimerSystem>();
    DrawableSystem& drawables = manager.addSystem<DrawableSystem>();

    createPlayer(manager);

    sf::Texture texture;
    texture.loadFromFile("Player.png");
    manager.getEntitiesByGroup(Groups::Player)[0]->getComponent<SpriteComponent>().sprite.setTexture(texture);

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
            else if (event.type == sf::Event::MouseButtonPressed)
                createBullet(manager, { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) });
            manager.handleEvent(event);
        }

        manager.update(dt);

        window.clear();
        window.draw(drawables);
        window.display();
    }

    return 0;
}