#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <math.h>
#include "Knight.h"
#include "Player.h"


//std::vector<SDL_Texture*> Knight::sprites;

/*
    Knight AI
        Positioning: Will try and stay rather close to the player so they can't kite the enemy.  Will stand in between the target player and the ranged units if possible - implement this later.
        In combat:
            It should face the target player
            If there is an attack at it and the player is far-ish away, it will try and dodge.
                After a dodge it should have some vulnerability.
            If there is an attack at it and the player is closer, it will go for a block.
                It should be directional - only protect from damage from one side.
                It should move slower while it is blocking
                It should block if there is a projectile about to hit it from a direction it can block, it isn't in the middle of an animation, and it has the ability to block
                It could also walk while blocking if it is being fired at and there are not a lot of players around
            Either way, if the player is within a certain distance of it when these maneuvers finish, it will go for a counterattack, leaving it vulnerable while it does.
                This counterattack should be fast but dodgeable.  That way the players can dodge them if they know its coming.
            It's attack pattern should be semi random.  It will mostly wait for the player to make a move, but if the player waits long enough it will strike out, once again being vulnerable
                We can have an attack cooldown, and if that cooldown is up we can have a 50% chance to attack every X period of time and the player is close enough.


*/

SDL_Texture* Knight::characterSheet;
std::vector<SDL_Rect*> Knight::characterRects;

Arrow* Knight::moveCharacter(std::vector<Player*> players, int** floorMap, int floorHeight, int floorWidth, double map_height, double map_width, double timestep){
    
    
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
		
		//try to spot player
		this->spotPlayer(closest);
		if(playerSeen)
		{
			targetPlayer=closest;
			facePlayer(targetPlayer->x_pos, targetPlayer->y_pos);
		}
    }

	int curTime=SDL_GetTicks();
    double distanceToPlayer;
	//if targetPlayer is null right now, set distanceToPlayer = 0
	if(targetPlayer == nullptr)
		distanceToPlayer = 0;
	else
		distanceToPlayer = pow((targetPlayer->x_pos-x_pos), 2)+pow((targetPlayer->y_pos-y_pos),2);
		
	//if in this loop, the target player has been seen, decide how to act
	if(playerSeen)
	{
		if(!isDodging && !isCountering)
		{
			//call pathfinding now, a* to target, if dodging or something else we can overwrite their path later
			this->pathfind(targetPlayer->x_pos, targetPlayer->y_pos, floorMap, floorHeight, floorWidth, map_height, map_width, timestep);
		}
		else if(isCountering)
		{
			counter();
			//call pathfinding now, a* to target, if dodging or something else we can overwrite their path later
			//this->pathfind(targetPlayer->x_pos-10, targetPlayer->y_pos+10, floorMap, floorHeight, floorWidth, map_height, map_width, timestep);
			this->enemySeek(targetPlayer->x_pos, targetPlayer->y_pos, map_height, map_width, timestep);
			//check if close enough to player upon lunging and if so, attack
			if(distanceToPlayer <= ATTACK_DISTANCE)
			{		
				isAttacking=true;
				attackTimer=curTime;
			}
		}
		
		//Gauge if we would like to initiate dodge
		if(isDodging){
			//handle whatever dodging animations are needed then turn off isDodging, isCountering, and hasCountered when the animation finishes
			//if(curTime - actionTime > actionInterval)
			//{
				dodge(map_height, map_width, timestep);
			//}
			//else
			//{
			//	isDodging = false;
				
				//just finished dodging, decide if we want to counterattack
				if(!isDodging)
				{
					//reset this dodging variable
					this->firstEntry = true;
					//change speed back to normal
					SPEED_LIMIT=230;
					
					//if(curTime - actionTime > COUNTER_INTERVAL && !hasCountered){
						if(rand()%2==1){ //50% chance to counter.  Could also treat this as a standard attack if you so wish
							//counter()
							//std::cout << "COUNTER";
							isCountering=true; //change to true when implemented
							actionTime=curTime;
							actionInterval=COUNTER_INTERVAL;
						}
						//hasCountered=true;
					//}
				}
			//}
			
		}
		else if(isCountering)
		{
			//counter();
			//once close enough to player, attack
			if(distanceToPlayer <= ATTACK_DISTANCE_COUNTER)
			{
				//change speed back to normal
				SPEED_LIMIT=230;
				isCountering = false;
				isAttacking = true;
			}
		}
		else if(isBlocking){
			//this should go for some range of time, and then the shield should go down.  When the shield goes down they should have a small action cooldown.  Also turn off isBlocking when it is done.			
			block();
			SPEED_LIMIT=115;

			//if done blocking, isBlocking will be set to false in the block() method right before this
			if(!isBlocking)
			{
				//When the shield goes down, do the next two lines
				//actionInterval = BLOCK_INTERVAL;
				SPEED_LIMIT=230;
			}
		
		}
		else if(isAttacking)
		{
			attack(players);
		}
		else if(targetPlayer->isAttacking){
			if(distanceToPlayer>DODGE_DISTANCE_SMALL && distanceToPlayer < DODGE_DISTANCE_LARGE && curTime - actionTime > actionInterval){
				//do anything else we need to initiate a dodge action
				actionTime=curTime;
				actionInterval=DODGE_INTERVAL;
				isDodging=true;//CHANGE THIS TO TRUE ONCE DODGE IS IMPLEMENTED
				//std::cout << "DODGE\n";
			}

			//Also need to implement blocking if a projectile is coming at them
			else if(distanceToPlayer < DODGE_DISTANCE_SMALL && curTime - actionTime > actionInterval){
				//block
				//std::cout << "BLOCK\n";
				isBlocking=true;
				actionInterval = BLOCK_INTERVAL;
			}
		}

		if(!isBlocking && !isDodging && !isAttacking){
			//TODO CHECK IF PLAYER IS IN RANGE OF ATTACK
			if(distanceToPlayer <= ATTACK_DISTANCE)
			{
				if(curTime - attackTimer > ATTACK_CHECK_INTERVAL){
					//roll to attack
					if(rand()%3==1){
						//attack();
						isAttacking=true;
						//std::cout << "ATTACK\n";
					}

					attackTimer=curTime;
				}
			}
		}
	}
    //move()
    return nullptr;
}

