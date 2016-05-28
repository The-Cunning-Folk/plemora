#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SFML/Graphics.hpp>

class SpriteSheet
{
public:
    SpriteSheet();

    std::string texture;
    std::map<std::string,std::vector<sf::FloatRect>> spriteFrames;

};

#endif // SPRITESHEET_H
