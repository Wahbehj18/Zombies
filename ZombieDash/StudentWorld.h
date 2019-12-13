#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>


class Actor;
class Penelope;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

	void activateOnActors(Actor* a);

	Penelope* getP() {return m_p;}
	void addActor(Actor *a);
	bool canMove(double x, double y, Actor* a);
	bool overlap(double x1, double y1, double x2, double y2);
	void levelComplete(bool b);
	bool blocksFlame(double x, double y);
	bool overlapAnything(double x, double y);
	bool blocksVomit(double x, double y);
	bool triggerVomit(double x, double y);
	Actor* closestInfectable(double x, double y);
	Actor* closestZombie(double x, double y);

	void dropVaccine(double x, double y);

	bool citizensLeft();
	bool zombiesLeft();

private:
	Penelope* m_p;
	std::list<Actor*> m_al;

	bool m_lvlComplete;
};

#endif // STUDENTWORLD_H_
