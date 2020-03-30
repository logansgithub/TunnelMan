#include "StudentWorld.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;


GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init()
{
	for (int i = 0; i < MAXCOLS; i++) {
		for (int j = 0; j < MAXROWS; j++) {
			ground[i][j] = new Earth(this, i, j);
			if (j >= MAXDIRTROWS) {
				ground[i][j]->setVisible(false);
			}
		}
	}
	//create ground for 64x60 blocks
	for (int i = 0; i < MAXDIRTCOLS; i++) { //fill in all ground
		for (int j = 0; j < MAXDIRTROWS; j++) {
			if (i< MAXDIRTCOLS/2 -2  || i > (MAXDIRTCOLS/2) + 1) { // everything not in the middle     
				ground[i][j]->setVisible(true);	// Dynamically allocate Earth objects
			}
			else{
				if(j<4)
					ground[i][j]->setVisible(true);	// we want the tunnel to not go all the way down
				else {
					ground[i][j]->setVisible(false);
				}
			}
		}
	}
	// Create Tunnelman at top middle
	player = new TunnelMan(this, 30, 60);	

	//CREATE AND PLACE BOULDERS
	int B = (getLevel() / 2) + 2;

	for (int i = 0; i < B && i < 9; i++) { //for each new boulder generate coordinates
		int m_x = rand() % (MAXDIRTCOLS-5);
		int m_y = rand() % (MAXDIRTROWS-5);
		while ((m_x >= (MAXDIRTCOLS / 2 - 6) && m_x <= ((MAXDIRTCOLS / 2) + 2)) || distToNearestObject(m_x,m_y) <= 6 ) { //make sure boulder not in tunnel, repeat coordinates if it is
			m_x = rand() % (MAXDIRTCOLS-5);
			m_y = rand() % (MAXDIRTROWS-5); //Dont think I need this since I just need the x coordinates to not be in the tunnel
		}
		Boulder* newBoulder = new Boulder(this, m_x, m_y);
		digEarth(m_x, m_y);
		actorVec.push_back(newBoulder);
		//boulders.push_back(newBoulder);
	}
	
	//CREATE AND PLACE OIL
	int L = (getLevel() + 2);
	L = min(21, L);
	for (int i = 0; i < L; i++) { //for each new boulder generate coordinates
		int m_x = rand() % (MAXDIRTCOLS - 5);
		int m_y = rand() % (MAXDIRTROWS - 5);
		while ((m_x >= (MAXDIRTCOLS / 2 - 3) && m_x <= ((MAXDIRTCOLS / 2) + 2)) || distToNearestObject(m_x, m_y) <= 6) { //make sure boulder not in tunnel, repeat coordinates if it is
			m_x = rand() % (MAXDIRTCOLS - 5);
			m_y = rand() % (MAXDIRTROWS-5); 
		}
		Oil* newOil = new Oil(this, m_x, m_y);
		actorVec.push_back(newOil);
		barrelsLeft++;
	}

	//CREATE AND PLACE GOLD
	int G = (5 - getLevel() / 2);
	G = max(2, G);
	for (int i = 0; i < G; i++) { //for each new boulder generate coordinates
		int m_x = rand() % (MAXDIRTCOLS - 5);
		int m_y = rand() % (MAXDIRTROWS - 5);
		while ((m_x >= (MAXDIRTCOLS / 2 - 3) && m_x <= ((MAXDIRTCOLS / 2) + 2)) || distToNearestObject(m_x, m_y) <= 6) { //make sure boulder not in tunnel, repeat coordinates if it is
			m_x = rand() % (MAXDIRTCOLS - 5);
			m_y = rand() % (MAXDIRTROWS - 5);
		}
		Nugget* newOil = new Nugget(this, m_x, m_y, false);
		actorVec.push_back(newOil);
	}

	//CREATE ONE NEW PROTESTER AND ITITALIZE COUNTDOWN
	
	if (!ishardcore()) {
		Protester* newProtester = new Protester(this, 60, 60);
		actorVec.push_back(newProtester);
	}
	else {
		HardcoreProtester* newProtester = new HardcoreProtester(this, 60, 60);
		actorVec.push_back(newProtester);
	}
	numProtesters = 1;
	//initialize protester variables
		ticksToNewProtester = 200 - getLevel();
		ticksToNewProtester = max(ticksToNewProtester, 25);

		MAXPROTESTERS = 2 + getLevel() * 1.5;
		MAXPROTESTERS = max(15, MAXPROTESTERS);
	
	//init other consts
		oddsOfGoodie = getLevel() * 25 + 300;

	return GWSTATUS_CONTINUE_GAME; //go on
}

