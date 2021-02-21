// DTInt.h
#ifndef DTINT_H
#define DTINT_H
#include "DTDecision.h"
#include <iostream>

class DTInt : public DTDecision {
public:
	using DTDecision::DTDecision;

	int* x; // integer value upon which getBranch() is based
	int check;	// value we want to check. if x < check, getBranch returns trueNode

	DTNode* trueNode;	// trueNode->decide() will be returned by decide()
						// if the condition is true
	
	DTNode* falseNode;	// falseNode->decide() will be returned by decide()
						// if the condition is false


	DTInt(DTNode*, DTNode*);	// constructor

	void setInt(int*);	// set value of boolean b
	void setCheck(int); // set value of int which x is checked against


	int testValue();	// returns test value (x) upon which this decision will be based
	DTNode* getBranch();// will check if x < check
	DTNode* decide();	// returns this->getBranch()->decide()
};
#endif