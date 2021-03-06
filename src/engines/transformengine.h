#ifndef TRANSFORMENGINE_H
#define TRANSFORMENGINE_H

#include <engine.h>
#include <transform.h>


namespace BQ{

class TransformEngine : public Engine
{
public:
    TransformEngine();

    CrossShape cross;

    std::vector<std::string> objectsInRange;

    bool wrapAround;
    sf::FloatRect bounds;

    void start();
    void run();
    void finish();
    void drawDebug();
    void wake();

    void runStep();
    void runCorrections();

    std::vector<GridSquare> activeGridLocations;

    bool getWrapAround() const;
    void setWrapAround(bool value);

    sf::FloatRect getBounds() const;
    void setBounds(const sf::FloatRect &value);
    std::vector<std::string> getObjectsInRange() const;
    void setObjectsInRange(const std::vector<std::string> &value);
};
}

#endif // TRANSFORMENGINE_H
