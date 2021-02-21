#include "Enemy.h"
#include <cmath>
#include <iostream>
#include <utility>
#include <set>
#include <stack> 
#include "DTAction.h"

void Enemy::buildTree(std::vector<Player*>* players) {
	chase = new DTAction();
	idle = new DTAction();

	playerClose = new DTClose(this->chase, this->idle, players);
	playerClose->setEnemy(this);

	seen = new DTDecision(chase, playerClose);
	seen->setBool(&playerSeen);

	root = this->seen;
}
DTAction* Enemy::getAction(std::vector<Player*>* players) {
	if (!treeBuilt) {
		this->buildTree(players);
		treeBuilt = true;
	}

	action = (DTAction*) this->root->decide();
	if (action == chase)
		playerSeen = true;
	return action;
}

SDL_Texture* Enemy::characterSheet;
std::vector<SDL_Rect*> Enemy::characterRects;

void Enemy::attack() { return; }

/*

AI Structure for grunt
	while player is not in LOS, do nothing
	when player enters LOS for the first time, pathfind to player until dead


*/

//Arrow* Enemy::moveCharacter(std::vector<Player*> players, double map_height, double map_width, double timestep) {return nullptr;}
//will need more map info for moving characters, need dimensions for A* pathfinding
Arrow* Enemy::moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep) {
	Player* closest;
	double dist=10000000000;
	//players.pop_back();
	for(Player* p:players){
		double tempdist=std::pow(p->y_pos-y_pos, 2)+std::pow(p->x_pos-x_pos, 2);
		if(tempdist<=dist){
			dist=tempdist;
			closest=p;
		}
	}
	
	DTAction* a = getAction(&players);
	if (a == chase)
		pathfind(closest->x_pos, closest->y_pos, floorMap, floorHeight, floorWidth, map_height, map_width, timestep);
	else if (a != idle)
		std::cout << "Decision Tree returned bad value!" << std::endl;
	


	SDL_Rect* attackBox=new SDL_Rect{static_cast<int>(this->x_pos), static_cast<int>(this->y_pos), x_size, y_size};
	for(Player* p:players){
		SDL_Rect result;
		if(SDL_IntersectRect(&p->characterTree->root->hitbox, attackBox, &result)){
			p->damageCharacter(getDamage());
		}
	}
	return nullptr;
	
	}

//method to move enemy from present position -> destination
bool Enemy::enemySeek(double xdest, double ydest, double map_height, double map_width, double timestep)
{
	//define x and y directions to travel in
	double x_dir = xdest - x_pos;
	double y_dir = ydest - y_pos;
	
	//if there, reset x/y velocities and deltav and return true
	if((int) x_dir == 0 && (int) y_dir == 0)
	{
		x_vel = 0;
		y_vel = 0;
		x_deltav = 0;
		y_deltav = 0;
		//update position
		updatePos(x_vel, y_vel, map_height, map_width, timestep);
		return true;
	}

	//call on enemyAccel to move enemy
	this->enemyAccel(x_dir, y_dir, timestep);
	
	//update enemy image/position
	updatePos(x_vel, y_vel, map_height, map_width, timestep);
	
	return false;
}

//method to move enemy based on A* pathfinding
void Enemy::pathfind(double xdest, double ydest, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep)
{
	//create a pair for both the start point of our enemy and destination of our enemy, given in tiles rather than coordinates
	Pair enemyStart;
	enemyStart.first = (x_pos /100);
	enemyStart.second = (y_pos /100);
	Pair enemyDest;
	enemyDest.first = ((xdest+20) /100);
	enemyDest.second = ((ydest+20) /100);
	
	//if(!isFloor(floorMap, enemyDest.first, enemyDest.second))
	//	std::cout << "Invalid x = " << xdest << " Invalid y = " << ydest << "\n";
	
	//call on A*
	aStar(floorMap, floorHeight, floorWidth, enemyStart, enemyDest);
	
	//now seek until destination is reached
	Pair seekTo;
	if(!finalPath.empty())
		seekTo = finalPath.top();
	else
	{
		seekTo.first = this->x_pos;
		seekTo.second = this->y_pos;
	}
	
	enemySeek(seekTo.first, seekTo.second, map_height, map_width, timestep);
}