//method to determine if the knight can spot the target player
void Knight::spotPlayer(Player* target)
{
	//using sqrt(a^2 + b^2) determine if distance to player is within sight
	if((std::pow((target->x_pos - this->x_pos), 2) + std::pow((target->y_pos - this->y_pos), 2)) <= sight)
	{
		playerSeen = true; //if true, chase/attack player
	}
}

//method to dodge incoming attacks, based off of which direction the knight is facing
void Knight::dodge(double map_height, double map_width, double timestep)
{
	//if dodgeTime = 0, resetting the dodge ie first time called
	if(dodgeTime == 0)
	{
		dodgeTime = SDL_GetTicks();
		dodgeEnd = SDL_GetTicks() + 300;
	}
	//if dodge has gone on for .3 second, reset dodgeTime to 0 as default and set isDodging to false
	else if(dodgeTime > dodgeEnd)
	{
		this->dodgeTime = 0;
		this->isDodging = false;
		spriteAssign = true;
	}
	else
	{
		//set initial sprite 
		if(spriteAssign)
		{
			firstSprite = this->spriteIndex;
			spriteAssign = false;
		}
		
		//first check which way he is facing
		if(firstSprite == faceForward || firstSprite == faceBackward) // knight facing towards screen - dodge left or right
		{
			this->SPEED_LIMIT = DODGE_SPEED; //speed knight up to dodge
			//rand number to determine dodge left or right
			int dir = rand() % 2;
			//change pathfinding from chasing player to moving right or left - only decided once
			if(firstEntry && (dir == 1)) //if true, knight will dodge right
			{
				//std::cout << "RIGHT\n" ;
				firstEntry = false;
				dodgeDir = DODGE_RIGHT;
			}
			else if(firstEntry && (dir == 0)) //if true, knight will dodge left
			{
				//std::cout << "LEFT\n" ;
				firstEntry = false;
				dodgeDir = DODGE_LEFT;
			}
			
			//now depending on dodgeDir, seek to a position to the right or left of where the knight currently is
			if(dodgeDir == 1)//DODGE_RIGHT)
			{
				//std::cout << "DODGE RIGHT\n" ;
				this->updatePos(SPEED_LIMIT, 0, map_height, map_width, timestep);
				
				this->x_vel = SPEED_LIMIT;
				this->y_vel = 0;
			}
			else if(dodgeDir == 0)//DODGE_LEFT)
			{
				//std::cout << "DODGE LEFT\n" ;
				this->updatePos(-SPEED_LIMIT, 0, map_height, map_width, timestep);
				
				this->x_vel = -SPEED_LIMIT;
				this->y_vel = 0;
			}
			//else
			//	std::cout << "DODGE DIR1 = " << dodgeDir << "\n";
			
			//increment time for dodging
			this->dodgeTime = SDL_GetTicks();
			
		}
		else if(firstSprite == faceLeft) //knight is facing left or right, depending on flip
		{
			this->SPEED_LIMIT = DODGE_SPEED; //speed knight up to dodge
			//rand number to determine dodge up or down
			int dir = rand() % 2;
			//change pathfinding from chasing player to moving up or down - only decided once
			if(firstEntry && (dir == 1)) //if true, knight will dodge up
			{
				//std::cout << "UP\n" ;
				firstEntry = false;
				dodgeDir = DODGE_UP;
			}
			else if(firstEntry && (dir == 0)) //if true, knight will dodge down
			{
				//std::cout << "DOWN\n" ;
				firstEntry = false;
				dodgeDir = DODGE_DOWN;
			}
			
			//now depending on dodgeDir, seek to a position above or below where the knight currently is
			if(dodgeDir == 2)//DODGE_UP)
			{
				//std::cout << "DODGE UP\n" ;
				this->updatePos(0, -SPEED_LIMIT, map_height, map_width, timestep);
				
				this->y_vel = -SPEED_LIMIT;
				this->x_vel = 0;
			}
			else if(dodgeDir == 3)//DODGE_DOWN)
			{
				//std::cout << "DODGE DOWN\n" ;
				this->updatePos(0, SPEED_LIMIT, map_height, map_width, timestep);
				
				this->y_vel = SPEED_LIMIT;
				this->x_vel = 0;
			}
			//else
			//	std::cout << "DODGE DIR2 = " << dodgeDir << "\n";
			
			//increment time for dodging
			this->dodgeTime = SDL_GetTicks();
		}
		//else
		//	std::cout << "DODGE DIR = " << dodgeDir << "\n";
	}
	
}

