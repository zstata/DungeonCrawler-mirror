#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Character.h"
#include "Player.h"
#include "Enemy.h"
#include "DTDecision.h"
#include "DTAction.h"
#include "Weapon.h"
#include "Floor.h"
#include "Archer.h"
#include "Chest.h"
#include "NetworkingClient.h"
#include "Arrow.h"
#include "HitboxTree.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include "Goblin.h"
#include "GoblinSquad.h"
#include "Assassin.h"
#include <sstream>
#include "Knight.h"
#include "GameMaster.h"


constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int BOX_WIDTH = 80;
constexpr int BOX_HEIGHT = 100;

constexpr double PRESSURE_GRID_SIZE=100.0;

constexpr int GOBLIN_WIDTH = 44;
constexpr int GOBLIN_HEIGHT = 40;
constexpr int PLAYER_WIDTH = 80;
constexpr int PLAYER_HEIGHT = 100;
constexpr int KNIGHT_WIDTH = 46;
constexpr int KNIGHT_HEIGHT = 87;
constexpr int ASSASSIN_WIDTH = 40;
constexpr int ASSASSIN_HEIGHT = 76;
constexpr int SKELETON_WIDTH = 33;
constexpr int SKELETON_HEIGHT = 87;

constexpr double PLAYER_MAX_HEALTH = 600;
constexpr double GOBLIN_MAX_HEALTH = 15;
constexpr double GRUNT_MAX_HEALTH = 30;
constexpr double ASSASSIN_MAX_HEALTH = 20;
constexpr double KNIGHT_MAX_HEALTH = 50;
constexpr double ARCHER_MAX_HEALTH = 30;

//for player respawns
int respawnX = 0;
int respawnY = 0;

int startUp = 0;
//multiplayer player list
std::vector<Player*> playerList;
int i_index = 0;
int indexer = 0;
bool playerFlag = true;
bool weaponPickedUp;
bool networkingFlag = false;

//Variables for floor generation
constexpr int S_FLOOR_SIZE=20;		//starting floor size
constexpr int F_FLOOR_SIZE=50;		//final floor size
constexpr int NUM_FLOORS=4;		//Total number of non-boss room floors, so actual total of floors is 1 + NUM_FLOORS

const int TILE_SWAP=(int)(ceil((double)(NUM_FLOORS)/4.0));

int curFloorNum=0;	//Number of current floor. Index beings at 0
int curTileType=0;
int FloorWidth;
int FloorHeight;

//MAP SIZE IS VARIABLE DO NOT MAKE THESE CONSTANT AGAIN
int BG_WIDTH = 2000;	// temporary dimensions for scrolltest.png
int BG_HEIGHT = 2000;

//FPS cap for game
//const int SCREEN_FPS = 60;

//amount of display time per frame
//const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

//Speed limit -> for 5px/frame @60fps = 300px/s
//constexpr double SPEED_LIMIT = 300.0;

//acceleration -> for 1px/frame^2 @60fps = 3600px/s^2
//constexpr double ACCEL = 3600.0;

int spriteIndex;
bool endGame = false;
// Function declarations
bool init();
SDL_Texture* loadImage(std::string fname);
void close();

int ttfInit = TTF_Init();
TTF_Font* font = TTF_OpenFont("Lato-Black.ttf", 8);
// Globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
std::vector<SDL_Texture*> gTex;
std::vector<SDL_Texture*>lobby;	//lobby menu image vector

//std::vector<SDL_Texture*> sprites;
std::vector<SDL_Texture*> maps; //the maps vector
std::vector<SDL_Texture*> tiles;	//Assets for map drawing (Currerntly all tiles are 32x32 pixels, stairs included)
std::vector<SDL_Texture*> inventory;
std::vector<Weapon*> all_weapons;	//All of the weapons currently in game (including on floor and in inventory)
int inventoryx[3][6];
int inventoryy[3][6];
Player *p;

//Chest *chestArray; //holds the two chests that will spawn on the map

//define strings for enemy sprite sheets
std::string playerSheet = ""; //CHARACTER SPRITE SHEET NEEDS DONE
std::string skeletonSheet = "sprites/skeletonSheet.png";
std::string goblinSheet = "sprites/Goblin.png";
std::string knightSheet = "sprites/Knight.png";
std::string assassinSheet = "sprites/finalAssassin.png";

bool init() {
	// Flag what subsystems to initialize
	// For now, just video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	// Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}
	gWindow = SDL_CreateWindow("Dungeon Crawler", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr) {
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	/* Create a renderer for our window
	 * Use hardware acceleration (last arg)
	 * Choose first driver that can provide hardware acceleration
	 *   (second arg, -1)
	 */
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);// | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == nullptr) {
		std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	// Set renderer draw/clear color
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	// Initialize PNG loading via SDL_image extension library
	int imgFlags = IMG_INIT_PNG;
	int retFlags = IMG_Init(imgFlags);
	if (retFlags != imgFlags) {
		std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return false;
	}
	return true;
}


SDL_Texture* loadImage(std::string fname) {
	SDL_Texture* newText = nullptr;
	SDL_Surface* startSurf = IMG_Load(fname.c_str());
	if (startSurf == nullptr) {
		std::cout << "Unable to load image " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}
	newText = SDL_CreateTextureFromSurface(gRenderer, startSurf);
	if (newText == nullptr) {
		std::cout << "Unable to create texture from " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
	}
	SDL_FreeSurface(startSurf);
	return newText;
}


void close() {
	for (auto i : gTex) {
		SDL_DestroyTexture(i);
		i = nullptr;
	}
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;
	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}
//Are the two objects in range/collide with each other. (In progress)
/*bool inRange( SDL_Rect A, SDL_Rect B )
{
   //checking if any sides of the rectangles touch
    if( (A.y + A.h) < B.y )
        return false;
    if( A.y > (B.y + B.h) )
        return false;
    if((A.x + A.w) < B.x)
        return false;
    if( A.x > (B.x + B.w) )
        return false;
    //there is a collision
    return true;
} */
// cameraRect is passed in so we know if we need to draw or not.
void drawEnemy(Enemy* c, SDL_Rect cameraRect, double timestep){
	// c->setSpriteAndDirection();

	// sanity check
	/*std::cout<<"skeleton x_vel, y_vel: " << c->x_vel << ", " << c->y_vel
	   << std::endl;*/

	c->dstrect.x = c->x_pos-cameraRect.x;
	c->dstrect.y = c->y_pos-cameraRect.y;

	SDL_RenderCopyEx(gRenderer, c->getSpriteSheet(timestep), *(&c->getCharacterRects()[c->spriteIndex]), &(c->dstrect), 0.0, nullptr, c->flip);
}


// cameraRect is passed in so we know if we need to draw or not.
void drawMultiplayer(Player* c, SDL_Rect cameraRect, double timestep){
	// c->setSpriteAndDirection();

	c->dstrect.x = c->x_pos-cameraRect.x;
	c->dstrect.y = c->y_pos-cameraRect.y;

	SDL_RenderCopyEx(gRenderer, c->getSprite(timestep), NULL, &(c->dstrect), 0.0, nullptr, c->flip);
}

void drawProjectile(Arrow* p, SDL_Rect cameraRect){
	p->dstrect.x = p->x_pos-cameraRect.x;
	p->dstrect.y = p->y_pos-cameraRect.y;

	SDL_RenderCopyEx(gRenderer, p->getSprite(), NULL, &(p->dstrect), 0.0, nullptr, p->flip);
}

// cameraRect is passed in so we know if we need to draw or not
void drawWeapon(std::vector<Weapon*> all_weapons, SDL_Rect cameraRect){
	for (auto w : all_weapons){
		//only draw if weapon is currently on the ground
		if (!w->picked_up) {
			w->dstrect.x = w->x_pos-cameraRect.x;
			w->dstrect.y = w->y_pos-cameraRect.y;

			//switches which png to push to renderer, can easily be extended for more weapons
			switch(w->weapon_type) {
				case 1: //is a sword
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				case 2: //is an axe
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				case 3: //is a longsword
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				case 4:
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				case 5:
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				case 6:
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				case 7:
					SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
					break;
				default:	//defaults to stickman for now
				SDL_RenderCopyEx(gRenderer, loadImage("sprites/stick.png"), NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
			}
		}
	}
}

void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Texture* target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    IMG_SavePNG(surface, file_name);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, target);
}

void respawnPlayer(){
	p->x_pos = respawnX;
	p->y_pos = respawnY;
	p->health = 600;
	p->healthBuffer = 600;
}
void respawnMultiplayer(){
	int z;
	for(z = 0; z < playerList.size(); z++){
		playerList[z]->x_pos = respawnX;
		playerList[z]->y_pos = respawnY;
	}
}
// draw the map based on the 2d array of bits
void drawMap(int** bitArray, Chest* chestArray, int chestArraySize, int spawnX, int spawnY, int stairsX, int stairsY){

	//int arrayWidth = sizeof(bitArray)/sizeof(int*);
	//int arrayHeight = sizeof(bitArray[0])/sizeof(int);

	//chestArray = new Chest[2];

	SDL_Texture* chestTexture = loadImage("chestClosed.png");
	//SDL_Rect currChest = {100, 100, 64, 64};

	SDL_Rect currTile = {0, 0, 100, 100};
	SDL_Texture * mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, FloorWidth*100, FloorHeight*100);
	SDL_SetRenderTarget(gRenderer, mTexture);

	//int sX = spawnX/100;
	//int sY = spawnY/100;
	//int stX = stairsX/100;
	//int stY = stairsY/100;

	//bitArray[sX][sY] = 3;
	//bitArray[stX][stY] = 2;

	for(int y = 0; y < FloorHeight; y++){

		for(int x = 0; x < FloorWidth; x++){

			//SDL_RenderCopyEx(gRenderer, tiles[0], NULL, &currTile, 0.0, nullptr, SDL_FLIP_NONE);
			if(bitArray[x][y] == WALL_TILE){
				//draw stairs
				SDL_RenderCopy(gRenderer, tiles[0], NULL, &currTile);
			}

			else if(bitArray[x][y] == FLOOR_TILE){
				SDL_Rect tileTex;
				if((y % 2) == 0){
					tileTex = {(x % 2) * 32, 0, 32, 32};
				}
				else{
					tileTex = {(x % 2) * 32, 32, 32, 32};
				}
				SDL_RenderCopy(gRenderer, tiles[1 + curTileType], &tileTex, &currTile);
			}

			else if(bitArray[x][y] == STAIR_TILE){
				//std::cout << "Stair placed: " << x << " " << y << "\n";
				SDL_RenderCopy(gRenderer, tiles[6], NULL, &currTile);
			}

			else if(bitArray[x][y] == SPAWN_TILE){
				//std::cout << "Spawm placed: " << x << " " << y << "\n";
				SDL_Rect tileTex;
				if((y % 2) == 0){
					tileTex = {(x % 2) * 32, 0, 32, 32};
				}
				else{
					tileTex = {(x % 2) * 32, 32, 32, 32};
				}
				SDL_RenderCopy(gRenderer, tiles[1 + curTileType], &tileTex, &currTile);
			}

			else if(bitArray[x][y] == ENEMY_TILE){
				std::cout << "Enemy placed: " << x << " " << y << "\n";
				SDL_Rect tileTex;
				if((y % 2) == 0){
					tileTex = {(x % 2) * 32, 0, 32, 32};
				}
				else{
					tileTex = {(x % 2) * 32, 32, 32, 32};
				}
				SDL_RenderCopy(gRenderer, tiles[1 + curTileType], &tileTex, &currTile);
			}
			else{
				std::cout << "Default placed: " << x << " " << y << " "<< bitArray[x][y] << "\n";
				SDL_RenderCopy(gRenderer, tiles[1], NULL, &currTile);
			}


			//SDL_RenderCopy(gRenderer, tiles[0], NULL, )
			//SDL_RenderPresent(gRenderer);
			currTile.x += 100; //tile width

		}

		currTile.y += 100; //tile height
		currTile.x = 0;

	}

	//now, draw the chests
	SDL_Rect currChest;
	for(int z = 0; z < chestArraySize; z++){

		int chestX = chestArray[z].getXCoord();
		int chestY = chestArray[z].getYCoord();

		currChest = {chestX, chestY, 64, 64};

		SDL_RenderCopy(gRenderer, chestTexture, NULL, &currChest);
	}

	//SDL_RenderCopy(gRenderer, chestTexture, NULL, &currChest); //render the chest
	//chestArray[0] = *(new Chest(100, 100));

	//currChest = {500, 500, 64, 64};
	//SDL_RenderCopy(gRenderer, chestTexture, NULL, &currChest); //render the chest
	//chestArray[1] = *(new Chest(500, 500));


	save_texture("testMapImg.png", gRenderer, mTexture);

	SDL_SetRenderTarget(gRenderer, NULL);




	//SDL_RenderPresent(gRenderer);
}

int checkIfChestClicked(SDL_Event e, int xOff, int yOff, Floor *floorparam){
	int x;
	bool click = false;
	for(x = 0; x < floorparam->getChestArraySize(); x++){
		click = floorparam->getChestArray()[x].chestClicked(e, xOff, yOff);
		if(click == true){
			break;
		}
	}
	if(click == false){
		x = -1;
	}
	return x; //returns the chest that was clicked on, or -1 if none clicked
}

void drawInventory(std::vector<Weapon*> inv, SDL_Rect cameraRect){
	inventoryx[0][0] = 100;
	inventoryy[0][0] = 400;
	inventoryx[0][1] = 300;
	inventoryy[0][1] = 400;
	inventoryx[0][2] = 500;
	inventoryy[0][2] = 400;
	inventoryx[0][3] = 700;
	inventoryy[0][3] = 400;
	inventoryx[0][4] = 900;
	inventoryy[0][4] = 400;
	inventoryx[0][5] = 1100;
	inventoryy[0][5] = 400;

	inventoryx[1][0] = 100;
	inventoryy[1][0] = 500;
	inventoryx[1][1] = 300;
	inventoryy[1][1] = 500;
	inventoryx[1][2] = 500;
	inventoryy[1][2] = 500;
	inventoryx[1][3] = 700;
	inventoryy[1][3] = 500;
	inventoryx[1][4] = 900;
	inventoryy[1][4] = 500;
	inventoryx[1][5] = 1100;
	inventoryy[1][5] = 500;

	inventoryx[2][0] = 100;
	inventoryy[2][0] = 620;
	inventoryx[2][1] = 300;
	inventoryy[2][1] = 620;
	inventoryx[2][2] = 500;
	inventoryy[2][2] = 620;
	inventoryx[2][3] = 700;
	inventoryy[2][3] = 620;
	inventoryx[2][4] = 900;
	inventoryy[2][4] = 620;
	inventoryx[2][5] = 1100;
	inventoryy[2][5] = 620;
	int x = 0;
	int y = 0;
	for(auto w: inv){
		w->dstrect.x = inventoryx[x][y];
		w->dstrect.y = inventoryy[x][y];
		y++;

		//set color based on rarity

			if(w->rarity == 1){

			//white, common
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			}

			else if(w->rarity == 2){

			//green, uncommon
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x80, 0x00, 0xFF);

			} 

			else if(w->rarity == 3){

			//blue, rare
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0xFF);

			} 

			else if(w->rarity == 4){

			//purple, epic
			SDL_SetRenderDrawColor(gRenderer, 0x80, 0x00, 0x80, 0xFF);

			}

			else if(w->rarity == 5){

			//yellow, legendary
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);

			}

			//copy the sdl rect to the renderer
			//make new SDL_Rect
			SDL_Rect* rarityRect = new SDL_Rect{w->dstrect.x - 60, w->dstrect.y + 42, 30, 30};

			//SDL_RenderFillRect(gRenderer, &(w->dstrect));
			SDL_RenderFillRect(gRenderer, rarityRect);

			//try to print weapon damage

			char Buffer[256];
			int number = w->damage;

			//sprintf(Buffer,"%i",number);

			SDL_Color col = {0, 0, 0};
			//TTF_Font* font = TTF_OpenFont("Lato-Black.ttf", 8);

			SDL_Surface *text_surface = TTF_RenderText_Solid(font, Buffer, col);
			SDL_Texture *text = SDL_CreateTextureFromSurface(gRenderer, text_surface);


			SDL_RenderCopyEx(gRenderer, text, NULL, rarityRect, 0.0, nullptr, SDL_FLIP_NONE);


			SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);


		SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
	}
}

void drawChestInventory(std::vector<Weapon*> inv, SDL_Rect cameraRect){
	//SDL_RenderClear(gRenderer);
	inventoryx[0][0] = 100;
	inventoryy[0][0] = 400;
	inventoryx[0][1] = 300;
	inventoryy[0][1] = 400;
	inventoryx[0][2] = 500;
	inventoryy[0][2] = 400;
	int x = 0;
	int y = 0;
	for(auto w: inv){

		if(w != nullptr){
			if(w->picked_up == false){
			w->dstrect.x = inventoryx[x][y];
			w->dstrect.y = inventoryy[x][y];
			y++;

			//set color based on rarity

			if(w->rarity == 1){

			//white, common
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			}

			else if(w->rarity == 2){

			//green, uncommon
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x80, 0x00, 0xFF);

			} 

			else if(w->rarity == 3){

			//blue, rare
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0xFF, 0xFF);

			} 

			else if(w->rarity == 4){

			//purple, epic
			SDL_SetRenderDrawColor(gRenderer, 0x80, 0x00, 0x80, 0xFF);

			}

			else if(w->rarity == 5){

			//yellow, legendary
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);

			}

			//copy the sdl rect to the renderer
			//make new SDL_Rect
			SDL_Rect* rarityRect = new SDL_Rect{w->dstrect.x - 60, w->dstrect.y + 42, 30, 30};

			//SDL_RenderFillRect(gRenderer, &(w->dstrect));
			SDL_RenderFillRect(gRenderer, rarityRect);

			//try to print weapon damage

			char Buffer[256];
			int number = w->damage;

			//sprintf(Buffer,"%i",number);

			SDL_Color col = {0, 0, 0};
			//TTF_Font* font = TTF_OpenFont("Lato-Black.ttf", 8);

			SDL_Surface *text_surface = TTF_RenderText_Solid(font, Buffer, col);
			SDL_Texture *text = SDL_CreateTextureFromSurface(gRenderer, text_surface);


			SDL_RenderCopyEx(gRenderer, text, NULL, rarityRect, 0.0, nullptr, SDL_FLIP_NONE);


			SDL_RenderCopyEx(gRenderer, w->sprites[0], NULL, &(w->dstrect), 0.0, nullptr, SDL_FLIP_NONE);
			delete rarityRect;
			}
			else{
				y++;
			}
		}
		else{
		y++;
		}

	}

}

