  
// DTDecision.h -- Decision Tree... Decision Node
// (kinda redundant but oh well)
#ifndef DTDECISION_H
#define DTDECISION_H
#include "DTNode.h"
#include <iostream>
/*
 * In a similar fashion to DTNode.h, this class is NOT meant to be instantiated
 * only inherited
 */
class DTDecision : public DTNode {
public:
	using DTNode::DTNode;

	bool* b; // boolean value upon which getBranch() is based

	DTNode* trueNode;	// trueNode->decide() will be returned by decide()
						// if the condition is true
	
	DTNode* falseNode;	// falseNode->decide() will be returned by decide()
						// if the condition is false


	DTDecision(DTNode*, DTNode*);	// constructor

	void setBool(bool*);	// set value of boolean b

	bool testValue();	// returns test value (b) upon which this decision will be based
	DTNode* getBranch();// performs actual test, returns either trueNode or falseNode
	DTNode* decide();	// returns this->getBranch()->decide()
};
#endif