#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Level.h"
#include "Actor.h"
#include "GameWorld.h"
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath), m_p(), m_al()
{
	m_lvlComplete = false;
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init()
{
	Level lev(assetPath());

	ostringstream oss;
	oss.fill('0');
	oss << "level" << setw(2) << getLevel() << ".txt";

	string levelFile =  oss.str();
	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found || getLevel() > 99)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		cerr << "Your level was improperly formatted" << endl;
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		Level::MazeEntry ge;
		for (int i = 0; i < LEVEL_HEIGHT; i++)
			for (int j = 0; j < LEVEL_WIDTH; j++) {
				ge = lev.getContentsOf(i, j); // x=5, y=10
				switch (ge)
				{
				case Level::empty:
					break;
				case Level::player:
					m_p = new Penelope(this, IID_PLAYER, SPRITE_HEIGHT * i, SPRITE_WIDTH * j);
					break;
				case Level::wall:
					m_al.push_back(new Wall(this, IID_WALL, SPRITE_HEIGHT * i, SPRITE_WIDTH * j));
					break;
				case Level::exit:
					m_al.push_back(new Exit(this, IID_EXIT, SPRITE_HEIGHT * i, SPRITE_WIDTH * j, 0, 1));
					break;
				case Level::vaccine_goodie:
					m_al.push_back(new MedKit(this, IID_VACCINE_GOODIE, SPRITE_HEIGHT * i, SPRITE_WIDTH * j, 0, 1));
					break;
				case Level::gas_can_goodie:
					m_al.push_back(new GasCan(this, IID_GAS_CAN_GOODIE, SPRITE_HEIGHT * i, SPRITE_WIDTH * j, 0, 1));
					break;
				case Level::landmine_goodie:
					m_al.push_back(new MineBox(this, IID_LANDMINE_GOODIE, SPRITE_HEIGHT * i, SPRITE_WIDTH * j, 0, 1));
					break;
				case Level::pit:
					m_al.push_back(new Pit(this, IID_PIT, SPRITE_HEIGHT * i, SPRITE_WIDTH * j, 0, 1));
					break;
				case Level::citizen:
					m_al.push_back(new Civilian(this, IID_CITIZEN, SPRITE_HEIGHT * i, SPRITE_WIDTH * j, 0, 0));
					break;
				case Level::dumb_zombie:
					m_al.push_back(new Dumb(this, IID_ZOMBIE, SPRITE_HEIGHT * i, SPRITE_WIDTH * j, 0, 0));
					break;
				case Level::smart_zombie:
					m_al.push_back(new Smart(this, IID_ZOMBIE, SPRITE_HEIGHT * i, SPRITE_WIDTH * j, 0, 0));
					break;
				}
			}
	}
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.


	if(m_p->alive())
		m_p->doSomething();
	else
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	list<Actor*>::iterator it;
	for (it = m_al.begin(); it != m_al.end(); it++)
		if ((*it)->alive())
		{
			(*it)->doSomething();

			if (m_lvlComplete)
			{
				playSound(SOUND_LEVEL_FINISHED);
				return GWSTATUS_FINISHED_LEVEL;
			}
		}
	
	for (it = m_al.begin(); it != m_al.end();)
	{
		if (!(*it)->alive())
		{
			delete *it;
			it = m_al.erase(it);
		}
		else
			it++;
	}
	
	ostringstream status;
	status.fill('0');
	status << "Score: " << setw(6) << getScore() << "  Level: " << getLevel();
	status << "  Lives: " << getLives() << "  Vaccines: " << m_p->getVacc();
	status << "  Flames: " << m_p->getFlames() << "  Mines: " << m_p->getMines();
	status << "  Infected: " << m_p->getInfLevel();
	setGameStatText(status.str());

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	if(m_p != nullptr)
		delete m_p;
	m_p = nullptr;

	list<Actor*>::iterator it;
	for (it = m_al.begin(); it != m_al.end();)
	{
		delete (*it);
		it = m_al.erase(it);
	}

	m_lvlComplete = false;
}

bool StudentWorld::canMove(double x, double y, Actor* a)
{
	list<Actor*>::iterator it;

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if((*it)->getBlock() && (*it) != a)
			if (abs(x - (*it)->getX()) < SPRITE_WIDTH  && abs(y - (*it)->getY()) < SPRITE_HEIGHT)
				return false;
	}

	if (abs(x - getP()->getX()) < SPRITE_WIDTH && abs(y - getP()->getY()) < SPRITE_HEIGHT && a != getP())
		return false;

	return true;
}

bool StudentWorld::overlap(double x1, double y1, double x2, double y2)
{
	if (pow(x1 - x2, 2) + pow(y1 - y2, 2) <= 100)
		return true;
	return false;
}

