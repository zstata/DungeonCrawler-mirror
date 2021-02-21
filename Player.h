#include "Character.h"
#include "Weapon.h"
#include "Enemy.h"
#include "Arrow.h"
#ifndef PLAYER_H
#define PLAYER_H

// this is a class specifically for the playable
// character controlled by the user

class Enemy;

class Player: public Character {
	public:
	using Character::Character;
	void attack(std::vector<Enemy*> enemies);
	int ATTACK_WIDTH = x_size;
	int ATTACK_HEIGHT = y_size;
	double attackTime=0;
	void moveCharacter(double x, double y, double map_height, double map_width, double timestep);
	std::vector<Weapon*> weapons;
	int currWeapon;
	int damage=10;
	bool shot=false;
	double pressureValue;
	bool isAttacking=false;
	//decide where the hitbox is depending on sprite in use
	void updateCollision();
	const char* playerNumber;
	static std::vector<SDL_Texture*> sprites;
	int weaponIndex=-1;
	//player hitbox/collision box
	//std::vector<SDL_Rect*> characterBox;
	int sideDir = 0; //0 if left 1 if right

    SDL_Texture* getSprite(double timestep){
		int value=damageSprite(timestep);
		if(value<=0)//If value is less than 0 then we show the sprite
			return sprites[spriteIndex];
		else //If value is 1 then we don't show the sprite
			return nullptr;
	}
	void damagePlayer(double damage);
	Arrow* shootBow();
};

#endif
