// DTClose.cpp

#include "DTClose.h"
#include <cmath>

DTClose::DTClose(
	DTNode* tn,
	DTNode* fn,
	std::vector<Player*>* plist) 
	: DTDecision(tn, fn)
{
	this->trueNode = tn;
	this->falseNode = fn;
	this->players = plist;
}

double DTClose::testValue() {
	if (this->enemy == nullptr) {
		std::cout << "In DTClose.cpp: Enemy not set!!" << std::endl;
		return double(-1);
	}

    double closestDist=100000000;
    for(Player* p: *this->players){
        double distance = std::pow((p->x_pos-enemy->x_pos), 2)+std::pow((p->y_pos-enemy->y_pos),2);
        if(distance<closestDist){
            closestDist=distance;
        }
    }

    return closestDist;
}

DTNode* DTClose::getBranch() {
	if (testValue() < 160000)
		return this->trueNode;
	else
		return this->falseNode;
}

DTNode* DTClose::decide() {
	return getBranch()->decide();
}