#include "Chest.h"
#include "Weapon.h"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

//private data
int xCoord;
int yCoord;
//Weapon* chestInventory;
std::vector<Weapon*> chestInventory;
std::vector<Weapon*> *inventoryPointer;

//constructor
Chest::Chest(int x, int y){
	xCoord = x;
	yCoord = y;
	inventoryPointer = &chestInventory;
}

Chest::Chest(){

}

//getter for x coord of the chest
int Chest::getXCoord(){
    return xCoord;
}

//getter for y coord of the chest
int Chest::getYCoord(){
    return yCoord;
}

std::vector<Weapon*> * Chest::getInventoryPointer(){
	return inventoryPointer;
}

//getter for the inventory of the chest
std::vector<Weapon*> Chest::getChestInventory(){
    return chestInventory;
}

//takes in a vector of weapons and will randomly place them into a chest i guess
void Chest::fillChestInventory(std::vector<Weapon*> weaponVector, int floorNumber){

	
	int x;
	int randWeapon;
	int randRarity;

	int rarity;
	int damage;

	int legendary = 1 + (floorNumber*1);
	int epic = 5 + (floorNumber*2);
	int rare = 10 + (floorNumber*3);
	int uncommon = 20 + (floorNumber*4);

	if(uncommon > 100){
		uncommon = 0;
	}

	if(rare > 100){
		rare = 0;
	}

	if(epic > 100){
		epic = 0;
	}

	for(x = 0; x < 3; x++){

		//generate a random number 0 through 6 for each of 7 weapon
		randWeapon = rand() % 7;
		randRarity = rand() % 100;

		if(randRarity < legendary){
			rarity = 5;
			damage = (rand() % 21) + 40;
		}

		else if(randRarity < epic){
			rarity = 4;
			damage = (rand() % 11) + 30;
		}

		else if(randRarity < rare){
			rarity = 3;
			damage = (rand() % 11) + 20;
		}

		else if(randRarity < uncommon){
			rarity = 2;
			damage = (rand() % 6) + 15;
		}

		else {
			rarity = 1;
			damage = (rand() % 6) + 10;
		}

		//change here
		if(randWeapon == 5){
			randWeapon = 6;
		}

		//set the weapon characteristics here

		Weapon *newWeapon = new Weapon(0, 0, 65, 65, randWeapon+1);
		//weapon rarity
		newWeapon->rarity = rarity;
		//weapon damage

		newWeapon->damage = damage;

		//chestInventory.push_back(weaponVector.at(randWeapon));
		chestInventory.push_back(newWeapon);
	}

}

bool Chest::chestClicked(SDL_Event e, int xOffset, int yOffset){

	int newXCoord = xCoord - xOffset;
	int newYCoord = yCoord - yOffset;

	int xEnd = newXCoord + 64;
	int yEnd = newYCoord + 64;

	int mouseClickX = e.button.x;
	int mouseClickY = e.button.y;

	if(mouseClickX > newXCoord && mouseClickX < xEnd && mouseClickY > newYCoord && mouseClickY < yEnd){
		//then we have clicked on the chest
		return true;
	}

	return false; //if we did not click on the chest
}




