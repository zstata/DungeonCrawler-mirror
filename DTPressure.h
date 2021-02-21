// DTPressure.h
// Checks if players are under low or high pressure

#ifndef DTPRESSURE_H
#define DTPRESSURE_H
#include "DTDecision.h"
#include "Player.h"
#include "Assassin.h"
#include <vector>
#include <iostream>

class Assassin;	// forward declaration to deal with circular dependency w Assassin.h

class DTPressure : public DTDecision {
public:
	using DTDecision::DTDecision;
	
	std::vector<Player*>* players;
	Assassin* a = nullptr;

	double p;	// pressure threshold

	DTPressure(DTNode*, DTNode*, std::vector<Player*>*);	// constructor

	void setAssassin(Assassin* a) {this->a = a;}
	void setPressure(double p) {this->p = p;}
	double testValue();
	DTNode* getBranch();
	DTNode* decide();
};
#endif