//mouse click pickup of weapon on floor, adds weapon to inventory
//only call if mouse was confirmed to have been left clicked
void checkWeaponPickup(std::vector<Weapon*> p_all_weapons, SDL_Event e, std::vector<Weapon*> p_inventory, SDL_Rect cameraBox){
	//declaring variables for the mouse click locations we will be tracking
	int mouseClickX = e.button.x;
	int mouseClickY = e.button.y;

	for (auto w : p_all_weapons){

			//continues immediately if this weapon is not on the floor
		if (w->picked_up) {
			continue;
		}

		int weapon_x = (int) w->x_pos - cameraBox.x;
		int weapon_y = (int) w->y_pos - cameraBox.y;
			//check to see if mouse click was within bounds of the weapon
		if (mouseClickX > weapon_x && mouseClickX < weapon_x + w->x_size && mouseClickY > weapon_y && mouseClickY < weapon_y + w->y_size) {
			p->weapons.push_back(w);
			p->currWeapon = w->weapon_type;
			w->picked_up = true;
					//weapon is now picked up
				break;	//stops us from picking up more than one weapon at a time
			}
		}
	}

int collision(Arrow* proj, Character* c){

	SDL_Rect result;

	if(SDL_IntersectRect(&c->characterTree->root->hitbox, proj->getHitBox(), &result)){
		//std::cout<<"HIT"<<std::endl;
		return -1;
	}
	return 0;
}

int collision(int** bitArray, Projectile* proj){
	int charX = (proj->x_pos);
	int charY = (proj->y_pos);
	int charXSize = (proj->x_size);
	int charYSize = (proj->y_size);

	//now, we need to check which tile(s) the entity is on
	//use integer division I guess

	int tileXLeft = (int)(floor(charX/100.0));	//Round down
	int tileXRight = (int) (floor((charX + charXSize)/100.0));
	int tileYTop = (int)(floor(charY/100.0));	//Round down
	int tileYBottom = (int) (floor((charY + charYSize)/100.0));
	if(tileXLeft<0 || tileXLeft>FloorHeight) return -1;
	if(tileXRight<0 || tileXRight>FloorHeight) return -1;
	if(tileYTop<0 || tileYTop > FloorWidth) return -1;
	if(tileYBottom < 0 || tileYBottom > FloorWidth) return -1;

	//now, check the collisions

	if(proj->x_vel < 0){
		//moving left, so check left
		if(bitArray[tileXLeft][tileYTop] == 0 || bitArray[tileXLeft][tileYBottom] == 0){
			//entityOne->x_pos += 5;
			return -1;
			//entityOne->x_vel = 0;

		}

	}

	else if(proj->x_vel > 0){
		//moving right, so check right
		if(bitArray[tileXRight][tileYTop] == 0 || bitArray[tileXRight][tileYBottom] == 0){
			//entityOne->x_pos -=5;
			return -1;
			//entityOne->x_vel = 0;
		}

	}

	if(proj->y_vel < 0){
		//moving up, so check the top
		if(bitArray[tileXLeft][tileYTop] == 0 || bitArray[tileXRight][tileYTop] == 0){
			//entityOne->y_pos += 5;
			return -1;
			//entityOne->y_vel = 0;
		}

	}

	else if(proj->y_vel > 0){
		//moving down, so check the bottom
		if(bitArray[tileXLeft][tileYBottom] == 0 || bitArray[tileXRight][tileYBottom] == 0){
			//entityOne->y_pos -= 5;
			return -1;
			//entityOne->y_vel = 0;
		}

	}
	return 0;
}

//collision method for a player/enemy and the map
void collision(int** bitArray, Character* entityOne, double timestep){

	//getting info about the current entity
	int charX = (entityOne->x_pos)+10;
	int charY = (entityOne->y_pos)+20;
	int charXSize = (entityOne->x_size)-30;
	int charYSize = (entityOne->y_size)-20;
	int charYMid = charY+charYSize/2;
	int charXMid = charX+charXSize/2;


	//now, we need to check which tile(s) the entity is on
	//use integer division I guess

	int tileXLeft = (int)(floor(charX/100.0));	//Round down
	int tileXRight = (int) (floor((charX + charXSize)/100.0));
	int tileYTop = (int)(floor(charY/100.0));	//Round down
	int tileYBottom = (int) (floor((charY + charYSize)/100.0));
	int tileYMid= (int) (floor((charYMid)/100));
	int tileXMid= (int) (floor((charXMid)/100));
	//now, check the collisions
/*
	if(entityOne->x_vel < 0){
		//moving left, so check left
		if(bitArray[tileXLeft][tileYTop] == 0 || bitArray[tileXLeft][tileYBottom] == 0){
			//entityOne->x_pos += 5;
			std::cout<<"left"<<std::endl;
			entityOne->x_pos -= (entityOne->x_vel * timestep);
			//entityOne->x_vel = 0;

		}

	}
	else if(entityOne->x_vel > 0){
		//moving right, so check right
		if(bitArray[tileXRight][tileYTop] == 0 || bitArray[tileXRight][tileYBottom] == 0){
			//entityOne->x_pos -=5;
			std::cout<<"right"<<std::endl;
			entityOne->x_pos -= (entityOne->x_vel * timestep);
			//entityOne->x_vel = 0;
		}
]
	}
	if(entityOne->y_vel < 0){
		//moving up, so check the top
		if(bitArray[tileXLeft][tileYTop] == 0 || bitArray[tileXRight][tileYTop] == 0){
			//entityOne->y_pos += 5;
			std::cout<<"up"<<std::endl;
			entityOne->y_pos -= (entityOne->y_vel * timestep);
			//entityOne->y_vel = 0;
		}

	}

	else if(entityOne->y_vel > 0){
		//moving down, so check the bottom
		if(bitArray[tileXLeft][tileYBottom] == 0 || bitArray[tileXRight][tileYBottom] == 0){
			//entityOne->y_pos -= 5;
			std::cout<<"down"<<std::endl;
			entityOne->y_pos -= (entityOne->y_vel * timestep);
			//entityOne->y_vel = 0;
		}

	}
*/


	if(entityOne->x_vel < 0){
		//moving left, so check left
		if(bitArray[tileXLeft][tileYMid] == 0){
			//entityOne->x_pos += 5;
			entityOne->x_pos -= (entityOne->x_vel * timestep);
			//entityOne->x_vel = 0;

		}
		else if(charYMid%100>60 && bitArray[tileXLeft][tileYMid+1]==0)
			entityOne->x_pos -= (entityOne->x_vel*timestep);
		else if(charYMid%100<40 && bitArray[tileXLeft][tileYMid-1]==0)
			entityOne->x_pos -= (entityOne->x_vel*timestep);

	}
	else if(entityOne->x_vel > 0){
		//moving right, so check right
		if(bitArray[tileXRight][tileYMid] == 0){
			//entityOne->x_pos -=5;
			entityOne->x_pos -= (entityOne->x_vel * timestep);
			//entityOne->x_vel = 0;
		}
		else if(charYMid%100>60 && bitArray[tileXRight][tileYMid+1]==0)
			entityOne->x_pos -= (entityOne->x_vel*timestep);
		else if(charYMid%100<40 && bitArray[tileXRight][tileYMid-1]==0)
			entityOne->x_pos -= (entityOne->x_vel*timestep);

	}

	if(entityOne->y_vel < 0){
		//moving up, so check the top
		if(bitArray[tileXMid][tileYTop] == 0){
			//entityOne->y_pos += 5;
			entityOne->y_pos -= (entityOne->y_vel * timestep);
			//entityOne->y_vel = 0;
		}
		else if(charXMid%100>80 && bitArray[tileXMid+1][tileYTop]==0)
			entityOne->y_pos -= (entityOne->y_vel*timestep);
		else if(charXMid%100<20 && bitArray[tileXMid-1][tileYTop]==0)
			entityOne->y_pos -= (entityOne->y_vel*timestep);
	}

	else if(entityOne->y_vel > 0){
		//moving down, so check the bottom
		if(bitArray[tileXMid][tileYBottom] == 0){
			//entityOne->y_pos -= 5;
			entityOne->y_pos -= (entityOne->y_vel * timestep);
			//entityOne->y_vel = 0;
		}
		else if(charXMid%100>80 && bitArray[tileXMid+1][tileYBottom]==0)
			entityOne->y_pos -= (entityOne->y_vel*timestep);
		else if(charXMid%100<20 && bitArray[tileXMid-1][tileYBottom]==0)
			entityOne->y_pos -= (entityOne->y_vel*timestep);

	}

}



//create collision methods here so we have access to both player and enemy classes
void collision(Character* entityOne, Character* entityTwo, double timestep)
{
	//check if collision with entityOne or entityTwo
	SDL_Rect result;
	int slideThreshold=10;
	if(entityOne->isGoblin ^ entityTwo->isGoblin) return;
	if(SDL_IntersectRect(&entityOne->characterTree->root->hitbox, &entityTwo->characterTree->root->hitbox, &result))
	{


		//tried just offsetting by width and height of result rect, but didn't work as expected
		//X COLLISION
		if(result.h >= slideThreshold){
			//compare x velocities and adjust for collisions
			//case 1, entityTwo and entityOne are moving directly into each other
			if(((entityTwo->x_vel > 0) && (entityOne->x_vel < 0)) || ((entityTwo->x_vel < 0) && (entityOne->x_vel > 0)))
			{
				entityOne->x_pos -= (entityOne->x_vel * timestep);
				entityTwo->x_pos -= (entityTwo->x_vel * timestep);
				//entityOne->x_vel = 0;
				//entityTwo->x_vel = 0;
			}
			//case 2, entityOne is not moving in x direction
			else if((entityOne->x_vel == 0) && (entityTwo->x_vel != 0)){
				entityTwo->x_pos -= (entityTwo->x_vel * timestep);
				//entityTwo->x_vel = 0;
			}
			//case 3, entityTwo is not moving in x direction
			else if((entityTwo->x_vel == 0) && (entityTwo->y_vel != 0) && (entityOne->x_vel != 0)){
				entityOne->x_pos -= (entityOne->x_vel * timestep);
				//entityOne->x_vel = 0;
			}
			else if((entityTwo->x_vel == 0) && (entityOne->x_vel != 0)){
				entityOne->x_pos -= (entityOne->x_vel * timestep);//= entityOne->x_pos;
				//entityOne->x_vel = 0;
			}
			//if entityOne is faster than the entityTwo/ they caught up to them
			else if((((entityOne->x_vel > 0) && (entityTwo->x_vel > 0)) && (entityOne->x_vel > entityTwo->x_vel)) || (((entityOne->x_vel < 0) && (entityTwo->x_vel < 0)) && (entityOne->x_vel < entityTwo->x_vel))){ //  ((entityOne->x_vel < 0) && (entityTwo->x_vel < 0)))
				entityOne->x_pos -= (entityOne->x_vel * timestep);
				//entityOne->x_vel = entityTwo->x_vel;
			}
			//if entityTwo is faster than the entityOne/ they caught up to them
			else if((((entityOne->x_vel > 0) && (entityTwo->x_vel > 0)) && (entityOne->x_vel < entityTwo->x_vel)) || (((entityOne->x_vel < 0) && (entityTwo->x_vel < 0)) && (entityOne->x_vel > entityTwo->x_vel))){// || ((entityTwo->x_vel < 0) && (entityOne->x_vel < 0)))
				entityTwo->x_pos -= (entityTwo->x_vel * timestep);
				//entityTwo->x_vel = entityOne->x_vel;
			}
			else
			{
				entityOne->x_pos -= (entityOne->x_vel * timestep);
				entityTwo->x_pos -= (entityTwo->x_vel * timestep);

			}
		}

		//Y COLLISION


		//compare y velocities and adjust for collisions
		if(result.w >= slideThreshold){
			//case 1, entityTwo and entityOne are moving directly into each other
			if(((entityTwo->y_vel > 0) && (entityOne->y_vel < 0)) || ((entityTwo->y_vel < 0) && (entityOne->y_vel > 0)))
			{
				entityOne->y_pos -= (entityOne->y_vel * timestep);
				entityTwo->y_pos -= (entityTwo->y_vel * timestep);

				//entityOne->y_vel = 0;
				//entityTwo->y_vel = 0;
			}
			//case 2, entityOne is not moving in y direction
			else if((entityOne->y_vel == 0) && (entityTwo->y_vel != 0)){
				entityTwo->y_pos -= (entityTwo->y_vel * timestep);
				//entityTwo->y_vel = 0;
			}
			//case 3, entityTwo is not moving in y direction
			else if((entityTwo->y_vel == 0) && (entityTwo->x_vel != 0) && (entityOne->y_vel != 0)){
				entityOne->y_pos -= (entityOne->y_vel*timestep);
				//entityOne->y_vel=0;
			}
			else if((entityTwo->y_vel == 0) && (entityOne->y_vel != 0)){
				entityOne->y_pos -= (entityOne->y_vel * timestep);//= entityOne->x_pos;
				//entityOne->y_vel=0;
			}
			//case 3, entityTwo is not moving in y direction & entityOne is above or below entire entityTwo
			else if(((entityTwo->y_vel == 0) && (entityOne->y_vel != 0)) && (((entityOne->y_pos)+BOX_HEIGHT > (entityTwo->y_pos)+BOX_HEIGHT) || ((entityOne->y_pos)+BOX_HEIGHT < (entityTwo->y_pos)))){
				entityOne->y_pos -= (entityOne->y_vel * timestep);
			}
			//if entityOne is faster than the entityTwo/ they caught up to them
			else if((((entityOne->y_vel > 0) && (entityTwo->y_vel > 0)) && (entityOne->y_vel > entityTwo->y_vel)) || (((entityOne->y_vel < 0) && (entityTwo->y_vel < 0)) && (entityOne->y_vel < entityTwo->y_vel))){ //  ((entityOne->x_vel < 0) && (entityTwo->x_vel < 0)))
				entityOne->y_pos -= (entityOne->y_vel * timestep);
				//entityOne->y_vel=entityTwo->y_vel;

			}
			//if entityTwo is faster than the entityOne/ they caught up to them
			else if((((entityOne->y_vel > 0) && (entityTwo->y_vel > 0)) && (entityOne->y_vel < entityTwo->y_vel)) || (((entityOne->y_vel < 0) && (entityTwo->y_vel < 0)) && (entityOne->y_vel > entityTwo->y_vel))){ // || ((entityTwo->x_vel < 0) && (entityOne->x_vel < 0)))
				entityTwo->y_pos -= (entityTwo->y_vel * timestep);
				entityTwo->y_vel=entityOne->y_vel;
			}
			else
			{
				entityOne->y_pos -= (entityOne->y_vel * timestep);
				entityTwo->y_pos -= (entityTwo->y_vel * timestep);
			}
		}
	}
}