//counter method, once finished dodging, has ~50% chance to do this
void Knight::counter()
{
	//if starting the counter, begin time keeping
	if(counterTime == 0)
	{
		counterTime = SDL_GetTicks();
		counterEnd = SDL_GetTicks() + 300;
	}
	//if done countering, reset boolean and speed
	else if(counterTime > counterEnd)
	{
		counterTime = 0;
		this->SPEED_LIMIT = 230;
		this->isCountering = false;
	}
	//if already in countering process, just "lunge" at the player by increasing speed
	else
	{
		//up the speed for knight
		this->SPEED_LIMIT = COUNTER_SPEED;
		
		//increment time for countering
		this->counterTime = SDL_GetTicks();
	}
}

//method to block incoming attacks from the direction the knight is facing - very similar to attack method
//main method for the knight to block
void Knight::block()
{
	//make knight face player being attacked
	this->facePlayer(targetPlayer->x_pos, targetPlayer->y_pos);
	
	//SDL rect for the block box
	//SDL_Rect* blockBox;
	//SDL rect for the block box
	
	//if blockTime = 0, resetting the block ie first time called
	if(blockTime == 0)
	{
		blockTime = SDL_GetTicks();
		blockEnd = SDL_GetTicks() + 1500;
	}
	//if block has gone on for 1.5 seconds, reset blockTime to 0 as default and set isBlocking to false
	else if(blockTime > blockEnd)
	{
		this->blockTime = 0;
		this->isBlocking = false;
	}
	//else knight is blocking right now
	else
	{
		//check what direction facing and set sprite accordingly
		if(this->spriteIndex == faceForward) //knight facing front - block down
		{
			this->spriteIndex = blockFront;
			//define the size of the hitbox, will be below knight sprite
			this->blockBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos+this->y_size-blockBoxFront_Y),blockBoxFront_X,blockBoxFront_Y};
			//this->blockBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos),this->x_size,this->y_size};
		}
		else if(this->spriteIndex == faceLeft) //knight facing left or right depending on flip - block left or right
		{
			this->spriteIndex = blockLeft;
			//define the size of the hitbox
			if(this->flip == SDL_FLIP_NONE) //block left
				//this->blockBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos),this->x_size,this->y_size};
				this->blockBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos),blockBoxSide_X,blockBoxSide_Y};
			else //block right
				//this->blockBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos),this->x_size,this->y_size};
				this->blockBox = new SDL_Rect{static_cast<int>(this->x_pos+this->x_size-blockBoxSide_X),static_cast<int>(this->y_pos),blockBoxSide_X,blockBoxSide_Y};
		}
		else if(this->spriteIndex == faceBackward) //knight facing back - block upward
		{
			this->spriteIndex = blockBack;
			//define the size of the hitbox, will be above knight sprite
			//this->blockBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos),this->x_size,this->y_size};
			this->blockBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos),blockBoxFront_X,blockBoxFront_Y};
		}
		
		//CHECK FOR COLLISIONS WITH THIS BOX AND MAKE SURE THEY DONT HIT
		
		//increment time for blocking
		this->blockTime = SDL_GetTicks();
	}		

}


