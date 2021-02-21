
#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"
#include "Player.h"
#include "Arrow.h"
#include <utility>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <stack> 
#include "DTDecision.h"
#include "DTClose.h"
#include "DTAction.h"

// Plan to implement an enemy
// First, give it the ability to exist
// How?  Well it has an x and y coord, so in the game loop just make a vector of enemies and push this on it.  Then, check the representation like a player would.

// Then, give it the ability to move

// Then, make it move under its own volition

//struct to hold info for each tile/node - used in A* methods
struct tile
{
	//hold indicies of parent node
	int parent_row;
	int parent_col;
	//hold values for F = G + H
	double F;
	double G;
	double H;
};

class DTClose;	// forward declaration to deal with circular dependency with DTClose.h

class Enemy: public Character{
        public:
        using Character::Character;
		int PRESSURE_VALUE=32;
		int SPEED_LIMIT=200;
		static SDL_Texture* characterSheet;
		static std::vector<SDL_Rect*> characterRects;
		static std::vector<SDL_Texture*> sprites;
		
		//bool to mark if a goblin squad
		bool isSquad = false;
		virtual bool checkSquad(){ return isSquad; }
		//keep track of # of goblins - only used by goblin squad
		int goblinNum = 0;

		// Decision Tree Values
		bool treeBuilt = false;
		bool playerSeen = false;

		// Decision Tree Nodes
		// Action Nodes
		DTAction* chase;
		DTAction* idle;
		//Decision Nodes
		DTClose* playerClose;	// if player close enough, returns trueNode
		DTDecision* seen;	// checks if player has been seen yet


		DTDecision* root;	// set root = whatever node is root of tree for consistency
							// this way, functions in this class can invoke root->decide()

		DTAction* action;	// this will hold the value returned by root->decide()

		// Decision Tree Functions
		void buildTree(std::vector<Player*>*);
		DTAction* getAction(std::vector<Player*>*);
		virtual double getSpeedLimit(){return SPEED_LIMIT;}
        // This is a placeholder method just to differentiate the Enemy class vs the Character class, presumably this would be something that is handled on an enemy by enemy basis
        void attack();
		double damage=11;
		//virtual Arrow* moveCharacter(std::vector<Player*> players, double map_height, double map_width, double timestep);
		virtual Arrow* moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep);
		//enemy seek is to move an enemy from A -> B, must pass in destination coordinates, return true once there
		bool enemySeek(double xdest, double ydest, double map_height, double map_width, double timestep);
		bool enemyFollow(double xdest, double ydest, double map_height, double map_width, double timestep);
		//to accel enemy in a direction
		void enemyAccel(double x_dir, double y_dir, double timestep);
		//to update enemy image/dstrect
		virtual void updatePos(double x_vel, double y_vel, double map_height, double map_width, double timestep);
		//subtract health when hit
		void hit();
		bool enemyFlee(double xdest, double ydest, double map_height, double map_width, double timestep);
		void enemyAccelAway(double x_dir, double y_dir, double timestep);
		void facePlayer(double x, double y);
		//decide where the hitbox is depending on sprite in use
		void updateCollision();
		virtual SDL_Texture* getSpriteSheet(double timestep){
			int value=damageSprite(timestep);
            if(value<=0)//If value is less than 0 then we show the sprite
			    return characterSheet;
            else //If value is 1 then we don't show the sprite
                return nullptr;
		}
		virtual int getPressure() {return PRESSURE_VALUE;}
		
		//bool for if the enemy is blocking, will only be true if it's the knight
		bool isBlocking = false;
		//SDL_Rect* to hold a blocking box, again, only used by knight
		SDL_Rect* blockBox;
		
		
		//whole bunch of methods and variables for A* pathfinding
		void pathfind(double xdest, double ydest, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep);
		
		//create typedef for a pair of (int, int) - used for x/y location
		typedef std::pair<int, int> Pair;
		//create a pair of doubles to hold coordinates 
		typedef std::pair<double, double> pathPair;
		//create typedef for a pair of (double, pair(int, int0)) - used to hold F, row, col
		typedef std::pair<double, std::pair<int, int>> weight_pair; 

		//main a star search function
		void aStar(int** floorMap, int floorHeight, int floorWidth, Pair start, Pair destination);
		//will hold how many rows/cols are in the map, will be filled in aStar method, which occurs before this variable is used
		int maxRows = 0;
		int maxCols = 0;
		
		//a stack of pairs of x/y coordinates to traverse to in order to reach destination
		std::stack<pathPair> finalPath;
		
		//helper functions
		//check if a node is valid - ie row !< 0 || row !> map_rows
		bool isValid(int row, int col);
		//check if a node is floor and not wall/obstacle
		bool isFloor(int** floorMap, int row, int col, int floorWidth, int floorHeight);
		//check if the node in question is the destination
		bool isDestination(int row, int col, Pair destination);
		//calculate heuristic value - the Euclidean distance
		double findHeuristic(int row, int col, Pair destination);
		virtual std::vector<SDL_Rect*> getCharacterRects(){return characterRects;}
		
		virtual bool returnBlocking(){return isBlocking;}
		virtual SDL_Rect* returnBlockBox(){return blockBox;}
		//return the path from start to destination - in form of 2D array containing x/y coordinates to traverse to in order
		//double** returnPath(tile** tiles, Pair destination);		
		virtual double getDamage(){return damage;}
};





#endif
