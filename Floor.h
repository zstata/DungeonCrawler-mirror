#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "BSPLeaf.h"
#include "TileList.h"
#include "Chest.h"

#ifndef FLOOR_H
#define FLOOR_H
struct enemySpawn {
	int x;
	int y;
};


class Floor{
    public:
        //Just generates 1 room empty room for now not really 
        Floor(int height, int width);
        Floor(int height, int width, unsigned int Seed);
        int** getFloorMap();
        int getStairX();
        int getStairY();
		int getSpawnX();
        int getSpawnY();
        int getFloorWidth();
        int getFloorHeight();
        std::vector<int> getMultiSpawnX();
        std::vector<int> getMultiSpawnY();
        void generate(int rMinHeight, int rMinWidth, bool netFlag);
        void generateBossFloor(bool netFlag);
        void generateLootFloor(bool netFlag);
        int getChestArraySize();
        Chest* getChestArray();
		std::vector<enemySpawn> getEnemySpawns();
        ~Floor();
  
    private:
        bool seedFlag;
        unsigned int seed;
        int** floorMap;
        int floorHeight;
        int floorWidth;
        int stairX;
        int stairY;
		int spawnX;
		int spawnY;
        Chest* chestArray;
        int chestArraySize;
		std::vector<enemySpawn> enemySpawns;
};

#endif