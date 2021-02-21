// DTAlone.cpp
#include "DTAlone.h"

DTAlone::DTAlone(DTNode* trueNode, DTNode* falseNode, std::vector<Enemy*>* enemies) : DTDecision(trueNode, falseNode) { 
	this->trueNode = trueNode; 
	this->falseNode = falseNode;
	this->enemies = enemies;
}

int DTAlone::testValue() {
	return this->enemies->size();
}

DTNode* DTAlone::getBranch() {
	if (this->testValue() == 1) {
		return this->trueNode;
	}

	else
		return this->falseNode;
}

DTNode* DTAlone::decide() {
	return this->getBranch();
}