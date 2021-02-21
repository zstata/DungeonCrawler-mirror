#include "Assassin.h"

SDL_Texture* Assassin::characterSheet;
std::vector<SDL_Rect*> Assassin::characterRects;

void Assassin::buildTree(std::vector<Player*>* players) {
	stalk = new DTAction();
	creep = new DTAction();
	strike = new DTAction();
	flee = new DTAction();
	chase = new DTAction();

	mediumPressure = new DTPressure(creep, strike, players);
	mediumPressure->setPressure(MEDIUM_PRESSURE_THRESHOLD);
	mediumPressure->setAssassin(this);

	lowPressure = new DTPressure(stalk, mediumPressure, players);
	lowPressure->setPressure(LOW_PRESSURE_THRESHOLD);
	lowPressure->setAssassin(this);

	lowHealth = new DTDouble(flee, lowPressure);
	lowHealth->setDouble(&this->health);	// give node pointer to assassin's health
	lowHealth->setCheck(this->health/4);	// give node threshold for low health

	root = lowHealth;
}
void Assassin::getAction(std::vector<Player*>* players) {
	if (!treeBuilt) {
		buildTree(players);
		treeBuilt = true;
	}

	DTAction* a = (DTAction*)root->decide();
	action = a;
	// return a;
	// return chase;	// for temporary testing purposes
}
Arrow* Assassin::moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep)
{
	this->getAction(&players);	// decide action

	// determine which player is experiencing the most pressure
	double highestPressure = 0;
	Player* hpp = nullptr; // highest pressured player

	for (Player* p : players) {
		if (p->pressureValue >= highestPressure) {
			highestPressure = p->pressureValue;
			hpp = p;
		}
	}

	// check distance to highest pressured player
	double hpp_dist = 0;
	if(hpp != nullptr)
		hpp_dist = std::pow((hpp->x_pos-this->x_pos), 2)+std::pow((hpp->y_pos-this->y_pos),2);
	else
		hpp_dist =  165000;

	if (action == stalk) {
		if (hpp_dist < 200000 && hpp != nullptr) 
			this->enemyFlee(hpp->x_pos, hpp->y_pos, map_height, map_width, timestep);

		else if (hpp_dist < 350000) 
			return nullptr;
		
		else if(hpp != nullptr){
			this->pathfind(hpp->x_pos, 
				hpp->y_pos, 
				floorMap, 
				floorHeight, 
				floorWidth, 
				map_height,
				map_width,
				timestep);

		}
		else
			//do nothing
		this->facePlayer(hpp->x_pos, hpp->y_pos);
	}

	else if (action == creep) {
		if (hpp_dist < 160000 && hpp != nullptr) 
			this->enemyFlee(hpp->x_pos, hpp->y_pos, map_height, map_width, timestep);

		else if (hpp_dist < 200000 && hpp != nullptr)
			return nullptr;

		else if(hpp != nullptr){
			this->pathfind(hpp->x_pos, 
				hpp->y_pos, 
				floorMap, 
				floorHeight, 
				floorWidth, 
				map_height,
				map_width,
				timestep);
		}
		else
			//do nothing
		this->facePlayer(hpp->x_pos, hpp->y_pos);
	}

	else if (action == strike && hpp != nullptr) {
		double closest_dist = 999999999;
		Player* cp = nullptr;	// closest player
		double x;
		for (Player* p: players) {
			x = std::pow((p->x_pos-this->x_pos), 2)+std::pow((p->y_pos-this->y_pos),2);
			if (x<closest_dist) {
				closest_dist = x;
				cp = p;
			}
		}
		this->pathfind(cp->x_pos, 
				cp->y_pos, 
				floorMap, 
				floorHeight, 
				floorWidth, 
				map_height,
				map_width,
				timestep);
		attack(cp, closest_dist);
	}

	else if (action == flee && hpp != nullptr) {
		this->enemyFlee(hpp->x_pos, hpp->y_pos, map_height, map_width, timestep);
		this->facePlayer(hpp->x_pos, hpp->y_pos);
	}

	else {
		// decision tree returned bad value
		return nullptr;
	}
	return nullptr;
}

