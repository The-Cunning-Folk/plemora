#ifndef GAMEOBJECTFACTORY_H
#define GAMEOBJECTFACTORY_H

#include <gameobject.h>
#include <gameobjectstack.h>

#include <componentfactory.h>

#include<factory.h>

#include<playerbehaviours.h>
#include<flowpathingbehaviours.h>

namespace BQ{
class GameObjectFactory : public Factory
{
public:

    GameObjectFactory();
    void setStack(GameObjectStack*);

    //ridiculous number of add functions go here, need some way to generate archetypes for items from file

    ComponentFactory* componentFactory;

    GameObject& newObject();
    GameObject& newObject(std::string);

    GameObject& newCollisionObject();

    GameObject & newImmovableObject();

    GameObject& newPlayerObject();

    GameObject& newPathingObject();
    GameObject& newPathingObject(std::string);

    GameObject & makePhysicsObject(GameObject &);
    GameObject & makePathingObject(GameObject &);

    ComponentFactory *getComponentFactory() const;
    void setComponentFactory(ComponentFactory *value);


private:

    GameObjectStack * gameObjects;

};
}

#endif // GAMEOBJECTFACTORY_H