//main method for the knight to attack
void Knight::attack(std::vector<Player*> players)
{
	//make knight face player being attacked
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
			if(this->spriteIndex == faceForward) //knight facing front
			{
				this->spriteIndex = attackStartFront;
			}
			else if(this->spriteIndex == faceLeft) //knight facing left or right depending on flip
			{
				this->spriteIndex = attackStartLeft;
			}
			else if(this->spriteIndex == faceBackward) //knight facing back
			{
				this->spriteIndex = attackStartBack;
			}
			
			//increment time for attacking
			this->attackTime = SDL_GetTicks();
		}
		//if attack is still going on - knight is on down swing, change animation and also create the hurt box
		else if(attackEnd - attackTime < 500 && attackEnd - attackTime >= 0)
		{
			//check what direction facing and set sprite accordingly
			if(this->spriteIndex == faceForward) //knight facing front - attacking down
			{
				this->spriteIndex = attackEndFront;
				//define the size of the hitbox, will be below knight sprite
				attackBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos+this->y_size),attackBoxFront_X,attackBoxFront_Y};
			}
			else if(this->spriteIndex == faceLeft) //knight facing left or right depending on flip - attacking left or right
			{
				this->spriteIndex = attackEndLeft;
				//define the size of the hitbox
				if(this->flip == SDL_FLIP_NONE) //attacking left
					attackBox = new SDL_Rect{static_cast<int>(this->x_pos-30),static_cast<int>(this->y_pos),attackBoxSide_X,attackBoxSide_Y};
				else //attacking right
					attackBox = new SDL_Rect{static_cast<int>(this->x_pos+this->x_size-30),static_cast<int>(this->y_pos),attackBoxSide_X,attackBoxSide_Y};
			}
			else if(this->spriteIndex == faceBackward) //knight facing back - attacking upward
			{
				this->spriteIndex = attackEndBack;
				//define the size of the hitbox, will be above knight sprite
				attackBox = new SDL_Rect{static_cast<int>(this->x_pos),static_cast<int>(this->y_pos-30),attackBoxFront_X,attackBoxFront_Y};
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
			this->isAttacking = false;
		}
		
	}
	//else the knight is on cooldown
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
}