void StudentWorld::levelComplete(bool b)
{
	m_lvlComplete = b;
}

void StudentWorld::addActor(Actor* a)
{
	m_al.push_back(a);
}

void StudentWorld::activateOnActors(Actor* a)
{
	if (overlap(a->getX(), a->getY(), getP()->getX(), getP()->getY()) && getP()->alive() && a->alive())
		a->activate(getP());

	list<Actor*>::iterator it;
	for (it = m_al.begin(); it != m_al.end(); it++)
		if(overlap(a->getX(), a->getY(), (*it)->getX(), (*it)->getY()) && a->alive() && (*it)->alive())
			a->activate((*it));
}


bool StudentWorld::blocksFlame(double x, double y)
{
	list<Actor*>::iterator it;

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if (!(*it)->getFlammable())
			if (abs(x - (*it)->getX()) < SPRITE_WIDTH && abs(y - (*it)->getY()) < SPRITE_HEIGHT)
				return true;
	}
	return false;
}

bool StudentWorld::citizensLeft()
{
	list<Actor*>::iterator it;

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if ((*it)->getInfectable())
				return true;
	}
	return false;
}

bool StudentWorld::zombiesLeft()
{
	list<Actor*>::iterator it;

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if (!(*it)->getInfectable() && (*it)->getPittable())
			return true;
	}
	return false;
}


bool StudentWorld::blocksVomit(double x, double y)
{
	list<Actor*>::iterator it;

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if ((*it)->getBlocksVomit())
			if (abs(x - (*it)->getX()) < SPRITE_WIDTH && abs(y - (*it)->getY()) < SPRITE_HEIGHT)
				return true;
	}
	return false;
}

bool StudentWorld::triggerVomit(double x, double y)
{
	list<Actor*>::iterator it;

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if ((*it)->getInfectable())
			if (overlap(x, y, (*it)->getX(), (*it)->getY()))
			{
				return true;
			}
	}

	if (overlap(x, y, getP()->getX(), getP()->getY()))
	{
		return true;
	}


	return false;
}

Actor* StudentWorld::closestInfectable(double x, double y)
{
	list<Actor*>::iterator it;
	double min = sqrt(pow(x - getP()->getX(), 2) + pow(y - getP()->getY(), 2));
	Actor* a = getP();

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if ((*it)->getInfectable())
			if (min > sqrt(pow(x - (*it)->getX(), 2) + pow(y - (*it)->getY(), 2)))
			{
				a = *it;
				min = sqrt(pow(x - (*it)->getX(), 2) + pow(y - (*it)->getY(), 2));
			}

	}

	if (min > sqrt(pow(x - getP()->getX(), 2) + pow(y - getP()->getY(), 2)))
	{
		a = getP();
		min = sqrt(pow(x - getP()->getX(), 2) + pow(y - getP()->getY(), 2));
	}
	return a;
}

Actor* StudentWorld::closestZombie(double x, double y)
{
	list<Actor*>::iterator it;
	double min = 1000;
	Actor* a;

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if (!(*it)->getInfectable() && (*it)->getPittable())
			if (min > sqrt(pow(x - (*it)->getX(), 2) + pow(y - (*it)->getY(), 2)))
			{
				a = *it;
				min = sqrt(pow(x - (*it)->getX(), 2) + pow(y - (*it)->getY(), 2));
			}

	}

	return a;
}

bool StudentWorld::overlapAnything(double x, double y)
{
	if (overlap(x, y, getP()->getX(), getP()->getY()))
		return true;

	list<Actor*>::iterator it;

	for (it = m_al.begin(); it != m_al.end(); it++)
	{
		if (overlap(x, y, (*it)->getX(), (*it)->getY()))
			return true;
	}

	return false;
}

void StudentWorld::dropVaccine(double x, double y)
{
	Direction d;
	switch (randInt(1, 4))
	{
	case 1:
		d = 0;
		break;
	case 2:
		d = 90;
		break;
	case 3:
		d = 180;
		break;
	case 4:
		d = 270;
		break;
	}

	if (d == 0 && !overlapAnything(x + SPRITE_WIDTH, y))
		addActor(new MedKit(this, IID_VACCINE_GOODIE, x + SPRITE_WIDTH, y));
	if (d == 180 && !overlapAnything(x - SPRITE_WIDTH, y))
		addActor(new MedKit(this, IID_VACCINE_GOODIE, x - SPRITE_WIDTH, y));
	if (d == 90 && !overlapAnything(x, y + SPRITE_HEIGHT))
		addActor(new MedKit(this, IID_VACCINE_GOODIE, x, y + SPRITE_HEIGHT));
	if (d == 270 && !overlapAnything(x, y - SPRITE_HEIGHT))
		addActor(new MedKit(this, IID_VACCINE_GOODIE, x, y - SPRITE_HEIGHT));
}