//created a second collision just for enemies, orignal one is wonky with the swarmer goblins
void collision(Enemy* entityOne, Enemy* entityTwo, double timestep)
{
	//check if collision with entityOne or entityTwo
	SDL_Rect result;
	int slideThreshold=10;
	if(entityOne->isGoblin ^ entityTwo->isGoblin) return;
	if(SDL_IntersectRect(&entityOne->characterTree->root->hitbox, &entityTwo->characterTree->root->hitbox, &result))
	{

		//tried just offsetting by width and height of result rect, but didn't work as expected
		//X COLLISION

		//compare x velocities and adjust for collisions
		//case 1, entityTwo and entityOne are moving directly into each other
		if(result.h>=slideThreshold){
			if(((entityTwo->x_vel > 0) && (entityOne->x_vel < 0)) || ((entityTwo->x_vel < 0) && (entityOne->x_vel > 0)))
			{
				entityOne->x_pos -= (entityOne->x_vel * timestep);
				entityTwo->x_pos -= (entityTwo->x_vel * timestep);
			}
			//case 2, entityOne is not moving in x direction
			else if((entityOne->x_vel == 0) && (entityTwo->x_vel != 0))
				entityTwo->x_pos -= (entityTwo->x_vel * timestep);
			//case 3, entityTwo is not moving in x direction
			else if((entityTwo->x_vel == 0) && (entityTwo->y_vel != 0) && (entityOne->x_vel != 0))
				entityOne->x_pos -= (entityTwo->x_vel * timestep);

			else if((entityTwo->x_vel == 0) && (entityOne->x_vel != 0))
				entityOne->x_pos -= (entityOne->x_vel * timestep);//= entityOne->x_pos;
			//if entityOne is faster than the entityTwo/ they caught up to them
			else if((((entityOne->x_vel > 0) && (entityTwo->x_vel > 0)) && (entityOne->x_vel >= entityTwo->x_vel)) || (((entityOne->x_vel < 0) && (entityTwo->x_vel < 0)) && (entityOne->x_vel <= entityTwo->x_vel))) //  ((entityOne->x_vel < 0) && (entityTwo->x_vel < 0)))
				entityOne->x_pos -= (entityOne->x_vel * timestep);
			//if entityTwo is faster than the entityOne/ they caught up to them
			else if((((entityOne->x_vel > 0) && (entityTwo->x_vel > 0)) && (entityOne->x_vel <= entityTwo->x_vel)) || (((entityOne->x_vel < 0) && (entityTwo->x_vel < 0)) && (entityOne->x_vel >= entityTwo->x_vel)))// || ((entityTwo->x_vel < 0) && (entityOne->x_vel < 0)))
				entityTwo->x_pos -= (entityTwo->x_vel * timestep);
			else
			{
				entityOne->x_pos -= (entityOne->x_vel * timestep);
				entityTwo->x_pos -= (entityTwo->x_vel * timestep);
			}
		}
		//Y COLLISION


		//compare y velocities and adjust for collisions
		//case 1, entityTwo and entityOne are moving directly into each other
		if(result.w>=slideThreshold){
			if(((entityTwo->y_vel > 0) && (entityOne->y_vel < 0)) || ((entityTwo->y_vel < 0) && (entityOne->y_vel > 0)))
			{
				entityOne->y_pos -= (entityOne->y_vel * timestep);
				entityTwo->y_pos -= (entityTwo->y_vel * timestep);
			}
			//case 2, entityOne is not moving in y direction
			else if((entityOne->y_vel == 0) && (entityTwo->y_vel != 0))
				entityTwo->y_pos -= (entityTwo->y_vel * timestep);
			//case 3, entityTwo is not moving in y direction
			else if((entityTwo->y_vel == 0) && (entityTwo->x_vel != 0) && (entityOne->y_vel != 0))
				entityOne->y_pos -= (entityOne->y_vel*timestep);
			else if((entityTwo->y_vel == 0) && (entityOne->y_vel != 0))
				entityOne->y_pos -= (entityOne->y_vel * timestep);//= entityOne->x_pos;
			//case 3, entityTwo is not moving in y direction & entityOne is above or below entire entityTwo
			else if(((entityTwo->y_vel == 0) && (entityOne->y_vel != 0)) && (((entityOne->y_pos)+BOX_HEIGHT > (entityTwo->y_pos)+BOX_HEIGHT) || ((entityOne->y_pos)+BOX_HEIGHT < (entityTwo->y_pos))))
				entityOne->y_pos -= (entityOne->y_vel * timestep);
			//if entityOne is faster than the entityTwo/ they caught up to them
			else if((((entityOne->y_vel > 0) && (entityTwo->y_vel > 0)) && (entityOne->y_vel >= entityTwo->y_vel)) || (((entityOne->y_vel < 0) && (entityTwo->y_vel < 0)) && (entityOne->y_vel <= entityTwo->y_vel))) //  ((entityOne->x_vel < 0) && (entityTwo->x_vel < 0)))
				entityOne->y_pos -= (entityOne->y_vel * timestep);
			//if entityTwo is faster than the entityOne/ they caught up to them
			else if((((entityOne->y_vel > 0) && (entityTwo->y_vel > 0)) && (entityOne->y_vel <= entityTwo->y_vel)) || (((entityOne->y_vel < 0) && (entityTwo->y_vel < 0)) && (entityOne->y_vel >= entityTwo->y_vel)))// || ((entityTwo->x_vel < 0) && (entityOne->x_vel < 0)))
				entityTwo->y_pos -= (entityTwo->y_vel * timestep);
			else
			{
				entityOne->y_pos -= (entityOne->y_vel * timestep);
				entityTwo->y_pos -= (entityTwo->y_vel * timestep);
			}
		}

	}
}

void updateFloorSize(){
	//I do not wana write out this long equation every time so just use this tiny function for now.
	FloorHeight = FloorWidth = (int)(pow( (double)(curFloorNum) / (double)(NUM_FLOORS-1) , 2.0) * (double)(F_FLOOR_SIZE-S_FLOOR_SIZE)) + S_FLOOR_SIZE + 1;
	BG_HEIGHT = BG_WIDTH = (FloorWidth * 100);
}


void loadSprites(){
	Assassin::characterSheet = loadImage(assassinSheet);
	//13 assassin sprites, SDL_Rect for each
	for (int i = 0; i < 13; i++) {
		SDL_Rect* currSprite = new SDL_Rect{0,0,0,0};
		currSprite->x = i * ASSASSIN_WIDTH;
		currSprite->y = 0;
		currSprite->w = ASSASSIN_WIDTH;
		currSprite->h = ASSASSIN_HEIGHT;
		Assassin::characterRects.push_back(currSprite);
	}
	Archer::characterSheet = loadImage(skeletonSheet);
	//create an SDL_Rect for each skeleton sprite, currently only 4
	for(int i = 0; i < 4; i++)
	{
		SDL_Rect* currSprite = new SDL_Rect{0,0,0,0};
		currSprite->x = i * SKELETON_WIDTH;
		currSprite->y = 0;
		currSprite->w = SKELETON_WIDTH;
		currSprite->h = SKELETON_HEIGHT;
		Archer::characterRects.push_back(currSprite);
	}

	Arrow::sprites.push_back(loadImage("sprites/arrowDown.png"));
    Arrow::sprites.push_back(loadImage("sprites/arrowUp.png"));
    Arrow::sprites.push_back(loadImage("sprites/arrowLeft.png"));

	Enemy::characterSheet = loadImage(skeletonSheet);

	//create an SDL_Rect for each skeleton sprite, currently only 4
	for(int i = 0; i < 4; i++)
	{
		SDL_Rect* currSprite = new SDL_Rect{0,0,0,0};
		currSprite->x = i * SKELETON_WIDTH;
		currSprite->y = 0;
		currSprite->w = SKELETON_WIDTH;
		currSprite->h = SKELETON_HEIGHT;
		Enemy::characterRects.push_back(currSprite);
	}

	Goblin::characterSheet = loadImage(goblinSheet);
	//create an SDL_Rect for each goblin sprite, only 3 for now
	for(int i = 0; i < 9; i++)
	{
		SDL_Rect* currSprite = new SDL_Rect{0,0,0,0};
		currSprite->x = i * GOBLIN_WIDTH;
		currSprite->y = 0;
		currSprite->w = GOBLIN_WIDTH;
		currSprite->h = GOBLIN_HEIGHT;
		Goblin::characterRects.push_back(currSprite);
	}

	//load knight sprites - will probably need to add more
	Knight::characterSheet = loadImage(knightSheet);
	//create SDL rects for each sprite, 13 right now
	for(int i = 0; i < 13; i++)
	{
		SDL_Rect* currSprite = new SDL_Rect{0,0,0,0};
		currSprite->x = i * KNIGHT_WIDTH;
		currSprite->y = 0;
		currSprite->w = KNIGHT_WIDTH;
		currSprite->h = KNIGHT_HEIGHT;
		Knight::characterRects.push_back(currSprite);
	}

	Player::sprites.push_back(loadImage("sprites/mainCharacterfrontWithWeapon.png")); // 0
	Player::sprites.push_back(loadImage("sprites/mainCharacterBackWithWeapon.png")); // 1
	Player::sprites.push_back(loadImage("sprites/swingStart.png")); // 2
	Player::sprites.push_back(loadImage("sprites/swingEnd.png")); // 3
	Player::sprites.push_back(loadImage("sprites/swingfront.png")); // 4
	Player::sprites.push_back(loadImage("sprites/swingUpFronnt.png")); // 5
	Player::sprites.push_back(loadImage("sprites/mainCharacterfrontWithWeaponEquipped.png")); // 6
	Player::sprites.push_back(loadImage("sprites/mainCharacterBackWithWeaponEquipped.png")); // 7
	Player::sprites.push_back(loadImage("sprites/swingUpFrontEquipped.png")); // 8
	Player::sprites.push_back(loadImage("sprites/swingfrontDownEquipped.png")); // 9
	Player::sprites.push_back(loadImage("sprites/swingStartEquipped.png")); // 10
	Player::sprites.push_back(loadImage("sprites/swingEndEquipped.png")); // 11
	Player::sprites.push_back(loadImage("sprites/mainCharacterfrontWithWeaponAxe.png")); // 12
	Player::sprites.push_back(loadImage("sprites/swingUpFrontAxe.png")); // 13
	Player::sprites.push_back(loadImage("sprites/swingfrontAxe.png")); // 14
	Player::sprites.push_back(loadImage("sprites/mainCharacterBackWithWeaponAxe.png")); // 15
	Player::sprites.push_back(loadImage("sprites/swingStartAxe.png")); // 16
	Player::sprites.push_back(loadImage("sprites/swingEndAxe.png")); // 17
	Player::sprites.push_back(loadImage("sprites/longsword/mainCharacterBackWithWeaponLongSword.png")); //36
	Player::sprites.push_back(loadImage("sprites/longsword/mainCharacterfrontWithWeaponLongSword.png")); //19
	Player::sprites.push_back(loadImage("sprites/longsword/swingEndLongSword.png")); //20
	Player::sprites.push_back(loadImage("sprites/longsword/swingfrontLongSword.png")); //21
	Player::sprites.push_back(loadImage("sprites/longsword/swingStartLongSword.png")); //22
	Player::sprites.push_back(loadImage("sprites/longsword/swingUpFronntLongSword.png")); //23
	Player::sprites.push_back(loadImage("sprites/hammer/HammerBack.png")); //24
	Player::sprites.push_back(loadImage("sprites/hammer/mainCharacterfrontWithWeaponHammer.png")); //25
	Player::sprites.push_back(loadImage("sprites/hammer/swingEndHammer.png")); //26
	Player::sprites.push_back(loadImage("sprites/hammer/swingfrontHammer.png")); //27
	Player::sprites.push_back(loadImage("sprites/hammer/swingStartHammer.png")); //28
	Player::sprites.push_back(loadImage("sprites/hammer/swingUpFronntHammer.png")); //29
	Player::sprites.push_back(loadImage("sprites/trident/mainCharacterBackWithWeaponTrident.png")); //30
	Player::sprites.push_back(loadImage("sprites/trident/mainCharacterfrontWithWeaponTrident.png")); //31
	Player::sprites.push_back(loadImage("sprites/trident/swingEndTrident.png")); //32
	Player::sprites.push_back(loadImage("sprites/trident/swingfrontTrident.png")); //33
	Player::sprites.push_back(loadImage("sprites/trident/swingStartTrident.png")); //34
	Player::sprites.push_back(loadImage("sprites/trident/swingUpFronntTrident.png")); //35
	//left/right
	Player::sprites.push_back(loadImage("sprites/mainCharactersideWaysLeftWithWeapon.png")); // 36
	Player::sprites.push_back(loadImage("sprites/swingleft.png")); // 37
	Player::sprites.push_back(loadImage("sprites/swingright.png")); // 38
	Player::sprites.push_back(loadImage("sprites/sidewaysSwingUp.png")); // 39
	Player::sprites.push_back(loadImage("sprites/leftWithSwordEquipped.png")); // 40
	Player::sprites.push_back(loadImage("sprites/sidewaysSwingUpEquipped.png")); // 41
	Player::sprites.push_back(loadImage("sprites/swingleftEquipped.png")); // 42
	Player::sprites.push_back(loadImage("sprites/mainCharactersideWaysLeftWithWeaponAxe.png")); // 43
	Player::sprites.push_back(loadImage("sprites/sidewaysSwingUpAxe.png")); // 44
	Player::sprites.push_back(loadImage("sprites/swingleftAxe.png")); // 45
	Player::sprites.push_back(loadImage("sprites/longsword/mainCharactersideWaysLeftWithWeaponLongSword.png")); //46
	Player::sprites.push_back(loadImage("sprites/longsword/sidewaysSwingUpLongSword.png")); //47
	Player::sprites.push_back(loadImage("sprites/longsword/swingleftLongSword.png")); //48
	Player::sprites.push_back(loadImage("sprites/hammer/mainCharactersideWaysLeftWithWeaponHammer.png")); //49
	Player::sprites.push_back(loadImage("sprites/hammer/sidewaysSwingUpHammer.png")); //50
	Player::sprites.push_back(loadImage("sprites/hammer/swingleftHammer.png")); //51
	Player::sprites.push_back(loadImage("sprites/trident/mainCharactersideWaysLeftWithWeaponTrident.png")); //52
	Player::sprites.push_back(loadImage("sprites/trident/sidewaysSwingUpTrident.png")); //53
	Player::sprites.push_back(loadImage("sprites/trident/swingleftTrident.png")); //54
	Player::sprites.push_back(loadImage("sprites/bow/BowBack.png")); //55
	Player::sprites.push_back(loadImage("sprites/bow/BowDown.png")); //56
	Player::sprites.push_back(loadImage("sprites/bow/LeftBow.png")); //57
	Player::sprites.push_back(loadImage("sprites/bow/BowBackShot.png")); //58
	Player::sprites.push_back(loadImage("sprites/bow/BowDownShot.png")); //59
	Player::sprites.push_back(loadImage("sprites/bow/LeftBowShot.png")); //60


}



