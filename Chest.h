#include "Weapon.h"
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#ifndef CHEST_H
#define CHEST_H

class Chest{
    public:
        Chest(int x, int y);
        Chest();
        int getXCoord();
        int getYCoord();
        std::vector<Weapon*> * getInventoryPointer();
        std::vector<Weapon*> getChestInventory(); //will get a pointer to the start of the array
        void fillChestInventory(std::vector<Weapon*> weaponVector, int floorNumber);
        bool chestClicked(SDL_Event e, int xOffset, int yOffset);
  
    private:
        int xCoord;
        int yCoord;
        std::vector<Weapon*> chestInventory;
        std::vector<Weapon*> *inventoryPointer;
};

#endif