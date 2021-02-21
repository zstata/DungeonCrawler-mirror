#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Archer.h"
#include <cmath>
#include <iostream>
#include "Arrow.h"
#include "Player.h"

SDL_Texture* Archer::characterSheet;
std::vector<SDL_Rect*> Archer::characterRects;
//x and y are the coords of the player
void Archer::kitePlayer(double x, double y, double map_height, double map_width, double timestep, int** floorMap, double floorHeight, double floorWidth){  
	//Find the closest point in a certain radius around the player where the enemy can go
    //Accelerate the archer towards that point

    //If we are >= the radius in distance, seek.  Else, flee.

    //This is the distance formula in 2 dimensions

    double distance = std::pow((x-x_pos), 2)+std::pow((y-y_pos),2);

    
    if(distance>=CHASE_DISTANCE){
        pathfind(x, y, floorMap, floorHeight, floorWidth, map_height, map_width, timestep);
    }
    else if(distance<=RUN_DISTANCE){
        enemyFlee(x, y, map_height, map_width, timestep);
    }


}



/*

    AI Structure
        If alone or engaged with a player in close combat, kite and fire less accurately or rapidly
        If player engaged with other units, stand still and fire more accurately or rapidly, while still keeping distance
            We can do this via a "pressure" gauge, see the game AI textbook
        If there are multiple in an area, spread out and stand still (except for any that are actively being engaged in melee combat)
        If low health, slow down (like being injured)
        Also need some sort of target acquisition for multiplayer environments - for these guys it can just be closest player


*/
void Archer::spotPlayer(Player* target){

	if((std::pow((target->x_pos-x_pos), 2) + std::pow((target->y_pos-y_pos), 2)) <= sight)
	{
		playerSeen=true;
	}


}

Arrow* Archer::moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep){

//This needs to be the x and y of the closest target, right?  Or should I just pass in an array of Players?
//Arrow* Archer::moveCharacter(double x, double y, double map_height, double map_width, double timestep, double pressureValue){

    

	Player* closest;
    double closestDist=100000000;
    for(Player* p: players){ //Have this one just be distance based - attack the closest player
        double distance = std::pow((p->x_pos-x_pos), 2)+std::pow((p->y_pos-y_pos),2);
        if(distance<closestDist){
            closestDist=distance;
            closest=p;
        }
    }

	spotPlayer(closest);
    double x=closest->x_pos+(closest->x_size/2);
    double y=closest->y_pos+(closest->y_size/2);

	//std::cout<<x<<", "<<y<<std::endl;

    double pressureValue=closest->pressureValue;

	if(!playerSeen) return nullptr;
    //Firstly, adjust the speed based on health
    if(health<=20){
        SPEED_LIMIT=125;
    }
    else{
        SPEED_LIMIT=200;
    }

    //Determine the environment state - need offensive pressure on the players, number of skeletons nearby, and the nearest player


    //numSkel=getNearbyArchers();


    /*
        We can achieve this in a few ways.  We can search a radius for any Archers and report back the number, but that doesnt help us figure out what direction to go.
        I propose a heatmap style system, where each archer exerts a pressure value on the nearby area that decays with distance.  We could then have the skeleton move to a point of lower pressure if the pressure where it currently is is too high.
        This could lead to some unnatural movement, so I think this should only come into effect when the skeletons are standing by, not kiting.
    */



    //nearestPlayer = getNearestPlayer();
    //This is for target acquisition in a multiplayer environment


    //If the players are alone with the skeleton, or they are trying to attack it in melee, then kite the nearest one 

    if(pressureValue<KITE_PRESSURE_THRESHOLD || closestDist<RUN_DISTANCE || closestDist>CHASE_DISTANCE){
	kitePlayer(x, y, map_height, map_width, timestep, floorMap, floorHeight, floorWidth);
        shotInterval=2000;
    }
    else{
        shotInterval=1000;
        //standLocation=findStandLocation();
    }


        /*
            This will do some sort of rate of change analysis on the heatmap, be it calc based or algebra based, to find in which direction the pressure gradient is decreasing
            Can also just find a local min on a 3D surface.
            It will then move in that direction.  IMPORTANT TO MAKE SURE IT DOES NOT TAKE ITS OWN AURA INTO ACCOUNT?
            We could also just poll N/S/E/W and the diagonals and then move to the location with the lowest value...
        */
	if(x_pos<0) x_pos=0;
	if(y_pos<0) y_pos=0;
	if(y_pos>map_height) y_pos=map_height;
	if(x_pos>map_width) x_pos=map_width;

    facePlayer(x, y);
    Arrow* a = nullptr;

    //I don't want it to shoot immediately - give it a few seconds
	int curTime=SDL_GetTicks();
	//std::cout<<curTime;
	//std::cout<<"  ";
	//std::cout<<shotTime << std::endl;


    if(curTime-shotTime>=shotInterval || shotTime == -1){
        a=fireArrow(x, y);
        shotTime=curTime;
	//std::cout << "Shot fired" << std::endl;
    }

    return a;
}



//Fire an arrow towards (x,y)
Arrow* Archer::fireArrow(double x, double y){
    //std::cout<<x<<" "<<y<<std::endl;
    Arrow* a = new Arrow(x_pos+x_size/2, y_pos+y_size/2);
    a->target(x,y);
    return a;
}