int StudentWorld::move()
{
	DisplayText();
	if (player->isDead()) { //die if player dies
		decLives();
		return GWSTATUS_PLAYER_DIED; 
	}
	else if (barrelsLeft <= 0) {
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
	else {
	
		player->doSomething(); //ask the player to do something
		int vecsize = actorVec.size();
		for (int i = 0; i < vecsize; i++){ //ask actors to doSomething
			actorVec[i]->doSomething();
		}
		
		//create new Protester every __ clicks
		
		ticksToNewProtester--;
		if (ticksToNewProtester <= 0 && numProtesters <= MAXPROTESTERS ) { //if conditions met then add protester
			Protester* newProtester = new Protester(this, 60, 60);
			actorVec.push_back(newProtester);
			numProtesters++;
			
			//reset ticks to new protester
			ticksToNewProtester = 200 - getLevel();
			ticksToNewProtester = max(ticksToNewProtester, 25);
		}

		//PLACE NEW GOODIE
		
		if (rand() % oddsOfGoodie == 1) { //one in Odds chance of placing a goodie at all
			int WaterGoodie = rand() % 5; //odds of a water goodie
			if (WaterGoodie == 0) {//add sonar
				Sonar* newSonar = new Sonar(this,0,60);
				actorVec.push_back(newSonar);
			}
			else {//add water
				int m_x = rand() % (MAXDIRTCOLS - 5);
				int m_y = rand() % (MAXROWS-4);
				while ((m_x >= (MAXDIRTCOLS / 2 - 3) && m_x <= ((MAXDIRTCOLS / 2) + 2)) || distToNearestObject(m_x, m_y) <= 6 || !canPlaceWater(m_x, m_y)) { //make sure boulder not in tunnel, repeat coordinates if it is
					m_x = rand() % (MAXDIRTCOLS - 5);
					m_y = rand() % (MAXROWS-4);
				}
				WaterPool* newWater = new WaterPool(this, m_x, m_y);
				actorVec.push_back(newWater);
			}
		}
		
		//////////////NEED TO DELETE DEAD ACTORS
		for (auto it = actorVec.begin(); it != actorVec.end(); ) {
			if ((*it)->isDead()) {
				auto temp = it;
				delete *temp;
				it = actorVec.erase(it);
			}
			else {
				it++;
			}
		}

	


		/* GROUND DOES NOTHING */
		return GWSTATUS_CONTINUE_GAME;
	}
}

void StudentWorld::cleanUp(){
		//delete all EARTH BLOCK MEMORY
		for (int i = 0; i < MAXCOLS; i++) {
			for (int j = 0; j < MAXROWS; j++) {
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
		barrelsLeft = 0;
}


void StudentWorld::digEarth(int x, int y) {
	bool dugSomething = false;
	//Earth* ep;
	for (int i = x; i < x + 4 && i<MAXDIRTCOLS && i>=0; i++) {
		for (int j = y ; j < y + 4 && j< MAXDIRTROWS && j>=0; j++) {
			if (ground[i][j] != nullptr) {
				if (ground[i][j]->isVisible()) {
					ground[i][j]->setVisible(false);
					dugSomething = true;
				}
			}
		}
	}
	if (dugSomething) {
		playSound(SOUND_DIG);
	}
}

bool StudentWorld::EarthBelowBoulder(int x, int y) { //returns true if at least 1 earth below, false if no earth below
	if (y < 1) return true; //treating the bottom of screen as dirt
	for (int i = 0; i < 4; i++) {
		if (ground[x+i][y-1]->isVisible())
			return true;
	}
	return false;
}


double StudentWorld::distance(int x1, int x2, int y1, int y2) {
	double dist = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	return dist;
}

double StudentWorld::distToTunnelman(int x, int y) {
	double dist = distance(player->getX(), x, player->getY(), y);
	return dist;
}

double StudentWorld::distToNearestObject(int x, int y) {
	double smallest = 1000;
	int vecsize = actorVec.size();
	
	for (int i = 0; i < vecsize; i++) { //looking through actors
		double dist = distance(x, actorVec[i]->getX(), y, actorVec[i]->getY());
		if (dist > 0 && dist < smallest) {
			smallest = dist;
		}
	}
	return smallest;
}

void StudentWorld::destroyBarrel() {
	barrelsLeft--;
}

void StudentWorld::DisplayText() {
	int gold = player->getGold();
	int hp = player->getHP() * 10; // percentage of full
	int level = getLevel();
	int lives = getLives();
	int sonar = player->getSonar();
	int waterShots = player->getWaterShots();
	int score = getScore();

	ostringstream oss;
	oss << "Lvl: " << setw(2) << level
		<< "  Lives: " << setw(1) << lives
		<< "  Hlth: " << setw(3) << hp << "%"
		<< "  Wtr: " << setw(2) << waterShots
		<< "  Gld: " << setw(2) << gold
		<< "  Oil Left: " << setw(2) << barrelsLeft
		<< "  Sonar: " << setw(2) << sonar;
	oss.fill('0');
	oss << "  Scr: " << setw(6) << score;
	string s = oss.str();
	setGameStatText(s);
}


bool StudentWorld::boulderToDir(char dir, int x, int y) { //returns true if there is a boulder
	if (y >= MAXDIRTROWS) { return false; }
	if (dir == 'l') {//check left for boulder
		int vecsize = actorVec.size();
		for (int i = 0; i < vecsize; i++) { //looking through actors
			if (actorVec[i]->isBoulder()) {
				if (x == actorVec[i]->getX() + 4) {
					if (y >= actorVec[i]->getY() - 3 && y <= actorVec[i]->getY() + 3) {
						return true;
					}
				}
			}
		}
	}
	else if (dir == 'r') {//check left for boulder
		int vecsize = actorVec.size();
		for (int i = 0; i < vecsize; i++) { //looking through actors
			if (actorVec[i]->isBoulder()) {
				if (x == actorVec[i]->getX() - 4) {
					if (y >= actorVec[i]->getY() - 3 && y <= actorVec[i]->getY() + 3) {
						return true;
					}
				}
			}
		}
	}
	else if (dir == 'd') {//check left for boulder
		int vecsize = actorVec.size();
		for (int i = 0; i < vecsize; i++) { //looking through actors
			if (actorVec[i]->isBoulder()) {
				if (y == actorVec[i]->getY() + 4) {
					if (x >= actorVec[i]->getX() - 3 && x <= actorVec[i]->getX() + 3) {
						return true;
					}
				}
			}
		}
	}
	else if (dir == 'u') {//check left for boulder
		int vecsize = actorVec.size();
		for (int i = 0; i < vecsize; i++) { //looking through actors
			if (actorVec[i]->isBoulder()) {
				if (y == actorVec[i]->getY() - 4) {
					if (x >= actorVec[i]->getX() - 3 && x <= actorVec[i]->getX() + 3) {
						return true;
					}
				}
			}
		}
	}
	
	return false;

}

bool StudentWorld::earthtoDir(char dir, int x, int y) { //returns true if theres a block there
	if (y >= MAXDIRTROWS && dir != 'd') { return false; }

	if (dir == 'l') {//check left for dirt
		if (x > 0 && ground[x - 1][y]->isVisible()) {
			return true;
		}
	}
	else if (dir == 'r') {//check left for dirt
		if (x < MAXDIRTCOLS && ground[x+4][y]->isVisible()) {
			return true;
		}
	}
	else if (dir == 'd') {//check left for dirt
		if (y > 0 && ground[x][y-1]->isVisible()) {
			return true;
		}
	}
	else if (dir == 'u') {//check up for dirt
		if (y < MAXDIRTROWS-4 && ground[x][y+4]->isVisible()) {
			return true;
		}
	}

	return false;
}

bool StudentWorld::earthToDir4x4(char dir, int x, int y) { //returns true if any blocks of earth there
	if (y >= MAXDIRTROWS && dir != 'd') { return false; }
	bool result = false;

	if (dir == 'l') {//check left for dirt
		for (int i = 0; i < 4; i++) {
			if (x > 0 && ground[x - 1][y + i] != nullptr) {
				if(ground[x - 1][y + i]->isVisible())
				return true;
			}
		}
		return false;
	}
	else if (dir == 'r') {//check left for dirt
		for (int i = 0; i < 4; i++) {
			if (x < (MAXCOLS - 5) && ground[x + 4][y + i] != nullptr) {
				if(ground[x + 4][y + i]->isVisible())
				return true;
			}
		}
		return false;
	}
	else if (dir == 'd') {//check left for dirt
		for (int i = 0; i < 4; i++) {
			if (y > 0 && ground[x + i][y - 1] != nullptr) {
				if(ground[x + i][y - 1]->isVisible())
				return true;
			}
		}
		return false;
	}
	else if (dir == 'u') {//check left for dirt
		for (int i = 0; i < 4; i++) {
			if (y < (MAXDIRTROWS - 4) && ground[x + i][y + 4] != nullptr) {
				if(ground[x + i][y + 4]->isVisible())
				return true;
			}
		}
		return false;
	}
	else {
		return false;
	}

}

TunnelMan* StudentWorld::getTunnelman(){
	return player;
}

void StudentWorld::newSquirt() {
	GraphObject::Direction dir = player->getDirection();
	char chardir;
	int x = player->getX();
	int y = player->getY();
	if (dir == GraphObject::left) {	x = player->getX() - 4; chardir = 'l';}
	else if (dir == GraphObject::right) { x = player->getX() + 4; chardir = 'r'; }
	else if (dir == GraphObject::down) { y = player->getY() - 4; chardir = 'd'; }
	else if (dir == GraphObject::up) { y = player->getY() +4 ; chardir = 'u'; }

	if (x < MAXCOLS - 4 && x>=0 && y>=0 && y<MAXROWS && !earthtoDir(chardir,player->getX(),player->getY())  && !boulderToDir(chardir,player->getX(), player->getY()) ) {
		Squirt* newsquirt = new Squirt(this, x, y);
		actorVec.push_back(newsquirt);
	}
}

void StudentWorld::decreaseSquirts() {
	player->decWaterShots();
}

bool StudentWorld::ishardcore() {
	int hardcoreProb = getLevel() * 10 + 30;
	if (hardcoreProb > 90) { hardcoreProb = 90; }

	int probability = rand() % 100;
	if (probability < hardcoreProb) {
		return true;
	}
	else {
		return false;
	}
}

void StudentWorld::decProtesters() {
	numProtesters--;
}

void StudentWorld::hurtTunnelman(int amt) {
	player->getHurt(amt);
}

bool StudentWorld::facingTunnelMan(GraphObject::Direction dir, int x, int y) {
	if (dir == GraphObject::left) {
		if (player->getX() < x && player->getY() == y) {
			return true;
		}
		else return false;
	}
	if (dir == GraphObject::right) {
		if (player->getX() > x && player->getY() == y) {
			return true;
		}
		else return false;
	}
	if (dir == GraphObject::down) {
		if (player->getX() == x && player->getY() < y) {
			return true;
		}
		else return false;
	}
	if (dir == GraphObject::up) {
		if (player->getX() < x && player->getY() == y) {
			return true;
		}
		else return false;
	}
	else return false;
}

bool StudentWorld::canSeetunnelMan(int x, int y) {
	if (x != player->getX() && y != player->getY()) return false;
	else if (x == player->getX()) {//if its on the same x (y) plane then see if theres any earth in the way
		if (y < player->getY()) {
			while (y < player->getY() -1) {
				if (earthToDir4x4('u', x, y) || boulderToDir('u',x,y)) { return false; }
				else y++;
			}
		if (y > player->getY() + 1) {
			while (y > player->getY() + 1) {
					if (earthToDir4x4('d', x, y) || boulderToDir('d', x, y)) { return false; }
					else y--;
				}
			}
		}
		return true;
	}
	else if (y == player->getY()) {//if its on the same y (x) plane then see if theres any earth in the way
		if (x < player->getX()) {
			while (x < player->getX() -1) {
				if (earthToDir4x4('r', x, y) || boulderToDir('r', x, y)) { return false; }
				else x++;
			}
		}
		else if (x > player->getX()) {
			while (x > player->getX() + 1) {
				if (earthToDir4x4('l', x, y) || boulderToDir('l', x, y)) { return false; }
				else x--;
			}
		}
		return true;
	}
	else return false;

}

bool StudentWorld::hurtProtester(int amt, int x, int y) {
	bool result = false;
	int vecsize = actorVec.size();
	for (int i = 0; i < vecsize; i++) { //looking through actors
		if (actorVec[i]->isBadGuy()) {
			double dist = distance(x, actorVec[i]->getX(), y, actorVec[i]->getY());
			if (dist > 0 && dist <= 3) {
				actorVec[i]->getHurt(amt);
				playSound(SOUND_PROTESTER_ANNOYED);
				int stunTime = 100 - getLevel() * 10;
				stunTime = max(50, stunTime);
				actorVec[i]->stun(stunTime);
				increaseScore(100);
				result = true;
			}
		}
	}
	return result;
}


GraphObject::Direction StudentWorld::firstMoveToExit(Protester* p, int x, int y) {
	return GraphObject::down;
}

void StudentWorld::increaseWater() {
	player->increaseWater();
	playSound(SOUND_GOT_GOODIE);
	increaseScore(100);
}

bool StudentWorld::canPlaceWater(int x, int y) {
	if (y >= MAXDIRTROWS) {
		return true;
	}
	else {
		int vecsize = actorVec.size();
		for (int i = 0; i < vecsize; i++) { //check for boulders
			if (actorVec[i]->isBoulder()) {
				if (x == actorVec[i]->getX() && y == actorVec[i]->getY()) {
					return false;
				}
			}
		}

		for (int i = x; i < x + 4; i++) { //check ground 4x4
			for (int j = y; j < y + 4; j++) {
				if (ground[i][j]->isVisible()) {
					return false;
				}
			}
		}
	}
	return true;	
}

void StudentWorld::useSonar(int x, int y) {
	int vecsize = actorVec.size();
	for (int i = 0; i < vecsize; i++) { //check for boulders
		if (distToTunnelman(actorVec[i]->getX(), actorVec[i]->getY()) < 12) {
			actorVec[i]->setVisible(true);
		}
		else continue;
	}
}

void StudentWorld::bribeProtester(int x, int y) {
	int vecsize = actorVec.size();
	for (int i = 0; i < vecsize; i++) { //check which actor is closest to the gold
		if (distance(x, actorVec[i]->getX(),y,actorVec[i]->getY() ) <=3 && actorVec[i]->isBadGuy() ) {
			if (actorVec[i]->canBeBribed()) {//then its a regular protester
				playSound(SOUND_PROTESTER_FOUND_GOLD);
				
				actorVec[i]->getHurt(50); //some large amount so the protester leaves
			}
			else { //then its a hardcore one and cant be bribed
				playSound(SOUND_PROTESTER_FOUND_GOLD);
				increaseScore(50);
				int stunTime = 100 - getLevel() * 10;
				stunTime = max(50, stunTime);
				actorVec[i]->stun(stunTime);
				////////////////////////////////////STUN PROTESTER
			}
		}
		else continue;
	}
}

void StudentWorld::newDroppedGold() {
	Nugget* newNug = new Nugget(this, player->getX(), player->getY(), true);
	actorVec.push_back(newNug);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
