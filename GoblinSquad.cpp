#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>  //for rand numbers
#include <cstdlib> //for abs
#include <cmath>
#include "GoblinSquad.h"


//spawn the entire set of goblins
void GoblinSquad::spawnSquad()
{
	//define where the bounding rectangle is for the squad, set size of rect manually
	boundingRect = {static_cast<int>(x_pos), static_cast<int>(y_pos), 175, 175}; //was 400x400 originally
	x_center = boundingRect.x+87.5; //was +200
	y_center = boundingRect.y+87.5; //was +200
	
	this->updateCollision();
	
	//set state to 0 to start
	state = 0;
	
	//number to spawn -> 0 to 2, and add 3 so we have 3 to 5
	goblinNum = rand() % 3 + 3; 

	//x and y coord arrays for goblin spawns - for 150x150 area
	double x_spawns [5];
	x_spawns[0] = x_pos + 0;
	x_spawns[1] = x_pos + 65;
	x_spawns[2] = x_pos + 124;
	x_spawns[3] = x_pos + 32;
	x_spawns[4] = x_pos + 110;
	double y_spawns [5];
	y_spawns[0] = y_pos + 57;
	y_spawns[1] = y_pos + 49;
	y_spawns[2] = y_pos + 52;
	y_spawns[3] = y_pos + 162;
	y_spawns[4] = y_pos + 158;
	

	//goblins.reserve(goblinNum);
	//for loop to create goblin objects
	for(int i = 0; i <= this->goblinNum-1 ; i++)
	{
		Goblin* goblin = new Goblin(x_spawns[i],y_spawns[i],44,40, 15);
		goblin->isGoblin = true;
		//add goblin onto the vector
		goblins.push_back(goblin);
	}
	
	
}

//move the squad according to their state
Arrow* GoblinSquad::moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep)
{	

	//for(int gob=0;gob<goblins.size();gob++){

		//if(goblins[gob]->health <= 0)
		//{
			/*
	int gob=;
	for(Goblin* g:goblins)
	{
		if(g->health <= 0)
		{
			goblins.erase(goblins.begin()+gob);
			goblinNum--;
		}
		gob++;
	}
	*/
	
	//if state == 0, look for player
	if(state == 0)
	{
		//if no target yet, see if there is a player close enough and within sight to become it
		if(targetPlayer==nullptr){
			//It will consistently target the same player once it selects one
			Player* closest;
			double closestDist=100000000;
			for(Player* p: players){
				double distance = pow((p->x_pos-x_pos), 2)+pow((p->y_pos-y_pos),2);
				if(distance<closestDist){
					closestDist=distance;
					closest=p;
				}

			}
			if(closest == nullptr)
				return nullptr;
			//try to spot player
			this->spotPlayer(closest);
			//if player is close enough to be spotted, set that player as the target 
			/*
			if(playerSeen)
			{
				//assign same target to all goblins
				for(Goblin* g:goblins)
				{
					g->targetPlayer=closest;
					g->facePlayer(g->targetPlayer->x_pos, g->targetPlayer->y_pos);
				}
			}
			*/
		}
	}
	//if state == 1, chase
	else if(state == 1)
	{
		//std::cout << "Here 1!";
		chase(floorMap, floorHeight, floorWidth, map_height, map_width, timestep);
	}
	//if state == 2, attack
	else if(state == 2)
		attack(players, floorMap, floorHeight, floorWidth, map_height, map_width, timestep);
	
	//update center of rectangle
	x_center = x_pos + 87.5;
	y_center = y_pos + 87.5;
	
	
	//debug
	//std::printf("x_center = %lf, y_center = %lf \n", this->x_center, this->y_center);

	return nullptr;
}


//determine if squad can see player - need to adjust later so they cannot see through the walls
void GoblinSquad::spotPlayer(Player* player)
{
	//debug
	//std::printf("%lf\n", std::sqrt((std::pow((x_player - x_center), 2) + std::pow((y_player - y_center), 2))));
	
	//take x and y of closest player
	double x_player = player->x_pos;
	double y_player = player->y_pos;
	
	//using sqrt(a^2 + b^2) determine if distance to player is within sight
	if((std::pow((x_player - x_center), 2) + std::pow((y_player - y_center), 2)) <= sight)
	{
		state = 1; //if true, chase/attack player
		this->playerSeen = true;
		this->targetPlayer = player;
		//assign same target to all goblins
		for(Goblin* g:goblins)
		{
			g->targetPlayer=player;
			g->facePlayer(g->targetPlayer->x_pos, g->targetPlayer->y_pos);
		}
	}
}

