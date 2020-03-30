#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//////////////////////////////////ACTOR IMPLEMENTATIONS/////////////////////////////////
Actor::~Actor() {
	setVisible(false); //this is how you stop showing it
	//insert Destructor
}

StudentWorld* Actor::getWorld() {
	return Actor::m_world;
}

bool Actor::isDead() {
	return m_isDead;
}

void Actor::moveToDir(Direction dir) {
	if (dir == left) {
		if (getDirection() != left) { setDirection(left); }
		if (getX() > 0 && !getWorld()->boulderToDir('l', getX(), getY()))
			this->moveTo(getX() - 1, getY());
	}
	if (dir == right) {
		if (getDirection() != right) { setDirection(right); }
		if (getX() < MAXCOLS - 4 && !getWorld()->boulderToDir('r', getX(), getY()))
			this->moveTo(getX() + 1, getY());
	}
	if (dir == up) {
		if (getDirection() != up) { setDirection(up); }
		if (getY() < MAXROWS - 4 && !getWorld()->boulderToDir('u', getX(), getY()))
			this->moveTo(getX(), getY() + 1);
	}
	if (dir == down) {
		if (getDirection() != down) { setDirection(down); }
		if (getY() > 0 && !getWorld()->boulderToDir('d', getX(), getY()))
			this->moveTo(getX(), getY() - 1);
	}
}



////////////////////////////////////EARTH IMPLEMENTATIONS////////////////////////////////////////


Earth::~Earth() { // no dynamic memory yet
	//actor destructor already changes setVisible
}

void Earth::doSomething() {
	return; //earth doesnt need to do anything from turn to turn
}


//////////////////////////////////TUNNELMAN IMPLEMENTATIONS/////////////////////////////////////

TunnelMan::~TunnelMan() {
	//dont need a destructor because no dynamic memory yet
	//setVisible(false); //Already in the Actor destructor
}

int TunnelMan::getGold() {
	return goldNugs;
}

int TunnelMan::getHP() {
	return hp;
}

int TunnelMan::getWaterShots() {
	return waterShots;
}

int TunnelMan::getSonar() {
	return SonarCharges;
}

void TunnelMan::decWaterShots() {
	waterShots--;
}

void TunnelMan::getHurt(int amt) {
	hp = hp - amt;
}

void TunnelMan::increaseWater() {
	waterShots = waterShots + 5;
}

void TunnelMan::increaseSonar() {
	SonarCharges = SonarCharges + 1;
}

void TunnelMan::increaseGold() {
	goldNugs = goldNugs + 1;
}

void TunnelMan::doSomething() {
	if (m_isDead == true) return;
	if (hp <= 0) {
		m_isDead = true;
		return;
	}//tunnelman needs to move from turn to turn
	int ch;
	if (getWorld()->getKey(ch) == true) { // user hit a key this tick
		switch (ch) {
		case KEY_PRESS_LEFT:
			if (getDirection() == left) { //if already facing direction then move that way, if not then face that way
				if (getX() > 0 && !getWorld()->boulderToDir('l',getX(),getY()))
					this->moveTo(getX() - 1, getY());
			}
			else {
				this->setDirection(left);
				
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() == right) {
				if (getX() < MAXCOLS - 4 && !getWorld()->boulderToDir('r', getX(), getY()))
					this->moveTo(getX() + 1, getY());
			}
			else {
				this->setDirection(right);
				
			}
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() == down) {
				if (getY() > 0 && !getWorld()->boulderToDir('d', getX(), getY()))
					this->moveTo(getX(), getY() - 1);
			}
			else {
				this->setDirection(down);
				
			}
			break;
		case KEY_PRESS_UP:
			if (getDirection() == up) {
				if (getY() < MAXROWS - 4 && !getWorld()->boulderToDir('u', getX(), getY()))
					this->moveTo(getX(), getY() + 1);
			}
			else {
				this->setDirection(up);
			
			}
			break;
		case 'Z':
		case 'z':
			if (SonarCharges > 0) {
				SonarCharges--;
				getWorld()->useSonar(getX(), getY());
				getWorld()->playSound(SOUND_SONAR);
			}
			else return;
			break;
		case KEY_PRESS_ESCAPE: //code to lose a live and reset level
			m_isDead = true;
			break;
		case KEY_PRESS_SPACE: // need to insert squirt code
			if (waterShots > 0) {
				getWorld()->newSquirt();
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
			}
			break;
		case KEY_PRESS_TAB: //need to insert drop gold code
			if (goldNugs > 0) {
				getWorld()->newDroppedGold();
				return;
			}
			else { return; }
			break;
		default:
			return;
		}
		this->getWorld()->digEarth(getX(), getY());
	}
}

