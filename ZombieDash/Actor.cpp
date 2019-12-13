#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>
using namespace std;
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	:GraphObject(imageID, startX, startY, dir, depth)
{
	m_sw = sw;
	m_alive = true;
	m_tick = 0;
	m_infected = false;
}

bool Actor::Poverlap()
{
	return getWorld()->overlap(getX(), getY(), getWorld()->getP()->getX(), getWorld()->getP()->getY());
}
double Actor::dist(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}
Mobs::Mobs(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Actor(sw, imageID, startX, startY)
{
	setBlocksVomit(false);
	setFlammable(true);
	setBlock(true);
	setPittable(true);
	setTriggersLandmine(true);
	setInfectable(false);
}

Human::Human(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Mobs(sw, imageID, startX, startY)
{
	setInfectable(true);
	m_infectionLevel = 0;
}

Activator::Activator(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Actor(sw, imageID, startX, startY, dir, depth)
{
	setBlocksVomit(true);
	setFlammable(false);
	setInfectable(false);
	setPittable(false);
	setBlock(false);
	setTriggersLandmine(false);
}

Goodies::Goodies(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Activator(sw, imageID, startX, startY, dir, depth)
{
	setBlocksVomit(true);
	setFlammable(true);
	setInfectable(false);
	setPittable(false);
	setBlock(false);
	setTriggersLandmine(false);
}

Projectiles::Projectiles(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Activator(sw, imageID, startX, startY, dir, depth)
{
	setBlocksVomit(true);
	setFlammable(false);
	setInfectable(false);
	setPittable(false);
	setBlock(false);
	setTriggersLandmine(false);
}

void Mobs::dieFromFlameorPit()
{
	otherDeath();
	SetAlive(false);
}


///////////
//PENELOPE
//////////

Penelope::Penelope(StudentWorld* sw, int imageID, double startX, double startY)
	: Human(sw, imageID, startX, startY)
{
	m_mines = 0;
	m_flames = 0;
	m_vaccs = 0;
}

void Penelope::PickupGoodie(Goodies* g)
{
	g->receiveGoodie();
}

void Penelope::useExit()
{
	if (!getWorld()->citizensLeft())
	{
		getWorld()->playSound(SOUND_LEVEL_FINISHED);
		getWorld()->levelComplete(true);
	}
}

void Penelope::doSomething()
{

	if (getInfected())
	{
		setInfLevel(getInfLevel() + 1);
		if (getInfLevel() >= 500)
		{
			SetAlive(false);
			getWorld()->playSound(SOUND_PLAYER_DIE);
			return;
		}
	}

	int ch;
	if (getWorld()->getKey(ch))
	{
		// user hit a key during this tick!
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			setDirection(left);
			if (getWorld()->canMove(getX() - 4, getY(), this))
				moveTo(getX() - 4, getY());
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (getWorld()->canMove(getX() + 4, getY(), this))
				moveTo(getX() + 4, getY());
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (getWorld()->canMove(getX(), getY() + 4, this))
				moveTo(getX(), getY() + 4);
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (getWorld()->canMove(getX(), getY() - 4, this))
				moveTo(getX(), getY() - 4);
			break;
		case KEY_PRESS_SPACE:
			if (m_flames > 0)
			{
				m_flames--;
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				int dir = getDirection();
				switch (dir)
				{
				case right:
					for (int i = 1; i <= 3; i++)
					{
						if (getWorld()->blocksFlame(getX() + (i * SPRITE_WIDTH), getY()))
							return;
						getWorld()->addActor(new Flame(getWorld(), IID_FLAME, getX() + (i * SPRITE_WIDTH), getY(), right));
					}
					break;
				case left:
					for (int i = 1; i <= 3; i++)
					{
						if (getWorld()->blocksFlame(getX() - (i * SPRITE_WIDTH), getY()))
							return;
						getWorld()->addActor(new Flame(getWorld(), IID_FLAME, getX() - (i * SPRITE_WIDTH), getY(), left));
					}
					break;
				case up:
					for (int i = 1; i <= 3; i++)
					{
						if (getWorld()->blocksFlame(getX(), getY() + (i * SPRITE_HEIGHT)))
							return;
						getWorld()->addActor(new Flame(getWorld(), IID_FLAME, getX(), getY() + (i * SPRITE_HEIGHT), up));
					}
					break;
				case down:
					for (int i = 1; i <= 3; i++)
					{
						if (getWorld()->blocksFlame(getX(), getY() - (i * SPRITE_HEIGHT)))
							return;
						getWorld()->addActor(new Flame(getWorld(), IID_FLAME, getX(), getY() - (i * SPRITE_HEIGHT), down));
					}
					break;
				}
			}
			break;
		case KEY_PRESS_TAB:
			if (getMines() > 0)
			{
				m_mines--;
				getWorld()->addActor(new Landmine(getWorld(), IID_LANDMINE, getX(), getY(), 0, 1));
			}
			break;
		case KEY_PRESS_ENTER:
			if (getVacc() > 0)
			{
				m_vaccs--;
				setInfected(false);
				setInfLevel(0);
			}
		}
	}
}

////////
//WALL
////////
Wall::Wall(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Actor(sw, imageID, startX, startY)
{
	setBlocksVomit(true);
	setFlammable(false);
	setInfectable(false);
	setPittable(false);
	setBlock(true);
	setTriggersLandmine(false);
}

void Wall::doSomething()
{
	return;
}

////////////
//Activator
////////////

void Activator::doSomething()
{
	if (alive())
		getWorld()->activateOnActors(this);
}

////////
//EXIT
////////

Exit::Exit(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Activator(sw, imageID, startX, startY, dir, depth)
{
	setBlocksVomit(false);
}

void Exit::activate(Actor* a)
{
	a->useExit();
}

////////
//PIT
///////

Pit::Pit(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Activator(sw, imageID, startX, startY, dir, depth)
{
	setFlammable(true);
	setBlocksVomit(false);
}

void Pit::activate(Actor* a)
{
	a->dieFromFlameorPit();
}

//////////////
//Land Mine
//////////////

Landmine::Landmine(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Activator(sw, imageID, startX, startY, dir, depth)
{
	setFlammable(true);
}

void Landmine::doSomething()
{
	increaseTick();
	if (getTick() > 30)
	{
		Activator::doSomething();
	}
}

void Landmine::dieFromFlameorPit()
{
	SetAlive(false);
	setOff();
}

void Landmine::activate(Actor* a)
{
	if (a->triggersLandmine())
		setOff();
}
void Landmine::setOff()
{
	double x = getX();
	double y = getY();
	SetAlive(false);
	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
	getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x, y));
	if (!getWorld()->blocksFlame(x + SPRITE_WIDTH, y))
		getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x + SPRITE_WIDTH, y, 90));
	if (!getWorld()->blocksFlame(x - SPRITE_WIDTH, y))
		getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x - SPRITE_WIDTH, y, 90));
	if (!getWorld()->blocksFlame(x, y + SPRITE_HEIGHT))
		getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x, y + SPRITE_HEIGHT, 90));
	if (!getWorld()->blocksFlame(x, y - SPRITE_HEIGHT))
		getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x, y - SPRITE_HEIGHT, 90));
	if (!getWorld()->blocksFlame(x + SPRITE_WIDTH, y + SPRITE_HEIGHT))
		getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x + SPRITE_WIDTH, y + SPRITE_HEIGHT, 90));
	if (!getWorld()->blocksFlame(x + SPRITE_WIDTH, y - SPRITE_HEIGHT))
		getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x + SPRITE_WIDTH, y - SPRITE_HEIGHT, 90));
	if (!getWorld()->blocksFlame(x - SPRITE_WIDTH, y - SPRITE_HEIGHT))
		getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x - SPRITE_WIDTH, y - SPRITE_HEIGHT, 90));
	if (!getWorld()->blocksFlame(x - SPRITE_WIDTH, y + SPRITE_HEIGHT))
		getWorld()->addActor(new Flame(getWorld(), IID_FLAME, x - SPRITE_WIDTH, y + SPRITE_HEIGHT, 90));
	getWorld()->addActor(new Pit(getWorld(), IID_PIT, x, y));
}

