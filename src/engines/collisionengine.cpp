#include "collisionengine.h"

#include<eventfactory.h>
#include<componentloader.h>
#include<gameobjectloader.h>
#include<game.h>


using namespace BQ;

CollisionEngine::CollisionEngine() : Engine()
{
    placeholder = "auto_coll_";
    quadtree.setRegion(sf::FloatRect(0,0,480,270));
    quadtree.initialise();
    rectShape.setOutlineThickness(1.0);
    rectShape.setFillColor(sf::Color::Transparent);
    overlapThreshold = 0.1f;
}

bool CollisionEngine::checkCollision(Collidable & a,Collidable & b)
{

    if(a.getParent() == b.getParent())
    {
        return false;
    }

    for(int i=0; i<a.collidingWith.size();i++) //avoid sending the same collision to the event engine twice!
    {
        if(a.collidingWith[i] == b.index)
        {
            return true;
        }
    }

    if(a.tBox.intersects(b.tBox))
    {

        //SAT HERE
        //do polygon sweep here

        Transform & tA = componentLoader->getTransform(a.getTransform());
        Transform & tB = componentLoader->getTransform(b.getTransform());

        sf::Vector2f dDA = tA.position - tA.lastFrame; //displacement of A
        sf::Vector2f dDB = tB.position - tB.lastFrame;

        float flSteps = maths->max(maths->max(dDA.x/(0.5*a.bBox.width),dDA.y/(0.5*a.bBox.height))
                                   ,maths->max(dDB.x/(0.5*b.bBox.width),dDB.y/(0.5*b.bBox.height)));

        flSteps = flSteps > 1.0f ? flSteps : 1.0f;

        float flSweepSteps = ceil(flSteps);
        int sweepSteps = (int) flSweepSteps;
        float invFlSweepSteps = 1.0f/flSweepSteps;

        sf::Vector2f DstepA = dDA*invFlSweepSteps;
        sf::Vector2f DstepB = dDB*invFlSweepSteps;

        sf::Vector2f overlap = sf::Vector2f(0,0);

        for(int i=0; i<sweepSteps+1; i++)
        {
            sf::Vector2f pPosA = ((float)i)*DstepA + tA.lastFrame;
            sf::Vector2f pPosB = ((float)i)*DstepB + tB.lastFrame;
            ConvexPolygon pA = a.polygon;
            ConvexPolygon pB = b.polygon;
            pA.position = pPosA;
            pB.position = pPosB;
            pA.update();
            pB.update();
            if(pA.bBox.intersects(pB.bBox))
            {
                overlap = separatingAxisCheck(pA,pB,tA,tB);

                if(maths->mag(overlap)>overlapThreshold)
                {
                    if(a.solid)
                    {
                        tA.correction += pPosA -tA.position;
                    }
                    if(b.solid)
                    {
                        tB.correction += pPosB - tB.position;
                    }
                    break;
                }
            }
        }

        //end polygon sweep
        float oMag = maths->mag(overlap);

        if(oMag > overlapThreshold) //allow for a 0.1% floating point error
        {
            a.colliding = true;
            b.colliding = true;

            a.collidingWith.push_back(b.index);
            b.collidingWith.push_back(a.index);

            if((maths->dot(tA.lastFrame - tB.lastFrame,overlap)) > 0)
            {
                overlap = -overlap;
            }

            if(a.solid && b.solid)
            {
                if((!a.immovable && !b.immovable)
                        && ((!a.diminutive && !b.diminutive)
                        || (a.diminutive && b.diminutive)))
                {
                    sf::Vector2f halfOverlap(0.5*overlap.x,0.5*overlap.y);
                    tA.correction -= halfOverlap;
                    tB.correction += halfOverlap;
                }
                else if((a.immovable && !b.immovable) || (!a.diminutive && b.diminutive))
                {
                    tB.correction += overlap;
                }
                else if((!a.immovable && b.immovable)  || (a.diminutive && !b.diminutive))
                {
                    tA.correction -= overlap;
                }
            }

            Collision c;
            c.objectA = a.getParent();
            c.objectB = b.getParent();
            c.collidableA = a.index;
            c.collidableB = b.index;
            c.overlap = overlap;
            collisions.push_back(c);



            return true;
        }
        else {
            return false;
        }
    }
}

