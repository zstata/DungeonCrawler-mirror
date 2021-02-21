// DTClose.h
// Tests if an enemy is close to a player

#ifndef DTCLOSE_H
#define DTCLOSE_H
#include "DTDecision.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>
#include <iostream>

class Enemy;	// forward declaration to deal with circular dependency w Enemy.h

class DTClose : public DTDecision {
public:
	using DTDecision::DTDecision;
	
	std::vector<Player*>* players;
	Enemy* enemy = nullptr;

	DTClose(DTNode*, DTNode*, std::vector<Player*>*);	// constructor

	void setEnemy(Enemy* e) {this->enemy = e;}
	double testValue();
	DTNode* getBranch();
	DTNode* decide();
};
#endif