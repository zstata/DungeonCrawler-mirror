// DTAction.h -- Decision Tree Action

#ifndef DTACTION_H
#define DTACTION_H
#include "DTNode.h"
/*
 * This class is the base case for the recursive decide() method.
 * An instance of DTAction will ALWAYS be a leaf in the Decision
 * Tree.
 */
class DTAction : public DTNode {
public:
	using DTNode::DTNode;
	DTAction();	// constructor
	
	DTNode* decide() {return this;}
};

#endif