#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "Character.h"


void Character::damageCharacter(double damage){
	if(damageTimer==-1)
		health=health-damage;
}

Character::Character(double xcoord, double ycoord, int width, int height, double maxHealth) :x_pos{xcoord}, y_pos{ycoord}, x_size{width}, y_size{height}, health{maxHealth}, healthBuffer{maxHealth} {
            x_vel = 0;
            y_vel = 0;
            x_deltav = 0;
            y_deltav = 0;
            isGoblin=false;
			//define a max velocity, can use absolute value of x/y_vel so we don't need a separate for negatives
			//max_vel = 5;
            dstrect = {static_cast<int>(x_pos), static_cast<int>(y_pos), x_size, y_size};
            //This is a placeholder value
            flip=SDL_FLIP_NONE;
            spriteIndex=0;
			
			//initialize SDL_rects for collision boxes
			SDL_Rect head = {static_cast<int>(x_pos),static_cast<int>(y_pos),x_size,y_size};
			SDL_Rect torso = {static_cast<int>(x_pos),static_cast<int>(y_pos),x_size,y_size};
			SDL_Rect legs = {static_cast<int>(x_pos),static_cast<int>(y_pos),x_size,y_size};
			SDL_Rect arms = {static_cast<int>(x_pos),static_cast<int>(y_pos),x_size,y_size};
			//push onto collision box vector
            characterTree = new HitboxTree();
			characterTree->setRoot(head);
			characterTree->addChild(torso);
			characterTree->addChild(legs);
			characterTree->addChild(arms);
			
}
std::vector<SDL_Texture*> sprites;
SDL_Texture* Character::characterSheet;
std::vector<SDL_Rect*> Character::characterRects;


void Character::setSpriteAndDirection(){
    if(x_vel < 0){
        spriteIndex=2;
        flip = SDL_FLIP_NONE;
    }
    else if(x_vel > 0){
        spriteIndex=2;
        flip = SDL_FLIP_HORIZONTAL;
    }
    else if(y_vel < 0){
        spriteIndex=1;
        flip = SDL_FLIP_NONE;
    }
    else if(y_vel > 0){
        spriteIndex=0;
        flip = SDL_FLIP_NONE;
    }
}

void Character::collides(Character* other, double timestep) {
    //check if collision with this or other
    SDL_Rect result;

    if(SDL_IntersectRect(this->characterTree->getRootHitbox(), other->characterTree->getRootHitbox(), &result))
    {
        //X COLLISION

        //compare x velocities and adjust for collisions
        //case 1, this character and other character are moving directly into each other
        if(((other->x_vel > 0) && (this->x_vel < 0)) || ((other->x_vel < 0) && (this->x_vel > 0)))
        {
            this->x_pos -= (this->x_vel * timestep);
            other->x_pos -= (other->x_vel * timestep);
        }
        //case 2, this is not moving in x direction
        else if((this->x_vel == 0) && (other->x_vel != 0))
            other->x_pos -= (other->x_vel * timestep);
        //case 3, other is not moving in x direction
        else if((other->x_vel == 0) && (this->x_vel != 0))
            this->x_pos = this->x_pos;
        //if this is faster than the other/ they caught up to them
        else if((((this->x_vel > 0) && (other->x_vel > 0)) && (this->x_vel > other->x_vel)) || (((this->x_vel < 0) && (other->x_vel < 0)) && (this->x_vel < other->x_vel))) //  ((this->x_vel < 0) && (other->x_vel < 0)))
            this->x_pos -= (this->x_vel * timestep);
        //if other is faster than the this/ they caught up to them
        else if((((this->x_vel > 0) && (other->x_vel > 0)) && (this->x_vel <= other->x_vel)) || (((this->x_vel < 0) && (other->x_vel < 0)) && (this->x_vel >= other->x_vel)))// || ((other->x_vel < 0) && (this->x_vel < 0)))
            other->x_pos -= (other->x_vel * timestep);
        else
        {
            this->x_pos -= (this->x_vel * timestep);
            other->x_pos -= (other->x_vel * timestep);
        }

        //Y COLLISION


        //compare y velocities and adjust for collisions
        //case 1, other and this are moving directly into each other
        if(((other->y_vel > 0) && (this->y_vel < 0)) || ((other->y_vel < 0) && (this->y_vel > 0)))
        {
            this->y_pos -= (this->y_vel * timestep);
            other->y_pos -= (other->y_vel * timestep);
        }
        //case 2, this is not moving in y direction
        else if((this->y_vel == 0) && (other->y_vel != 0))
            other->y_pos -= (other->y_vel * timestep);
        //case 3, other is not moving in y direction
        else if((other->y_vel == 0) && (this->y_vel != 0))
            this->y_pos = this->y_pos;
        //case 4, other is not moving in y direction & this is above or below entire other
       
        //if this is faster than the other/ they caught up to them
        else if((((this->y_vel > 0) && (other->y_vel > 0)) && (this->y_vel >= other->y_vel)) || (((this->y_vel < 0) && (other->y_vel < 0)) && (this->y_vel <= other->y_vel))) //  ((this->x_vel < 0) && (other->x_vel < 0)))
            this->y_pos -= (this->y_vel * timestep);

        
        //if other is faster than the this/ they caught up to them
        else if((((this->y_vel > 0) && (other->y_vel > 0)) && (this->y_vel <= other->y_vel)) || (((this->y_vel < 0) && (other->y_vel < 0)) && (this->y_vel >= other->y_vel)))// || ((other->x_vel < 0) && (this->x_vel < 0)))
            other->y_pos -= (other->y_vel * timestep);
        else
        {
            this->y_pos -= (this->y_vel * timestep);
            other->y_pos -= (other->y_vel * timestep);
        }

    }
}