//main method
int main() {

		if (!init()) {
			std::cout << "Failed to initialize!" << std::endl;
			close();
			return 1;
		}

	//gamestate variables
	int gamestate = 0; //the current gamestate. 0 will be menu, 1 will be playing the game, 2 will be quit
	bool quit = false; //set to true if the game should be quit
	bool paused = false; //set to true if the game is paused
	bool iPaused = false;
	bool iChest = false; //set to true if interacting with chest
	//player dimensions

	//camera box & thirds of camera
	SDL_Rect cameraRect = {750, 750, 1280, 720}; //setting the start coordinates for the map
	GameMaster gameMaster;
	//x and y coordinates/velocity/deltav
	double x_coord = SCREEN_WIDTH/2 - BOX_WIDTH/2;//cameraRect.w/2;;//SCREEN_WIDTH/2;// - PLAYER_WIDTH/2;
	double y_coord = SCREEN_HEIGHT/2 - BOX_HEIGHT/2;//SCREEN_HEIGHT/2;// - (PLAYER_HEIGHT * 2);
	//double x_vel = 0.0;
	//double x_deltav = 0.0;
	//double y_vel = 0.0;
	//double y_deltav = 0.0;

	//SDL_Rect moveBox = {BG_WIDTH/2 - PLAYER_WIDTH/2, BG_HEIGHT/2 - PLAYER_HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT};
	//displaying the start menu
	gTex.push_back(loadImage("menuImages/mainmenu.png")); //load in the start menu image
	gTex.push_back(loadImage("menuImages/pausemenu.png")); //load in the start menu image
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, gTex[0], NULL, NULL);
	SDL_RenderPresent(gRenderer);
	SDL_Event e; //declaring the SDL event type we will be using to detect specific events

	while(gamestate == 0) { //while we are on the start menu
		//declaring variables for the mouse click locations we will be tracking
		int mouseClickX;
		int mouseClickY;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_MOUSEBUTTONDOWN){
				if(e.button.button == SDL_BUTTON_LEFT){ //if the event type is a left mouse click
					//obtain the clicks x and y coordinates
					mouseClickX = e.button.x;
					mouseClickY = e.button.y;
					//check if button clicked is start
					if( (mouseClickX > 390) && (mouseClickX < 890) && (mouseClickY > 432) && (mouseClickY < 532) ){
						gamestate = 1; //enter the game, since start was pressed
					}
					//check if button clicked is quit
					else if( (mouseClickX > 391) && (mouseClickX < 891) && (mouseClickY > 562) && (mouseClickY < 662) ){
						quit = true; //setting the quit boolean to true
						gamestate = 2; //gamestate for quit
					}
				}
			}
		}
	}
	//check if the game was quit from the menu
	if(quit == true){
		close();
		return 0;
	}

	//load in the lobby menu screens
	lobby.push_back(loadImage("menuImages/lobbychoice.png"));
	lobby.push_back(loadImage("menuImages/lobbyconnect.png"));
	lobby.push_back(loadImage("menuImages/lobbyp1.png"));
	lobby.push_back(loadImage("menuImages/lobbyp2.png"));
	lobby.push_back(loadImage("menuImages/lobbyp3.png"));
	lobby.push_back(loadImage("menuImages/lobbyp4.png"));
	//display the first lobby menu
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, lobby[0], NULL, NULL);
	SDL_RenderPresent(gRenderer);

	int num_of_players = 1;
	//Join server loop

	while(gamestate == 1){
		//declaring variables for the mouse click locations we will be tracking
		int mouseClickX;
		int mouseClickY;
		while(SDL_PollEvent(&e)){

			if(e.type == SDL_MOUSEBUTTONDOWN){
				if(e.button.button == SDL_BUTTON_LEFT){ //if the event type is a left mouse click
					//obtain the clicks x and y coordinates
					mouseClickX = e.button.x;
					mouseClickY = e.button.y;

					// check if button clicked is yes
					if( (mouseClickX > 372) && (mouseClickX < 604) && (mouseClickY > 243) && (mouseClickY < 330) ){
						//display the lobby connecting menu
						SDL_RenderClear(gRenderer);
						SDL_RenderCopy(gRenderer, lobby[1], NULL, NULL);
						SDL_RenderPresent(gRenderer);
						SDL_Delay(2000);	//waits for 2 seconds
						
						bool finding_players = true;
						while (finding_players) {
							char buffer[7] = "HELLO!";
							//for sending data from the client to the server
							bool flag = true;
							NetworkingTest((void*)buffer, flag);
							SDL_Delay(4000);
							num_of_players = getPlayerNumber();
							SDL_RenderClear(gRenderer);
							switch (num_of_players) {
								case 1:	//display lobby showing only one player connected
									SDL_RenderCopy(gRenderer, lobby[2], NULL, NULL);
									break;
								case 2:	//display lobby showing 2 players connected
									SDL_RenderCopy(gRenderer, lobby[3], NULL, NULL);
									finding_players = false;
									networkingFlag = true;
									SDL_Delay(1000);	//waits for 1 second
									break;
								case 3: //display lobby showing 3 players connected
									SDL_RenderCopy(gRenderer, lobby[4], NULL, NULL);
									finding_players = false;
									networkingFlag = true;
									SDL_Delay(1000);	//waits for 1 second
									break;
								case 4: //display lobby showing 4 players connected
									SDL_RenderCopy(gRenderer, lobby[5], NULL, NULL);
									finding_players = false;
									networkingFlag = true;
									SDL_Delay(1000);	//waits for 1 second
									break;
								default:	//default to one player
									SDL_RenderCopy(gRenderer, lobby[2], NULL, NULL);
							}	//end of finding players switch case
							SDL_RenderPresent(gRenderer);

							if(SDL_PollEvent(&e)){
								if (e.type == SDL_QUIT) {
									close();
									return 0;
								}
								if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
									mouseClickX = e.button.x;
									mouseClickY = e.button.y;

									// int x, y;
									// SDL_GetMouseState(&x, &y);
									// printf("x = %\n" , x);
									// printf("y = %d\n", y);

									// check if button clicked is ready
									if( (mouseClickX > 932) && (mouseClickX < 1153) && (mouseClickY > 581) && (mouseClickY < 686) ){
										finding_players = false;
									}
								}
							}
						}
						

						gamestate = 3;	//go directly to the game
					}
					//check if button clicked is no
					else if( (mouseClickX > 372) && (mouseClickX < 604) && (mouseClickY > 386) && (mouseClickY < 476) ){
						gamestate = 3;	//go directly to the game
					}
				}
			}
		}
	}
	//gTex.push_back(loadImage("scrolltest.png")); //loading the map png into the game
	//int mapNumber = 0; //starting map number
	//maps.push_back(loadImage("map1.png")); //first map
	//maps.push_back(loadImage("map2.png")); //second map

	//int tileNum=0;	//Starting tile num;
	//Begins loading tiles (0 is meant for easier floors 4 for harder ones)
	tiles.push_back(loadImage("mapAssets/wall.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL0.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL1.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL2.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL3.png"));
	tiles.push_back(loadImage("mapAssets/floorTileL4.png"));
	tiles.push_back(loadImage("mapAssets/stairs.png"));

	//chestArray = new Chest[2];
	//generating and loading the map
	//srand(time(0));
	//int randomSeed = rand();
	//WARNING THIS MEANS ALL RNG HERE IS UNSEEDED PROCEED AT YOUR OWN RISK

	//This is only for test purposes remove later
	//std::cout << randomSeed << " - Seed for rng\n";

	//Floor* testRoom = new Floor(20,20); //create a new floor object
	//updateFloorSize();
	//Floor* testRoom = new Floor(FloorHeight,FloorWidth,randomSeed);	//The final 1 is needed to match rng for debugging purposes.
	//testRoom->generate(3,3);
	/*
	int **test = testRoom->getFloorMap();
	for(int y = 0; y < FloorHeight; y++){
			for(int x = 0; x < FloorWidth; x++){
				printf("---- Main Array ---- VALUE: %d\n", test[y][x]);
			}
		}
	printf("X SPAWN MAIN %d\n" , testRoom->getSpawnX());
	printf("Y SPAWN MAIN %d\n" , testRoom->getSpawnY());
	printf("X STAIR MAIN! : %d\n", testRoom->getStairX());
	printf("Y STAIR MAIN! : %d\n", testRoom->getStairY());
	*/
	
	//create the enemy spawn vector
	std::vector<enemySpawn> enemySpawns;
	//create the enemy vector
	std::vector<Enemy*> enemies;
	//create projectiles vector
	std::vector<Arrow*> projectiles;
	
	//PLACEHOLDER ENEMY SO WE CAN ALWAYS USE THE isFloor METHOD CHECK, DOES NOT ACTUALLY DO ANYTHING
	Enemy* placeHold = new Enemy(0, 0, 0, 0, 10); 
	
	//send map to the server
	Floor* testRoom;
	//printf("P SIZE IS %d\n" , playerList.size());
	const char *pNum = NULL;
	if(networkingFlag){
		playerList.reserve(num_of_players);
		int j = 0;
		for(j = 0; j < num_of_players; j++){
			playerList.push_back(NULL);
		}
		char buffer2[5] = "SEED";
		bool flag = false;
		NetworkingTest((void*)buffer2, flag);
		int seed = getSeed();
		// free(getSeed);

		//printf("SEED1! %d\n" , seed);
		//Floor* testRoom = new Floor(20,20); //create a new floor object
		updateFloorSize();
		testRoom = new Floor(FloorHeight,FloorWidth,seed);	//The final 1 is needed to match rng for debugging purposes.
		testRoom->generate(3,3,networkingFlag);
		//printf("X SPAWN MAIN %d\n" , testRoom->getMultiSpawnX()[0]);
		//printf("Y SPAWN MAIN %d\n" , testRoom->getMultiSpawnY()[0]);
		//printf("X STAIR MAIN! : %d\n", testRoom->getStairX());
		//printf("Y STAIR MAIN! : %d\n", testRoom->getStairY());
		//printf("DRAWING THE MAP FROM NETWORKING\n");
		respawnX = testRoom->getSpawnX();
		respawnY = testRoom->getSpawnY();
		drawMap(testRoom->getFloorMap(), testRoom->getChestArray(), testRoom->getChestArraySize(), testRoom->getMultiSpawnX()[0], testRoom->getMultiSpawnY()[0], testRoom->getStairX(), testRoom->getStairY());
		//drawMap(f, testRoomNetworking->getChestArray(), testRoomNetworking->getChestArraySize(), testRoomNetworking->getSpawnX(), testRoomNetworking->getSpawnY(), testRoomNetworking->getStairX(), testRoomNetworking->getStairY());

		// Request and save this player's number from the server
		char countPlea[5] = "PLAY";
		NetworkingTest((void*)countPlea, false);
		void *getNum = returnReceived();
		pNum = (const char*)getNum;
		std::stringstream streamCount2(pNum);
        streamCount2 >> i_index;
        //printf("i_index = %d\n", i_index);
		// free(returnReceivedP());

		// int *n = (int*)getNum;
		// int num = *n;
		//printf("Player number: %s\n", pNum);
		//grab enemy spawns from floor.cpp
		enemySpawns =  testRoom->getEnemySpawns();
	}
	else{
		srand(time(0));
		int randomSeed = rand();
		//WARNING THIS MEANS ALL RNG HERE IS UNSEEDED PROCEED AT YOUR OWN RISK
		//This is only for test purposes remove later
		//std::cout << randomSeed << " - Seed for rng\n";
		//Floor* testRoom = new Floor(20,20); //create a new floor object
		updateFloorSize();
		testRoom = new Floor(FloorHeight,FloorWidth,randomSeed);	//The final 1 is needed to match rng for debugging purposes.
		testRoom->generate(3,3,networkingFlag);
		//printf("DRAWING MAP FROM MAIN\n");
		respawnX = testRoom->getSpawnX();
		respawnY = testRoom->getSpawnY();
		drawMap(testRoom->getFloorMap(), testRoom->getChestArray(), testRoom->getChestArraySize(), testRoom->getSpawnX(), testRoom->getSpawnY(), testRoom->getStairX(), testRoom->getStairY());
		//grab enemy spawns from floor.cpp
		enemySpawns =  testRoom->getEnemySpawns();
	}
	maps.push_back(loadImage("testMapImg.png"));
	//maps.push_back(loadImage("map1.png"));
	//SDL_RenderCopy(gRenderer, maps[0], &cameraRect, NULL);
	//SDL_RenderPresent(gRenderer);
	int stairsXCoord = testRoom->getStairX();
	int stairsYCoord = testRoom->getStairY();
	//int mapNumber = 0;dddddddddd

	//variables for door/stairs on map
	//int mapxleft = 870;
	//int mapxright = 1100;
	//int mapytop = 270;
	//int mapybottom = 280;
	if(networkingFlag)
	{
		x_coord = testRoom->getMultiSpawnX()[0];
		y_coord = testRoom->getMultiSpawnY()[0];
	}
	else
	{
		//x and y coordinates/velocity/deltav
		x_coord = testRoom->getSpawnX();//SCREEN_WIDTH/2 - BOX_WIDTH/2;//cameraRect.w/2;;//SCREEN_WIDTH/2;// - PLAYER_WIDTH/2;
		y_coord = testRoom->getSpawnY();//SCREEN_HEIGHT/2 - BOX_HEIGHT/2;//SCREEN_HEIGHT/2;// - (PLAYER_HEIGHT * 2);
	}

	//declare player rect and object
	//dst rect for containing player display
	SDL_Rect dstrect = { static_cast<int>(x_coord), static_cast<int>(y_coord), PLAYER_WIDTH, PLAYER_HEIGHT }; //dstrect for the main character
	// creating player object
	p = new Player(x_coord, y_coord, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_MAX_HEALTH);
	p->dstrect = dstrect;
	if (networkingFlag) {
		 p->playerNumber = pNum;
		 playerList.erase(playerList.begin() + (i_index-1));
		 playerList.insert(playerList.begin() + (i_index-1), p);
	}
	else{
		playerList.push_back(p);
	}

	//Initialize other player objects if online
