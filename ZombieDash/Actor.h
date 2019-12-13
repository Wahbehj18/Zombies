#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Goodies;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void doSomething() = 0;
	virtual ~Actor() {}

	StudentWorld* getWorld() { return m_sw; }
	bool alive() { return m_alive; }
	void SetAlive(bool b) { m_alive = b; }
	bool getBlock() { return m_block; }
	void setBlock(bool b) { m_block = b; }
	bool getFlammable() { return m_flammable; }
	void setFlammable(bool b) { m_flammable = b; }
	bool getInfectable() { return m_infectable; }
	void setInfectable(bool b) { m_infectable = b; }
	bool getPittable() { return m_pittable; }
	void setPittable(bool b) { m_pittable = b; }
	bool getInfected() { return m_infected; }
	void setInfected(bool b) { m_infected = b; }
	bool getBlocksVomit() { return m_blocksVomit; }
	void setBlocksVomit(bool b) { m_blocksVomit = b; }
	bool triggersLandmine() { return m_triggers; }
	void setTriggersLandmine(bool b) { m_triggers = b; }

	int getTick() { return m_tick; }
	void increaseTick() { m_tick++; }
	bool Poverlap();
	double dist(double x1, double y1, double x2, double y2);

	virtual void activate(Actor* a) {}
	virtual void dieFromFlameorPit() {}
	virtual void PickupGoodie(Goodies* g) {}
	virtual void useExit() {}

private:
	StudentWorld* m_sw;
	bool m_alive;
	bool m_block;
	bool m_flammable;
	bool m_infectable;
	bool m_pittable;
	bool m_infected;
	bool m_blocksVomit;
	bool m_triggers;
	int m_tick;
};

class Activator : public Actor
{
public:
	Activator(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);
	virtual void doSomething();
};

class Mobs : public Actor
{
public:
	Mobs(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void doSomething() = 0;

	void moveDirection(Direction dir, int step);
	void determineDirection(double x, double y, int step);
	void randomDirection();
	virtual ~Mobs() {}
	void dieFromFlameorPit();
	virtual void otherDeath() = 0;

};

class Human : public Mobs
{
public:
	Human(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	int getInfLevel() { return m_infectionLevel; }
	void setInfLevel(int n) { m_infectionLevel = n; }

private:
	int m_infectionLevel;

};

class Penelope : public Human
{
public:
	Penelope(StudentWorld* sw, int imageID, double startX, double startY);

	virtual void doSomething();

	void pickUpMed() { m_vaccs++; }
	void pickUpGas() { m_flames+= 5; }
	void pickUpMine() { m_mines+= 2; }
	int getVacc() { return m_vaccs; }
	int getFlames() { return m_flames; }
	int getMines() { return m_mines; }
	
	virtual void PickupGoodie(Goodies* g);
	virtual void otherDeath() {}
	virtual void useExit();

private:
	int m_mines;
	int m_flames;
	int m_vaccs;
};

class Civilian : public Human
{
public:
	Civilian(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void doSomething();
	virtual void otherDeath();
	virtual void useExit();

	double Pdist();
	void runAway(double x, double y);

private:
	bool m_sound;
};

class Zombie : public Mobs
{
public:
	Zombie(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void doSomething();
	virtual void plan() = 0;
	int getPlan() { return m_plan; }
	void setPlan(int r) { m_plan = r; }
	void decreasePlan() { m_plan--; }
	virtual void otherDeath();
	virtual void drop() = 0;

private:
	int m_plan;
};

class Smart : public Zombie
{
public:
	Smart(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);
	virtual void drop();
	virtual void plan();
};

class Dumb : public Zombie
{
public:
	Dumb(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);
	virtual void drop();
	virtual void plan();
};

class Goodies : public Activator
{
public:
	Goodies(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void Pickup() = 0;
	virtual void activate(Actor* a);
	void receiveGoodie();
	virtual void dieFromFlameorPit();
};

class GasCan : public Goodies
{
public:
	GasCan(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void Pickup();
};

class MedKit : public Goodies
{
public:
	MedKit(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void Pickup();
};

class MineBox : public Goodies
{
public:
	MineBox(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void Pickup();
};

class Projectiles : public Activator
{
public:
	Projectiles(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void doSomething();
};

class Flame : public Projectiles
{
public:
	Flame(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void activate(Actor* a);
};

class Vomit : public Projectiles
{
public:
	Vomit(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = 0, int depth = 0);

	virtual void activate(Actor* a);
};


class Wall : public Actor
{
public:
	Wall(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = right, int depth = 0);
	virtual void doSomething();
};

class Pit : public Activator
{
public:
	Pit(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = right, int depth = 0);
	virtual void activate(Actor* a);
};

class Exit : public Activator
{
public:
	Exit(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = right, int depth = 0);
	virtual void activate(Actor* a);
};

class Landmine : public Activator
{
public:
	Landmine(StudentWorld* sw, int imageID, double startX, double startY, Direction dir = right, int depth = 0);
	virtual void activate(Actor* a);
	virtual void doSomething();
	void setOff();
	virtual void dieFromFlameorPit();
};



#endif // ACTOR_H_
