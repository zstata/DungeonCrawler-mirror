// DTDouble.cpp

#include "DTDouble.h"

DTDouble::DTDouble(DTNode* trueNode, DTNode* falseNode) 
: DTDecision(trueNode, falseNode) { // constructor
	this->trueNode = trueNode;
	this->falseNode = falseNode;
}

void DTDouble::setDouble(double* x) {
	this->x = x;
}

void DTDouble::setCheck(double c) {
	this->check = c;
}

double DTDouble::testValue() {
	return *this->x;
}

DTNode* DTDouble::getBranch() {
	if (this->testValue() < check)
		return this->trueNode;

	else {
		return this->falseNode;
	}
}

DTNode* DTDouble::decide() {
	DTNode* branch = this->getBranch();
	return branch->decide();
}