void Assassin::attack(Player* hpp, double dist)
{
	if (dist < 40000) {
		//make assassin face player being attacked
		this->facePlayer(hpp->x_pos, hpp->y_pos);
		
		//SDL rect for the attacking box
		SDL_Rect* attackBox;
		
		//if attackTime = 0, resetting the attack ie first time called
		if(attackTime == 0)
		{
			attackTime = SDL_GetTicks();
			attackEnd = SDL_GetTicks() + 1500;
			onCooldown = false;
		}
		
		//if cooldown isnt active for attacking
		if(!onCooldown)
		{
			//start animation for up swing, no hitbox yet
			if(attackEnd - attackTime >= 750)
			{
				//check what direction facing and set sprite accordingly
				if(this->spriteIndex == faceForward) //assassin facing front
				{
					this->spriteIndex = attackStartFront;
				}
				else if(this->spriteIndex == faceLeft) //assassin facing left or right depending on flip
				{
					this->spriteIndex = attackStartLeft;
				}
				else if(this->spriteIndex == faceBackward) //assassin facing back
				{
					this->spriteIndex = attackStartBack;
				}
				
				//increment time for attacking
				this->attackTime = SDL_GetTicks();
			}
			//if attack is still going on - assassin is on down swing, change animation and also create the hurt box
			else if(attackEnd - attackTime < 750 && attackEnd - attackTime >= 0)
			{
				//check what direction facing and set sprite accordingly
				if(this->spriteIndex == faceForward) //assassin facing front - attacking down
				{
					this->spriteIndex = attackEndFront;
					//define the size of the hitbox, will be below assassin sprite
					attackBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos+this->y_size),attackBoxFront_X,attackBoxFront_Y};
				}
				else if(this->spriteIndex == faceLeft) //assassin facing left or right depending on flip - attacking left or right
				{
					this->spriteIndex = attackEndLeft;
					//define the size of the hitbox
					if(this->flip == SDL_FLIP_NONE) //attacking left
						attackBox = new SDL_Rect{static_cast<int>(this->x_pos-30),static_cast<int>(this->y_pos),attackBoxSide_X,attackBoxSide_Y};
					else //attacking right
						attackBox = new SDL_Rect{static_cast<int>(this->x_pos+this->x_size-30),static_cast<int>(this->y_pos),attackBoxSide_X,attackBoxSide_Y};
				}
				else if(this->spriteIndex == faceBackward) //assassin facing back - attacking upward
				{
					this->spriteIndex = attackEndBack;
					//define the size of the hitbox, will be above assassin sprite
					attackBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos-30),attackBoxFront_X,attackBoxFront_Y};
				}
				
				//loop to check intersections with players, and remove health if a collision is there
				SDL_Rect result;
				if(SDL_IntersectRect(&hpp->characterTree->root->hitbox, attackBox, &result))
				{
					hpp->damageCharacter(getDamage());
				}
				
				//increment time for attacking
				this->attackTime = SDL_GetTicks();
			}
			//if attack has gone on for 1.5 seconds, needs to be put on cooldown and end attack
			else if(attackTime > attackEnd)
			{
				this->onCooldown = true;
				this->attackTime = SDL_GetTicks();
				this->attackCooldown = SDL_GetTicks() + 1500;
				this->isAttacking = false;
			}
			
		}
		//else the assassin is on cooldown
		else
		{
			//if time elapsed is >= cooldown time, turn off cooldown and reset time
			if(this->attackTime >= this->attackCooldown)
			{
				this->onCooldown = false;
				this->attackTime = 0;
			}
			//else it is still on cooldown
			else
			{
				this->attackTime = SDL_GetTicks();
			}		
		}
	}
}