if (networkingFlag) {
		Player *p;
		int netSpawnX;
		int netSpawnY;
		for(int i = 1; i < playerList.size(); i++){
			netSpawnX = testRoom->getMultiSpawnX()[i];
			netSpawnY = testRoom->getMultiSpawnY()[i];
			//printf("Player %d netSpawnX = %d\n" , i+1,netSpawnX);
			//printf("PLayer %d netSpawnY = %d\n\n", i+1,netSpawnY);

			p = new Player(netSpawnX, netSpawnY, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_MAX_HEALTH);
			//loading the 4 different movement orientations for the character
			//front/back
			int inserter = 0;
			for(inserter = 0; inserter < playerList.size(); inserter++){
				if(playerList[inserter] == NULL){
					//printf("INSERTER VALUE IS %d\n", inserter);
					playerList.erase(playerList.begin() + inserter);
					playerList.insert(playerList.begin() + inserter, p);
				}
			}
		}
	}
	//printf("PLAYERLIST SIZE IS %d\n", playerList.size());

	gTex.push_back(loadImage("menuImages/pausemenu.png")); //load in the start menu image

	inventory.push_back(loadImage("sprites/Menu.png"));
	inventory.push_back(loadImage("sprites/Menu_sword.png"));
	inventory.push_back(loadImage("sprites/Menu_axe.png"));
	inventory.push_back(loadImage("sprites/Menu_sword_axe.png"));


	//The index in the sprite array of our current sprite direction
	//int spriteIndex = 0;
	p->spriteIndex = 0;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	bool gameon = true; //the game loop is now active


	loadSprites();
	
	//---------------------- AI ENEMY SPAWN METHOD ------------------------
    std::vector<std::pair<std::string,double>> zScores = gameMaster.findZScores();

    for(auto& i : zScores){
        if(i.first == "Grunt"){
            if(i.second > gameMaster.modelGruntThreshold)
                i.second = gameMaster.modelGruntThreshold;
        }
        else if(i.first == "Archer"){
            if(i.second > gameMaster.modelArcherThreshold)
                i.second = gameMaster.modelArcherThreshold;
        }
        else if(i.first == "Goblin"){
            if(i.second > gameMaster.modelGoblinThreshold)
                i.second = gameMaster.modelGoblinThreshold;
        }
        else if(i.first == "Knight"){
            if(i.second > gameMaster.modelKnightThreshold)
                i.second = gameMaster.modelKnightThreshold;
        }
        else{
            if(i.second > gameMaster.modelAssassinThreshold)
                i.second = gameMaster.modelAssassinThreshold;
        }
    }
    //0 = grunt
    //1 = archer
    //2 = goblin
    //3 = knight
    //4 = assassin
    //vector of enemies to spawn in
    std::vector<int> enemySpawnVector;
    //highest z score hardest enemy
    // roll 1-8 7 and 8 to highest decreasing vals for each zscore
    int spawner;
    int highRand = 7;
    int lowRand = 6;
    int enemySpawnType;
    for(int i = 4; i > 0; i--){
        if(zScores.at(i).first == "Grunt")
            enemySpawnType = 0;
        else if(zScores.at(i).first == "Archer")
            enemySpawnType = 1;
        else if(zScores.at(i).first == "Goblin")
            enemySpawnType = 2;
        else if(zScores.at(i).first == "Knight")
            enemySpawnType = 3;
        else if(zScores.at(i).first == "Assassin")
            enemySpawnType = 4;

        //spawn enemies based on high and low vals
        spawner = rand() %(highRand-lowRand + 1) + lowRand;

        highRand--;
        lowRand--;
        for(int j = 0; j < spawner; j++){
            enemySpawnVector.push_back(enemySpawnType);
        }
    }
	int spawnCounterSizer = enemySpawns.size();
    while(enemySpawnVector.size() < enemySpawns.size()){
        int enemy = rand() % (4-0 + 1) + 0;
        enemySpawnVector.push_back(enemy);
    }
    //randomize for balance
    if(!networkingFlag){
		for (int i = 0; i < enemySpawnVector.size(); i++) {
        	int j = i + rand() % (enemySpawnVector.size() - i); 
        	std::swap(enemySpawnVector[i], enemySpawnVector[j]);
    	}
	}
	int spawnCounter = 0;
	//spawn in enemies for all spawn points given 
	for(enemySpawn point : enemySpawns)
	{
		//grab the x and y coord for this spawn point
		double x_coord = point.x;
		double y_coord = point.y;
		
		//check if the coordinates are a floor tile, if not, do not spawn on it
		if(placeHold->isFloor(testRoom->getFloorMap(), x_coord/100, y_coord/100, testRoom->getFloorHeight(), testRoom->getFloorWidth()))
		{
			int enemyType;
			//decide which enemy to spawn
			if(!networkingFlag){
				enemyType = enemySpawnVector.at(spawnCounter);
			}
			else{
				enemyType = rand() % 5;
			}
		
			spawnCounter++;
			
			//depending on enemyType value, spawn an enemy for that number
			if(enemyType == 0) //spawn grunt
			{
				Enemy* grunt = new Enemy(x_coord, y_coord, SKELETON_WIDTH, SKELETON_HEIGHT, GRUNT_MAX_HEALTH); 
				enemies.push_back(grunt);
				gameMaster.addGruntSample(1);
			}
			else if(enemyType == 1) //spawn archer
			{
				Archer* archer = new Archer(x_coord, y_coord, SKELETON_WIDTH, SKELETON_HEIGHT, ARCHER_MAX_HEALTH); 
				enemies.push_back(archer);
				gameMaster.addArcherSample(1);
			}
			else if(enemyType == 2) //goblin squad
			{
				GoblinSquad* squad = new GoblinSquad(x_coord, y_coord, 0, 0, 999999999999);
				squad->spawnSquad();
				enemies.push_back(squad);
				gameMaster.addGoblinSample(1);
				for(Goblin* g:squad->goblins){
					if(placeHold->isFloor(testRoom->getFloorMap(),(int) g->x_pos/100,(int) g->y_pos/100, testRoom->getFloorHeight(), testRoom->getFloorWidth()))
						enemies.push_back(g);
					else
						squad->goblinNum--;
				}
			}
			else if(enemyType == 3) //knight
			{
				Knight* knight = new Knight(x_coord, y_coord, KNIGHT_WIDTH, KNIGHT_HEIGHT, KNIGHT_MAX_HEALTH);
				enemies.push_back(knight);
				gameMaster.addKnightSample(1);
			}
			else if(enemyType == 4) //assassin
			{
				Assassin* sneaky = new Assassin(x_coord, y_coord, ASSASSIN_WIDTH, ASSASSIN_HEIGHT, ASSASSIN_MAX_HEALTH); 
				enemies.push_back(sneaky);
				gameMaster.addAssassinSample(1);
			}
		}
		
	}
	//-------------------- END AI ENEMY SPAWN METHOD ----------------------
	
	//offsets for enemy health, increases every floor
	int goblinHealthIncrease = 2;
	int gruntHealthIncrease = 3;
	int archerHealthIncrease = 3;
	int assassinHealthIncrease = 2;
	int knightHealthIncrease = 5;

	/*
	//enemy variables/declarations
	Enemy* badGuy = new Enemy(300, 300, SKELETON_WIDTH, SKELETON_HEIGHT, GRUNT_MAX_HEALTH);

	Archer* kite = new Archer(400, 400, SKELETON_WIDTH, SKELETON_HEIGHT, ARCHER_MAX_HEALTH);

	Assassin* sneaky = new Assassin(200, 200, ASSASSIN_WIDTH, ASSASSIN_HEIGHT, ASSASSIN_MAX_HEALTH);

	//test goblin
	//Goblin* goblin = new Goblin(600,600, GOBLIN_WIDTH, GOBLIN_HEIGHT, GOBLIN_MAX_HEALTH);

	//test goblin squad
	GoblinSquad* squad = new GoblinSquad(900,500,0,0, GOBLIN_MAX_HEALTH);
	squad->spawnSquad();

	
	std::vector<Enemy*> enemies;

	//test knight
	Knight* knight = new Knight(700, 700, KNIGHT_WIDTH, KNIGHT_HEIGHT, KNIGHT_MAX_HEALTH);
	*/


	//Our pressure map will be divided into 25 pixel squares

	int arrHeight=(int)std::ceil(BG_HEIGHT/PRESSURE_GRID_SIZE)*2;
	int arrWidth=(int)std::ceil(BG_WIDTH/PRESSURE_GRID_SIZE)*2;
	int pressureMap [arrHeight][arrWidth];
	double blurMap[arrHeight][arrWidth];
	double convolutionFilter[3][3] = {{.0625, .125, .0625}, {.125, .25, .125}, {.0625, .125, .0625}};

	//enemies.push_back(kite);
	//enemies.push_back(badGuy);
	//enemies.push_back(sneaky);
	//enemies.push_back(goblin);
	//enemies.push_back(knight);

	//for(Goblin* g:squad->goblins){
	//	enemies.push_back(g);
	//}

	//enemies.push_back(squad);



	//double enemyPos = 0;
	//bool arrived = false;
	//bool enemyFollow = false;
	//bool enemyAlive = true;

	//attack animation variable
	int frames = 0;

	//define a weapon to be on floor
	//setting up tester weapons currently on floor
	Weapon *sword;
	Weapon *axe;
	Weapon* longSword;
	Weapon* bow;
	Weapon* hammer;
	Weapon* throwingAxe;
	Weapon* trident;
	axe = new Weapon(310.0, 310.0, 75, 75, 2);
	sword = new Weapon(110.0, 110.0, 60, 60, 1);
	longSword = new Weapon(200.0, 200.0, 60, 60, 3);
	bow = new Weapon(40.0, 200.0, 60, 60, 4);
	hammer = new Weapon(80.0, 200.0, 60, 60, 5);
	throwingAxe = new Weapon(500.0, 200.0, 60, 60, 6);
	trident = new Weapon(400, 200.0, 60, 60, 7);
	longSword->sprites.push_back(loadImage("sprites/longsword.png"));
	axe->sprites.push_back(loadImage("sprites/axe.png"));
    sword->sprites.push_back(loadImage("sprites/sword.png"));
	bow->sprites.push_back(loadImage("sprites/bow.png"));
	hammer->sprites.push_back(loadImage("sprites/hammer.png"));
	throwingAxe->sprites.push_back(loadImage("sprites/throwingAxe.png"));
	trident->sprites.push_back(loadImage("sprites/trident.png"));
	all_weapons.push_back(sword);
	all_weapons.push_back(axe);
	all_weapons.push_back(longSword);
	all_weapons.push_back(bow);
	all_weapons.push_back(hammer);
	all_weapons.push_back(throwingAxe);
	all_weapons.push_back(trident);

	for (int i = 0; i < testRoom->getChestArraySize(); i++) {
		testRoom->getChestArray()[i].fillChestInventory(all_weapons, curFloorNum);

		for(int z = 0; z < 3; z++){

			if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 1){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/sword.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 2){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/axe.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 3){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/longsword.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 4){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/bow.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 5){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/hammer.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 6){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/throwingAxe.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 7){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/trident.png"));
			}

		}
	}






	//fill the chests with 3 random weapons each
	//chestArray[0].fillChestInventory(all_weapons);
	//chestArray[1].fillChestInventory(all_weapons);

	//moved out here so we dont run out of memory
	SDL_Texture* blackBar = loadImage("sprites/blackHealthBar.png");
	SDL_Texture* greenBar = loadImage("sprites/greenHealthBar.png");


	// Keep track of time for framerate/movements
	//Uint32 fps_last_time = SDL_GetTicks();
	//Uint32 fps_cur_time = 0;
	Uint32 move_last_time = SDL_GetTicks();
	//Uint32 anim_last_time = SDL_GetTicks();
	double timestep = 0.0;

	//dst rect for containing player display
	//SDL_Rect dstrect = { static_cast<int>(x_coord), static_cast<int>(y_coord), PLAYER_WIDTH, PLAYER_HEIGHT }; //dstrect for the main character

	// creating player object
	//p = new Player(x_coord, y_coord, PLAYER_WIDTH, PLAYER_HEIGHT);
	int chClick = 0; //keeps track of which chest is clicked

	//MAIN GAME LOOP
	while(gameon)
	{
		int mouseClickX;
		int mouseClickY;
		//first check for quit or pause
		while(SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				gameon = false;
			}else if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_p)
					paused = true;
				else if(e.key.keysym.sym == SDLK_i){
					iPaused = true;
				}
			}

			if(e.type == SDL_MOUSEBUTTONDOWN){
				if(e.button.button == SDL_BUTTON_LEFT){
					chClick = checkIfChestClicked(e, cameraRect.x, cameraRect.y, testRoom);
						if(chClick != -1){
							iChest = true;
						}
					//checkWeaponPickup(all_weapons, e, p->weapons, cameraRect);

					if(all_weapons.size() != 0){
						p->isAttacking=true;
						p->attackTime=SDL_GetTicks();

						mouseClickX=e.button.x;
						mouseClickY=e.button.y;
					}

		                //check if enemy is in range (In progress)
		               /* SDL_Rect playRec;
		                SDL_Rect badRec;
		                playRec.x=x_pos-100;
		                playRec.y=y_pos-100;
		                playRec.w=x_size+100;
		                playRec.h=y_size+100;
		                badRec.x=badGuy->x_pos;
		                badRec.y=badGuy->y_pos;
		                badRec.w=badGuy->x_size;
		                badRec.h=badGuy->y_size;
		                if(check_collision(playRec,badRec)) */

					/*
					if (enemyFollow)
						badGuy->hit();
		                //erase enemy if health depleted
					if (badGuy->health <= 0)
					{
						SDL_DestroyTexture(badGuy->sprites[badGuy->spriteIndex]);
						enemyAlive=false;
					}
					*/
				}
			}

		}

		//grab timestep since last ticks - need this before pause menu or else issues arise with timestep calculations
		timestep = ((SDL_GetTicks() - move_last_time) / 1000.0);

		//pause menu is rendered and waits for key press
		while(paused){

			SDL_RenderCopy(gRenderer, gTex[1], NULL, NULL);
			SDL_RenderPresent(gRenderer);
			//allows unpausing with p press
			// while(SDL_PollEvent(&e)){
			// 	if(e.type == SDL_KEYDOWN){
			// 		if(e.key.keysym.sym == SDLK_p){
			// 			paused = false;
			// 		}
			// 	}
			// }

			int mouseClickX;
			int mouseClickY;
			while(SDL_PollEvent(&e)){
				if(e.type == SDL_MOUSEBUTTONDOWN){
					if(e.button.button == SDL_BUTTON_LEFT){ //if the event type is a left mouse click
						//obtain the clicks x and y coordinates
						mouseClickX = e.button.x;
						mouseClickY = e.button.y;
						// SDL_Log(mouseClickX);
						// SDL_Log(mouseClickY);

						// int x, y;
						// SDL_GetMouseState(&x, &y);
						// printf("x = %d\n" , x);
						// printf("y = %d\n", y);
						// SDL_Log(mouseClickX);
						// SDL_Log(mouseClickY);

						//check if button clicked is resume
						if( (mouseClickX > 435) && (mouseClickX < 588) && (mouseClickY > 245) && (mouseClickY < 280) ){
							gamestate = 1; //enter the game, since start was pressed
							paused = false;
						}

						//check if button clicked is save
						// else if( (mouseClickX > 391) && (mouseClickX < 891) && (mouseClickY > 562) && (mouseClickY < 662) ){

						// }

						//check if button clicked is join
						// if((mouseClickX > 430) && (mouseClickX < 518) && (mouseClickY > 346) && (mouseClickY < 377)){
						//
						// 	char buffer[90];
						// 	int xPOS = p->x_pos;
						// 	int yPOS = p->y_pos;
						// 	int b_xPOS = badGuy->x_pos;
						// 	int b_yPOS = badGuy->y_pos;
						// 	int a_xPOS = kite->x_pos;
						// 	int a_yPOS = kite->y_pos;
						//
						// 	snprintf(buffer, sizeof(buffer), "XPOS: %d YPOS: %d\n Enemy XPOS: %d YPOS: %d\n Archer XPOS: %d YPOS: %d", xPOS, yPOS, b_xPOS, b_yPOS, a_xPOS, a_yPOS);
						// 	 //printf("%s\n", buffer);
						// 	 //NetworkingTest(buffer);
						//
						//
						//
						// }

						//check if button clicked is quit
						else if( (mouseClickX > 435) && (mouseClickX < 532) && (mouseClickY > 352) && (mouseClickY < 375) ){
							quit = true; //setting the quit boolean to true
							gamestate = 2; //gamestate for quit
						}
					}
				}
				else if(e.type == SDL_QUIT){
					quit = true;
				}
			}
			if(quit == true){
				close();
				return 0;
			}
		}
        //inventory pause
		//timestep = ((SDL_GetTicks() - move_last_time) / 1000.0);
		while(iPaused){
			SDL_RenderCopy(gRenderer, inventory[0], NULL, NULL);
			drawInventory(p->weapons, cameraRect);
			SDL_RenderPresent(gRenderer);
			int mouseClickX;
			int mouseClickY;
			while(SDL_PollEvent(&e)){
				if(e.type == SDL_MOUSEBUTTONDOWN){
					if(e.button.button == SDL_BUTTON_LEFT){ //if the event type is a left mouse click
						//obtain the clicks x and y coordinates
						mouseClickX = e.button.x;
						mouseClickY = e.button.y;

						int x, y;
						SDL_GetMouseState(&x, &y);

						if( (mouseClickX >= 40 && mouseClickX <= 200) && (mouseClickY >= 385 && mouseClickY <= 472) ){
							if(p->weapons.size() >= 1){
								//printf("SELECTED INV 1\n");
								p->currWeapon = p->weapons.at(0)->weapon_type;
								p->weaponIndex = 0;
							}
						}
						else if( (mouseClickX >= 242 && mouseClickX <= 400) && (mouseClickY >= 385 && mouseClickY <= 472) ){
							if(p->weapons.size() >= 2){
							//printf("SELECTED INV 2\n");
							p->currWeapon = p->weapons.at(1)->weapon_type;
							p->weaponIndex = 1;
							}
						}
						else if( (mouseClickX >= 442 && mouseClickX <= 600) && (mouseClickY >= 385 && mouseClickY <= 472) ){
							if(p->weapons.size() >= 3){
							//printf("SELECTED INV 3\n");
							p->currWeapon = p->weapons.at(2)->weapon_type;
							p->weaponIndex = 2;
							}
						}
						else if( (mouseClickX >= 642 && mouseClickX <= 800) && (mouseClickY >= 385 && mouseClickY <= 472) ){
							if(p->weapons.size() >= 4){
							//printf("SELECTED INV 4\n");
							p->currWeapon = p->weapons.at(3)->weapon_type;
							p->weaponIndex = 3;
							}
						}
						else if( (mouseClickX >= 842 && mouseClickX <= 1000) && (mouseClickY >= 385 && mouseClickY <= 472) ){
							if(p->weapons.size() >= 5){
							//printf("SELECTED INV 5\n");
							p->currWeapon = p->weapons.at(4)->weapon_type;
							p->weaponIndex = 4;
							}
						}
						else if( (mouseClickX >= 1042 && mouseClickX <= 1200) && (mouseClickY >= 385 && mouseClickY <= 472) ){
							if(p->weapons.size() >= 6){
							//printf("SELECTED INV 6\n");
							p->currWeapon = p->weapons.at(5)->weapon_type;
							p->weaponIndex = 5;
							}
						}
						else if( (mouseClickX >= 40 && mouseClickX <= 200) && (mouseClickY >= 498 && mouseClickY <= 585) ){
							if(p->weapons.size() >= 7){
							//printf("SELECTED INV 7\n");
							p->currWeapon = p->weapons.at(6)->weapon_type;
							p->weaponIndex = 6;
							}
						}
						else if( (mouseClickX >= 242 && mouseClickX <= 400) && (mouseClickY >= 498 && mouseClickY <= 585) ){
							if(p->weapons.size() >= 8){
							//printf("SELECTED INV 8\n");
							p->currWeapon = p->weapons.at(7)->weapon_type;
							p->weaponIndex = 7;
							}
						}
						else if( (mouseClickX >= 442 && mouseClickX <= 600) && (mouseClickY >= 498 && mouseClickY <= 585) ){
							if(p->weapons.size() >= 9){
							//printf("SELECTED INV 9\n");
							p->currWeapon = p->weapons.at(8)->weapon_type;
							p->weaponIndex = 8;
							}
						}
						else if( (mouseClickX >= 642 && mouseClickX <= 800) && (mouseClickY >= 498 && mouseClickY <= 585) ){
							if(p->weapons.size() >= 10){
							//printf("SELECTED INV 10\n");
							p->currWeapon = p->weapons.at(9)->weapon_type;
							p->weaponIndex = 9;
							}
						}
						else if( (mouseClickX >= 842 && mouseClickX <= 1000) && (mouseClickY >= 498 && mouseClickY <= 585) ){
							if(p->weapons.size() >= 11){
							//printf("SELECTED INV 11\n");
							p->currWeapon = p->weapons.at(10)->weapon_type;
							p->weaponIndex = 10;
							}
						}
						else if( (mouseClickX >= 1042 && mouseClickX <= 1200) && (mouseClickY >= 498 && mouseClickY <= 585) ){
							if(p->weapons.size() >= 12){
							//printf("SELECTED INV 12\n");
							p->currWeapon = p->weapons.at(11)->weapon_type;
							p->weaponIndex = 11;
							}
						}
						else if( (mouseClickX >= 40 && mouseClickX <= 200) && (mouseClickY >= 611 && mouseClickY <= 698) ){
							if(p->weapons.size() >= 13){
							//printf("SELECTED INV 7\n");
							p->currWeapon = p->weapons.at(12)->weapon_type;
							p->weaponIndex = 12;
							}
						}
						else if( (mouseClickX >= 242 && mouseClickX <= 400) && (mouseClickY >= 611 && mouseClickY <= 698) ){
							if(p->weapons.size() >= 14){
							//printf("SELECTED INV 8\n");
							p->currWeapon = p->weapons.at(13)->weapon_type;
							p->weaponIndex = 13;
							}
						}
						else if( (mouseClickX >= 442 && mouseClickX <= 600) && (mouseClickY >= 611 && mouseClickY <= 698) ){
							if(p->weapons.size() >= 15){
							//printf("SELECTED INV 9\n");
							p->currWeapon = p->weapons.at(14)->weapon_type;
							p->weaponIndex = 14;
							}
						}
						else if( (mouseClickX >= 642 && mouseClickX <= 800) && (mouseClickY >= 611 && mouseClickY <= 698) ){
							if(p->weapons.size() >= 16){
							//printf("SELECTED INV 10\n");
							p->currWeapon = p->weapons.at(15)->weapon_type;
							p->weaponIndex = 15;
							}
						}
						else if( (mouseClickX >= 842 && mouseClickX <= 1000) && (mouseClickY >= 611 && mouseClickY <= 698) ){
							if(p->weapons.size() >= 17){
							//printf("SELECTED INV 11\n");
							p->currWeapon = p->weapons.at(16)->weapon_type;
							p->weaponIndex = 16;
						}
						}
						else if( (mouseClickX >= 1042 && mouseClickX <= 1200) && (mouseClickY >= 611 && mouseClickY <= 698) ){
							if(p->weapons.size() >= 18){
							//printf("SELECTED INV 12\n");
							p->currWeapon = p->weapons.at(17)->weapon_type;
							p->weaponIndex = 17;
						}
						}
						//check if button clicked is resume
						if( (mouseClickX > 760) && (mouseClickX < 1300) && (mouseClickY > 0) && (mouseClickY < 250)){
							gamestate = 1; //enter the game, since start was pressed
							iPaused = false;
						}
					}


					else if(e.button.button == SDL_BUTTON_RIGHT){ //if the event type is a right mouse click
						//obtain the clicks x and y coordinates
						mouseClickX = e.button.x;
						mouseClickY = e.button.y;

						int x, y;
						SDL_GetMouseState(&x, &y);


						if(p->weapons.size() != 0){

						if( (mouseClickX >= 40 && mouseClickX <= 200) && (mouseClickY >= 385 && mouseClickY <= 472) ){
							//printf("REMOVED INV 1\n");

							if(p->weaponIndex == 0){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							p->weapons.at(0)->picked_up = false;
							delete p->weapons.at(0);
							p->weapons.at(0) = nullptr;
							p->weapons.erase(p->weapons.begin(), p->weapons.begin()+1);
						}
						else if( (mouseClickX >= 242 && mouseClickX <= 400) && (mouseClickY >= 385 && mouseClickY <= 472) ){

							if(p->weaponIndex == 1){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 2\n");
							p->weapons.at(1)->picked_up = false;
							delete p->weapons.at(1);
							p->weapons.at(1) = nullptr;
							p->weapons.erase(p->weapons.begin()+1, p->weapons.begin()+2);
						}
						else if( (mouseClickX >= 442 && mouseClickX <= 600) && (mouseClickY >= 385 && mouseClickY <= 472) ){

							if(p->weaponIndex == 2){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 3\n");
							p->weapons.at(2)->picked_up = false;
							delete p->weapons.at(2);
							p->weapons.at(2) = nullptr;
							p->weapons.erase(p->weapons.begin()+2, p->weapons.begin()+3);
						}
						else if( (mouseClickX >= 642 && mouseClickX <= 800) && (mouseClickY >= 385 && mouseClickY <= 472) ){

							if(p->weaponIndex == 3){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 4\n");
							p->weapons.at(3)->picked_up = false;
							delete p->weapons.at(3);
							p->weapons.at(3) = nullptr;
							p->weapons.erase(p->weapons.begin()+3, p->weapons.begin()+4);
						}
						else if( (mouseClickX >= 842 && mouseClickX <= 1000) && (mouseClickY >= 385 && mouseClickY <= 472) ){

							if(p->weaponIndex == 4){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 5\n");
							p->weapons.at(4)->picked_up = false;
							delete p->weapons.at(4);
							p->weapons.at(4) = nullptr;
							p->weapons.erase(p->weapons.begin()+4, p->weapons.begin()+5);
						}
						else if( (mouseClickX >= 1042 && mouseClickX <= 1200) && (mouseClickY >= 385 && mouseClickY <= 472) ){

							if(p->weaponIndex == 5){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 6\n");
							p->weapons.at(5)->picked_up = false;
							delete p->weapons.at(5);
							p->weapons.at(5) = nullptr;
							p->weapons.erase(p->weapons.begin()+5, p->weapons.begin()+6);
						}
						else if( (mouseClickX >= 40 && mouseClickX <= 200) && (mouseClickY >= 498 && mouseClickY <= 585) ){

							if(p->weaponIndex == 6){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 7\n");
							p->weapons.at(6)->picked_up = false;
							delete p->weapons.at(6);
							p->weapons.at(6) = nullptr;
							p->weapons.erase(p->weapons.begin()+6, p->weapons.begin()+7);
						}
						else if( (mouseClickX >= 242 && mouseClickX <= 400) && (mouseClickY >= 498 && mouseClickY <= 585) ){

							if(p->weaponIndex == 7){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 8\n");
							p->weapons.at(7)->picked_up = false;
							delete p->weapons.at(7);
							p->weapons.at(7) = nullptr;
							p->weapons.erase(p->weapons.begin()+7, p->weapons.begin()+8);
						}
						else if( (mouseClickX >= 442 && mouseClickX <= 600) && (mouseClickY >= 498 && mouseClickY <= 585) ){

							if(p->weaponIndex == 8){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 9\n");
							p->weapons.at(8)->picked_up = false;
							delete p->weapons.at(8);
							p->weapons.at(8) = nullptr;
							p->weapons.erase(p->weapons.begin()+8, p->weapons.begin()+9);
						}
						else if( (mouseClickX >= 642 && mouseClickX <= 800) && (mouseClickY >= 498 && mouseClickY <= 585) ){

							if(p->weaponIndex == 9){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 10\n");
							p->weapons.at(9)->picked_up = false;
							delete p->weapons.at(9);
							p->weapons.at(9) = nullptr;
							p->weapons.erase(p->weapons.begin()+9, p->weapons.begin()+10);
						}
						else if( (mouseClickX >= 842 && mouseClickX <= 1000) && (mouseClickY >= 498 && mouseClickY <= 585) ){

							if(p->weaponIndex == 10){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 11\n");
							p->weapons.at(10)->picked_up = false;
							delete p->weapons.at(10);
							p->weapons.at(10) = nullptr;
							p->weapons.erase(p->weapons.begin()+10, p->weapons.begin()+11);
						}
						else if( (mouseClickX >= 1042 && mouseClickX <= 1200) && (mouseClickY >= 498 && mouseClickY <= 585) ){

							if(p->weaponIndex == 11){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 12\n");
							p->weapons.at(11)->picked_up = false;
							delete p->weapons.at(11);
							p->weapons.at(11) = nullptr;
							p->weapons.erase(p->weapons.begin()+11, p->weapons.begin()+12);
						}
						else if( (mouseClickX >= 40 && mouseClickX <= 200) && (mouseClickY >= 611 && mouseClickY <= 698) ){
							
							if(p->weaponIndex == 12){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 7\n");
							p->weapons.at(12)->picked_up = false;
							delete p->weapons.at(12);
							p->weapons.at(12) = nullptr;
							p->weapons.erase(p->weapons.begin()+12, p->weapons.begin()+13);
						}
						else if( (mouseClickX >= 242 && mouseClickX <= 400) && (mouseClickY >= 611 && mouseClickY <= 698) ){

							if(p->weaponIndex == 13){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 8\n");
							p->weapons.at(13)->picked_up = false;
							delete p->weapons.at(13);
							p->weapons.at(13) = nullptr;
							p->weapons.erase(p->weapons.begin()+13, p->weapons.begin()+14);
						}
						else if( (mouseClickX >= 442 && mouseClickX <= 600) && (mouseClickY >= 611 && mouseClickY <= 698) ){

							if(p->weaponIndex == 14){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 9\n");
							p->weapons.at(14)->picked_up = false;
							delete p->weapons.at(14);
							p->weapons.at(14) = nullptr;
							p->weapons.erase(p->weapons.begin()+14, p->weapons.begin()+15);
						}
						else if( (mouseClickX >= 642 && mouseClickX <= 800) && (mouseClickY >= 611 && mouseClickY <= 698) ){

							if(p->weaponIndex == 15){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 10\n");
							p->weapons.at(15)->picked_up = false;
							delete p->weapons.at(15);
							p->weapons.at(15) = nullptr;
							p->weapons.erase(p->weapons.begin()+15, p->weapons.begin()+16);
						}
						else if( (mouseClickX >= 842 && mouseClickX <= 1000) && (mouseClickY >= 611 && mouseClickY <= 698) ){

							if(p->weaponIndex == 16){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 11\n");
							p->weapons.at(16)->picked_up = false;
							delete p->weapons.at(16);
							p->weapons.at(16) = nullptr;
							p->weapons.erase(p->weapons.begin()+16, p->weapons.begin()+17);
						}
						else if( (mouseClickX >= 1042 && mouseClickX <= 1200) && (mouseClickY >= 611 && mouseClickY <= 698) ){

							if(p->weaponIndex == 17){
								p->currWeapon = 0;
								p->weaponIndex=-1;
							}

							//printf("REMOVED INV 12\n");
							p->weapons.at(17)->picked_up = false;
							delete p->weapons.at(17);
							p->weapons.at(17) = nullptr;
							p->weapons.erase(p->weapons.begin()+17, p->weapons.begin()+18);
						}

					}
						//check if button clicked is resume
						if( (mouseClickX > 760) && (mouseClickX < 1300) && (mouseClickY > 0) && (mouseClickY < 250)){
							gamestate = 1; //enter the game, since start was pressed
							iPaused = false;
						}
					}



				}
			}
		}

		while(iChest){
			SDL_RenderClear(gRenderer);
			SDL_RenderCopy(gRenderer, inventory[0], NULL, NULL);
			drawChestInventory(testRoom->getChestArray()[chClick].getChestInventory(), cameraRect);
			SDL_RenderPresent(gRenderer);
			int mouseClickX;
			int mouseClickY;

			while(SDL_PollEvent(&e)){
				if(e.type == SDL_MOUSEBUTTONDOWN){
					if(e.button.button == SDL_BUTTON_LEFT){ //if the event type is a left mouse click
						//obtain the clicks x and y coordinates
						mouseClickX = e.button.x;
						mouseClickY = e.button.y;
						//check if button clicked is resume
						if( (mouseClickX > 760) && (mouseClickX < 1300) && (mouseClickY > 0) && (mouseClickY < 250)){
							gamestate = 1; //enter the game, since start was pressed
							iChest = false;
						}
						if(p->weapons.size() < 18){
						//40x, 382y
						//160 wide 90 tall
							if((mouseClickX > 40) && (mouseClickX < 200) && (mouseClickY > 382) && (mouseClickY < 472)){

								//check if the inventory size == 18, if so then full
								if(testRoom->getChestArray()[chClick].getChestInventory().at(0)->picked_up == false){

									testRoom->getChestArray()[chClick].getChestInventory().at(0)->picked_up = true;

									Weapon* newWep = new Weapon(testRoom->getChestArray()[chClick].getChestInventory().at(0));

									testRoom->getChestArray()[chClick].getChestInventory().at(0) = nullptr;

									p->weapons.push_back(newWep);

								}
							}
							else if((mouseClickX > 240) && (mouseClickX < 400) && (mouseClickY > 382) && (mouseClickY < 472)){
								if(testRoom->getChestArray()[chClick].getChestInventory().at(1)->picked_up == false){

									testRoom->getChestArray()[chClick].getChestInventory().at(1)->picked_up = true;

									Weapon* newWep = new Weapon(testRoom->getChestArray()[chClick].getChestInventory().at(1));

									testRoom->getChestArray()[chClick].getChestInventory().at(1) = nullptr;

									p->weapons.push_back(newWep);
								}
							}
							else if((mouseClickX > 440) && (mouseClickX < 600) && (mouseClickY > 382) && (mouseClickY < 472)){
								if(testRoom->getChestArray()[chClick].getChestInventory().at(2)->picked_up == false){

									testRoom->getChestArray()[chClick].getChestInventory().at(2)->picked_up = true;

									Weapon* newWep = new Weapon(testRoom->getChestArray()[chClick].getChestInventory().at(2));

									testRoom->getChestArray()[chClick].getChestInventory().at(2) = nullptr;

									p->weapons.push_back(newWep);
								}
							}
					}
						
					}
				}
			}
		} //end of while iChest

		//const Uint8* keystate = SDL_GetKeyboardState(nullptr);

		//move the player
		p->moveCharacter(p->x_pos, p->y_pos, BG_HEIGHT, BG_WIDTH, timestep);

		//Player collision is still iffy not good to test with

		//update camera rect
		cameraRect.x = (p->x_pos + PLAYER_WIDTH/2) - SCREEN_WIDTH/2;
		if (cameraRect.x < 0)
			cameraRect.x = 0;
		else if (cameraRect.x + SCREEN_WIDTH > BG_WIDTH)
			cameraRect.x = BG_WIDTH - SCREEN_WIDTH;
		cameraRect.y = (p->y_pos + PLAYER_HEIGHT/2) - SCREEN_HEIGHT/2;
		if (cameraRect.y < 0)
			cameraRect.y = 0;
		else if (cameraRect.y + SCREEN_HEIGHT > BG_HEIGHT)
			cameraRect.y = BG_HEIGHT - SCREEN_HEIGHT;

		//grab last time player coordinates moved
		move_last_time = SDL_GetTicks();


		//switching the character orientation based on direction
		if(p->x_vel < 0){
			p->spriteIndex=36;
			flip = SDL_FLIP_NONE;
			p->flip = SDL_FLIP_NONE;
		}
		else if(p->x_vel > 0){
			p->spriteIndex=36;
			flip = SDL_FLIP_HORIZONTAL;
			p->flip = SDL_FLIP_HORIZONTAL;		
		}
		else if(p->y_vel < 0){
			p->spriteIndex=1;
			flip = SDL_FLIP_NONE;
			p->flip = SDL_FLIP_NONE;
		}
		else if(p->y_vel > 0){
			p->spriteIndex=0;
			flip = SDL_FLIP_NONE;
			p->flip = SDL_FLIP_NONE;		
		}

		p->dstrect.x = p->x_pos - cameraRect.x;
		p->dstrect.y = p->y_pos - cameraRect.y;

		/*
		//now figure out if the character has entered the door
		if(p->y_pos > mapytop && p->y_pos < mapybottom){
			if(p->x_pos > mapxleft && p->x_pos < mapxright){
				//increase map number
				mapNumber++;
				//also change player location to corner position
				p->x_pos = 100;
				p->y_pos = 100;
				//and change new door location
				mapxleft = 870;
				mapxright = 970;
				mapytop = 800;
				mapybottom = 810;
				SDL_Delay(1000);
			}
		}
		if(mapNumber == 2){
			//out of maps, quit the game
			SDL_Delay(1000); //delay for 1 second
			break; //break out of game loop and quit
		}
		*/

		//drawing everything to the screen
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);
		// draw the current map on the screen
		SDL_RenderCopy(gRenderer, maps[0], &cameraRect, NULL);
		// draw player


		//determine if the player entered the stairs

		//stairsXCoord
		//stairsYCoord

		/*
		if(enemyAlive)
		{
			DTAction* action = badGuy->getAction();
			if (action == badGuy->wander)
				enemyFollow = false;
			else if (action == badGuy->chase)
				enemyFollow = true;
			else
				std::cout << "Decision Tree returned bad value" << std::endl;
			if(enemyPos == 0 && !enemyFollow)
			{
				arrived = badGuy->enemySeek(1500 , 250, BG_HEIGHT, BG_WIDTH, timestep);
				//if enemy arrived at destination, change course
				if(arrived)
					enemyPos++;
			}
			else if(enemyPos == 1 && !enemyFollow)
			{
				arrived = badGuy->enemySeek(1500 , 900, BG_HEIGHT, BG_WIDTH, timestep);
				//if enemy arrived at destination, change course
				if(arrived)
					enemyPos++;
			}
			else if(enemyPos == 2 && !enemyFollow)
			{
				arrived = badGuy->enemySeek(250 , 900, BG_HEIGHT, BG_WIDTH, timestep);
				//if enemy arrived at destination, change course
				if(arrived)
					enemyPos++;
			}
			else if(enemyPos == 3 && !enemyFollow)
			{
				arrived = badGuy->enemySeek(250 , 250, BG_HEIGHT, BG_WIDTH, timestep);
				//if enemy arrived at destination, change course
				if(arrived)
					enemyPos = 0;;
			}
			//if spacebar is pressed, enemy will chase player - TESTING PURPOSES
			if (keystate[SDL_SCANCODE_SPACE])
				badGuy->playerSeen = !badGuy->playerSeen;
			//as long as enemy should chase, update destination locataions/check if reached
			if(enemyFollow)
			{
				arrived = badGuy->enemyFollow(p->x_pos, p->y_pos, BG_HEIGHT, BG_WIDTH, timestep);
				if(arrived)
					enemyFollow = false;
			}
		}
		*/


		//move goblin squad



		//squad->moveCharacter(playerList, BG_HEIGHT, BG_WIDTH, timestep);

		//drawWeapon(all_weapons, cameraRect);


		int temp=p->spriteIndex;
		//animation for attacking
		double attackReference=SDL_GetTicks()-p->attackTime;
		//std::cout<<attackReference<<std::endl;
		if(attackReference < 400 && p->isAttacking){
			//std::cout<<"HERE"<<std::endl;
			if(p->attackTime==0) p->attackTime=SDL_GetTicks();
			switch(p->spriteIndex){
				case 0:
				if(weaponPickedUp){
					switch(p->currWeapon){
						case 1:
						p->spriteIndex = 8;
						
						break;
						case 2:
						p->spriteIndex = 13;
						
						break;
						case 3:
						p->spriteIndex = 23;
						
						break;
						case 4:
						p->spriteIndex=59;
						break;
						case 5:
						p->spriteIndex = 29;
						
						break;
						case 7:
						p->spriteIndex = 35;
						
						break;
					}
				}else{
					p->spriteIndex = 5;
					
				}
				break;
				case 1:
				if(weaponPickedUp){
					switch(p->currWeapon){
						case 1:
						p->spriteIndex = 10;
						
						break;
						case 2:
						p->spriteIndex = 16;
						
						break;
						case 3:
						p->spriteIndex = 22;
						
						break;
						case 4:
						p->spriteIndex=58;
						break;
						case 5:
						p->spriteIndex = 28;
						
						break;
						case 7:
						p->spriteIndex = 34;
						
						break;
					}
				}
				else{
					p->spriteIndex = 2;
					
				}
				break;
				case 36:
				if(weaponPickedUp){
					switch(p->currWeapon){
						case 1:
						p->spriteIndex = 41;
						
						break;
						case 2:
						p->spriteIndex = 44;
						
						break;
						case 3:
						p->spriteIndex = 47;
						
						break;
						case 4:
						p->spriteIndex = 60;
						break;
						case 5:
						p->spriteIndex = 50;
						
						break;
						case 7:
						p->spriteIndex = 53;
						
						break;
					}
				}
				else{
					p->spriteIndex = 39;
					
				}
				break;
				default:
				break;
			}
		}
		else if(attackReference<800 && p->isAttacking){
			if(attackReference<750 && !p->shot){ //attack
				if(weaponPickedUp && p->currWeapon==4){
					projectiles.push_back(p->shootBow());
					p->shot=true;
				}
				else{
					p->attack(enemies);
					//revieve enemy health vector
					//if online, udate enemy health
					//update enemy health here

					if(networkingFlag)
					{
							int index=0;
							for(Enemy* e:enemies) { 
								//send this enemy's health to server
								std::string hStr("U");
								hStr.append(std::to_string(index));
								hStr.append(".");
								hStr.append(std::to_string((int)e->health));
								//printf("--------------The Health string is: %s\n-----------------", hStr.c_str());

							//send out health update
								//printf("-------------Enemy Health update start---------------\n");
								NetworkingTest((void*)hStr.c_str(), false); 
								index++;
								//printf("-------------Enemy Health update end---------------\n");
							}
						}
							//end udate
					}
				}
			switch(p->spriteIndex){
				case 0:
				if(weaponPickedUp){
					switch(p->currWeapon){
						case 1:
						p->spriteIndex = 9;
						
						break;
						case 2:
						p->spriteIndex = 14;
						
						break;
						case 3:
						p->spriteIndex = 21;
						
						break;
						case 4:
						p->spriteIndex=59;
						break;
						case 5:
						p->spriteIndex = 27;
						
						break;
						case 7:
						p->spriteIndex = 33;
						
						break;
					}
				}
				else{
					p->spriteIndex = 4;
					
				}
				break;
				case 1:
				if(weaponPickedUp){
					switch(p->currWeapon){
						case 1:
						p->spriteIndex = 11;
						
						break;
						case 2:
						p->spriteIndex = 17;
						
						break;
						case 3:
						p->spriteIndex = 20;
						
						break;
						case 4:
						p->spriteIndex=58;
						break;
						case 5:
						p->spriteIndex = 26;
						
						break;
						case 7:
						p->spriteIndex = 32;
						
						break;
					}
				}
				else{
					p->spriteIndex = 3;
					
				}
				break;
				case 36:
				if(weaponPickedUp){
					switch(p->currWeapon){
						case 1:
						p->spriteIndex = 42;
						
						break;
						case 2:
						p->spriteIndex = 45;
						
						break;
						case 3:
						p->spriteIndex = 48;
						
						break;
						case 4:
						p->spriteIndex=60;
						break;
						case 5:
						p->spriteIndex = 51;
						
						break;
						case 7:
						p->spriteIndex = 54;
						
						break;
					}
				}
				else{
					p->spriteIndex = 37;
					
				}
				break;
				default:
				break;
			}
		}
		else if(p->attackTime != 0 && p->isAttacking)
		{
			p->attackTime=0;
			p->isAttacking=false;
			p->shot=false;
		}
		switch(p->currWeapon){
			case 1:
			weaponPickedUp = true;
			switch(p->spriteIndex){
				case 0:
				p->spriteIndex = 6;
				break;
				case 1:
				p->spriteIndex = 7;
				break;
				case 36:
				p->spriteIndex = 40;
				break;
			}
			break;
			case 2:
			weaponPickedUp = true;
			switch(p->spriteIndex){
				case 0:
				p->spriteIndex = 12;
				break;
				case 1:
				p->spriteIndex = 15;
				break;
				case 36:
				p->spriteIndex = 43;
				break;
			}
			break;
			case 3:
			weaponPickedUp = true;
			switch(p->spriteIndex){
				case 0:
				p->spriteIndex = 19;
				break;
				case 1:
				p->spriteIndex = 18;
				break;
				case 36:
				p->spriteIndex = 46;
				break;
			}
			break;
			case 4:
			weaponPickedUp=true;
			switch(p->spriteIndex){
				case 0:
				p->spriteIndex=56;
				break;
				case 1:
				p->spriteIndex=55;
				break;
				case 36:
				p->spriteIndex=57;
				break;
			}
			break;
			case 5:
			weaponPickedUp = true;
			switch(p->spriteIndex){
				case 0:
				p->spriteIndex = 25;
				break;
				case 1:
				p->spriteIndex = 24;
				break;
				case 36:
				p->spriteIndex = 49;
				break;
			}
			break;
			case 7:
			weaponPickedUp = true;
			switch(p->spriteIndex){
				case 0:
				p->spriteIndex = 31;
				break;
				case 1:
				p->spriteIndex = 30;
				break;
				case 36:
				p->spriteIndex = 52;
				break;
			}
			break;
		}
				//added this so the vector will have enemy health values initialized before damage updates start
		if(networkingFlag && startUp == 0)
			{
				int index=0;
				for(Enemy* e:enemies) { 
					//send this enemy's health to server
					std::string hStr("U");
					hStr.append(std::to_string(index));
					hStr.append(".");
					hStr.append(std::to_string((int)e->health));
					//printf("--------------The Health string is: %s\n-----------------", hStr.c_str());

				//send out health update
					//printf("-------------Enemy Health update start---------------\n");
					NetworkingTest((void*)hStr.c_str(), false); 
					index++;
					//printf("-------------Enemy Health update end---------------\n");
				}
				startUp++;
				//printf("out of startup\n");
			}
				//end udate


		//const char* currCount = NULL;
		// If online, update positions and draw other players
		if (networkingFlag) {
			std::string cStr("C");
			cStr.append(p->playerNumber);
			cStr.append(std::to_string(p->spriteIndex));
			cStr.append(".");
			cStr.append(std::to_string((int)p->x_pos));
			cStr.append(",");
			cStr.append(std::to_string((int)p->y_pos));
			cStr.append("*");
			if (flip == SDL_FLIP_HORIZONTAL) {
				cStr.append("1");
			}
			else{
				cStr.append("0");
			}
			// printf("Sending: %s\n", cStr.c_str());

			//send out our position
			NetworkingTest((void*)cStr.c_str(), false);
			// Receive the number of players from the server
			void *getNum = returnReceived();
			const char* currCount = (const char*)getNum;
			// free(getNum);

			//Turn the recieved value into an int
			std::stringstream streamCount(currCount);
			int i_count = 0;
			streamCount >> i_count;
			//printf("Curr Count: %d\n", i_count);

			std::string request("R");
			request.append(p->playerNumber);
			//printf("SENDING %s\n", request.c_str());
			// const char* request = "R";
			NetworkingTest((void*)request.c_str(), false);

			//Receive the coordinates of all other players
			const char* rec_test;
			int x = 0;
			int y = 0;
			int s = 0;
			for (int i = 1; i <= i_count; i++){
				if (cStr[1] == i + '0') {
					continue;
				}
				switch (i) {
					case 1:
						rec_test = (const char*)returnReceivedC1();
						// printf("--------------We received %s ----------------- %d\n", rec_test, i);
						break;
					case 2:
						rec_test = (const char*)returnReceivedC2();
						// printf("--------------We received %s ----------------- %d\n", rec_test, i);
						break;
					case 3:
						rec_test = (const char*)returnReceivedC3();
						// printf("--------------We received %s ----------------- %d\n", rec_test, i);
						break;
					case 4:
						rec_test = (const char*)returnReceivedC4();
						break;
					default:
						rec_test = (const char*)returnReceivedC1();
						// printf("--------------We received %s ----------------- %d\n", rec_test, i);

				}

				//Break down what we read in for this player into coordinates and a sprite index
				std::string mp_Coord(rec_test);
				if (mp_Coord.length() > 2) {
					size_t comma_index = mp_Coord.find(",");
					size_t period_index = mp_Coord.find(".");
					size_t star_index = mp_Coord.find("*");
					std::string spriteNum = mp_Coord.substr(2, period_index - 1);
					std::string mp_x = mp_Coord.substr(period_index + 1, comma_index - 2);
					std::string mp_y = mp_Coord.substr(comma_index + 1, star_index - 1);
					std::stringstream streamX(mp_x.c_str());
					std::stringstream streamY(mp_y.c_str());
					std::stringstream streamSprite(spriteNum);

					char flipNum = mp_Coord[star_index + 1];
					int f = flipNum - '0';

					streamX >> x;
					streamY >> y;
					streamSprite >> s;

					//printf("X: %d --- Y: %d\n", x, y);
					//printf("Sprite num: %d\n", s);
					// printf("Flip num: %d\n", f);

					Player *mp = playerList[i-1];
					mp->x_pos = x;
					mp->y_pos = y;
					mp->spriteIndex = s;
					if (f == 1) {
						mp->flip = SDL_FLIP_HORIZONTAL;
						// printf("Flip num: %d\n", f);
					}
					else{
						mp->flip = SDL_FLIP_NONE;
						// printf("Flip num: %d\n", f);
					}
					playerList[i-1] = mp;

					drawMultiplayer(mp, cameraRect, timestep);

					x = 0;
					y = 0;
					s = 0;
				}
			}
		}
				//revieve enemy health vector
		//if online, udate enemy health
		if(networkingFlag){
			int index=0;
			//printf("REal deal\n");
			for(Enemy* e:enemies) { 
				std::string request("V");
				request.append(std::to_string(index));
				//printf("BEFORE NET\n");
				NetworkingTest((void*)request.c_str(), false);
				//printf("AFTER NET\n");
				const char* rec_test = (const char*)returnReceivedH();

				int hp = 0;
				std::string hpStr(rec_test);
				std::string eHp = hpStr.substr(1, hpStr.length());
				std::stringstream streamH(eHp.c_str());

				streamH >> hp;
				e->health = hp;
				//printf("health updated! HP is now %d\n", hp);
				
				index++;
			}
		} 

		//The easiest way to handle the pressure map is to zero it out each time - since the operation is cheap, frequently changing, and pretty space out, it isn't too harmful to do this for now
		//To make it more efficient we can only focus on enemies that are in the same room as a player - worry about that later.  We can store the enemies by the rooms they spawn in/are in.
		memset(pressureMap, 0, sizeof(pressureMap));
		memset(blurMap, 0, sizeof(blurMap));

		int ymax=sizeof(pressureMap)/sizeof(pressureMap[0]);
		int xmax=sizeof(pressureMap[0])/sizeof(pressureMap[0][0]);

		//This is calculating our pressureMap and determining what indices we need to blur on our blurMap
		for(Enemy* pressureApp:enemies){

			//What indices of the pressure grid the enemy is at
			int moddedx=(int) pressureApp->x_pos/PRESSURE_GRID_SIZE;
			int moddedy=(int) pressureApp->y_pos/PRESSURE_GRID_SIZE;

			int z=(int)log2(pressureApp->getPressure());


			for(int i=z+1; i>=0; i--){
				//The furthest out indices just need to be adjusted for our blur matrix, not our pressure matrix
				if(i==z+1){
					int j=(z+1)-i;
					if(moddedy-i>=0){
						//Is it bad to check these twice?  Prolly.  Will it kill us?  Nah.
						if(moddedx-j>=0) 	{
							blurMap[moddedy-i][moddedx-j]=-1;
						}
						if(moddedx+j<xmax){
							blurMap[moddedy-i][moddedx+j]=-1;
						}
					}
					if(moddedy+i<ymax){
						if(moddedx-j>=0) {
							blurMap[moddedy+i][moddedx-j]=-1;
						}
						if(moddedx+j<xmax) {
							blurMap[moddedy+i][moddedx+j]=-1;
						}
					}
				}
				else{
					for(int j=z-i; j>=0; j--){
						int v=std::pow(2, z-(i+j));
						if(moddedy-i>=0){
							//Is it bad to check these twice?  Prolly.  Will it kill us?  Nah.
							if(moddedx-j>=0) 	{
								pressureMap[moddedy-i][moddedx-j]+=v;
								blurMap[moddedy-i][moddedx-j]=-1;
							}
							if(moddedx+j<xmax){
								pressureMap[moddedy-i][moddedx+j]+=v;
								blurMap[moddedy-i][moddedx+j]=-1;
							}
						}
						if(moddedy+i<ymax && i!=0 && j!=0){
							if(moddedx-j>=0) {
								pressureMap[moddedy+i][moddedx-j]+=v;
								blurMap[moddedy+i][moddedx-j]=-1;
							}
							if(moddedx+j<xmax) {
								pressureMap[moddedy+i][moddedx+j]+=v;
								blurMap[moddedy+i][moddedx+j]=-1;
							}
						}
					}
				}
			}
		}

		//blurPressure.  We should eventually get smarter with where we start and end our i and j, as this is pretty naive right now.
		//This follows the Image Blur algorithm.
		int mapXSize=sizeof(blurMap[0])/sizeof(blurMap[0][0]);
		int mapYSize=sizeof(blurMap)/sizeof(blurMap[0]);
		for(int i=0; i<mapYSize; i++){
			for(int j=0; j<mapXSize; j++){
				//-1 means that we need to calculate the space's value
				if(blurMap[i][j]==-1){
					double sum=0;
					for(int f=0; f<3; f++){
						for(int z=0; z<3; z++){
							//We are OOB in any direction
							//This isn't a perfect solution, but it can be argued that being against a wall in this case would have a higher pressure value than normal
							if(i+f-1<0 || i+f-1>ymax || j+z-1<0 || j+z-1>xmax) sum=sum+(pressureMap[i][j]/2)*convolutionFilter[f][z];
							else sum=sum+pressureMap[i+f-1][j+z-1]*convolutionFilter[f][z];
						}
					}
					blurMap[i][j]=sum;
				}
			}
		}


		int playerXADJ=p->x_pos/PRESSURE_GRID_SIZE;
		int playerYADJ=p->y_pos/PRESSURE_GRID_SIZE;
		p->pressureValue=blurMap[playerYADJ][playerXADJ];
		//std::cout<<blurMap[playerYADJ][playerXADJ]<<std::endl;
		int enemyIndex=0;
        for(Enemy* e:enemies){
            if(e->health<=0 && enemyIndex<enemies.size())
                enemies.erase(enemies.begin()+enemyIndex);
            else{
                Arrow* arr = e->moveCharacter(playerList, testRoom->getFloorMap(), FloorHeight, FloorWidth, BG_HEIGHT, BG_WIDTH, timestep);
                if(arr!=nullptr) projectiles.push_back(arr);
                enemyIndex++;
            }
        }






		/*
		int enemyIndex=0;
		for(Enemy* e:enemies){
			if(e->health<=0 && !e->checkSquad()) //if enemy has <= 0 health, and is not a goblin squad
				enemies.erase(enemies.begin()+enemyIndex);
			else if(e->checkSquad() && e->goblinNum == 0) //if enemy is a goblin squad, and has no more goblins
				enemies.erase(enemies.begin()+enemyIndex);
				if(enemies.size() == 0 && curFloorNum == NUM_FLOORS + 1){
					endGame = true;
				}
			else{
				Arrow* arr = e->moveCharacter(playerList, testRoom->getFloorMap(), FloorHeight, FloorWidth, BG_HEIGHT, BG_WIDTH, timestep);
				if(arr!=nullptr) projectiles.push_back(arr);
			}
			enemyIndex++;
		}
		*/
		//printf("----NUM_ OF _ PLAYERS %d---\n", num_of_players);
		//printf("-------- PLAYERLIST SIZE -------%d\n", playerList.size());
		//Handling collisions
		//for(int i=0; i<num_of_players; i++){
			//Player v Map
		collision(testRoom->getFloorMap(), p, timestep);
    	//}

		//Enemy v Enemy
		for(int i=0; i<enemies.size(); i++){
			for(int j=i+1; j<enemies.size(); j++)
				collision(enemies[i], enemies[j], timestep);
			//Enemy v Map
			collision(testRoom->getFloorMap(), enemies[i], timestep);
		}



		//Arrow v Player
		//Arrow v Wall
		//Arrow v Enemy
		int projtemp=0;
		while(projtemp<projectiles.size()){
			Arrow* proj=projectiles[projtemp];
			Character* charImpacted=nullptr;
			if(!(proj->friendly)){
				for(Player* p:playerList){
					if(collision(proj, p)==-1){
						p->damageCharacter(proj->getDamage());
						//std::cout<<p->health;
						//std::cout<<"  ";
						//std::cout<<p->healthBuffer<<std::endl;
						projectiles.erase(projectiles.begin()+projtemp);
						charImpacted=p;
						break;

					}

				}
				if(charImpacted==nullptr){
					if(proj->moveProjectile(BG_HEIGHT, BG_WIDTH, timestep)==-1 || collision(testRoom->getFloorMap(), proj)==-1){
						//std::cout<<"Collision Detected at "<<proj->x_pos<<", "<<proj->y_pos<<"; Archer coords "<<kite->x_pos<<" "<<kite->y_pos<<std::endl;
						projectiles.erase(projectiles.begin()+projtemp);

					}
					else{

						projtemp++;
						drawProjectile(proj, cameraRect);
					}
				}
			}
			else{
				for(Enemy* e:enemies){
					if(collision(proj, e)==-1){
						SDL_Rect result;
						//first check if the enemy is blocking - will only be true for knight
						if(e->returnBlocking())
						{
							//if enemy is blocking, check if the incoming attack intersects with the blocking rect
							if(SDL_IntersectRect(proj->getHitBox(), e->returnBlockBox(), &result)){
								//if in here, attack hit the blocking box, do not remove health
							}
							//else, check if attack hit the blocking knight, but did not get blocked, so remove health
							else if(SDL_IntersectRect(proj->getHitBox(), &e->characterTree->root->hitbox, &result))
								e->damageCharacter(proj->getDamage());
								//e->damageCharacter(damage); //Make this weapons[currWeapon]->damage eventually once they figure out how to handle equipping weapons.  Remember unarmed attacks!
						}
						else
							e->damageCharacter(proj->getDamage());
						
						projectiles.erase(projectiles.begin()+projtemp);
						
						charImpacted=e;
						break;
					}
				}
				if(charImpacted==nullptr){
					if(proj->moveProjectile(BG_HEIGHT, BG_WIDTH, timestep)==-1 || collision(testRoom->getFloorMap(), proj)==-1){
						//std::cout<<"Collision Detected at "<<proj->x_pos<<", "<<proj->y_pos<<"; Archer coords "<<kite->x_pos<<" "<<kite->y_pos<<std::endl;
						projectiles.erase(projectiles.begin()+projtemp);

					}
					else{

						projtemp++;
						drawProjectile(proj, cameraRect);
					}
				}
			}

		}


		for(Enemy* e:enemies){
			drawEnemy(e, cameraRect, timestep);
		}

		//SDL_Texture* blackBar = loadImage("sprites/blackHealthBar.png");
		//SDL_Texture* greenBar = loadImage("sprites/greenHealthBar.png");
		SDL_Rect blackHealth = {0, 0, 200, 200};
		SDL_Rect greenHealth = {0, 0, static_cast<int>(p->health)/3, 200};
		SDL_RenderCopy(gRenderer, blackBar, NULL, &blackHealth);
		SDL_RenderCopy(gRenderer, greenBar, NULL, &greenHealth);
		//Add player to screen and present
		if(p->health <= 0){
			respawnPlayer();
			flip = SDL_FLIP_NONE;
			p->spriteIndex = 0;
			timestep = 0;
			temp = 0;
		}
		SDL_RenderCopyEx(gRenderer, p->getSprite(timestep), NULL, &p->dstrect, 0.0, nullptr, flip);
		SDL_RenderPresent(gRenderer);
		p->spriteIndex = temp;

    	//determine if the player entered the stairs

		if(networkingFlag){
			int c;
			for(c = 0; c < num_of_players; c++){
				if(playerList[c]->x_pos +30 > stairsXCoord && playerList[c]->x_pos + 30 < (stairsXCoord + 100) && playerList[c]->y_pos + 60 > stairsYCoord && playerList[c]->y_pos + 60 < (stairsYCoord + 100)){
				//we have an intersection with the stairs
				//printf("NUM_OF_PLAYERS = %d\n", num_of_players);
				//printf("SIZE OF PLAYER LIST = %d\n", playerList.size());
				//so we need to regenerate the map
				//printf("SENDING SEED!\n");
				delete testRoom;
				char *resetSeed = "EEEE";
				bool flag = false;
				NetworkingTest((void*)resetSeed, flag);
				char *buffer3 = "SEED";
				bool flag2 = false;
				NetworkingTest((void*)buffer3, flag2);
				int seed2 = getSeed();
				//printf("SEED 2 IS %d\n", seed2);

				//Code for boss room spawning
				curFloorNum++;
				if(curFloorNum % TILE_SWAP == 0){
					curTileType++;
				}
        		if(curFloorNum == NUM_FLOORS + 1){
					FloorHeight = FloorWidth = 11;
					BG_HEIGHT = BG_WIDTH = (FloorWidth * 100);
					testRoom = new Floor(11,11, seed2);
					testRoom->generateBossFloor(networkingFlag);
				}
				else{
					updateFloorSize();
					testRoom = new Floor(FloorHeight,FloorWidth,seed2);
					testRoom->generate(3,3,networkingFlag);
				}
				maps.clear(); //clear current map out of array
				respawnX = testRoom->getSpawnX();
				respawnY = testRoom->getSpawnY();
				drawMap(testRoom->getFloorMap(), testRoom->getChestArray(), testRoom->getChestArraySize(), testRoom->getSpawnX(), testRoom->getSpawnY(), testRoom->getStairX(), testRoom->getStairY());
				maps.push_back(loadImage("testMapImg.png"));
				stairsXCoord = testRoom->getStairX();
				stairsYCoord = testRoom->getStairY();

				p->x_pos = testRoom-> getSpawnX();
				p->y_pos = testRoom-> getSpawnY();
				
				//---------------------- AI ENEMY SPAWN METHOD ------------------------
				//remove any leftover enemies from the floor - ie those that have not been killed
				while(!enemies.empty())
				{
					enemies.clear();
				}
				    std::vector<std::pair<std::string,double>> zScores = gameMaster.findZScores();

				for(auto& i : zScores){
					if(i.first == "Grunt"){
						if(i.second > gameMaster.modelGruntThreshold)
							i.second = gameMaster.modelGruntThreshold;
					}
					else if(i.first == "Archer"){
						if(i.second > gameMaster.modelArcherThreshold)
							i.second = gameMaster.modelArcherThreshold;
					}
					else if(i.first == "Goblin"){
						if(i.second > gameMaster.modelGoblinThreshold)
							i.second = gameMaster.modelGoblinThreshold;
					}
					else if(i.first == "Knight"){
						if(i.second > gameMaster.modelKnightThreshold)
							i.second = gameMaster.modelKnightThreshold;
					}
					else{
						if(i.second > gameMaster.modelAssassinThreshold)
							i.second = gameMaster.modelAssassinThreshold;
					}
				}
				//0 = grunt
				//1 = archer
				//2 = goblin
				//3 = knight
				//4 = assassin
				//vector of enemies to spawn in
				std::vector<int> enemySpawnVector;
				//highest z score hardest enemy

				// roll 1-8 7 and 8 to highest decreasing vals for each zscore
				int spawner;
				int highRand = 7;
				int lowRand = 6;
				int enemySpawnType;
				for(int i = 4; i > 0; i--){
					if(zScores.at(i).first == "Grunt")
						enemySpawnType = 0;
					else if(zScores.at(i).first == "Archer")
						enemySpawnType = 1;
					else if(zScores.at(i).first == "Goblin")
						enemySpawnType = 2;
					else if(zScores.at(i).first == "Knight")
						enemySpawnType = 3;
					else if(zScores.at(i).first == "Assassin")
						enemySpawnType = 4;

					//spawn enemies based on high and low vals
					spawner = rand() %(highRand-lowRand + 1) + lowRand;

					highRand--;
					lowRand--;
					for(int j = 0; j < spawner; j++){
						enemySpawnVector.push_back(enemySpawnType);
					}
				}
				while(enemySpawnVector.size() < enemySpawns.size()){
					int enemy = rand() % (4-0 + 1) + 0;
					enemySpawnVector.push_back(enemy);
				}
				//randomize for balance
				if(!networkingFlag){
					for (int i = 0; i < enemySpawnVector.size(); i++) {
        				int j = i + rand() % (enemySpawnVector.size() - i); 
        				std::swap(enemySpawnVector[i], enemySpawnVector[j]);
    				}
				}
			
				int spawnCounter = 0;
				//spawn in enemies for all spawn points given 
				for(enemySpawn point : enemySpawns)
				{
					//grab the x and y coord for this spawn point
					double x_coord = point.x;
					double y_coord = point.y;
					
					//check if the coordinates are a floor tile, if not, do not spawn on it
					if(placeHold->isFloor(testRoom->getFloorMap(), x_coord/100, y_coord/100, testRoom->getFloorHeight(), testRoom->getFloorWidth()))
					{
						int enemyType;
						//decide which enemy to spawn
						if(!networkingFlag){
							enemyType = enemySpawnVector.at(spawnCounter);
						}
						else{
							enemyType = rand() % 5;
							}
						spawnCounter++;
						//depending on enemyType value, spawn an enemy for that number
						if(enemyType == 0) //spawn grunt
						{
							Enemy* grunt = new Enemy(x_coord, y_coord, SKELETON_WIDTH, SKELETON_HEIGHT, GRUNT_MAX_HEALTH+gruntHealthIncrease); 
							enemies.push_back(grunt);
							gameMaster.addGruntSample(1);
						}
						else if(enemyType == 1) //spawn archer
						{
							Archer* archer = new Archer(x_coord, y_coord, SKELETON_WIDTH, SKELETON_HEIGHT, ARCHER_MAX_HEALTH+archerHealthIncrease); 
							enemies.push_back(archer);
							gameMaster.addArcherSample(1);
						}
						else if(enemyType == 2) //goblin squad
						{
							GoblinSquad* squad = new GoblinSquad(x_coord, y_coord, 0, 0, 999999999999);
							squad->spawnSquad();
							enemies.push_back(squad);
							gameMaster.addGoblinSample(1);
							for(Goblin* g:squad->goblins){
								if(placeHold->isFloor(testRoom->getFloorMap(),(int) g->x_pos/100,(int) g->y_pos/100, testRoom->getFloorHeight(), testRoom->getFloorWidth()))
									enemies.push_back(g);
								else
									squad->goblinNum--;
							}
						}
						else if(enemyType == 3) //knight
						{
							Knight* knight = new Knight(x_coord, y_coord, KNIGHT_WIDTH, KNIGHT_HEIGHT, KNIGHT_MAX_HEALTH+knightHealthIncrease);
							enemies.push_back(knight);
							gameMaster.addKnightSample(1);
						}
						else if(enemyType == 4) //assassin
						{
							Assassin* sneaky = new Assassin(x_coord, y_coord, ASSASSIN_WIDTH, ASSASSIN_HEIGHT, ASSASSIN_MAX_HEALTH); 
							enemies.push_back(sneaky);
							gameMaster.addAssassinSample(1);
						}
					}
					
				}
				//increment how much health is added to enemies per level
				gruntHealthIncrease = gruntHealthIncrease + 3;
				archerHealthIncrease = archerHealthIncrease + 3;
				goblinHealthIncrease = goblinHealthIncrease + 2;
				knightHealthIncrease = knightHealthIncrease + 5;
				assassinHealthIncrease = assassinHealthIncrease +2;
				//-------------------- END AI ENEMY SPAWN METHOD ----------------------

				for (int i = 0; i < testRoom->getChestArraySize(); i++) {
			testRoom->getChestArray()[i].fillChestInventory(all_weapons, curFloorNum);

			for(int z = 0; z < 3; z++){

			if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 1){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/sword.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 2){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/axe.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 3){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/longsword.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 4){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/bow.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 5){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/hammer.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 6){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/throwingAxe.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 7){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/trident.png"));
			}
			
		}
		
	}

				SDL_Delay(500);

				//fill the chests with new items
				//chestArray[0].fillChestInventory(all_weapons);
				//chestArray[1].fillChestInventory(all_weapons);
				}
			}
		}

		else{
			if(p->x_pos +30 > stairsXCoord && p->x_pos + 30 < (stairsXCoord + 100) && p->y_pos + 60 > stairsYCoord && p->y_pos + 60 < (stairsYCoord + 100)){
						//we have an intersection with the stairs
						//so we need to regenerate the map
						delete testRoom;
				//Code for boss room spawning
						curFloorNum++;
						if(curFloorNum % TILE_SWAP == 0){
							curTileType++;
						}
						int floorSeed=rand();	//Randomize the seed for next floor.
	        			if(curFloorNum == NUM_FLOORS + 1){
							FloorHeight = FloorWidth = 11;
							BG_HEIGHT = BG_WIDTH = (FloorWidth * 100);
							testRoom = new Floor(11,11, floorSeed);
							testRoom->generateBossFloor(networkingFlag);
						}
						else{
							updateFloorSize();
							testRoom = new Floor(FloorHeight,FloorWidth,floorSeed);
							testRoom->generate(3,3,networkingFlag);
							
						}

						maps.clear(); //clear current map out of array
						drawMap(testRoom->getFloorMap(), testRoom->getChestArray(), testRoom->getChestArraySize(), testRoom->getSpawnX(), testRoom->getSpawnY(), testRoom->getStairX(), testRoom->getStairY());
						maps.push_back(loadImage("testMapImg.png"));
						stairsXCoord = testRoom->getStairX();
						stairsYCoord = testRoom->getStairY();

						p->x_pos = testRoom-> getSpawnX();
						p->y_pos = testRoom-> getSpawnY();
						respawnX = testRoom->getSpawnX();
						respawnY = testRoom->getSpawnY();
						
						
						//---------------------- AI ENEMY SPAWN METHOD ------------------------
						//remove any leftover enemies from the floor - ie those that have not been killed
						while(!enemies.empty())
						{
							enemies.clear();
						}
						std::vector<std::pair<std::string,double>> zScores = gameMaster.findZScores();

						for(auto& i : zScores){
							if(i.first == "Grunt"){
								if(i.second > gameMaster.modelGruntThreshold)
									i.second = gameMaster.modelGruntThreshold;
							}
							else if(i.first == "Archer"){
								if(i.second > gameMaster.modelArcherThreshold)
									i.second = gameMaster.modelArcherThreshold;
							}
							else if(i.first == "Goblin"){
								if(i.second > gameMaster.modelGoblinThreshold)
									i.second = gameMaster.modelGoblinThreshold;
							}
							else if(i.first == "Knight"){
								if(i.second > gameMaster.modelKnightThreshold)
									i.second = gameMaster.modelKnightThreshold;
							}
							else{
								if(i.second > gameMaster.modelAssassinThreshold)
									i.second = gameMaster.modelAssassinThreshold;
							}
						}
						//0 = grunt
						//1 = archer
						//2 = goblin
						//3 = knight
						//4 = assassin
						//vector of enemies to spawn in
						std::vector<int> enemySpawnVector;
						//highest z score hardest enemy

						// roll 1-8 7 and 8 to highest decreasing vals for each zscore
						int spawner;
						int highRand = 7;
						int lowRand = 6;
						int enemySpawnType;
						for(int i = 4; i > 0; i--){
							if(zScores.at(i).first == "Grunt")
								enemySpawnType = 0;
							else if(zScores.at(i).first == "Archer")
								enemySpawnType = 1;
							else if(zScores.at(i).first == "Goblin")
								enemySpawnType = 2;
							else if(zScores.at(i).first == "Knight")
								enemySpawnType = 3;
							else if(zScores.at(i).first == "Assassin")
								enemySpawnType = 4;

							//spawn enemies based on high and low vals
							spawner = rand() %(highRand-lowRand + 1) + lowRand;

							highRand--;
							lowRand--;
							for(int j = 0; j < spawner; j++){
								enemySpawnVector.push_back(enemySpawnType);
							}
						}
						while(enemySpawnVector.size() < enemySpawns.size()){
							int enemy = rand() % (4-0 + 1) + 0;
							enemySpawnVector.push_back(enemy);
						}
						//randomize for balance
						if(!networkingFlag){
							for (int i = 0; i < enemySpawnVector.size(); i++) {
        						int j = i + rand() % (enemySpawnVector.size() - i); 
        						std::swap(enemySpawnVector[i], enemySpawnVector[j]);
   							}
						}
						int spawnCounter = 0;
				
						//spawn in enemies for all spawn points given 
						for(enemySpawn point : enemySpawns)
						{
							//grab the x and y coord for this spawn point
							double x_coord = point.x;
							double y_coord = point.y;
							
							//check if the coordinates are a floor tile, if not, do not spawn on it
							if(placeHold->isFloor(testRoom->getFloorMap(), x_coord/100, y_coord/100, testRoom->getFloorHeight(), testRoom->getFloorWidth()))
							{
								int enemyType;
								//decide which enemy to spawn
								if(!networkingFlag){
									enemyType = enemySpawnVector.at(spawnCounter);
								}
								else{
									enemyType = rand() % 5;
								}
								spawnCounter++;
								
								//depending on enemyType value, spawn an enemy for that number
								if(enemyType == 0) //spawn grunt
								{
									Enemy* grunt = new Enemy(x_coord, y_coord, SKELETON_WIDTH, SKELETON_HEIGHT, GRUNT_MAX_HEALTH+gruntHealthIncrease); 
									enemies.push_back(grunt);
									gameMaster.addGruntSample(1);
								}
								else if(enemyType == 1) //spawn archer
								{
									Archer* archer = new Archer(x_coord, y_coord, SKELETON_WIDTH, SKELETON_HEIGHT, ARCHER_MAX_HEALTH+archerHealthIncrease); 
									enemies.push_back(archer);
									gameMaster.addArcherSample(1);
								}
								else if(enemyType == 2) //goblin squad
								{
									GoblinSquad* squad = new GoblinSquad(x_coord, y_coord, 0, 0, 999999999999);
									squad->spawnSquad();
									enemies.push_back(squad);
									gameMaster.addGoblinSample(1);
									for(Goblin* g:squad->goblins){
										if(placeHold->isFloor(testRoom->getFloorMap(),(int) g->x_pos/100,(int) g->y_pos/100, testRoom->getFloorHeight(), testRoom->getFloorWidth()))
											enemies.push_back(g);
										else
											squad->goblinNum--;
									}
								}
								else if(enemyType == 3) //knight
								{
									Knight* knight = new Knight(x_coord, y_coord, KNIGHT_WIDTH, KNIGHT_HEIGHT, KNIGHT_MAX_HEALTH+knightHealthIncrease);
									enemies.push_back(knight);
									gameMaster.addKnightSample(1);
								}
								else if(enemyType == 4) //assassin
								{
									Assassin* sneaky = new Assassin(x_coord, y_coord, ASSASSIN_WIDTH, ASSASSIN_HEIGHT, ASSASSIN_MAX_HEALTH+assassinHealthIncrease); 
									enemies.push_back(sneaky);
									gameMaster.addAssassinSample(1);
								}
							}
							
						}
						//increment how much health is added to enemies per level
						gruntHealthIncrease = gruntHealthIncrease + 3;
						archerHealthIncrease = archerHealthIncrease + 3;
						goblinHealthIncrease = goblinHealthIncrease + 2;
						knightHealthIncrease = knightHealthIncrease + 5;
						assassinHealthIncrease = assassinHealthIncrease +2;
						//-------------------- END AI ENEMY SPAWN METHOD ----------------------
							

						for (int i = 0; i < testRoom->getChestArraySize(); i++) {
			testRoom->getChestArray()[i].fillChestInventory(all_weapons, curFloorNum);

			for(int z = 0; z < 3; z++){

			if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 1){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/sword.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 2){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/axe.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 3){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/longsword.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 4){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/bow.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 5){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/hammer.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 6){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/throwingAxe.png"));
			}

			else if(testRoom->getChestArray()[i].getChestInventory().at(z)->weapon_type == 7){
				testRoom->getChestArray()[i].getChestInventory().at(z)->sprites.push_back(loadImage("sprites/trident.png"));
			}

		}
	}

						//fill the chests with new items
						//chestArray[0].fillChestInventory(all_weapons);
						//chestArray[1].fillChestInventory(all_weapons);

						SDL_Delay(500);
			     }
       }


}
	//bool to activate credits & if statement for it
	bool credits = false;
	if(credits)
	{

				// Load media
		gTex.push_back(loadImage("credits/ConnorAllington.png"));
		gTex.push_back(loadImage("credits/TristinButz.png"));
		gTex.push_back(loadImage("credits/bickerstaff.png"));
		gTex.push_back(loadImage("credits/SebastianMarcano.png"));
		gTex.push_back(loadImage("credits/DestinyThompson.png"));
		gTex.push_back(loadImage("credits/ZachStata.png"));
		gTex.push_back(loadImage("credits/AndrewPreston.png"));
		gTex.push_back(loadImage("credits/AdamMattioli.png"));
		gTex.push_back(loadImage("credits/BryceRyan.png"));
		gTex.push_back(loadImage("credits/tristanpossessky.png"));
		gTex.push_back(loadImage("credits/MConlon.png"));

		SDL_RenderClear(gRenderer);
		SDL_RenderCopy(gRenderer, loadImage("credits/youWin.png"), NULL, NULL);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(3000);

		SDL_RenderClear(gRenderer);
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
		SDL_RenderCopy(gRenderer, gTex[1], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image
		SDL_RenderCopy(gRenderer, gTex[2], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image
		SDL_RenderCopy(gRenderer, gTex[3], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image
		SDL_RenderCopy(gRenderer, gTex[4], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image
		SDL_RenderCopy(gRenderer, gTex[5], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
		SDL_RenderCopy(gRenderer, gTex[6], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
		SDL_RenderCopy(gRenderer, gTex[7], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
		SDL_RenderCopy(gRenderer, gTex[8], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
		SDL_RenderCopy(gRenderer, gTex[9], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
		SDL_RenderCopy(gRenderer, gTex[10], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
			//Add this chunk of code for each image - start
			// Render the image- change gTex[i] for i = image number
		SDL_RenderCopy(gRenderer, gTex[11], NULL, NULL);
			// Display rendering
		SDL_RenderPresent(gRenderer);
			// Wait 3 seconds
		SDL_Delay(3000);
			//************************************* - end
	}
	gameMaster.writeFile();
	close();
	return 0;
}
