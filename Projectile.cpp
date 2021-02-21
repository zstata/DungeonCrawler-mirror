#include "Projectile.h"
#include <string>
#include <cmath>
#include <iostream>

Projectile::Projectile(double x, double y): x_pos{x}, y_pos{y}{
	
    flip=SDL_FLIP_NONE;
    spriteIndex=0;

}

void Projectile::target(double x, double y){
    double xdist = x-x_pos;
    double ydist = y-y_pos;
    double dist = std::sqrt(std::pow((xdist), 2)+std::pow((ydist),2));

    
    x_vel=getSpeed()*xdist/dist;
    y_vel=getSpeed()*ydist/dist;


	//std::cout<<"Arrow pos: "<<x_pos << " " << y_pos << std::endl;

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

std::vector<SDL_Texture*> Projectile::sprites;

int Projectile::moveProjectile(double map_height, double map_width, double timestep){
    y_pos += (y_vel * timestep);

	//std::cout<<"Arrow coords: "<<y_pos<<", "<<x_pos<<std::endl;
	if (y_pos < 0){
		std::cout<<"1"<<std::endl;
        return -1;
    }
	else if(y_pos + y_size > map_height){
		
		std::cout<<"2 y: "<<y_pos<<" ys: "<<y_size<<" mh: "<<map_height<<std::endl;
        return -1;
    }
	x_pos += (x_vel * timestep);
	if (x_pos < 0){
		std::cout<<"3"<<std::endl;
        return -1;
    }
	else if(x_pos + x_size > map_width){
		std::cout<<"4 y: "<<y_pos<<" ys: "<<y_size<<" mh: "<<map_height<<std::endl;
        return -1;
    }


    // Move the hitbox accordingly
	setMovebox();
    setHitbox();
    return 0;
}


void Projectile::setHitbox(){}

int Projectile::hitsCharacter(Character* c) {
		SDL_Rect result;

		if(SDL_IntersectRect(c->characterTree->getRootHitbox(), this->getHitBox(), &result)){
			return -1;
		}
		
		return 0;
}
