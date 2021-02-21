// DTDouble.h
#ifndef DTDOUBLE_H
#define DTDOUBLE_H
#include "DTDecision.h"
#include <iostream>

class DTDouble : public DTDecision {
public:
	using DTDecision::DTDecision;

	double* x; // double value upon which getBranch() decision is based
	double check;	// value we want to check. if x < check, getBranch returns trueNode

	DTNode* trueNode;	// trueNode->decide() will be returned by decide()
						// if the condition is true
	
	DTNode* falseNode;	// falseNode->decide() will be returned by decide()
						// if the condition is false


	DTDouble(DTNode*, DTNode*);	// constructor

	void setDouble(double*);	// set value of boolean b
	void setCheck(double); // set value of double which x is checked against


	double testValue();	// returns test value (x) upon which this decision will be based
	DTNode* getBranch();// will check if x < check
	DTNode* decide();	// returns this->getBranch()->decide()
};
#endif