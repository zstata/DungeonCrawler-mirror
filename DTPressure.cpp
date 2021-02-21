// DTPressure.cpp

#include "DTPressure.h"

DTPressure::DTPressure(
	DTNode* tn,
	DTNode* fn,
	std::vector<Player*>* plist) 
	: DTDecision(tn, fn)
{
	this->trueNode = tn;
	this->falseNode = fn;
	this->players = plist;
}

double DTPressure::testValue() {
	if (this->a == nullptr) {
		std::cout << "In DTPressure.cpp: Assassin not set!!" << std::endl;
		return double(-1);
	}
	double highestPressure = 0;
    for(Player* pl: *this->players){
    	if (pl->pressureValue > highestPressure)
        	highestPressure = pl->pressureValue;
    }

    return highestPressure;
}

DTNode* DTPressure::getBranch() {
	if (testValue() < p)
		return this->trueNode;
	else
		return this->falseNode;
}

DTNode* DTPressure::decide() {
	return getBranch()->decide();
}