/////////////////////////////////BOULDER IMPLEMENTATIONS////////////////////////////

void Boulder::doSomething(){
	if (!getWorld()->EarthBelowBoulder(getX(), getY())) { //if theres nothing below then it needs to set stable to false
		stable = false;
	}
	if (stable == false) { //if its unstable then it needs to fall until theres ground below after 30 ticks
		waiting++;
		if (waiting == 30) getWorld()->playSound(SOUND_FALLING_ROCK);
		if (waiting > 30) {
			getWorld()->hurtProtester(50, getX(), getY());
			if (getWorld()->distToTunnelman(getX(),getY()) <= 3) {
				getWorld()->hurtTunnelman(50);
			}
			if (!getWorld()->EarthBelowBoulder(getX(), getY())) //if theres earth below then itll fall
				this->moveTo(getX(), getY() - 1);
			else {
				//setVisible(false);
				m_isDead = true;
				//this = true; //if this then
			}
		}
	}

	return;
}



bool Boulder::isStable() {
	return this->stable;
}


/////////////////////////////////OIL IMPLEMENTATION

void Oil::doSomething() {
	double distToPlayer = getWorld()->distToTunnelman(getX(), getY());

	if (distToPlayer <= 5 && found == false) {
		setVisible(true);
		found = true;
		return;
	}
	if (distToPlayer <= 3) {
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		m_isDead = true;
	}
}

Oil::~Oil() {
	getWorld()->destroyBarrel();
}

//////////////////////////////SQUIRT IMPLEMENTATION
void Squirt::doSomething() {
	if (m_isDead == true) { return; }
	if (distTraveled == 0) {
		setDirection(getWorld()->getTunnelman()->getDirection());
		distTraveled++;
		return;
	}

	if (getWorld()->hurtProtester(2, getX(), getY())) {
		m_isDead = true; return;
	}
	

	if (distTraveled < 5) {
		distTraveled++;
		
		if (getDirection() == left) { //if already facing direction then move that way, if not then face that way
			if (getX() > 0 && !getWorld()->boulderToDir('l', getX(), getY()) && !getWorld()->earthtoDir('l', getX(), getY()) )
				this->moveTo(getX() - 1, getY());
			else m_isDead= true;
		}
		else if (getDirection() == right) {
			if (getX() < MAXCOLS - 4 && !getWorld()->boulderToDir('r', getX(), getY()) && !getWorld()->earthtoDir('r', getX(), getY()) )
				this->moveTo(getX() + 1, getY());
			else m_isDead = true;
		}
		else if (getDirection() == down) {
			if (getY() > 0 && !getWorld()->boulderToDir('d', getX(), getY()) && !getWorld()->earthtoDir('d', getX(), getY()) )
				this->moveTo(getX(), getY() - 1);
			else m_isDead = true;
		}
		else if (getDirection() == up) {
			if (getY() < MAXROWS - 4 && !getWorld()->boulderToDir('u', getX(), getY()) && !getWorld()->earthtoDir('u', getX(), getY()) )
				this->moveTo(getX(), getY() + 1);
			else m_isDead = true;
		}
		
	}
	else {
		m_isDead = true;
		setVisible(false);
	}
}

Squirt::~Squirt() {
	//m_isDead = true;
	getWorld()->decreaseSquirts();
}


//////////////////////////////////////////PROTESTER
bool Protester::canMoveDir(Direction dir) {
	if (dir == left) { 
		if (getX() > 0 &&  !getWorld()->earthToDir4x4('l', getX(), getY()) ){
			return true;
		}
		else return false;
	}
	else if (dir == right) {
		if (getX() < MAXCOLS - 4  && !getWorld()->earthToDir4x4('r', getX(), getY()) ) {
			return true;
		}
		else return false;
	}
	else if (dir == down) {
		if (getY() > 0 && !getWorld()->earthToDir4x4('d', getX(), getY()) ) {
			return true;
		}
		else return false;
	}
	else if (dir == up) {
		if (getY() < MAXROWS - 4 && !getWorld()->earthToDir4x4('u', getX(), getY()) ){
			return true;
		}
		else return false;
	}
	else
	return false;
}

