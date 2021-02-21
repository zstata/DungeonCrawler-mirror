#include "Enemy.h"

#ifndef KNIGHT_H
#define KNIGHT_H


class Knight: public Enemy{
    public:
        using Enemy::Enemy;
        int PRESSURE_VALUE=64;
        static std::vector<SDL_Texture*> sprites;
        void attack();
        Player* targetPlayer=nullptr;
        //Arrow* moveCharacter(std::vector<Player*> players, double map_height, double map_width, double timestep);
		Arrow* moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep);
        SDL_Texture* getSprites(){return sprites[spriteIndex];}
        double SPEED_LIMIT=230;
        double getSpeedLimit(){return SPEED_LIMIT;}
		static SDL_Texture* characterSheet;
		static std::vector<SDL_Rect*> characterRects;
		SDL_Texture* getSpriteSheet(double timestep){
			int value=damageSprite(timestep);
            if(value<=0)//If value is less than 0 then we show the sprite
			    return characterSheet;
            else //If value is 1 then we don't show the sprite
                return nullptr;
		}
		
		//radius that the squad can detect the player, not using a cone just a circle 
		double sight = 122500;
		void spotPlayer(Player* target);
				
		//attack method and assisting variables
		void attack(std::vector<Player*> players);
		//damage value of knight, no idea what a good amount is right now 11/12/2020
		double damage = 45;
		//variables for attacking
		double attackEnd = 1500;
		double attackTime = 0; //will add SDL_GetTicks() every iteration and add to this running total, compare to attackEnd
		double attackCooldown = 1500; //1.5 second cooldown, time from SDL_GetTicks measured in milliseconds
		//double cooldownTime = 0;
		bool onCooldown = false; //mark true after an attack is done
		//ints to hold dimensions of hitbox created while attacking, to check if hitting player - WILL DEPEND ON SPRITE AND SIZE OF ENEMY
		int attackBoxFront_X = 50;
		int attackBoxFront_Y = 35;
		int attackBoxSide_X = 45;
		int attackBoxSide_Y = this->y_size;
		
		//block method
		void block();
        //variables for blocking
		double blockEnd = 1250;
		double blockTime = 0; //will add SDL_GetTicks() every iteration and add to this running total, compare to blockEnd
		//ints to hold dimensions of hitbox created while blocking, to check if hitting player - WILL DEPEND ON SPRITE AND SIZE OF ENEMY
		int blockBoxFront_X = this->x_size;
		int blockBoxFront_Y = 20;
		int blockBoxSide_X = 20;
		int blockBoxSide_Y = this->y_size;
		
		//dodge method
		void dodge(double map_height, double map_width, double timestep);
		double dodgeEnd = 300;
		double dodgeTime = 0; //will add SDL_GetTicks() every iteration and add to this running total, compare to dodgeEnd
		//variables for dodging
		bool firstEntry = true;
		int firstSprite = 0;
		bool spriteAssign = true;
		//distance to dodge/move to
		int dodgeDistance = 200;
		double DODGE_SPEED = 400;
		//dodge direction variables and constants to utilize as well
		int dodgeDir = -1; // 0 if dodging left, 1 if dodging right, 2 if up, 3 if down
		int DODGE_LEFT = 0;
		int DODGE_RIGHT = 1;
		int DODGE_UP = 2;
		int DODGE_DOWN = 3;
		
		//counter method
		void counter();
		//time variables for countering
		double counterEnd = 300;
		double counterTime = 0;		
		int ATTACK_DISTANCE_COUNTER = 2500;
		double COUNTER_SPEED = 500;
		
		
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
		//ints for holding sprite indexes of blocking
		int blockFront = 10;
		int blockBack = 12;
		int blockLeft = 11; //will use this but flip for right side


        double actionTime=0;
        
        //All of these constants need to be messed with, since I can't test at the moment someone else will need to handle that.   
        double DODGE_INTERVAL=3000;
        double ATTACK_INTERVAL=2000;
        double ATTACK_CHECK_INTERVAL=750;
        double BLOCK_INTERVAL=5000;
        double COUNTER_INTERVAL=2000;
        double actionInterval=0; //The cooldown for actions - set based off of the last action taken
        double DODGE_DISTANCE_SMALL=25000; //The lower end of the squared distance in px for the knight to dodge
        double DODGE_DISTANCE_LARGE=60000; //The upper end of the squared distance in px for the knight to dodge.  Anything beyond this and the knight should not react.
		double ATTACK_DISTANCE = 15625; //The threshold at which the knight may attack, in px^2
        bool isDodging=false;
        bool isBlocking=false;
        bool isCountering=false;
        bool hasCountered=false;
        bool isAttacking=false;
        double attackTimer=0;
        std::vector<SDL_Rect*> getCharacterRects(){return characterRects;}
		bool returnBlocking(){return isBlocking;}
		SDL_Rect* returnBlockBox(){return blockBox;}
		double getDamage(){return damage;}
		//void updatePos(double x_vel, double y_vel, double map_height, double map_width, double timestep);
};


#endif
