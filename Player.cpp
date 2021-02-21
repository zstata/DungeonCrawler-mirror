#include "Player.h"


std::vector<SDL_Texture*> Player::sprites;

Arrow* Player::shootBow(){
	Arrow* a=new Arrow(x_pos+x_size/2, y_pos+y_size/2);
	a->friendly=true;
	a->setDamage(weapons[weaponIndex]->damage);
	if(spriteIndex==0){ a->target(a->x_pos, a->y_pos+20);} //Down
	else if(spriteIndex==1){a->target(a->x_pos, a->y_pos-20);} //Up
	else if(flip==SDL_FLIP_NONE) {a->target(a->x_pos-20, a->y_pos);} //Left
	else {a->target(a->x_pos+20, a->y_pos);} //Right
	return a;
}


void Player::attack(std::vector<Enemy*> enemies){
	SDL_Rect retVal;

	switch(spriteIndex){
		case 0: //Down
			retVal.x=x_pos;
			retVal.y=y_pos+y_size/2;
			retVal.h=y_size;
			retVal.w=x_size;
			break;
		case 1: //Up
			retVal.x=x_pos;
			retVal.y=y_pos-y_size/2;
			retVal.h=y_size;
			retVal.w=x_size;			
			break;
		case 36: //Left
			if(sideDir == 0){
				retVal.x=x_pos-x_size/2;
				retVal.y=y_pos;
				retVal.w=x_size;
				retVal.h=y_size;						
			}
			else if(sideDir == 1){  //Right
				retVal.x=x_pos+x_size/2;
				retVal.y=y_pos;
				retVal.w=x_size;
				retVal.h=y_size;	
			}
			else
			{
				retVal.x=x_pos+x_size/2;
				retVal.y=y_pos;
				retVal.w=x_size;
				retVal.h=y_size;
			}
			break;
		default:

			break;
	}
	
	
	for(Enemy* e:enemies){
		SDL_Rect result;
		//first check if the enemy is blocking - will only be true for knight
		if(e->returnBlocking())
		{
			//if enemy is blocking, check if the incoming attack intersects with the blocking rect
			if(SDL_IntersectRect(&retVal, e->returnBlockBox(), &result) || SDL_IntersectRect(&this->characterTree->root->hitbox, &e->characterTree->root->hitbox, &result)){
				//if in here, attack hit the blocking box, do not remove health
			}
			//else, check if attack hit the blocking knight, but did not get blocked, so remove health
			else if(SDL_IntersectRect(&retVal, &e->characterTree->root->hitbox, &result))
				if(weaponIndex==-1){
					e->damageCharacter(damage);
				}
				else
					e->damageCharacter(weapons.at(weaponIndex)->damage);
				//e->damageCharacter(damage); //Make this weapons[currWeapon]->damage eventually once they figure out how to handle equipping weapons.  Remember unarmed attacks!
		}
		//else enemy is not blocking, or is just a regular enemy, so check for a hit
		else if(SDL_IntersectRect(&retVal, &e->characterTree->root->hitbox, &result)){
			if(weaponIndex==-1){
				e->damageCharacter(damage);
			}
			else e->damageCharacter(weapons.at(weaponIndex)->damage);
			//e->damageCharacter(damage); //Make this weapons[currWeapon]->damage eventually once they figure out how to handle equipping weapons.  Remember unarmed attacks!
		}
	}

}


void Player::moveCharacter(double x, double y, double map_height, double map_width, double timestep)
{
	//delta x and y for camera/character

	x_deltav = 0;
	y_deltav = 0;
	//detecting key states for movement
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);
	if (keystate[SDL_SCANCODE_W])
		y_deltav -= (ACCEL * timestep);
	if (keystate[SDL_SCANCODE_A])
		x_deltav -= (ACCEL * timestep);
	if (keystate[SDL_SCANCODE_S])
		y_deltav += (ACCEL * timestep);
	if (keystate[SDL_SCANCODE_D])
		x_deltav += (ACCEL * timestep);

	//check if delta_v = 0 (no key pressed), if so decrement velocity
	//first checking x_deltav
	if(x_deltav == 0)
	{
		if(x_vel > 0)
		{
			if(x_vel < (ACCEL * timestep))
				x_vel = 0;
			else
				x_vel -= (ACCEL * timestep);
			sideDir = 1;
		}
		else if(x_vel < 0)
		{
			if(-x_vel < (ACCEL * timestep))
				x_vel = 0;
			else
				x_vel += (ACCEL * timestep);
			sideDir = 0;
		}
	}
	else
		x_vel += x_deltav;

	//now checking y_deltav
	if(y_deltav == 0)
	{
		if(y_vel > 0){
			if(y_vel < (ACCEL * timestep)){
				y_vel = 0;
			}
			else{
				y_vel -= (ACCEL * timestep);
			}
		}
		else if(y_vel < 0){
			if(-y_vel < (ACCEL * timestep)){
				y_vel = 0;
			}
			else{
				y_vel += (ACCEL * timestep);
			}
		}
	}
	else
		y_vel += y_deltav;
	//check if speed limit is exceeded
	//x direction
	if(x_vel < -SPEED_LIMIT)
		x_vel = -SPEED_LIMIT;
	else if(x_vel > SPEED_LIMIT)
		x_vel = SPEED_LIMIT;
	//y direction
	if(y_vel < -SPEED_LIMIT)
		y_vel = -SPEED_LIMIT;
	else if(y_vel > SPEED_LIMIT)
		y_vel = SPEED_LIMIT;

	//update coordinates
	x_pos += (x_vel * timestep);
	y_pos += (y_vel * timestep);
	//check if out of bounds
	//x direction
	if (x_pos < 0)//-27)
		x_pos = 0;
	if (x_pos + x_size > map_width)
		x_pos = map_width - x_size;
	//y direction
	if (y_pos < 0)//-27)
		y_pos = 0;
	if (y_pos + y_size > map_height)
		y_pos = map_height - y_size;
	
	//update collision boxes
	updateCollision();
	
}

//decide which collision boxes to use
void Player::updateCollision()
{
	//using sprite indexes to tell what hitboxes to use for now, once sprite sheets are in use & animations improved can implement a direction variable
	
	//if front or back facing
	//if(spriteIndex <= 17)
	//{
		//tried making just one hitbox for a direction instead of complicating things
		characterTree->root->hitbox.x = x_pos;
		characterTree->root->hitbox.y = y_pos+27;
		characterTree->root->hitbox.w = 60;
		characterTree->root->hitbox.h = 80;
		/*
	}
	//if left facing
	else if(spriteIndex >= 18 && (flip == SDL_FLIP_NONE))
	{
		characterTree->root->hitbox.x = x_pos;
		characterTree->root->hitbox.y = y_pos+27;
		characterTree->root->hitbox.w = 47;
		characterTree->root->hitbox.h = 80;
	}
	//if right facing
	else if(spriteIndex >= 18 && (flip == SDL_FLIP_HORIZONTAL))
	{
		characterTree->root->hitbox.x = x_pos-24;
		characterTree->root->hitbox.y = y_pos+28;
		characterTree->root->hitbox.w = 47;
		characterTree->root->hitbox.h = 80;
	}
	*/
		
}
