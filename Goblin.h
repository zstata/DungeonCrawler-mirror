#include "Enemy.h" 
#include <string>

#ifndef GOBLIN_H
#define GOBLIN_H

class Goblin:public Enemy{
	public:
		using Enemy::Enemy;
		//update collisions for goblin sprites
		void updateCollision();
		static SDL_Texture* characterSheet;
		static std::vector<SDL_Rect*> characterRects;
		//define a speed limit
		double SPEED_LIMIT = 215;
		double SPEED_LIMIT_HOLDER = 230;
		double wander_speed_limit = 5;
		double getSpeedLimit(){return SPEED_LIMIT;}		
		Player* targetPlayer=nullptr;
		
		//method to attack player, where x_player and y_player are player coords
		void attack(std::vector<Player*> players);
		double damage = 10;
		//method to face player, overloaded from original enemy.cpp
		void facePlayer(double x, double y);
		double getDamage(){return damage;}
		//variables for attacking
		double attackEnd = 1500;
		double attackTime = 0; //will add SDL_GetTicks() every iteration and add to this running total, compare to attackEnd
		double attackCooldown = 1500; //1.5 second cooldown, time from SDL_GetTicks measured in milliseconds
		//double cooldownTime = 0;
		bool onCooldown = false; //mark true after an attack is done
		
		SDL_Texture* getSpriteSheet(double timestep){
			int value=damageSprite(timestep);
			if(value<=0)//If value is less than 0 then we show the sprite
				return characterSheet;
			else //If value is 1 then we don't show the sprite
				return nullptr;
		}

		//ints to hold values for the various sprites
		int GOBLIN_FRONT = 0;
		int GOBLIN_SIDE = 1;
		int GOBLIN_BACK = 2;
		int GOBLIN_FRONT_ATTACK_UP = 3;
		int GOBLIN_FRONT_ATTACK_DOWN = 4;
		int GOBLIN_SIDE_ATTACK_UP = 5;
		int GOBLIN_SIDE_ATTACK_DOWN = 6;
		int GOBLIN_BACK_ATTACK_UP = 7;
		int GOBLIN_BACK_ATTACK_DOWN = 8;
		

		std::vector<SDL_Rect*> getCharacterRects(){return characterRects;}

		//string to hold name of sprite sheet
		//std::string spriteSheet = "sprites/GoblinFront1.png";
};

#endif