//method to have squad go after the player
void GoblinSquad::chase(int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep)
{	

	if(this->targetPlayer == nullptr)
		return;
	//std::cout << "Here 2!";
	//take x and y of closest player
	double x_player = targetPlayer->x_pos+30;
	double y_player = targetPlayer->y_pos+30;
	
	//std::cout << "Here!";
	//go after player, moving entire bounding box along with the goblins
	//bounding box first - sub 200 for x_player and y_player so the bounding box will center on the player
	//enemySeek(x_player-100, y_player-100, map_height, map_width, timestep);
	pathfind(x_player-75, y_player-75, floorMap, floorHeight, floorWidth, map_height, map_width, timestep); //-75 for 1/2 size of the rectangle 
	//make sure there isnt a hitbox for the overall squad
	this->updateCollision();
	
	//change this speed
	this->SPEED_LIMIT = 195;
	
	//go through goblin vector and move them all, just match velocities to the bounding box's
	for(Goblin* g:goblins)
	{
		if(g->health > 0)
		{
		//rand values for offsetting speed slightly to give each goblin a better feel of being their own entity
		//double speedOffset = rand() % 20;
		
		//CHANGE THIS SO THAT THE GOBLINS USE A* SEARCH TO GET TO THEIR RESPECTIVE SPAWN POINTS IN SPOTS OF THE BOUNDING BOX
		//MAKES IT SO IF THE PLAYER RUNS INTO A HALLWAY THE GOBLINS WONT GET STUCK - PROBABLY
		
		//lower speed limit for each goblin, had weird behavior when it was higher
		g->SPEED_LIMIT = 195;
		g->pathfind(x_center, y_center, floorMap, floorHeight, floorWidth, map_height, map_width, timestep);
		
		//update goblin orientation and position
		//g->updatePos(g->x_vel, g->y_vel, map_height, map_width, timestep);
		g->facePlayer(x_player, y_player);
		}
	}
	
	//check if player is close to the center of bounding box, if so, sick the dogs
    if(std::sqrt((std::pow((x_player - x_center), 2) + std::pow((y_player - y_center), 2))) <= 100)
		state = 2; //attack state
	
}

//method to have squad attack the player - will trigger once the boundingRect collides with the player
void GoblinSquad::attack(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep)
{

	
	//keep bounding box centered on the player
	//bounding box first - sub 75 for x_player and y_player so the bounding box will center on the player
	//don't care if outside box goes outside of the map, as it is not seen so use 9999999999 instead of map dimensions
	//will still have issues at x < 0 or y > 0 I think
	pathfind(targetPlayer->x_pos-87.5, targetPlayer->y_pos-87.5, floorMap, floorHeight, floorWidth, map_height, map_width, timestep);
	//make sure there isnt a hitbox for the overall squad
	this->updateCollision();
	
	//change this speed
	this->SPEED_LIMIT = 180;
	
	//go through goblin vector and move them all towards the player
	for(Goblin* g:goblins)
	{
		if(g->health > 0)
		{
			g->SPEED_LIMIT = 180;
			//UPDATE TO A* SEARCH TO PLAYER POSITION
			
			//update goblin orientation and position
			//+40 and +50 for offsets to stay locked onto player better
			g->pathfind(targetPlayer->x_pos+20, targetPlayer->y_pos+50, floorMap, floorHeight, floorWidth, map_height, map_width, timestep); 
			g->facePlayer(targetPlayer->x_pos, targetPlayer->y_pos);
			//if goblin 100px from player or less, attack
			if((std::pow((targetPlayer->x_pos - g->x_pos), 2) + std::pow((targetPlayer->y_pos - g->y_pos), 2)) <= 10000 && g->targetPlayer != nullptr)
				g->attack(players);
		}
	}

}

//need this to avoid weird behaviors
void GoblinSquad::updateCollision()
{
	characterTree->root->hitbox.x = 0;//+24;
	characterTree->root->hitbox.y = 0;
	characterTree->root->hitbox.w = 0;
	characterTree->root->hitbox.h = 0;
}





