#include "Floor.h"
#include <cstdlib>
#include <ctime>
#include "TileList.h"
#include <iostream>
#include "Chest.h"

    //constant values to represent the codes for each tile type

    const int MAX_LEAF_SIZE=10; //Max leaf size is defined here but min is defined in the leaf class.

    int BOSS_FLOOR[11][11] ={   {00000000000},
                                {01111111110},
                                {01111111110},
                                {01111111110},
                                {01111111110},
                                {01111311110},
                                {01111111110},
                                {01111111110},
                                {01111111110},
                                {01111111110},
                                {00000000000}}; 
    int LOOT_FLOOR[11][11] ={   {00000000000},
                                {01111111110},
                                {01111111110},
                                {01111111110},
                                {01111111110},
                                {02111311110},
                                {01111111110},
                                {01111111110},
                                {01111111110},
                                {01111111110},
                                {00000000000}}; 

    int** floorMap;
    int floorHeight;
    int floorWidth;
    int stairX;
    int stairY;
	int spawnX; 
    std::vector<int> spawnXArr;
    std::vector<int> spawnYArr;
	int spawnY; 
    //Chest* chestArray;
    int chestArraySize;

    struct Room** roomArray;

    bool seedFlag;
    unsigned int seed;

    //used to determine amount of players for spawn tiles
    int getPlayerNumber();

    int Floor::getFloorWidth(){
        return floorWidth;
    }

    int Floor::getFloorHeight(){
        return floorHeight;
    }

    Floor::Floor(int height,int width){
        seedFlag=false;
        seed=0;

        floorHeight=height;
        floorWidth=width;
        floorMap=new int*[floorWidth];

        srand(time(0));
        int xRand = rand() % 19; //generate random x value for stairs
        int yRand = rand() % 9; //generates random y value for stairs

        int xSpawn = rand() % 19; //generate random x value for spawn
        int ySpawn = rand() % 9; //generates random y value for spawn

		while(xRand == xSpawn && yRand == ySpawn) { //in case the spawn coordinate overlaps with the stair coordinate
			xSpawn = rand() % 19;
			ySpawn = rand() % 9;
		}

        //Should generate a floor but for now creates a blank map of only floor tiles
        for(int x=0;x<floorWidth;x++){
            floorMap[x]=new int[floorHeight];
            for(int y=0;y<floorHeight;y++){

                //here, we are setting the actual tile values
                //0 is a floor tile
                //1 is a wall tile
                //2 is a stair tile
                /*if(x == xRand && y == yRand){
                    //generate a stair tile here
                    //floorMap[x][y] = STAIR_TILE;
                    //set the floors stair coordinates for later use
                    //stairX = x * 100;
                    //stairY = y * 100;
                }
				else if(x == xSpawn && y == ySpawn) {
					//generate a stair tile here
                    floorMap[x][y] = SPAWN_TILE;
					spawnX = x * 100;
                    spawnY = y * 100;
				}*/
                //if(!(x == xRand && y == yRand) && !(x == xSpawn && y == ySpawn)){
                    floorMap[x][y] = WALL_TILE;
                //}

            }
        }
    }

    Floor::Floor(int height,int width, unsigned int Seed){
        seedFlag=true;
        seed=Seed;
        srand(Seed);

        floorHeight=height;
        floorWidth=width;
        floorMap=new int*[floorWidth];

        int xRand = rand() % 19; //generate random x value for stairs
        int yRand = rand() % 9; //generates random y value for stairs

        //Should generate a floor but for now creates a blank map of only floor tiles
        for(int x=0;x<floorWidth;x++){
            floorMap[x]=new int[floorHeight];
            for(int y=0;y<floorHeight;y++){

                //here, we are setting the actual tile values
                //0 is a floor tile
                //1 is a wall tile
                //2 is a stair tile
                if(x == xRand && y == yRand){
                    //generate a stair tile here
                    //floorMap[x][y] = STAIR_TILE;
                    //set the floors stair coordinates for later use
                    //stairX = x * 100;
                    //stairY = y * 100;
                }
                else{
                    floorMap[x][y] = WALL_TILE;
                }

            }
        }
    }

    int Floor::getChestArraySize(){
        return chestArraySize;
    }

    Chest* Floor::getChestArray(){
        return chestArray;
    }

	std::vector<enemySpawn> Floor::getEnemySpawns(){
		return enemySpawns;
	}


    void Floor::generate(int rMinHeight, int rMinWidth, bool netFlag){
        std::vector<BSPLeaf*> leafs;    //List of all leafs created in generation

        BSPLeaf* root;
        if(seedFlag){
            root = new BSPLeaf(0,0,floorWidth - 1,floorHeight - 1,seed);   //Root leaf that the loop will start with.
        }
        else{
            root = new BSPLeaf(0,0,floorWidth - 1,floorHeight - 1);   //Root leaf that the loop will start with.
        }
        leafs.push_back(root);

        bool split=true;    //Flag to monitor if the loop is done or not.
        BSPLeaf* cur;
        while(split){
            split=false;
            //BSPLeaf* cur;
            //This will go over each leaf in Leafs

            //for(BSPLeaf* cur : leafs){
            int i;
            for(i = 0; i < leafs.size(); i++){

                cur = leafs.at(i);

                //std::cout << cur << "Splitting Cur\n";
                if(cur->leftChild == nullptr && cur->rightChild == nullptr){    //leaf is not split
                    //Will split if the current leaf is too big or a 75% chance occurs
                    if(cur->width > MAX_LEAF_SIZE || cur->height > MAX_LEAF_SIZE || rand()%100 < 75){
                        if(cur->split()){
                            //std::cout << cur->leftChild << "Left Address\n";
                            //std::cout << cur->rightChild << "Right Address\n";
                            leafs.push_back(cur->leftChild);
                            leafs.push_back(cur->rightChild);
                            //std::cout << leafs.at(1) << "Left Address\n";
                            //std::cout << leafs.at(2) << "Right Address\n";

                            split=true;
                        }
                    }
                }




            } //end of for
        }

        /*
        *   End of leaf generation segment.
        *   Start of room Generation
        */

        //Small bit of code to add padding to top and left of map.
        int** tempMap = new int*[floorWidth-1];
        for(int x=0;x<floorWidth-1;x++){
            tempMap[x]=new int[floorHeight-1];
            for(int y=0;y<floorHeight-1;y++){
                tempMap[x][y] = WALL_TILE;
            }
        }

        std::vector<struct Room*> rooms;
        root->createRooms(rMinHeight, rMinWidth, tempMap, rooms);
        for(int x=1;x<floorWidth;x++){
            for(int y=1;y<floorHeight;y++){
                floorMap[x][y] = tempMap[x-1][y-1];
            }
        }
        delete[] tempMap;

        roomArray=new struct Room*[rooms.size()];

        chestArraySize = 0;
        chestArray = new Chest[rooms.size()]; //Chests can never exceed number of rooms.
        //bool spawnPlaced = false;
        //bool stairsPlaced = false;
        int numRooms = rooms.size();

        if(numRooms==1){
            std::cout << "WARNING ONLY 1 ROOM\n";
        }

        int randomRoom = rand() % numRooms;
        int randomRoom2 = rand() % numRooms;

        while(randomRoom == randomRoom2){
            randomRoom2 = rand() % numRooms;
        }


        int i=0;
        for (std::vector<struct Room*>::iterator it = rooms.begin(); it != rooms.end(); ++it){
            roomArray[i]=*it;

            roomArray[i]->x++;
            roomArray[i]->y++;

            //generate the chest right here
            int randomChance = rand() % 100;
            if(randomChance < 100){
                //33 percent chance to spawn a chest in this current room

                //now, figure out the coordinates of the chest
                int chestX = 0;
                int chestY = 0;

                int roomX = roomArray[i]->x * 100;
                int roomY = roomArray[i]->y * 100;

                int roomWidth = roomArray[i]->width * 100;
                int roomHeight = roomArray[i]->height * 100;


                //
                int xBound = roomX + roomWidth - 64;
                int yBound = roomY + roomHeight - 64;
                //std::cout << roomArray[i]->x << "roomX\n";

                chestX = (rand() % (xBound - roomX)) + roomX;
                chestY = (rand() % (yBound - roomY)) + roomY;

                //std::cout << chestX << "ChestX\n";
                //std::cout << chestY << "ChestY\n";

                chestArray[i] = *(new Chest(chestX, chestY));
                chestArraySize++;
            }
			//generate enemy spawn locations within the room
			if (randomChance < 100) {
				int eSpawnX = 0;
				int eSpawnY = 0;

				int roomX = roomArray[i]->x * 100;
				int roomY = roomArray[i]->y * 100;

				int roomWidth = roomArray[i]->width * 100;
				int roomHeight = roomArray[i]->height * 100;

				int xBound = roomX + roomWidth - 64;
				int yBound = roomY + roomHeight - 64;

				eSpawnX = (rand() % (xBound - roomX)) + roomX;
				eSpawnY = (rand() % (yBound - roomY)) + roomY;
				enemySpawns.push_back({ eSpawnX, eSpawnY });
			}

            if(randomRoom == i){
                //generate the spawn and stairs here, in different places
                //so, pick a random tile within the room and generate stairs

                //maybe always make spawn and stairs block spawn in middle of room each time (this avoids hallway collisions)

                int roomX = roomArray[i]->x;
                int roomY = roomArray[i]->y;

               // std::cout << roomX << "RoomX\n";
               // std::cout << roomY << "RoomY\n";

                int roomWidth = roomArray[i]->width;
                int roomHeight = roomArray[i]->height;

                //std::cout << roomWidth << "RoomWidth\n";
                //std::cout << roomHeight << "RoomHeight\n";

                //stairX = (rand() % (roomWidth - 1)) + roomX;
               // stairY = (rand() % (roomHeight - 1)) + roomY;

                stairX = roomWidth/2 + roomX;
                stairY = roomHeight/2 + roomY;


                floorMap[stairX][stairY] = STAIR_TILE;
                stairX = stairX * 100;
                stairY = stairY * 100;


            }

            if(randomRoom2 == i){

                int roomX = roomArray[i]->x;
                int roomY = roomArray[i]->y;

                //std::cout << roomX << "RoomX\n";
                //std::cout << roomY << "RoomY\n";

                int roomWidth = roomArray[i]->width;
                int roomHeight = roomArray[i]->height;

                //std::cout << roomWidth << "RoomWidth\n";
                //std::cout << roomHeight << "RoomHeight\n";

                //create multiple spawn points if networking
                //srand(time(0)); STOP CALLING SRAND it only needs to be called once at the start, otherwise seeds are worthelss
                if (netFlag)
                {
                    for(int i=0; i<getPlayerNumber();i++)
                    {
                        spawnX = rand() % 19; //generate random x value for spawn
                        spawnY = rand() % 9; //generates random y value for spawn

                        //in case the spawn coordinate overlaps with the stair or wall coordinate
                        while((stairX == spawnX && stairY == spawnY) || floorMap[spawnX][spawnY] == WALL_TILE){ 
                            spawnX = rand() % 19;
                            spawnY = rand() % 9;
                        }
                        spawnX = spawnX * 100;
                        spawnY = spawnY * 100;
                        spawnXArr.push_back(spawnX);
                        spawnYArr.push_back(spawnY);

                    }
                }
                else
                {
                    spawnX = roomWidth/2 + roomX;
                    spawnY = roomHeight/2 + roomY;
                    floorMap[spawnX][spawnY] = SPAWN_TILE;
                    spawnX = spawnX * 100;
                    spawnY = spawnY * 100;
                }


            }

            i++;
        }


        /*
        int totalRooms=rooms.size();
        int r=rand() % totalRooms;
        int sx,sy;
        sx= rand() % roomArray[r]->width;
        sy= rand() % roomArray[r]->height;
        floorMap[sx][sy]=STAIR_TILE;

        r=rand() % totalRooms;
        sx= rand() % roomArray[r]->width;
        sy= rand() % roomArray[r]->height;
        while(floorMap[sx][sy]==STAIR_TILE){
            r=rand() % totalRooms;
            sx= rand() % roomArray[r]->width;
            sy= rand() % roomArray[r]->height;
            floorMap[sx][sy]=SPAWN_TILE;
        }
        */
    }

    void Floor::generateBossFloor(bool netFlag){
        //9x9 room with a border, players start in the middle and enemies spawn near walls
        for(int x = 0; x < floorWidth; x++){
            for(int y = 0; y < floorHeight; y++){
                if(x == 0 || y==0 || x==floorWidth-1 || y==floorHeight-1){
                    floorMap[x][y]=WALL_TILE;
                }
                else{
                    floorMap[x][y]=FLOOR_TILE;
                }
            }
        }
        floorMap[5][5]=SPAWN_TILE;
        
        chestArray =  new Chest[1];
        chestArraySize = 0; //There are no chests in the boss room

		enemySpawns.push_back({ 100, 100 });
		enemySpawns.push_back({ 100, 500 });
		enemySpawns.push_back({ 100, 900 });
		enemySpawns.push_back({ 500, 100 });
		enemySpawns.push_back({ 500, 900 });
		enemySpawns.push_back({ 900, 100 });
		enemySpawns.push_back({ 900, 500 });
		enemySpawns.push_back({ 900, 900 });

        if(netFlag){
            spawnXArr.push_back(500);
            spawnYArr.push_back(400);
            for(int i = 0; i < getPlayerNumber();i++){
                spawnXArr.push_back(400 + (i * 100));
                spawnYArr.push_back(500);
            }
        }
        else{
            spawnX = 500;
            spawnY = 500;
        }

        //The map is 1100x1100 its impossible to leave this level unless game over is triggered.
        stairX = 1200;
        stairY = 1200;
    }

    void Floor::generateLootFloor(bool netFlag){
        //9x9 room with a border, players start in the middle and enemies spawn near walls
        for(int x = 0; x < floorWidth; x++){
            for(int y = 0; y < floorHeight; y++){
                if(x == 0 || y==0 || x==floorWidth-1 || y==floorHeight-1){
                    floorMap[x][y]=WALL_TILE;
                }
                else{
                    floorMap[x][y]=FLOOR_TILE;
                }
            }
        }
        floorMap[5][5]=SPAWN_TILE;
        floorMap[5][1]=STAIR_TILE;

        chestArray =  new Chest[1];
        int chestX = (rand() % (836)) + 100;
        int chestY = (rand() % (836)) + 100;
        chestArray[0] = *(new Chest(chestX, chestY));
        chestArraySize = 1; //There is 1 chest in this room

        if(netFlag){
            spawnXArr.push_back(500);
            spawnYArr.push_back(400);
            for(int i = 0; i < getPlayerNumber();i++){
                spawnXArr.push_back(400 + (i * 100));
                spawnYArr.push_back(500);
            }
        }
        else{
            spawnX = 500;
            spawnY = 500;
        }

        //The map is 1100x1100 its impossible to leave this level unless game over is triggered.
        stairX = 100;
        stairY = 500;
    }

    int** Floor::getFloorMap(){
        /*int** ret;
        ret=new int*[floorWidth];
        for(int x=0;x<floorWidth;x++){
            ret[x]=new int[floorHeight];
            for(int y=0;y<floorHeight;y++){
                ret[x][y]=floorMap[x][y];
            }
        }

        return ret;
        */

        return floorMap;
    }

    //getter for x coord of the stairs
    int Floor::getStairX(){
        return stairX;
    }

    //getter for the y coord of the stairs
    int Floor::getStairY(){
        return stairY;
    }

	    //getter for x coord of the spawn
    int Floor::getSpawnX(){
        return spawnX;
    }

    //getter for the y coord of the spawn
    int Floor::getSpawnY(){
        return spawnY;
    }
    std::vector<int> Floor::getMultiSpawnX(){
        return spawnXArr;
    }

    std::vector<int> Floor::getMultiSpawnY(){
        return spawnYArr;
    }

    Floor::~Floor(){
        delete floorMap;
        spawnXArr.clear();
        spawnYArr.clear();
    }
