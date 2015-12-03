#ifndef COLLISIONENGINE_H
#define COLLISIONENGINE_H

#include <engine.h>
#include <circularcollidable.h>

namespace BQ{
class CollisionEngine : public Engine
{
public:
    CollisionEngine();

    CircularCollidable* addCircle();

protected:

    std::vector<CircularCollidable> circles;
};
}

#endif // COLLISIONENGINE_H
