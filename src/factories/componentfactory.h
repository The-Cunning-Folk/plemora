#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include <factory.h>
#include <component.h>
#include <transform.h>
#include <gameobject.h>

#include <transformengine.h>
#include <inputengine.h>
#include <eventengine.h>
#include <collisionengine.h>
#include <physicsengine.h>

#include <componentloader.h>
#include <gameobjectloader.h>
#include <grid.h>

using namespace BQ;

class ComponentFactory : public Factory
{
public:
    ComponentFactory();

    ComponentLoader* componentLoader;
    GameObjectLoader* gameObjectLoader;
    Grid* grid;

    Transform& newTransform();
    Transform& newTransform(std::string);
    Transform& newChildTransform(Transform&);

    PlayerInput& newPlayerInput();
    PlayerInput& newPlayerInput(std::string);

    GameLogic& newGameLogic();
    GameLogic& newGameLogic(std::string);

    Collidable& newCollidable();
    Collidable& newCollidable(std::string);
    Collidable& newRectCollidable(sf::FloatRect);
    Collidable& newRandomCollidable();

    RigidBody& newRigidBody();
    RigidBody& newRigidBody(std::string);

    TransformEngine *getTransformEngine() const;
    void setTransformEngine(TransformEngine *value);

    InputEngine *getInputEngine() const;
    void setInputEngine(InputEngine *value);

    EventEngine *getEventEngine() const;
    void setEventEngine(EventEngine *value);

    CollisionEngine *getCollisionEngine() const;
    void setCollisionEngine(CollisionEngine *value);

    ComponentLoader *getComponentLoader() const;
    void setComponentLoader(ComponentLoader *value);

    PhysicsEngine *getPhysicsEngine() const;
    void setPhysicsEngine(PhysicsEngine *value);

    GameObjectLoader *getGameObjectLoader() const;
    void setGameObjectLoader(GameObjectLoader *value);

    Grid *getGrid() const;
    void setGrid(Grid *value);

private:

    TransformEngine* transformEngine;
    InputEngine* inputEngine;
    EventEngine* eventEngine;
    CollisionEngine* collisionEngine;
    PhysicsEngine* physicsEngine;


};

#endif // COMPONENTFACTORY_H
