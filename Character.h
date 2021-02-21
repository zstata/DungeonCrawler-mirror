#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <iostream>
#include <string>
#include "HitboxTree.h"
#ifndef CHARACTER_H
#define CHARACTER_H

class Character{

    public:
        Character(double xcoord, double ycoord, int width, int height, double maxHealth);
        void setSpriteAndDirection();
        double x_vel;
        double ON_TIME=100;
        double OFF_TIME=25;
        double y_vel;
        double x_pos; //these are absolute
        double y_pos;
        double x_deltav;
        double y_deltav;
        bool isGoblin = false;
		//Speed limit -> for 5px/frame @60fps = 300px/s
		double SPEED_LIMIT = 300.0;
		//acceleration -> for 1px/frame^2 @60fps = 3600px/s^2
		double ACCEL = 3600.0;
        double health;
        double healthBuffer;
        int x_size;
        int y_size;
        double damageTimer=-1;
        int spriteIndex;
        SDL_RendererFlip flip;
        SDL_Rect dstrect;
        static std::vector<SDL_Texture*> sprites; //0 faces forward, 1 faces backward, 2 faces left
		//following items are for holding the sprite sheet (characterSheet), and individual sprites from the sheet (characterRects)
		static SDL_Texture* characterSheet;
		static std::vector<SDL_Rect*> characterRects;
		
        void moveCharacter(); //For enemies this will run the AI, for players this will run the player input routine
            // This should be virtual but I was having problems at compile time with it
		//player hitbox/collision box 
		//index 0 = head, 1 = torso, 2 = legs, 3 = arms
		HitboxTree *characterTree;

		//check for collision between characters
		void collides(Character* other, double timestep);
	void damageCharacter(double damage);
        //bool checkCollision(SDL_Rect

        //This works this way because there is too much depending on this class to fully refactor it like it should be.  0 means show sprite, 1 means don't show sprite.  -1 means stop.
    int damageSprite(double timestep){
		int curTime=SDL_GetTicks();
		//std::cout<<health;
		//std::cout<<"  ";
		//std::cout<<healthBuffer<<std::endl;
            if(health<healthBuffer){
                healthBuffer=health;
                damageTimer=curTime;
            }

            if(curTime-damageTimer > 500){
                damageTimer=-1;
		//std::cout<<"2"<<std::endl;
                return -1;
            }
            else if(((int)(curTime-damageTimer)) % ((int)(ON_TIME+OFF_TIME)) < OFF_TIME){
		//std::cout<<"3"<<std::endl;
                return 1;
            }
            else return 0;

    }
        
        virtual SDL_Texture* getSpriteSheet(double timestep){
			int value=damageSprite(timestep);
            if(value<=0)//If value is less than 0 then we show the sprite
			    return characterSheet;
            else //If value is 1 then we don't show the sprite
                return nullptr;
		}
		virtual double getSpeedLimit() {return SPEED_LIMIT;}
		//check for collision between SDL_Rects
		//bool checkCollision(SDL_Rect
	
        virtual std::vector<SDL_Rect*> getCharacterRects(){return characterRects;}
		//define the method for loading the sprite sheet for character - create within each enemy type cpp file
		//void loadSpriteSheet();
		//define dstrect method
		//void createRects();

};


#endif
