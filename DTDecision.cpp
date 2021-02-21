// DTDecision.cpp

#include "DTDecision.h"

DTDecision::DTDecision(DTNode* trueNode, DTNode* falseNode) { // constructor
	this->trueNode = trueNode;
	this->falseNode = falseNode;
}

void DTDecision::setBool(bool* b) {
	this->b = b;
}

bool DTDecision::testValue() {
	return *this->b;
}

DTNode* DTDecision::getBranch() {
	if (this->testValue())
		return this->trueNode;

	else {
		return this->falseNode;
	}
}

DTNode* DTDecision::decide() {
	DTNode* branch = this->getBranch();
	return branch->decide();
}