sf::Vector2f CollisionEngine::separatingAxisCheck(ConvexPolygon & a, ConvexPolygon & b, Transform& tA, Transform& tB)
{
    //first poly
    std::vector<sf::Vector2f> axes;
    MTV mtv;
    sf::Vector2f dA = tA.position - tA.lastFrame;
    sf::Vector2f dB = tB.position - tB.lastFrame;
    float dMA = maths->mag(dA);
    float dMB = maths->mag(dB);
    if(a.points.size() < 1 || b.points.size() < 1) {return sf::Vector2f(0,0);}
    for(int i=1; i<a.points.size();i++)
    {
        //get difference
        sf::Vector2f axis = a.points[i] - a.points[i-1];
        //find normal vector
        sf::Vector2f normAxis = maths->unitNormal(axis);
        if(maths->dot(a.points[i],normAxis) < 0) {normAxis = -normAxis;}

        axes.push_back(normAxis);
    }
    for(int i=1; i<b.points.size();i++)
    {
        //get difference
        sf::Vector2f axis = b.points[i] - b.points[i-1];
        //find normal vector
        sf::Vector2f normAxis = maths->unitNormal(axis);
        if(maths->dot(b.points[i],normAxis) < 0) {normAxis = -normAxis;}

        axes.push_back(normAxis);

    }
    for(int i=0; i<axes.size(); i++)
    {
        Projection pA = projection(a,axes[i]);
        Projection pB = projection(b,axes[i]);

        if(!pA.overlaps(pB)) {return sf::Vector2f(0,0);}

        float o = pA.getOverlap(pB);

        float dotA = maths->dot(dA,axes[i]);
        float dotB = maths->dot(dB,axes[i]);

        if(o < mtv.overlap && (dotA >=0 || dotB>=0))
        {
            mtv.overlap = o;
            mtv.direction = axes[i];
        }

    }

    sf::Vector2f mtvVector = mtv.overlap*mtv.direction;
    return mtvVector;
}

Projection CollisionEngine::projection(ConvexPolygon & shape, sf::Vector2f axis)
{
    float min = maths->dot(shape.position + shape.points[0],axis);
    float max = min;
    for(int i=0; i<shape.points.size(); i++)
    {
        sf::Vector2f p = shape.position + shape.points[i];
        float dot = maths->dot(p,axis);
        if(dot < min)
        {
            min = dot;
        } else if (dot > max)
        {
            max = dot;
        }
    }
    return (Projection(min,max));
}

void CollisionEngine::start()
{
    collisions.clear();
    quadtree.clear();
    ComponentLoader& components = *componentLoader;
    std::vector<GridSquare> gridSquares = grid->getActiveSquares();

    float gridEdge = (float) grid->getScale();
    float halfGridEdge =0.5*(gridEdge);

    for(int i=0; i<gridSquares.size(); i++)
    {
        GridSquare & g = gridSquares[i];
        g.objectsInContact.clear();
        g.collidablesInContact.clear();
    }

    for(unsigned int j=0; j<activeComponents.size(); j++)
    {

        std::map<int, int> xRowsMin;
        std::map<int, int> xRowsMax;

        int minY;
        int maxY;

        int i=activeComponents[j];

        Collidable & c = game->getCurrentLevel().objects.collidables[i];

        Transform & t = components.getTransform(c.getTransform());
        ConvexPolygon& p = c.polygon;
        p.setPosition(t.getPosition());

        sf::FloatRect lastBox=c.polygon.bBox;

        c.update();

        //bresenham precalc

        c.gridEdges.clear();

        c.gridEdges = grid->bresenhamPolygonEdge(c.polygon);

        for(int k=0; k<c.gridEdges.size(); k++)
        {
            GridSquare & g = grid->getActiveGridSquareFromGlobalCoords(c.gridEdges[k]);
            g.debugColor = sf::Color::Magenta;
        }

        c.gridInnerArea.clear();

        c.tBox = maths->makeCompoundRect(lastBox,c.polygon.bBox);


        for(int k=0 ; k<c.gridEdges.size(); k++)
        {
            if(!grid->isActiveGlobal(c.gridEdges[k])) {continue;}
            GridSquare& gReal = grid->getActiveGridSquareFromGlobalCoords(c.gridEdges[k]);
            gReal.addObjectInContact(c.getParent());
            gReal.addCollidableInContact(c.index);
//            if(!gReal.impassable)
//            {
//                gReal.impassable = !c.pathable;
//            }
            if(!c.pathable)
            {
                //is it more than half full?
                if(maths->getArea(maths->findIntersectionRegion(gReal.region,p.bBox)) >= halfGridEdge*gridEdge)
                {
                    //gReal.impassable = true;
                    gReal.workFunction += 1E7;
                    gReal.impassable = true;

                }
            }

            if(k==0 || minY > gReal.position.y){
                minY = gReal.position.y;
            }
            if(k==0 || maxY < gReal.position.y){
                maxY = gReal.position.y;
            }

            if(xRowsMin.find(gReal.position.y) != xRowsMin.end())
            {
                if(xRowsMin[gReal.position.y] > gReal.position.x)
                {
                    xRowsMin[gReal.position.y] = gReal.position.x;
                }
            }
            else
            {
                xRowsMin[gReal.position.y] = gReal.position.x;
            }

            if(xRowsMax.find(gReal.position.y) != xRowsMax.end())
            {
                if(xRowsMax[gReal.position.y] < gReal.position.x)
                {
                    xRowsMax[gReal.position.y] = gReal.position.x;
                }
            }
            else
            {
                xRowsMax[gReal.position.y] = gReal.position.x;
            }

        }


            for(auto const &row : xRowsMin) {

                if(row.first == minY || row.first == maxY)
                {
                    continue;
                }
                for(int r=row.second+1; r<xRowsMax[row.first];r++)
                {
                    sf::Vector2i gridPos = sf::Vector2i(r,row.first);
                    if(!grid->isActiveGlobal(gridPos)){continue;}
                    GridSquare& gInner = grid->getActiveGridSquareFromGlobalCoords(sf::Vector2i(r,row.first));
                    if(!gInner.impassable && gInner.position.x != 0 && gInner.position.y != 0)
                    {
                        gInner.debugColor = sf::Color::Cyan;
                        if(!c.pathable)
                        {
                            if(!gInner.impassable && gInner.position.x != 0 && gInner.position.y != 0)
                            {
                                gInner.impassable = true;
                            }
                        }

                    }
                    gInner.addObjectInContact(c.getParent());
                    gInner.addCollidableInContact(c.index);
                }
            }



        for(unsigned int p0=0; p0<p.points.size();p0++)
        {
            unsigned int p1 = p0+1;
            if(p1==p.points.size())
            {
                p1 = 0;
            }


        }

        quadtree.addObject(c,i);
    }

    quadtree.build();
}

