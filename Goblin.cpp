#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>  //for rand numbers
#include <cstdlib> //for abs
#include <cmath>
#include "Goblin.h"

SDL_Texture* Goblin::characterSheet;
std::vector<SDL_Rect*> Goblin::characterRects;
//main attack method, creates an sdl_rect briefly and an intersection must be checked for
void Goblin::attack(std::vector<Player*> players)
{
	if(this->targetPlayer == nullptr)
		return;
	//make goblin face player being attacked
	this->facePlayer(targetPlayer->x_pos, targetPlayer->y_pos);
	
	//SDL rect for the attacking box
	SDL_Rect* attackBox;
	
	//if attackTime = 0, resetting the attack ie first time called
	if(attackTime == 0)
	{
		attackTime = SDL_GetTicks();
		attackEnd = SDL_GetTicks() + 1000;
		onCooldown = false;
	}
	
	//if cooldown isnt active for attacking
	if(!onCooldown)
	{
		//start animation for up swing, no hitbox yet
		if(attackEnd - attackTime >= 500)
		{
			//check what direction facing and set sprite accordingly
			if(this->spriteIndex == GOBLIN_FRONT) //goblin facing front
			{
				this->spriteIndex = GOBLIN_FRONT_ATTACK_UP;
			}
			else if(this->spriteIndex == GOBLIN_SIDE) //goblin facing left or right depending on flip
			{
				this->spriteIndex = GOBLIN_SIDE_ATTACK_UP;
			}
			else if(this->spriteIndex == GOBLIN_BACK) //goblin facing back
			{
				this->spriteIndex = GOBLIN_BACK_ATTACK_UP;
			}
			
			//increment time for attacking
			this->attackTime = SDL_GetTicks();
		}
		//if attack is still going on - goblin is on down swing, change animation and also create the hurt box
		else if(attackEnd - attackTime < 500 && attackEnd - attackTime >= 0)
		{
			//check what direction facing and set sprite accordingly
			if(this->spriteIndex == GOBLIN_FRONT) //goblin facing front - attacking down
			{
				this->spriteIndex = GOBLIN_FRONT_ATTACK_DOWN;
				//define the size of the hitbox, will be below goblin sprite
				attackBox = new SDL_Rect{static_cast<int>(this->x_pos-10),static_cast<int>(this->y_pos+this->y_size),60,30};
			}
			else if(this->spriteIndex == GOBLIN_SIDE) //goblin facing left or right depending on flip - attacking left or right
			{
				this->spriteIndex = GOBLIN_SIDE_ATTACK_DOWN;
				//define the size of the hitbox
				if(this->flip == SDL_FLIP_NONE) //attacking left
					attackBox = new SDL_Rect{static_cast<int>(this->x_pos-7),static_cast<int>(this->y_pos-10),20,60};
				else //attacking right
					attackBox = new SDL_Rect{static_cast<int>(this->x_pos+this->x_size-30),static_cast<int>(this->y_pos-10),20,60};
			}
			else if(this->spriteIndex == GOBLIN_BACK) //goblin facing back - attacking upward
			{
				this->spriteIndex = GOBLIN_BACK_ATTACK_DOWN;
				//define the size of the hitbox, will be above goblin sprite
				attackBox = new SDL_Rect{static_cast<int>(this->x_pos-10),static_cast<int>(this->y_pos-30),60,30};
			}
			
			//loop to check intersections with players, and remove health if a collision is there
			for(Player* p : players)
			{
				SDL_Rect result;
				if(SDL_IntersectRect(&p->characterTree->root->hitbox, attackBox, &result))
				{
					p->damageCharacter(getDamage());
				}
			}
			
			//increment time for attacking
			this->attackTime = SDL_GetTicks();
		}
		//if attack has gone on for 1.5 seconds, needs to be put on cooldown and end attack
		else if(attackTime > attackEnd)
		{
			this->onCooldown = true;
			this->attackTime = SDL_GetTicks();
			this->attackCooldown = SDL_GetTicks() + 1000;
		}
		
	}
	//else the goblin is on cooldown
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
		
	
	return;
}

//Face the player
void Goblin::facePlayer(double x, double y){
    
	//Check if the x/y difference is greater
	if(std::abs(x-x_pos) >= std::abs(y-y_pos)){

		//To the left of the player
		if(x_pos > x){
			spriteIndex=GOBLIN_SIDE;
			flip = SDL_FLIP_NONE;
		}
		//To the right of the player
		else if(x_pos < x){
			spriteIndex=GOBLIN_SIDE;
			flip = SDL_FLIP_HORIZONTAL;
		}
	}
	else{

		//Underneath the player
		if(y_pos > y){
			spriteIndex=GOBLIN_BACK;
			flip = SDL_FLIP_NONE;
		}
		//Above the player
		else if(y_pos < y){
			spriteIndex=GOBLIN_FRONT;
			flip = SDL_FLIP_NONE;
		}
	}
}

//update collision box for Goblin
void Goblin::updateCollision()
{
	//using sprite indexes to tell what hitboxes to use for now, once sprite sheets are in use & animations improved can implement a direction variable
	
	//if front or back facing
	if(spriteIndex <= 1)
	{
		characterTree->root->hitbox.x = x_pos;
		characterTree->root->hitbox.y = y_pos;
		characterTree->root->hitbox.w = 42;
		characterTree->root->hitbox.h = 40;

	}
	//if left facing
	else if(spriteIndex == 2)
	{
		characterTree->root->hitbox.x = x_pos+10;
		characterTree->root->hitbox.y = y_pos;
		characterTree->root->hitbox.w = 23;
		characterTree->root->hitbox.h = 40;

	}
	//if right facing
	else if(spriteIndex == 3)
	{
		characterTree->root->hitbox.x = x_pos;//+25;
		characterTree->root->hitbox.y = y_pos;//+13;
		characterTree->root->hitbox.w = 42;
		characterTree->root->hitbox.h = 40;

	}
}


