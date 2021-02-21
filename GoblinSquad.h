#include "Goblin.h" 

#ifndef GOBLINSQUAD_H
#define GOBLINSQUAD_H

class GoblinSquad:public Enemy{
	public:
		using Enemy::Enemy;
	
		//will be the rect that all goblins stay in, unless attacking the player
		//will move in a direction, and the goblins in the squad will follow as a pack
		SDL_Rect boundingRect;
		//define center point for bounding rect
		double x_center;
		double y_center;
		//update collisions for goblin sprites
		void updateCollision();
		
		//bool to mark if a goblin squad
		bool isSquad = true;
		bool checkSquad(){ return isSquad; }
		
		SDL_Texture* getSpriteSheet(double timestep){
            return nullptr;
		}
		
		//define a speed limit
		double SPEED_LIMIT = 215;
		double SPEED_LIMIT_HOLDER = 170;
		double wander_speed_limit = 5; 
		double getSpeedLimit(){return SPEED_LIMIT;}
		
		//keep a vector full of the goblins in the squad
		std::vector<Goblin*> goblins;
		//keep track of # of goblins
		//int goblinNum;
		//keep track of what squad is doing
		//0 = wander, 1 = chase, 2 = attack, 3 = flee
		int state = 0;
		//radius that the squad can detect the player, not using a cone just a circle 
		double sight = 80000;

				
		//method to spawn the squad, will be random 3-5
		void spawnSquad();
		
		//overarching method to move squad
		//Arrow* moveCharacter(std::vector<Player*> players, double map_height, double map_width, double timestep);
		Arrow* moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep);
		
		//method to tell goblins to wander
		void wander(double map_height, double map_width, double timestep);
		//method to flag if the squad can see the player
		void spotPlayer(Player* player);
		Player* targetPlayer=nullptr;
		bool playerSeen = false;
		
		//method to chase player
		void chase(int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep);
		//method to attack player once close enough
		void attack(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep);
		
		std::vector<SDL_Rect*> getCharacterRects(){return characterRects;}
		
};

#endif