//method to follow player
bool Enemy::enemyFollow(double xdest, double ydest, double map_height, double map_width, double timestep)
{
	//define x and y directions to travel in
	double x_dir = xdest - x_pos;
	double y_dir = ydest - y_pos;
	
	//if there, reset x/y velocities and deltav and return true, eventually make it so if collision = true, do something
	if(((x_dir <= 15 && x_dir > 0) || (x_dir >= -15 && x_dir < 0))  && ((y_dir <= 15 && y_dir > 0) || (y_dir >= -15 && y_dir < 0)))
	{
		x_vel = 0;
		y_vel = 0;
		x_deltav = 0;
		y_deltav = 0;
		return true;
	}
	
	//call on enemyAccel to move enemy
	this->enemyAccel(x_dir, y_dir, timestep);
	
	//update enemy image/position
	updatePos(x_vel, y_vel, map_height, map_width, timestep);
	
	return false;	
}

bool Enemy::enemyFlee(double xdest, double ydest, double map_height, double map_width, double timestep)
{
	//define x and y directions to travel in
	double x_dir = xdest - x_pos;
	double y_dir = ydest - y_pos;
	
	//call on enemyAccel to move enemy
	this->enemyAccelAway(x_dir, y_dir, timestep);
	
	//update enemy image/position
	updatePos(x_vel, y_vel, map_height, map_width, timestep);
	
	return false;	
}

//method to accelerate enemy towards the character
void Enemy::enemyAccel(double x_dir, double y_dir, double timestep)
{
	//set x and y deltav to 0
	x_deltav = 0;
	y_deltav = 0;
	//check if we should move in the positive or negative x direction
	if((int) x_dir < 0)
		x_deltav -= (ACCEL * timestep);
	else if((int) x_dir > 0)
		x_deltav += (ACCEL * timestep);
	if((int) y_dir < 0)
		y_deltav -= (ACCEL * timestep);
	else if((int) y_dir > 0)
		y_deltav += (ACCEL * timestep);
	
	//check if delta_v = 0 (reached x/y destination), if so decrement velocity
	//first checking x_deltav
	if(x_deltav == 0)
	{
		if(x_vel > 0){
			if(x_vel < (ACCEL * timestep))
				x_vel = 0;
			else
				x_vel -= (ACCEL * timestep);
		}
		else if(x_vel < 0){
			if(-x_vel < (ACCEL * timestep)){
				x_vel = 0;
			}
			else {
				x_vel  += (ACCEL * timestep);
			}
		}
	}
	else
		x_vel += x_deltav;
	//now checking y_deltav
	if(y_deltav == 0){
	{
		if(y_vel > 0){
			if(y_vel < (ACCEL * timestep))
				y_vel = 0;
			else {
				y_vel -= (ACCEL * timestep);
			}
		}
		else if(y_vel < 0){
			if(-y_vel < (ACCEL * timestep))
				y_vel = 0;
			else {
				y_vel  += (ACCEL * timestep);
			}
		}
	}
}
	else {
		y_vel += y_deltav;
	}
		
	
	//check if speed limit is exceeded
	//x direction
	if(x_vel < -getSpeedLimit())
		x_vel = -getSpeedLimit();
	else if(x_vel > getSpeedLimit())
		x_vel = getSpeedLimit();
	//y direction
	if(y_vel < -getSpeedLimit())
		y_vel = -getSpeedLimit();
	else if(y_vel > getSpeedLimit())
		y_vel = getSpeedLimit();

	return;
}

//method to accelerate the enemy away from the player
void Enemy::enemyAccelAway(double x_dir, double y_dir, double timestep)
{
	//set x and y deltav to 0
	x_deltav = 0;
	y_deltav = 0;
	//check if we should move in the positive or negative x direction
	
	if((int) x_dir < 0)
		x_deltav += (ACCEL * timestep);
	else if((int) x_dir > 0)
		x_deltav -= (ACCEL * timestep);
	if((int) y_dir < 0)
		y_deltav += (ACCEL * timestep);
	else if((int) y_dir > 0)
		y_deltav -= (ACCEL * timestep);
	

	x_vel += x_deltav;

	y_vel += y_deltav;
		
	
	//check if speed limit is exceeded
	//x direction
	if(x_vel < -getSpeedLimit())
		x_vel = -getSpeedLimit();
	else if(x_vel > getSpeedLimit())
		x_vel = getSpeedLimit();
	//y direction
	if(y_vel < -getSpeedLimit())
		y_vel = -getSpeedLimit();
	else if(y_vel > getSpeedLimit())
		y_vel = getSpeedLimit();

	return;
}


