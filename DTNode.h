// DTNode.h - Decision Tree Node

#ifndef DTNODE_H
#define DTNODE_H
#include <vector>
#include <iostream>


/*
 * Base class for Decision Tree Structure
 * 
 * Child classes: DTDecision.h, DTAction.h
 * 
 * A decision node recursively decides the tree. Its
 * version of the decide() method returns the next node
 * in the tree, depending upon the decision that the
 * node makes.
 * 
 * An action node is the base case and will return itself
 * if reached.
 *
 * The DTNode class itself is not designed to be instantiated;
 * just inherited.
 * 
 * After building a Decision Tree by creating appropriate action
 * and decision nodes, invoke decide() from the root note, and if
 * built properly, the tree will recursively make decisions until
 * it returns an action.
 */

class DTNode {
public:
	/*virtual DTNode(const DTNode& other);	// copy constructor
	virtual DTNode& operator=(const DTNode &other);	// copy assignment operator
	virtual DTNode(DTNode&& other);	// move constructor
	virtual DTNode& operator=(DTNode&& other);	// move assignment operator
	virtual ~DTNode();	// destructor*/

	// recursively decides through tree
	virtual DTNode* decide() { return this; }
};
#endif