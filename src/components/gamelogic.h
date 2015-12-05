#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <component.h>

namespace BQ{

class Behaviour;

typedef std::shared_ptr<Behaviour> logic_ptr;


class GameLogic : public Component
{
public:
    GameLogic();

    void addBehaviour(Behaviour*);

    void addEvent(std::string,GameObject*,std::map<std::string,std::string>);

    void setDelta(float);

    void update();

private:

    std::vector<logic_ptr> behaviours;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {

    }
};
}

#endif // GAMELOGIC_H