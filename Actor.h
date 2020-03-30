#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>
#include <string>
#include <algorithm>

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(StudentWorld* studWorld, int imageID, int startX, int startY, Direction dir , double size, 
		unsigned int depth) : GraphObject(imageID, startX, startY, dir, size, depth) , m_isDead(false) , m_world(studWorld){
		if (studWorld == nullptr) {
			std::cerr << "THE PLAYER MUST BE IN AN ARENA" << std::endl;
		}
		setVisible(true);
	}
	virtual ~Actor();

	virtual void doSomething() = 0; //pure virtual function, diff for every derived class
	bool isDead();
	StudentWorld* getWorld();
	void moveToDir(Direction dir); //realized i needed this a little late

	virtual bool isBadGuy() { return false; }
	virtual bool canBeBribed() { return false; }
	virtual bool isBoulder() { return false; }

	virtual void stun(int amtTime) { return; }
	virtual void getHurt(int amt) { return; }
	
	
protected:
	bool m_isDead;
	StudentWorld* m_world;
};



class Earth : public Actor { //class for all the dirt
public:
	//CONSTRUCTOR
	Earth(StudentWorld* studWorld, int startX, int startY, Direction dir = right, double size = 0.25, unsigned int depth = 3) : 
		Actor(studWorld, TID_EARTH, startX, startY, dir, size, depth) {
	
		setVisible(true); //dont know if I need this because its in the actor constructor
		
	}
	//DESTRUCTOR
	virtual ~Earth();
	

	//FUNCTIONS
	virtual void doSomething();// Earth doesnt do anything during a tick so this can be empty

	
};

class TunnelMan : public Actor {
public:
	TunnelMan(StudentWorld* studWorld, int startX = 30, int startY = 60, Direction dir = right, double size = 1, unsigned int depth = 0) :
		Actor(studWorld, TID_PLAYER, startX, startY, dir, size, depth) {
		//insert constructor here
		setVisible(true);

	}
	virtual ~TunnelMan();
	virtual void doSomething();

	int getHP();
	int getGold();
	int getWaterShots();
	int getSonar();
	void decWaterShots();
	void increaseWater();
	void increaseSonar();
	void increaseGold();

	void getHurt(int amt);


private:
	//FOR LATER
	int hp = 10;
	int waterShots = 5;
	int SonarCharges = 1;
	int goldNugs = 0;

};

class Boulder : public Actor {
public:
	//CONSTRUCTOR
	Boulder(StudentWorld* studWorld, int startX, int startY, Direction dir = right, double size = 1, unsigned int depth = 1) :
		Actor(studWorld, TID_BOULDER, startX, startY, dir, size, depth) {
		setVisible(true); 
		stable = true;
		waiting = 0;
	}
	virtual ~Boulder() {
		setVisible(false);
	}
	//FUNCTIONS
	virtual void doSomething();
	
	bool isStable();
	virtual bool isBoulder() { return true; }

private:
	//VARIABLES
	bool stable;
	int waiting;
};


class Oil : public Actor {
public:
	Oil(StudentWorld* studWorld, int startX, int startY, Direction dir = right, double size = 1, unsigned int depth = 2) :
		Actor(studWorld, TID_BARREL, startX, startY, dir, size, depth) {
		setVisible(false);
		found = false;
	}
	~Oil();
	virtual void doSomething();

private:
	bool found;
};



class Squirt : public Actor {
public:
	Squirt(StudentWorld* studWorld, int startX, int startY, Direction dir = right, double size = 1, unsigned int depth = 1) :
		Actor(studWorld, TID_WATER_SPURT, startX, startY, dir, size, depth), distTraveled(0) {
		setVisible(true);
	}

	virtual ~Squirt();
	virtual void doSomething();

private:
	int distTraveled;
};


class Protester : public Actor {
public:
	Protester(StudentWorld* studWorld, int startX = 30, int startY = 60, Direction dir = left, double size = 1, unsigned int depth = 0, int image_ID = TID_PROTESTER) :
		Actor(studWorld, image_ID, startX, startY, dir, size, depth), perpCountdown(0), moveCountdown(3), yellCountdown(15), leavefield(false),  squaresInDir(8), hp(5){
		setVisible(true);
		squaresInDir = rand() % 53 + 8;
	}
	~Protester();

	virtual bool isBadGuy() { return true; }
	virtual bool canBeBribed() { return true; }

	virtual void doSomething();
	bool canMoveDir(Direction dir);
	bool canMovePerp(Direction Dir);
	void Yell();
	virtual void getHurt(int amt);

	virtual void stun(int amtTime);
	
protected:
	bool leavefield;
	int squaresInDir;
	int perpCountdown;
	int hp;
	int moveCountdown;
	int yellCountdown;

};

class HardcoreProtester : public Protester {
public:
	HardcoreProtester(StudentWorld* studWorld, int startX = 30, int startY = 60, Direction dir = left, double size = 1, unsigned int depth = 0, int image_ID = TID_HARD_CORE_PROTESTER) : Protester(studWorld, startX = 60, startY = 60, dir = left, size = 1, depth = 0 , TID_HARD_CORE_PROTESTER)  {
		setVisible(true);
		squaresInDir = rand() % 53 + 8;
		hp = 20;
	}

	virtual bool canBeBribed() { return false; }
};


class Sonar : public Actor {
public:
	Sonar(StudentWorld* studWorld, int startX = 0, int startY = 60, Direction dir = right, double size = 1, unsigned int depth = 2);
	virtual void doSomething();

private:
	int ticksRemaining;
};

class WaterPool : public Actor {
public:
	WaterPool(StudentWorld* studWorld, int startX, int startY, Direction dir = right, double size = 1, unsigned int depth = 2);
	virtual void doSomething();
	virtual bool isBadGuy() { return false; }
	virtual bool canBeBribed() { return false; }
private:
	int ticksRemaining;
};

class Nugget : public Actor {
public:
	Nugget(StudentWorld* studWorld, int startX, int startY, bool dropped, Direction dir = right, double size = 1, unsigned int depth = 2);
	virtual void doSomething();
	virtual bool isBadGuy() { return false; }
	virtual bool canBeBribed() { return false; }
private:
	bool canPlayerPickUp;
	int ticksRemaining;

};
#endif // ACTOR_H_