//////////////////
//Goodies
//////////////////
void Goodies::dieFromFlameorPit()
{
	SetAlive(false);
}

void Goodies::activate(Actor* a)
{
	a->PickupGoodie(this);
}

void Goodies::receiveGoodie()
{
	getWorld()->increaseScore(50);
	SetAlive(false);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	Pickup();
}

MedKit::MedKit(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Goodies(sw, imageID, startX, startY, dir, depth)
{
}

void MedKit::Pickup()
{
	getWorld()->getP()->pickUpMed();
}

GasCan::GasCan(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Goodies(sw, imageID, startX, startY, dir, depth)
{
}

void GasCan::Pickup()
{
	getWorld()->getP()->pickUpGas();
}

MineBox::MineBox(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Goodies(sw, imageID, startX, startY, dir, depth)
{
}

void MineBox::Pickup() 
{
	getWorld()->getP()->pickUpMine();
}

///////////////
//PROJECTILES
///////////////



Flame::Flame(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	:Projectiles(sw, imageID, startX, startY, dir, depth)
{
	setTriggersLandmine(true);
}

Vomit::Vomit(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Projectiles(sw, imageID, startX, startY, dir, depth)
{
}

void Projectiles::doSomething()
{

	if (getTick() >= 2)
		SetAlive(false);
	else
	{
		increaseTick();
		Activator::doSomething();
	}

}

void Flame::activate(Actor* a)
{
	a->dieFromFlameorPit();
}

void Vomit::activate(Actor* a)
{

	a->setInfected(true);
		
}

/////////////
//CIVILIAN
/////////////

void Civilian::useExit()
{
	getWorld()->increaseScore(500);
	SetAlive(false);
	getWorld()->playSound(SOUND_CITIZEN_SAVED);
}

Civilian::Civilian(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	:Human(sw, imageID, startX, startY, dir, depth)
{
	m_sound = false;
}

double Civilian::Pdist()
{
	return dist(getWorld()->getP()->getX(),getWorld()->getP()->getY(), getX(), getY());
}

void Civilian::doSomething()
{
	if (alive())
	{
		double zX, zY, zDist;

		if (getWorld()->zombiesLeft())
		{
			zX = getWorld()->closestZombie(getX(), getY())->getX();
			zY = getWorld()->closestZombie(getX(), getY())->getY();
			zDist = dist(zX,zY, getX(), getY());
		}


		if (getInfected())
		{
			if (!m_sound)
			{
				getWorld()->playSound(SOUND_CITIZEN_INFECTED);
				m_sound = true;
			}

			setInfLevel(getInfLevel() + 1);
			if (getInfLevel() >= 500)
			{
				SetAlive(false);
				getWorld()->playSound(SOUND_ZOMBIE_BORN);
				getWorld()->increaseScore(-1000);
				switch (randInt(1, 10))
				{
				case 1:
				case 2:
				case 3:
					getWorld()->addActor(new Smart(getWorld(), IID_ZOMBIE, getX(), getY(), getDirection()));
					break;
				default:
					getWorld()->addActor(new Dumb(getWorld(), IID_ZOMBIE, getX(), getY(), getDirection()));
					break;
				}
				return;
			}
		}

		increaseTick();
		if (getTick() % 2 == 1)
		{
			if ((!getWorld()->zombiesLeft() || zDist > Pdist()) && Pdist() <= 80)
				determineDirection(getWorld()->getP()->getX(), getWorld()->getP()->getY(), 2);



			else if (getWorld()->zombiesLeft() && zDist <= 80)
			{
				runAway(zX,zY);
			}
		}
	}
}

void Civilian::otherDeath()
{
	getWorld()->increaseScore(-1000);
	getWorld()->playSound(SOUND_CITIZEN_DIE);
}

void Civilian::runAway(double x, double y)
{
	Direction dir;
	double max = dist(x, y, getX(), getY());

	if (getWorld()->canMove(getX(), getY() + 2, this) && max <= dist(x, y, getX(), getY() + 2))
	{
		max = dist(x, y, getX(), getY() + 2);
		dir = up;
	}
	if (getWorld()->canMove(getX(), getY() - 2, this) && max <= dist(x, y, getX(), getY() - 2))
	{
		max = dist(x, y, getX(), getY() - 2);
		dir = down;
	}
	if (getWorld()->canMove(getX() + 2, getY(), this) && max <= dist(x, y, getX() + 2, getY()))
	{
		max = dist(x, y, getX() + 2, getY());
		dir = right;
	}
	if (getWorld()->canMove(getX() - 2, getY(), this) && max <= dist(x, y, getX() - 2, getY()))
	{
		max = dist(x, y, getX() - 2, getY());
		dir = left;
	}
	
	if (max > dist(x, y, getX(), getY())) // found new direction
	{
		setDirection(dir);
		moveDirection(dir, 2);
	}
	else
		return;
}
void Mobs::moveDirection(Direction dir, int step)
{
	switch (dir)
	{
	case up:
		if(getWorld()->canMove(getX(), getY() + step, this))
			moveTo(getX(), getY() + step);
		break;
	case down:
		if (getWorld()->canMove(getX(), getY() - step, this))
			moveTo(getX(), getY() - step);
		break;
	case right:
		if (getWorld()->canMove(getX() + step, getY(), this))
			moveTo(getX() + step, getY());
		break;
	case left:
		if (getWorld()->canMove(getX() - step, getY(), this))
			moveTo(getX() - step, getY());
		break;
	}
}

//////////////
//ZOMBIES
//////////////

Zombie::Zombie(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Mobs(sw, imageID, startX, startY)
{
	setInfectable(false);
	m_plan = 0;
}

Dumb::Dumb(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Zombie(sw, imageID, startX, startY)
{
}

Smart::Smart(StudentWorld* sw, int imageID, double startX, double startY, Direction dir, int depth)
	: Zombie(sw, imageID, startX, startY)
{
}

void Zombie::otherDeath()
{
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	drop();
}

void Zombie::doSomething()
{
	if (alive())
	{
		increaseTick();
		if (getTick() % 2 == 1)
		{
			if (getWorld()->triggerVomit(getX() + SPRITE_WIDTH, getY()) && getDirection() == right && randInt(1, 3) == 1)
			{
				getWorld()->addActor(new Vomit(getWorld(), IID_VOMIT, getX() + SPRITE_WIDTH, getY(), right));
				getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			}
			if (getWorld()->triggerVomit(getX() - SPRITE_WIDTH, getY()) && getDirection() == left && randInt(1, 3) == 1)
			{
				getWorld()->addActor(new Vomit(getWorld(), IID_VOMIT, getX() - SPRITE_WIDTH, getY(), left));
				getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			}
			if (getWorld()->triggerVomit(getX(), getY() + SPRITE_HEIGHT) && getDirection() == up && randInt(1, 3) == 1)
			{
				getWorld()->addActor(new Vomit(getWorld(), IID_VOMIT, getX(), getY() + SPRITE_HEIGHT, up));
				getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			}
			if (getWorld()->triggerVomit(getX(), getY() - SPRITE_HEIGHT) && getDirection() == down && randInt(1, 3) == 1)
			{
				getWorld()->addActor(new Vomit(getWorld(), IID_VOMIT, getX(), getY() - SPRITE_HEIGHT, down));
				getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			}

			plan();

			double curX = getX();
			double curY = getY();

			moveDirection(getDirection(), 1);
			if (curX == getX() && curY == getY()) //didnt move
				setPlan(0);
			else
				decreasePlan();
		}
	}
}

void Dumb::drop()
{
	getWorld()->increaseScore(1000);
	if(randInt(1,10)==10)
		getWorld()->dropVaccine(getX(), getY());
}

void Dumb::plan()
{
	if (getPlan() == 0)
	{
		setPlan(randInt(3, 10));

		randomDirection();
	}
}

void Smart::drop()
{
	getWorld()->increaseScore(2000);
}

void Smart::plan()
{
	double cX = getWorld()->closestInfectable(getX(), getY())->getX();
	double cY = getWorld()->closestInfectable(getX(), getY())->getY();

	if (getPlan() == 0)
	{
		setPlan(randInt(3, 10));

		if (80 < sqrt(pow(getX() - cX, 2) + pow(getY() - cY, 2)))
			randomDirection();

		else
			determineDirection(cX, cY, 0);
	}
}

void Mobs::randomDirection()
{
	switch (randInt(1, 4))
	{
	case 1:
		setDirection(right);
		break;
	case 2:
		setDirection(left);
		break;
	case 3:
		setDirection(up);
		break;
	case 4:
		setDirection(down);
		break;
	}
}

void Mobs::determineDirection(double x, double y, int step)   //diagonals
{
	if (x == getX())  //same x coord
	{
		if (y > getY() && getWorld()->canMove(getX(), getY() + step, this))
		{
			setDirection(up);
			if(step > 0)
				moveDirection(up, step);
		}
		else if (y < getY() && getWorld()->canMove(getX(), getY() - step, this))
		{
			setDirection(down);
			if (step > 0)
				moveDirection(down, step);
		}
	}

	else if (y == getY())  //same y coord
	{
		if (x > getX() && getWorld()->canMove(getX() + step, getY(), this))
		{
			setDirection(right);
			if (step > 0)
				moveDirection(right, step);
		}
		else if (x < getX() && getWorld()->canMove(getX() - step, getY(), this))
		{
			setDirection(left);
			if (step > 0)
				moveDirection(left, step);
		}
	}

	else if (x > getX() && y > getY())   //up right
	{
		switch (randInt(1, 2))
		{
		case 1:
			if (getWorld()->canMove(getX(), getY() + step, this))
			{
				setDirection(up);
				if (step > 0)
					moveDirection(up, step);
				break;
			}
		case 2:
			if (getWorld()->canMove(getX() + step, getY(), this))
			{
				setDirection(right);
				if (step > 0)
					moveDirection(right, step);
			}
			break;
		}
	}

	else if (x < getX() && y > getY())   //up left
	{
		switch (randInt(1, 2))
		{
		case 1:
			if (getWorld()->canMove(getX(), getY() + step, this))
			{
				setDirection(up);
				if (step > 0)
					moveDirection(up, step);
				break;
			}
		case 2:
			if (getWorld()->canMove(getX() - step, getY(), this))
			{
				setDirection(left);
				if (step > 0)
					moveDirection(left, step);
			}
			break;
		}
	}
	else if (x > getX() && y < getY())   //down right
	{
		switch (randInt(1, 2))
		{
		case 1:
			if (getWorld()->canMove(getX(), getY() - step, this))
			{
				setDirection(down);
				if (step > 0)
					moveDirection(down, step);
				break;
			}
		case 2:
			if (getWorld()->canMove(getX() + step, getY(), this))
			{
				setDirection(right);
				if (step > 0)
					moveDirection(right, step);
			}
			break;
		}
	}

	else if (x < getX() && y < getY())   //down left
	{
		switch (randInt(1, 2))
		{
		case 1:
			if (getWorld()->canMove(getX(), getY() - step, this))
			{
				setDirection(down);
				if (step > 0)
					moveDirection(down, step);
				break;
			}
		case 2:
			if (getWorld()->canMove(getX() - step, getY(), this))
			{
				setDirection(left);
				if (step > 0)
					moveDirection(left, step);
			}
			break;
		}
	}

}