bool Protester::canMovePerp(Direction dir) {
	if (dir == left) {
		if( canMoveDir(up) || canMoveDir(down) ) {
			return true;
		}
		else return false;
	}
	else if (dir == right) {
		if (canMoveDir(up) || canMoveDir(down)) {
			return true;
		}
		else return false;
	}
	else if (dir == down) {
		if (canMoveDir(left) || canMoveDir(right)) {
			return true;
		}
		else return false;
	}
	else if (dir == up) {
		if (canMoveDir(left) || canMoveDir(right)) {
			return true;
		}
		else return false;
	}
	else
		return false;
}

Protester::~Protester() {
	getWorld()->decProtesters();
}

void Protester::Yell(){
	getWorld()->playSound(SOUND_PLAYER_ANNOYED);
	
}

void Protester::getHurt(int amt) {
	hp = hp - amt;
}

void Protester::stun(int amtTime) {
	moveCountdown = amtTime;
}

void Protester::doSomething() {
	if (m_isDead == true) return;
	if (hp <= 0) {
		//leavefield = true; WOULD BE THIS IF I IMPLEMENTED LEAVEFIELD
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_isDead = true;
		return;
	}
	if (moveCountdown > 0) {
		moveCountdown--;
		return;
	}
	else { //reset the countdown and then do one of the moves below
		moveCountdown = 3 - getWorld()->getLevel() / 4;
		moveCountdown = max(0, moveCountdown);
		yellCountdown--;
		perpCountdown--;
	}

	if (leavefield == true) { //NEED TO TRY TO LEAVE ASAP
		//HERE IS WHERE I WOULD TRY TO MAKE THEM LEAVE, instead I just kill them
		m_isDead = true;
		if (getX() == 60 && getY() == 60) {
			m_isDead = true;
			return;
		}
		else {//move to exit and return

			return;
		}
	}
	else { //then it should try to get the TUNNELMAN
		//FIRST SEE IF CAN YELL AT ICEMAN  
		if (getWorld()->distToTunnelman(getX(),getY()) <= 4 && yellCountdown<= 0 ){// && getWorld()->facingTunnelMan(getDirection(),getX(),getY()) ){
			Yell();
			getWorld()->hurtTunnelman(2);
			yellCountdown = 15; //reset the countdown
			return;
		}
		
		//NEXT TRY TO SEE TMAN AND MOVE TOWARDS
		if (getWorld()->canSeetunnelMan(getX(), getY())) {//then we need to move towards him
			
			if (getY() == getWorld()->getTunnelman()->getY()) {//if same y coord then move l or r
				if (getX() < getWorld()->getTunnelman()->getX() && canMoveDir(right)) {//then the prot is left of the tman
					moveToDir(right); squaresInDir = 0; return;
				}
				else if (getX() >= getWorld()->getTunnelman()->getX() && canMoveDir(left)) {//then the prot is right of the tman
					 moveToDir(left); squaresInDir = 0; return;
				}
				else {
					if (canMoveDir(getDirection()) && squaresInDir > 0) { moveToDir(getDirection()); squaresInDir--;  return; } //keep moving in same dir
					else {
						Direction randDir = Direction(rand() % 4 + 1);
						squaresInDir = rand() % 53 + 8; //53
						if (canMoveDir(randDir)) {
							moveToDir(randDir);
							return;
						}
					}
				}
			}
			else if (getX() == getWorld()->getTunnelman()->getX()) { //if same x coord
				if (getY() < getWorld()->getTunnelman()->getY() && canMoveDir(up)) {//then the prot is below the tman
					moveToDir(up); squaresInDir = 0; return;
				}
				else if (getY() >= getWorld()->getTunnelman()->getY() && canMoveDir(down)) {//then the prot is aobve the tman
					moveToDir(down); squaresInDir = 0; return;
				}
				else {
					if (canMoveDir(getDirection()) && squaresInDir > 0) { moveToDir(getDirection()); squaresInDir--;  return; } //keep moving in same dir
					else {
						Direction randDir = Direction(rand() % 4 + 1);
						squaresInDir = rand() % 53 + 8; //53
						if (canMoveDir(randDir)) {
							moveToDir(randDir);
							return;
						}
					}
				}
			}
			
			return;
		} //NEXT TRY TO KEEP MOVING IN PERP DIR
		else if (canMovePerp(getDirection()) && perpCountdown <= 0) {
			int chosenDir = rand() % 2;
			if (getDirection() == left || getDirection() == right) {
				if (canMoveDir(up) && canMoveDir(down) ) {//pick a random one and reset perpCound
					if (chosenDir == 0) { setDirection(down);  moveToDir(down);  perpCountdown = 200; return; }
					else { setDirection(up);   moveToDir(up); perpCountdown = 200; return; }
				}
				else if (canMoveDir(up)) { setDirection(up);   moveToDir(up); perpCountdown = 200; return;  }
				else { setDirection(down);  moveToDir(down); perpCountdown = 200; return; }
			}

			if (getDirection() == up || getDirection() == down) {
				if (canMoveDir(left) && canMoveDir(right)) {//pick a random one and reset perpCound
					if (chosenDir == 0) { setDirection(left);   moveToDir(left);  perpCountdown = 200; return; }
					else {
						setDirection(right);  moveToDir(right); perpCountdown = 200; return;
					}
				}
				else if (canMoveDir(left)) { setDirection(left);  moveToDir(left); perpCountdown = 200; return;}
				else { setDirection(right);   moveToDir(right); perpCountdown = 200; return; }
			}
		}
		else if (canMoveDir(getDirection()) && squaresInDir > 0) { moveToDir(getDirection()); squaresInDir--;  return; } //NEXT TRY TO KEEP MOVING IN SAME DIR
		else { //otherwise move in random direction
			Direction randDir = Direction(rand() % 4 + 1);
			squaresInDir = rand() % 53 + 8; //53
			if (canMoveDir(randDir)) {
				moveToDir(randDir);
				return;
			}
		}
	}
}

