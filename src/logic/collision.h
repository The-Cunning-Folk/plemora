#ifndef COLLISION_H
#define COLLISION_H

#include <SFML/Graphics.hpp>
#include <string>

namespace BQ {
class Collision
{
public:
    Collision();

    std::string objectA;
    std::string objectB;

    int collidableA;
    int collidableB;

    sf::Vector2f overlap;

};
}

#endif // COLLISION_H