void CollisionEngine::run()
{
    for(unsigned int i=0; i<quadtree.flatNodes.size(); i++)
    {
        QuadtreeNode & node = quadtree.flatNodes[i];
        int indexChecked = 0;
        for(unsigned int j=0; j<node.objects.size();j++)
        {
            if(node.objects[j].quadtreeLevel == node.level) //primary nodes only fool
            {
                int jIndex = node.objects[j].cIndex;
                for(unsigned int k=0; k<node.objects.size();k++)
                {
                    if(k==j){continue;}
                    int kIndex = node.objects[k].cIndex;
                    Collidable & A = game->getCurrentLevel().objects.collidables[jIndex];
                    Collidable & B = game->getCurrentLevel().objects.collidables[kIndex];
                    if(checkCollision(A,B))
                    {

                    }
                }
                indexChecked = j;
            }
        }
    }

    for(int i=0; i<collisions.size(); i++)
    {
        eventFactory->createCollision(collisions[i]);
    }



}

void CollisionEngine::finish()
{

}

void CollisionEngine::drawDebug()
{
    if(gameWindow != NULL){
        GameWindow& window = *gameWindow;
        rectShape.setOutlineThickness(1.0);
        for(unsigned int i=0; i<quadtree.flatNodes.size(); i++)
        {
            sf::FloatRect bBox = quadtree.flatNodes[i].bounds;
            rectShape.setOutlineColor(sf::Color::Cyan);
            rectShape.setPosition(bBox.left+1,bBox.top+1);
            rectShape.setSize(sf::Vector2f(bBox.width-2,bBox.height-2));
            //window.draw(rectShape);
        }
        rectShape.setOutlineThickness(1.0);
        for(unsigned int j=0; j<activeComponents.size(); j++)
        {
            int i = activeComponents[j];
            Collidable & c = game->getCurrentLevel().objects.collidables[i];
            sf::FloatRect bBox = c.tBox;
            rectShape.setOutlineColor(sf::Color::Red);
            rectShape.setPosition(bBox.left+1,bBox.top);
            rectShape.setSize(sf::Vector2f(bBox.width-1,bBox.height));
            if(c.colliding)
            {
                rectShape.setOutlineColor(sf::Color::Green);
            }
            window.draw(rectShape);
        }

        for(unsigned int j=0; j<activeComponents.size(); j++)
        {
            int i=activeComponents[j];
            sf::ConvexShape shape;
            Collidable & c = game->getCurrentLevel().objects.collidables[i];
            ConvexPolygon& p = c.polygon;
            shape.setPointCount(p.points.size());
            for(unsigned int k=0; k<p.points.size(); k++)
            {
                shape.setPoint(k,p.points[k]);
            }

            shape.setPosition(p.position);

            shape.setFillColor(sf::Color::White);
            if(c.immovable)
            {
                shape.setFillColor(sf::Color::Yellow);
            }
            if(c.colliding)
            {
                shape.setFillColor(sf::Color::Green);
            }
            window.draw(shape);
        }


    }
}

void CollisionEngine::wake()
{
    GameObjectStore & os = game->getCurrentLevel().objects;
    for(it_collidable it = os.collidables.begin(); it != os.collidables.end(); it++)
    {
        Collidable & c = it->second;
        c.polygon.setPosition(componentLoader->getTransform(c.transform).position);
        c.wake();

    }
}

