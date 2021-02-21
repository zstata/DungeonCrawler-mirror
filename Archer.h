#include "Enemy.h"
#include "Arrow.h"
#ifndef ARCHER_H
#define ARCHER_H

class Archer:public Enemy{
    public:
        const int RUN_DISTANCE=40000;
        const int CHASE_DISTANCE=160000;
        const int KITE_PRESSURE_THRESHOLD=15;
        double shotInterval=3000;
        double SPEED_LIMIT=250;
	double sight=250000;
	bool playerSeen=false;
		static SDL_Texture* characterSheet;
		static std::vector<SDL_Rect*> characterRects;
        //The last time this enemy fired a shot
        double shotTime = -1;
        using Enemy::Enemy;
        //try and maintain a set distance away from the player 
        void kitePlayer(double x, double y, double map_height, double map_width, double timestep, int** floorMap, double floorHeight, double floorWidth);
		//Arrow* moveCharacter(std::vector<Player*> players, double map_height, double map_width, double timestep);
		Arrow* moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep);
        Arrow* fireArrow(double x, double y);
		SDL_Texture* getSpriteSheet(double timestep){
			int value=damageSprite(timestep);
            if(value<=0)//If value is less than 0 then we show the sprite
			    return characterSheet;
            else //If value is 1 then we don't show the sprite
                return nullptr;
		}
	void spotPlayer(Player* target);
        std::vector<SDL_Rect*> getCharacterRects(){return characterRects;}
	double getSpeedLimit(){return SPEED_LIMIT;}
};





#endif