//method to update enemy x/y positioning
void Enemy::updatePos(double x_vel, double y_vel, double map_height, double map_width, double timestep)
{
	setSpriteAndDirection();
	// move the sdl_rect
	y_pos += (y_vel * timestep);
	if (y_pos < 0)
		y_pos = 0;
	else if(y_pos + y_size > map_height)
		y_pos = map_height - y_size;
	x_pos += (x_vel * timestep);
	if (x_pos < 0)
		x_pos = 0;
	else if(x_pos + x_size > map_width)
		x_pos = map_width - x_size;
	
	//update collision boxes
	updateCollision();
}


//subtract health when hit
void Enemy::hit()
{	
	health-=10;
}

//Face the player
void Enemy::facePlayer(double x, double y){
    
	//Check if the x/y difference is greater
	if(std::abs(x-x_pos) >= std::abs(y-y_pos)){

		//To the left of the player
		if(x_pos > x){
			spriteIndex=2;
			flip = SDL_FLIP_NONE;
		}
		//To the right of the player
		else if(x_pos < x){
			spriteIndex=2;
			flip = SDL_FLIP_HORIZONTAL;
		}
	}
	else{

		//Underneath the player
		if(y_pos > y){
			spriteIndex=1;
			flip = SDL_FLIP_NONE;
		}
		//Above the player
		else if(y_pos < y){
			spriteIndex=0;
			flip = SDL_FLIP_NONE;
		}
	}



}

//update collision box for enemy - tailored only for skeleton enemy right now
void Enemy::updateCollision()
{
	//using sprite indexes to tell what hitboxes to use for now, once sprite sheets are in use & animations improved can implement a direction variable
	
	//if front or back facing
	//if(spriteIndex <= 1)
	//{
		characterTree->root->hitbox.x = x_pos;//+24;
		characterTree->root->hitbox.y = y_pos+13;
		characterTree->root->hitbox.w = 31;
		characterTree->root->hitbox.h = 86;
	//}
	/*
	//if left facing
	else if(spriteIndex == 2)
	{
		characterTree->root->hitbox.x = x_pos;//+25;
		characterTree->root->hitbox.y = y_pos;//+13;
		characterTree->root->hitbox.w = 29;
		characterTree->root->hitbox.h = 86;
	}
	//if right facing
	else if(spriteIndex == 3)
	{
		characterTree->root->hitbox.x = x_pos;//+25;
		characterTree->root->hitbox.y = y_pos;//+13;
		characterTree->root->hitbox.w = 29;
		characterTree->root->hitbox.h = 86;
	}
	*/
}


//*************************A* METHODS BEGIN HERE *************************	
//create typedef for a pair of (int, int) - used for x/y location
//typedef std::pair<int, int> Pair;
//create typedef for a pair of (double, pair(int, int0) - used to hold F = G + H weights
//typedef std::pair<double, pair<int, int>> weight_pair; 

//function for determining if a tile is valid or not
bool Enemy::isValid(int row, int col)
{
	//returns true if the row and column number are in a valid range, otherwise false
	return(row >= 0 && row < maxRows && col >= 0 && col < maxCols);
}

//function to check if a node is traversable floor and not a wall/obstacle
bool Enemy::isFloor(int** floorMap, int row, int col, int floorWidth, int floorHeight)
{
	 //check if tile is not blocked by a wall/obstacle, if so return true

	if(row >= floorHeight || col >= floorWidth){
		return false;
	}

	 if(floorMap[row][col] != 0)
		 return true;
	 else 
		 return false;
}
 
//function to check if the node being looked at is the destination
bool Enemy::isDestination(int row, int col, Pair destination)
{
	//compare row and col values to the destination pair values
	if(row == destination.first && col == destination.second)
		return true; //node is the destination
	else
		return false; //not at destination
}
 
//function to calculate the heuristic value - Euclidean distance
double Enemy::findHeuristic(int row, int col, Pair destination)
{
	//use Euclidean distance formula and return the result - do not need sqrt
	return(static_cast<double> ((destination.first - row) * (destination.first - row) + (destination.second - col) * (destination.second - col)));
}
 
