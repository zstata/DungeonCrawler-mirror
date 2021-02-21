// DTAlone.h


/*
 * This is a subclass of DTDecision. This node will
 * evaluate whether an enemy is alone on the screen.
 * If so (enemies->size() == 1), this->decide() will
 * return trueNode->decide(). else it will return
 * falseNode->decide()
 */

#ifndef DTALONE_H
#define DTALONE_H
#include "DTDecision.h"
#include <vector>
#include "Character.h"
#include "Enemy.h"

class DTAlone : public DTDecision {
public:
	DTNode* trueNode;
	DTNode* falseNode;
	std::vector<Enemy*>* enemies;

	DTAlone(DTNode*, DTNode*, std::vector<Enemy*>*); 
	/*: DTDecision(DTNode* trueNode, DTNode* falseNode)*/
	int testValue();
	DTNode* getBranch();
	DTNode* decide();

};
#endif