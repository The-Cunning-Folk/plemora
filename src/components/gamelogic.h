#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <component.h>
#include <collidable.h>

namespace BQ{
class Behaviour;
class GameLogic : public Component
{
public:
    GameLogic();

    void addBehaviour(Behaviour *);

    void addEvent(std::string,std::string,std::map<std::string,std::string>);

    void collisionWith(GameObject&,const Collidable &,const Collidable&);

    void setDelta(float);

    void update();

     std::vector<Behaviour*> behaviours;

private:


};
}

#endif // GAMELOGIC_H
