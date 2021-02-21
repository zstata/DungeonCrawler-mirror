// DTInt.cpp

#include "DTInt.h"

DTInt::DTInt(DTNode* trueNode, DTNode* falseNode) : DTDecision(trueNode, falseNode) { // constructor
	this->trueNode = trueNode;
	this->falseNode = falseNode;
}

void DTInt::setInt(int* x) {
	this->x = x;
}

void DTInt::setCheck(int c) {
	this->check = c;
}

int DTInt::testValue() {
	return *this->x;
}

DTNode* DTInt::getBranch() {
	if (this->testValue() < check)
		return this->trueNode;

	else {
		return this->falseNode;
	}
}

DTNode* DTInt::decide() {
	DTNode* branch = this->getBranch();
	return branch->decide();
}