//main A* search method 
void Enemy::aStar(int** floorMap,  int floorHeight, int floorWidth, Pair start, Pair destination)
{
	//vaiable to hold a "max" weight for nodes upon initization
	long maxWeight = 999999999999999999;
	
	//set max row and col values
	maxRows = floorHeight;//sizeof(*floorMap) / sizeof(floorMap[0]);
	maxCols = floorWidth;//sizeof(**floorMap) / sizeof(floorMap[0][0]);
	 
	
	//debug - print info
	//std::cout << "Start x = " << start.first << " Start y = " << start.second << "\n";
	//std::cout << "destination x = " << destination.first << " destination y = " << destination.second << "\n"; 
	//std::cout << "maxRows = " << maxRows << " maxCols = " << maxCols << "\n"; 
	 
	//check if start or end points are invalid first
	if(isValid(start.first, start.second) == false)
	{
		//debug
		//std::cout << "Invalid start\n" ;
		return;
	}
	if(isValid(destination.first, destination.second) == false)
	{
		//debug
		//std::cout << "Invalid destination\n" ;
		return;
	}
	//make sure the start and destination are both floor spaces
	if(!(isFloor(floorMap, start.first, start.second, floorWidth, floorHeight)))
	{
		//std::cout << "Start not floor\n";
		return;
	}
	if(!(isFloor(floorMap, destination.first, destination.second, floorWidth, floorHeight)))
	{
		//std::cout << "Destination not floor\n";
		return;
	}
	//if destination is source, return
	if(isDestination(start.first, start.second, destination))
	{
		//std::cout << "Already at destination\n";
		return;
	}
	
	//create a closed list for nodes and set all to false initially
	bool closedList[maxRows][maxCols];
	memset(closedList, false, sizeof(closedList));
	
	//create a 2D array to hold details of tiles 
	tile tileInfo[maxRows][maxCols];
	
	//default the values of each index to a "max" number, and set parent info to -1 to mean "unvisited"
	for(int i=0; i < maxRows; i++)
	{
		for(int j=0; j < maxCols; j++)
		{
			//setting F = G + H values to a max
			tileInfo[i][j].F = static_cast<double>(maxWeight);
			tileInfo[i][j].G = static_cast<double>(maxWeight);
			tileInfo[i][j].H = static_cast<double>(maxWeight);
			//setting parent locations to -1
			tileInfo[i][j].parent_row = -1;
			tileInfo[i][j].parent_col = -1;
		}
	}
	
	//initialize info for starting node
	tileInfo[start.first][start.second].F = 0.0;
	tileInfo[start.first][start.second].G = 0.0;
	tileInfo[start.first][start.second].H = 0.0;
	tileInfo[start.first][start.second].parent_row = start.first;
	tileInfo[start.first][start.second].parent_col = start.second;
	
	//create an open list of nodes to consider adding to the path
	//will be a set of form <F, <row, col>>, where F = G + H is cost to consider
	std::set<weight_pair> openList;
	
	//place starting tile in the openList, setting its cost F to 0
	openList.insert(std::make_pair(0.0, std::make_pair(start.first, start.second)));
	
	//create a bool to hold if destination was reached
	bool destReached = false;
	
	//ints to hold row/col within the search
	int row;
	int col;
	
	//main loop to traverse through the map/openList
	while(!openList.empty())
	{
		//define a weight_pair to hold values for a node being examined - set it to front of openList set
		weight_pair curr = *openList.begin();
		
		//remove this node from the open list
		openList.erase(openList.begin());
		
		//add the node to the closedList since it is being processed
		row = curr.second.first;
		col = curr.second.second;
		closedList[row][col] = true; //mark as processed within the closed list
		
		//debug
		//std::cout << "row = " << row << " col = " << col << "\n"; 
		
		//create variables to hold values for F, G, H - used to calculate cost F = G + H
		double nodeF;
		double nodeG;
		double nodeH;
		
		//------------------PROCESS ALL NEIGHBORING TILES - 8 TILES------------------
		
		//NORTH
		//examine node directly above current one
		//must first see if it is valid
		if(isValid(row-1, col) == true)
		{
			//check if it is the destination
			if(isDestination(row-1, col, destination) == true)
			{
				//set parent info of the destination node/tile
				tileInfo[row-1][col].parent_row = row;
				tileInfo[row-1][col].parent_col = col;
				//mark destination as reached and break to create the optimal path in x/y coords
				destReached = true;
				break;
				//since destination is found in terms of tiles, call on return path to translate into x/y coordinates, then return result
				//return returnPath(tileInfo, destination);
			}
			//if not the destination, check if it's on the closed list or if it's a wall- if not then analyze its values
			else if(closedList[row-1][col] == false && isFloor(floorMap, row-1, col, floorWidth, floorHeight) == true)
			{
				//set the values for F,G, and H of the tile
				nodeG =  tileInfo[row][col].G + 1.0; //cost to reach examined tile is cost to reach parent's + 1.0
				nodeH = findHeuristic(row-1, col, destination); //find heuristic for this tile examined
				nodeF = nodeG + nodeH; //calculate total cost F to reach the tile
				
				//now add it to the open list if not already done & make curr its parent
				//or if it is on the open list already, see if the path examined now is better in terms of cost F
				if(tileInfo[row-1][col].F == static_cast<double>(maxWeight) || tileInfo[row-1][col].F > nodeF)
				{
					//adding to openList, if not already there
					openList.insert(std::make_pair(nodeF, std::make_pair(row-1, col)));
					
					//updating info of this node within the openList, if already in openList, updates existing item
					tileInfo[row-1][col].F = nodeF;
					tileInfo[row-1][col].G = nodeG;
					tileInfo[row-1][col].H = nodeH;
					tileInfo[row-1][col].parent_row = row;
					tileInfo[row-1][col].parent_col = col;
				}
			
			}			
			
		}
		
		//SOUTH
		//examine node directly below current one
		//must first see if it is valid
		if(isValid(row+1, col) == true)
		{
			//check if it is the destination
			if(isDestination(row+1, col, destination) == true)
			{
				//set parent info of the destination node/tile
				tileInfo[row+1][col].parent_row = row;
				tileInfo[row+1][col].parent_col = col;
				//mark destination as reached and break to create the optimal path in x/y coords
				destReached = true;
				break;
				//since destination is found in terms of tiles, call on return path to translate into x/y coordinates, then return result
				//return returnPath(tileInfo, destination);
			}
			//if not the destination, check if it's on the closed list or if it's a wall- if not then analyze its values
			else if(closedList[row+1][col] == false && isFloor(floorMap, row+1, col, floorWidth, floorHeight) == true)
			{
				//set the values for F,G, and H of the tile
				nodeG =  tileInfo[row][col].G + 1.0; //cost to reach examined tile is cost to reach parent's + 1.0
				nodeH = findHeuristic(row+1, col, destination); //find heuristic for this tile examined
				nodeF = nodeG + nodeH; //calculate total cost F to reach the tile
				
				//now add it to the open list if not already done & make curr its parent
				//or if it is on the open list already, see if the path examined now is better in terms of cost F
				if(tileInfo[row+1][col].F == static_cast<double>(maxWeight) || tileInfo[row+1][col].F > nodeF)
				{
					//adding to openList, if not already there
					openList.insert(std::make_pair(nodeF, std::make_pair(row+1, col)));
					
					//updating info of this node within the openList, if already in openList, updates existing item
					tileInfo[row+1][col].F = nodeF;
					tileInfo[row+1][col].G = nodeG;
					tileInfo[row+1][col].H = nodeH;
					tileInfo[row+1][col].parent_row = row;
					tileInfo[row+1][col].parent_col = col;
				}
			
			}			
			
		}
		
		//EAST
		//examine node directly to the right current one
		//must first see if it is valid
		if(isValid(row, col+1) == true)
		{
			//check if it is the destination
			if(isDestination(row, col+1, destination) == true)
			{
				//set parent info of the destination node/tile
				tileInfo[row][col+1].parent_row = row;
				tileInfo[row][col+1].parent_col = col;
				//mark destination as reached and break to create the optimal path in x/y coords
				destReached = true;
				break;
				//since destination is found in terms of tiles, call on return path to translate into x/y coordinates, then return result
				//return returnPath(tileInfo, destination);
			}
			//if not the destination, check if it's on the closed list or if it's a wall- if not then analyze its values
			else if(closedList[row][col+1] == false && isFloor(floorMap, row, col+1, floorWidth, floorHeight) == true)
			{
				//set the values for F,G, and H of the tile
				nodeG =  tileInfo[row][col].G + 1.0; //cost to reach examined tile is cost to reach parent's + 1.0
				nodeH = findHeuristic(row, col+1, destination); //find heuristic for this tile examined
				nodeF = nodeG + nodeH; //calculate total cost F to reach the tile
				
				//now add it to the open list if not already done & make curr its parent
				//or if it is on the open list already, see if the path examined now is better in terms of cost F
				if(tileInfo[row][col+1].F == static_cast<double>(maxWeight) || tileInfo[row][col+1].F > nodeF)
				{
					//adding to openList, if not already there
					openList.insert(std::make_pair(nodeF, std::make_pair(row, col+1)));
					
					//updating info of this node within the openList, if already in openList, updates existing item
					tileInfo[row][col+1].F = nodeF;
					tileInfo[row][col+1].G = nodeG;
					tileInfo[row][col+1].H = nodeH;
					tileInfo[row][col+1].parent_row = row;
					tileInfo[row][col+1].parent_col = col;
				}
			
			}			
			
		}
		
		//WEST
		//examine node directly to the left current one
		//must first see if it is valid
		if(isValid(row, col-1) == true)
		{
			//check if it is the destination
			if(isDestination(row, col-1, destination) == true)
			{
				//set parent info of the destination node/tile
				tileInfo[row][col-1].parent_row = row;
				tileInfo[row][col-1].parent_col = col;
				//mark destination as reached and break to create the optimal path in x/y coords
				destReached = true;
				break;
				//since destination is found in terms of tiles, call on return path to translate into x/y coordinates, then return result
				//return returnPath(tileInfo, destination);
			}
			//if not the destination, check if it's on the closed list or if it's a wall- if not then analyze its values
			else if(closedList[row][col-1] == false && isFloor(floorMap, row, col-1, floorWidth, floorHeight) == true)
			{
				//set the values for F,G, and H of the tile
				nodeG =  tileInfo[row][col].G + 1.0; //cost to reach examined tile is cost to reach parent's + 1.0
				nodeH = findHeuristic(row, col-1, destination); //find heuristic for this tile examined
				nodeF = nodeG + nodeH; //calculate total cost F to reach the tile
				
				//now add it to the open list if not already done & make curr its parent
				//or if it is on the open list already, see if the path examined now is better in terms of cost F
				if(tileInfo[row][col-1].F == static_cast<double>(maxWeight) || tileInfo[row][col-1].F > nodeF)
				{
					//adding to openList, if not already there
					openList.insert(std::make_pair(nodeF, std::make_pair(row, col-1)));
					
					//updating info of this node within the openList, if already in openList, updates existing item
					tileInfo[row][col-1].F = nodeF;
					tileInfo[row][col-1].G = nodeG;
					tileInfo[row][col-1].H = nodeH;
					tileInfo[row][col-1].parent_row = row;
					tileInfo[row][col-1].parent_col = col;
				}
			
			}			
			
		}
		
		//NORTH - EAST
		//examine node directly above and to the right of current one
		//must first see if it is valid
		if(isValid(row-1, col+1) == true)
		{
			//check if it is the destination
			if(isDestination(row-1, col+1, destination) == true)
			{
				//set parent info of the destination node/tile
				tileInfo[row-1][col+1].parent_row = row;
				tileInfo[row-1][col+1].parent_col = col;
				//mark destination as reached and break to create the optimal path in x/y coords
				destReached = true;
				break;
				//since destination is found in terms of tiles, call on return path to translate into x/y coordinates, then return result
				//return returnPath(tileInfo, destination);
			}
			//if not the destination, check if it's on the closed list or if it's a wall- if not then analyze its values
			else if(closedList[row-1][col+1] == false && isFloor(floorMap, row-1, col+1, floorWidth, floorHeight) == true && isFloor(floorMap, row, col+1, floorWidth, floorHeight) == true && isFloor(floorMap, row-1, col, floorWidth, floorHeight) == true)
			{
				//set the values for F,G, and H of the tile
				nodeG =  tileInfo[row][col].G + 1.0; //cost to reach examined tile is cost to reach parent's + 1.0
				nodeH = findHeuristic(row-1, col+1, destination); //find heuristic for this tile examined
				nodeF = nodeG + nodeH; //calculate total cost F to reach the tile
				
				//now add it to the open list if not already done & make curr its parent
				//or if it is on the open list already, see if the path examined now is better in terms of cost F
				if(tileInfo[row][col-1].F == static_cast<double>(maxWeight) || tileInfo[row][col-1].F > nodeF)
				if(tileInfo[row-1][col+1].F == static_cast<double>(maxWeight) || tileInfo[row-1][col+1].F > nodeF)
				{
					//adding to openList, if not already there
					openList.insert(std::make_pair(nodeF, std::make_pair(row-1, col+1)));
					
					//updating info of this node within the openList, if already in openList, updates existing item
					tileInfo[row-1][col+1].F = nodeF;
					tileInfo[row-1][col+1].G = nodeG;
					tileInfo[row-1][col+1].H = nodeH;
					tileInfo[row-1][col+1].parent_row = row;
					tileInfo[row-1][col+1].parent_col = col;
				}
			
			}			
			
		}
		
		//NORTH - WEST
		//examine node directly above and to the left of current one
		//must first see if it is valid
		if(isValid(row-1, col-1) == true)
		{
			//check if it is the destination
			if(isDestination(row-1, col-1, destination) == true)
			{
				//set parent info of the destination node/tile
				tileInfo[row-1][col-1].parent_row = row;
				tileInfo[row-1][col-1].parent_col = col;
				//mark destination as reached and break to create the optimal path in x/y coords
				destReached = true;
				break;
				//since destination is found in terms of tiles, call on return path to translate into x/y coordinates, then return result
				//return returnPath(tileInfo, destination);
			}
			//if not the destination, check if it's on the closed list or if it's a wall- if not then analyze its values
			else if(closedList[row-1][col-1] == false && isFloor(floorMap, row-1, col-1, floorWidth, floorHeight) == true && isFloor(floorMap, row, col-1, floorWidth, floorHeight) == true && isFloor(floorMap, row-1, col, floorWidth, floorHeight) == true)
			{
				//set the values for F,G, and H of the tile
				nodeG =  tileInfo[row][col].G + 1.0; //cost to reach examined tile is cost to reach parent's + 1.0
				nodeH = findHeuristic(row-1, col-1, destination); //find heuristic for this tile examined
				nodeF = nodeG + nodeH; //calculate total cost F to reach the tile
				
				//now add it to the open list if not already done & make curr its parent
				//or if it is on the open list already, see if the path examined now is better in terms of cost F
				if(tileInfo[row-1][col-1].F == static_cast<double>(maxWeight) || tileInfo[row-1][col-1].F > nodeF)
				{
					//adding to openList, if not already there
					openList.insert(std::make_pair(nodeF, std::make_pair(row-1, col-1)));
					
					//updating info of this node within the openList, if already in openList, updates existing item
					tileInfo[row-1][col-1].F = nodeF;
					tileInfo[row-1][col-1].G = nodeG;
					tileInfo[row-1][col-1].H = nodeH;
					tileInfo[row-1][col-1].parent_row = row;
					tileInfo[row-1][col-1].parent_col = col;
				}
			
			}			
			
		}
		
		//SOUTH - EAST
		//examine node directly below and to the right of current one
		//must first see if it is valid
		if(isValid(row+1, col+1) == true)
		{
			//check if it is the destination
			if(isDestination(row+1, col+1, destination) == true)
			{
				//set parent info of the destination node/tile
				tileInfo[row+1][col+1].parent_row = row;
				tileInfo[row+1][col+1].parent_col = col;
				//mark destination as reached and break to create the optimal path in x/y coords
				destReached = true;
				break;
				//since destination is found in terms of tiles, call on return path to translate into x/y coordinates, then return result
				//return returnPath(tileInfo, destination);
			}
			//if not the destination, check if it's on the closed list or if it's a wall- if not then analyze its values
			else if(closedList[row+1][col+1] == false && isFloor(floorMap, row+1, col+1, floorWidth, floorHeight) == true && isFloor(floorMap, row, col+1, floorWidth, floorHeight) == true && isFloor(floorMap, row+1, col, floorWidth, floorHeight) == true)
			{
				//set the values for F,G, and H of the tile
				nodeG =  tileInfo[row][col].G + 1.0; //cost to reach examined tile is cost to reach parent's + 1.0
				nodeH = findHeuristic(row+1, col+1, destination); //find heuristic for this tile examined
				nodeF = nodeG + nodeH; //calculate total cost F to reach the tile
				
				//now add it to the open list if not already done & make curr its parent
				//or if it is on the open list already, see if the path examined now is better in terms of cost F
				if(tileInfo[row+1][col+1].F == static_cast<double>(maxWeight) || tileInfo[row+1][col+1].F > nodeF)
				{
					//adding to openList, if not already there
					openList.insert(std::make_pair(nodeF, std::make_pair(row+1, col+1)));
					
					//updating info of this node within the openList, if already in openList, updates existing item
					tileInfo[row+1][col+1].F = nodeF;
					tileInfo[row+1][col+1].G = nodeG;
					tileInfo[row+1][col+1].H = nodeH;
					tileInfo[row+1][col+1].parent_row = row;
					tileInfo[row+1][col+1].parent_col = col;
				}
			
			}			
			
		}
		
		//SOUTH - WEST
		//examine node directly below and to the left of current one
		//must first see if it is valid
		if(isValid(row+1, col-1) == true)
		{
			//check if it is the destination
			if(isDestination(row+1, col-1, destination) == true)
			{
				//set parent info of the destination node/tile
				tileInfo[row+1][col-1].parent_row = row;
				tileInfo[row+1][col-1].parent_col = col;
				//mark destination as reached and break to create the optimal path in x/y coords
				destReached = true;
				break;
				//since destination is found in terms of tiles, call on return path to translate into x/y coordinates, then return result
				//return returnPath(tileInfo, destination);
			}
			//if not the destination, check if it's on the closed list or if it's a wall- if not then analyze its values
			else if(closedList[row+1][col-1] == false && isFloor(floorMap, row+1, col-1, floorWidth, floorHeight) == true && isFloor(floorMap, row, col-1, floorWidth, floorHeight) == true && isFloor(floorMap, row+1, col, floorWidth, floorHeight) == true)
			{
				//set the values for F,G, and H of the tile
				nodeG =  tileInfo[row][col].G + 1.0; //cost to reach examined tile is cost to reach parent's + 1.0
				nodeH = findHeuristic(row+1, col-1, destination); //find heuristic for this tile examined
				nodeF = nodeG + nodeH; //calculate total cost F to reach the tile
				
				//now add it to the open list if not already done & make curr its parent
				//or if it is on the open list already, see if the path examined now is better in terms of cost F
				if(tileInfo[row+1][col-1].F == static_cast<double>(maxWeight) || tileInfo[row+1][col-1].F > nodeF)
				{
					//adding to openList, if not already there
					openList.insert(std::make_pair(nodeF, std::make_pair(row+1, col-1)));
					
					//updating info of this node within the openList, if already in openList, updates existing item
					tileInfo[row+1][col-1].F = nodeF;
					tileInfo[row+1][col-1].G = nodeG;
					tileInfo[row+1][col-1].H = nodeH;
					tileInfo[row+1][col-1].parent_row = row;
					tileInfo[row+1][col-1].parent_col = col;
				}
			
			}			
			
		}
		
		
	}
	
	//a stack of pairs of x/y coordinates to traverse to in order to reach destination
	//std::stack<pathPair> finalPath;
	
	//ensures finalPath stack is empty
	while(finalPath.empty() == false)
	{
		finalPath.pop();
	}
	
	//if the destination has been reached, create a 2d array of double values, translating the tiles to x/y coordinates to traverse to
	if(destReached == true)
	{
		//double** finalPath;
		//a stack of pairs of x/y coordinates to traverse to in order to reach destination
		//std::stack<pathPair> finalPath;
		//set row and col equal to destination row and col
		row = destination.first;
		col = destination.second;
		
		//IN ORDER TO GO FROM TILE ROW/COLUMN TO X/Y COORDINATES, SINCE THEY ARE 100X100, 
		//WE MUST MULTIPLY ROW AND COLUMN BY 100, AND THEN ADD 50 TO END UP IN THE MIDDLE OF A TILE
		
		//loop through tileMap starting from destination, back to start
		while(!(tileInfo[row][col].parent_row == row && tileInfo[row][col].parent_col == col))
		{
			//debug
			//std::cout << "row = " << row << " col = " << col << "\n"; 
			//if(row == -1 || col == -1)
			//	break;
			
			//push the coordinates onto the stack
			if(this->isGoblin)
				finalPath.push(std::make_pair((row * 100 + 25), (col * 100 + 40))); //for some reason doesn't work unless I add the offset?
			else
				finalPath.push(std::make_pair((row * 100 + 25), (col * 100 + 5))); //for some reason doesn't work unless I add the offset?
			//create temp variables to grab the next row and column - ie those of the parent
			int temp_row = tileInfo[row][col].parent_row;
			int temp_col = tileInfo[row][col].parent_col;
			//now assign the temp to the actual row and col
			row = temp_row;
			col = temp_col;
		}
		
		//make one final push onto the stack
		//if(row != -1 && col != -1)
		//	finalPath.push(std::make_pair((row * 100 + 50), (col * 100 + 50)));
	}
	
	return;
	
}
 
//function to return path from start to destination, will be 2D array of [x][y], with each being coordinates to traverse to in order
//double** Enemy::returnPath(tile tiles[][maxCols], Pair destination)
//{
	
//	return 0;
//}




















