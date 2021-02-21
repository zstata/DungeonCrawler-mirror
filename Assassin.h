#ifndef ASSASSIN_H
#define ASSASSIN_H
#include "Enemy.h"
#include "DTDouble.h"
#include "DTPressure.h"
#include "DTAction.h"
#include <cmath>

class DTPressure;	// forward declaration of pressure checking decision tree node

class Assassin : public Enemy {
public:
	using Enemy::Enemy;

	static SDL_Texture* characterSheet;
	static std::vector<SDL_Rect*> characterRects;

	int PRESSURE_VALUE = 60; // the amount of pressure the assassin applies to the player

	int** floorMap; 
	int floorHeight;
	int floorWidth;

	double LOW_PRESSURE_THRESHOLD = 2.0;
	double MEDIUM_PRESSURE_THRESHOLD = 6.0;

	double damage = 30;

	//ints to hold dimensions of hitbox created while attacking, to check if hitting player - WILL DEPEND ON SPRITE AND SIZE OF ENEMY
	int attackBoxFront_X = 50;
	int attackBoxFront_Y = 35;
	int attackBoxSide_X = 45;
	int attackBoxSide_Y = this->y_size;

	//variables for attacking
	double attackEnd = 1500;
	double attackTime = 0; //will add SDL_GetTicks() every iteration and add to this running total, compare to attackEnd
	double attackCooldown = 2000; //1.5 second cooldown, time from SDL_GetTicks measured in milliseconds
	bool onCooldown = false; //mark true after an attack is done
	bool isAttacking=false;

	//ints for holding sprite indexes of each sprite in attack animation
	int faceForward = 0;
	int faceBackward = 1;
	int faceLeft = 2; //use this for right facing as well, just flip 
	int attackStartFront = 4;
	int attackStartBack = 8;
	int attackStartLeft = 6;
	int attackEndFront = 5;
	int attackEndBack = 9;
	int attackEndLeft = 7;

	// Decision Tree Nodes
	DTAction* stalk;	// stalks enemy from a distance
	DTAction* creep;	// creep closer as player begins getting pressured
	DTAction* strike;	// strike with powerful sneak attack
	DTAction* flee;		// flee when health is low

	DTDouble* lowHealth;// check if assassin health is low

	DTPressure* lowPressure;	// check if player pressure is low
	DTPressure* mediumPressure;	// check if player pressure is medium

	DTNode* root;
	DTAction* action;

	// Decision Tree functions
	void buildTree(std::vector<Player*>*);
	void getAction(std::vector<Player*>* players);

	// other functions
	Arrow* moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep);
	
	SDL_Texture* getSpriteSheet(double timestep){
		int value=damageSprite(timestep);
		if(value<=0)//If value is less than 0 then we show the sprite
			return characterSheet;
		else //If value is 1 then we don't show the sprite
			return nullptr;
	}
	std::vector<SDL_Rect*> getCharacterRects(){return characterRects;}
	int getPressure() {return PRESSURE_VALUE;}
	double getDamage(){return damage;}
	void attack(Player* hpp, double dist);
};

#endif