////////////////////////////////////////WATERPOOL IMP
void WaterPool::doSomething() {
	if (ticksRemaining <= 0) {
		m_isDead = true;
	}
	if (getWorld()->distToTunnelman(getX(), getY()) <= 3) {
		m_isDead = true;
		getWorld()->increaseWater();
	}
}

WaterPool::WaterPool(StudentWorld* studWorld, int startX, int startY, Direction dir , double size, unsigned int depth ) :
	Actor(studWorld, TID_WATER_POOL, startX, startY, dir, size, depth) {
	setVisible(true);
	ticksRemaining = 300 - getWorld()->getLevel() * 10;
	ticksRemaining = max(100, ticksRemaining);
}

////////////////////////////////////////SONAR
Sonar::Sonar(StudentWorld* studWorld, int startX, int startY, Direction dir, double size, unsigned int depth) :
	Actor(studWorld, TID_SONAR, startX, startY, dir, size, depth) {
	setVisible(true);
	ticksRemaining = 300 - getWorld()->getLevel() * 10;
	ticksRemaining = max(100, ticksRemaining);
}

void Sonar::doSomething() {
	if (m_isDead) {
		return;
	}
	if (ticksRemaining <= 0) {
		m_isDead = true;
		return;
	}
	if (getWorld()->distToTunnelman(getX(), getY()) <= 3) { //if picked up by tunnelman
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getTunnelman()->increaseSonar();
		getWorld()->increaseScore(75);
		m_isDead = true;
	}
}

///////////////////////////////////////////NUGGET
Nugget::Nugget(StudentWorld* studWorld, int startX, int startY, bool dropped, Direction dir, double size, unsigned int depth) :
	Actor(studWorld, TID_GOLD, startX, startY, dir, size, depth) , canPlayerPickUp(dropped){
	if (dropped) { setVisible(true); }
	else { setVisible(false); }
	ticksRemaining = 300 - getWorld()->getLevel() * 10;
	ticksRemaining = max(100, ticksRemaining);
}

void Nugget::doSomething() {
	if (ticksRemaining <= 0) {
		m_isDead = true;
		return;
	}
	if (!canPlayerPickUp) { //hidden state , can be picked up by player
		if (getWorld()->distToTunnelman(getX(), getY()) <= 4 && getWorld()->distToTunnelman(getX(), getY()) > 3) {
			setVisible(true);
			return;
		}
		else if (getWorld()->distToTunnelman(getX(), getY()) <= 3) {
			m_isDead = true;
			getWorld()->getTunnelman()->increaseGold();
			getWorld()->playSound(SOUND_GOT_GOODIE);
		}
		else return;
	}
	else {//then it was dropped by the TunnelMan
		ticksRemaining--;
		if (getWorld()->distToNearestObject(getX(), getY()) <= 3) {
			getWorld()->bribeProtester(getX(), getY());
			m_isDead = true;
		}
	}
}