#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"

#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>

using namespace std;

class Earth;
class TunnelMan;
class Boulder;
class Oil;
class Squirt;
class Protester;
class Sonar;

const int MAXDIRTROWS = 60;
const int MAXDIRTCOLS = 64;
const int MAXROWS = 64;
const int MAXCOLS = 64;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{	//just gonna leave this blank
	}

	//this will be the same code as my cleanUp
	~StudentWorld() {
		//delete all EARTH BLOCK MEMORY
		for (int i = 0; i < MAXDIRTCOLS; i++) {
			for (int j = 0; j < MAXDIRTROWS; j++) {
				delete ground[i][j];
			}
		}
		delete player;	//delete current player
		//delete all actors
		for (auto it = actorVec.begin(); it != actorVec.end(); ) {
			auto temp = it;
			delete *temp;
			it = actorVec.erase(it);
		}
		actorVec.clear();
	}

	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void digEarth(int x, int y);
	bool EarthBelowBoulder(int x, int y);
	double distToTunnelman(int x, int y);
	double distance(int x1, int x2, int y1, int y2);
	double distToNearestObject(int x, int y);
	void destroyBarrel();
	void DisplayText();
	bool boulderToDir(char dir, int x, int y);
	TunnelMan* getTunnelman();
	void newSquirt();
	void newDroppedGold();
	bool earthtoDir(char dir, int x, int y);
	void decreaseSquirts();
	bool ishardcore();
	void decProtesters();
	void hurtTunnelman(int amt);
	bool facingTunnelMan(GraphObject::Direction dir, int x, int y);
	bool canSeetunnelMan(int x, int y);
	bool earthToDir4x4(char dir, int x, int y);
	bool hurtProtester(int amt, int x, int y);
	GraphObject::Direction firstMoveToExit(Protester* p, int x, int y);
	void increaseWater();
	bool canPlaceWater(int x, int y);
	void useSonar(int x, int y);
	void bribeProtester(int x, int y);

private:
	vector <Actor*> actorVec;
	TunnelMan* player; //make a player and ground in the arena
	Earth* ground[MAXROWS][MAXCOLS];
	int barrelsLeft;
	int ticksToNewProtester;
	int numProtesters;
	int MAXPROTESTERS;
	//int countdownToProtesterMove;
	int oddsOfGoodie;


};



#endif // STUDENTWORLD_H_
