#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "BSPLeaf.h"
#include "TileList.h"
#include <iostream>

    const int MIN_LEAF_SIZE=5;  //Min leaf size is defined here but max is defined in the floor class.
    int x, y, width, height;    //Position and size of leaf
    unsigned int seedBSP;
    bool seedFlagBSP;

    BSPLeaf* leftChild;
    BSPLeaf* rightChild;
    struct Room* room;   //Room the leaf contains.
    //std::vector<struct Hall*> hallways;
    //TODO Some vector to handle hallways


    BSPLeaf::BSPLeaf(int X,int Y, int w, int h){
        leftChild=nullptr;
        rightChild=nullptr;
        room=NULL;
        x=X;
        y=Y;
        width=w;
        height=h;
        srand(time(NULL));
        seedBSP=0;
        seedFlagBSP=false;
    }
    //A clone of the normal construtor but with a seed for the randomization.
    BSPLeaf::BSPLeaf(int X,int Y, int w, int h, unsigned int Seed){
        leftChild=nullptr;
        rightChild=nullptr;
        room=NULL;
        x=X;
        y=Y;
        width=w;
        height=h;
        srand(Seed);
        seedBSP=Seed;
        seedFlagBSP=true;
    }

    BSPLeaf::BSPLeaf(){

    }

    bool BSPLeaf::split(){
        if(leftChild!=NULL||rightChild!=NULL){
            return false;   //Already split
        }
        bool splitH = (rand() % 2) == 0;   //50/50 on whether we split vertically or horizontally
        if(width > height && (float)width / (float)height >= 1.25){
            splitH=false;   //Width is 25% larger than height so split horizontally
        }
        else if(height > width && (float)height / (float)width >= 1.25){
            splitH=true;   //Height is 25% larger than width so split vertically
        }
        //If neither of these are true it'll be the result of the random earlier.

        int max;
        if(splitH){
            max=height-MIN_LEAF_SIZE;   //Max size of a leaf
        }
        else{
            max=width-MIN_LEAF_SIZE;
        }

        if(max<=MIN_LEAF_SIZE){
            return false;   //Leaf is too small to split further
        }

        int split = (rand() % (max-MIN_LEAF_SIZE)) + MIN_LEAF_SIZE;

        //BSPLeaf leftTemp;
        //BSPLeaf rightTemp;

        if(splitH){ //Spliting along height
            //Checks if seed was used in generation.
            if(seedFlagBSP){
                leftChild = new BSPLeaf(x,y,width,split,seedBSP);
                rightChild = new BSPLeaf(x,y+split,width,height-split,seedBSP);
            }
            else{
                leftChild = new BSPLeaf(x,y,width,split);
                rightChild = new BSPLeaf(x,y+split,width,height-split);
            }
        }
        else{   //Spliting along the width
            if(seedFlagBSP){
                leftChild = new BSPLeaf(x,y,split,height,seedBSP);
                rightChild = new BSPLeaf(x+split,y,width-split,height,seedBSP);
            }
            else{
                leftChild = new BSPLeaf(x,y,split,height);
                rightChild = new BSPLeaf(x+split,y,width-split,height);
            }
        }

        //leftChild = &leftTemp;
        //rightChild = &rightTemp;

        return true;
    }

    void BSPLeaf::createRooms(int rMinHeight, int rMinWidth, int **FloorMap, std::vector<struct Room*> &Rooms){    //Long function but stuff important to make adding rooms to the map easier.
        if(leftChild!=NULL||rightChild!=NULL){
            //Go down until it has reached the leaves
            if(leftChild!=NULL){
                leftChild->createRooms(rMinHeight, rMinWidth, FloorMap, Rooms);
            }
            if(rightChild!=NULL){
                rightChild->createRooms(rMinHeight, rMinWidth, FloorMap, Rooms);
            }
            //if both children are not null then they are both leaves, create a hall between them
            if (leftChild != NULL && rightChild != NULL) {
                createHall(leftChild->getRoom(), rightChild->getRoom(), FloorMap);
            }
        }
        else{   //This leaf is a... leaf, the binary tree kind
            //Sets rooms pos but not on the edge of the leaf to avoid merging rooms
            room = new(Room);

            room->width=(rand() % (width - rMinWidth)) + rMinWidth;

            //if(room->width < 3){
               // room->width = 3;
           // }

            room->height=(rand() % (height - rMinHeight)) + rMinHeight;

            //if(room->height < 3){
               // room->height = 3;
            //}

            room->x=(rand() % (width - room->width)) + x;
            room->y=(rand() % (height - room->height)) + y;

            Rooms.push_back(room);  //Save room's data for special tile spawning.

            //Map Room to floor map.
            int i;
            int j;
            for(i=room->x; i < (room->x + room->width); i++){
                for(j=room->y; j < (room->y + room->height); j++){
                    //std::cout << i << "I\n";
                    //std::cout << j << "I\n";
                    //std::cout << FloorMap[i][j] << "Floor Tile\n";
                    FloorMap[i][j]=FLOOR_TILE;

                }
            }
        }
    }

    struct Room* BSPLeaf::getRoom(){
        if(room!=NULL){
            return room;
        }
        else{
            struct Room* left = nullptr;
            struct Room* right = nullptr;
            if(leftChild!=NULL){
                left=leftChild->getRoom();
            }
            if(rightChild!=NULL){
                right=rightChild->getRoom();
            }
            if(left==NULL && right==NULL){
                return NULL;    //This should never happen seriously for the love of god hope it never hits this point
            }
            else if(right==NULL){
                return left;
            }
            else if(left==NULL){
                return right;
            }
            else if(rand() % 2 == 0){
                return left;
            }
            else{
                return right;
            }
        }
    }

    void BSPLeaf::createHall(struct Room* left, struct Room* right, int** FloorMap) {
        if (left == NULL || right == NULL) { return; }
        int leftX, leftY, rightX, rightY;
        struct Hall* hall1 = new(Hall);
        struct Hall* hall2 = new(Hall);
        //choose a random point within each room
        leftX = (rand() % left->width) + left->x;
        leftY = (rand() % left->height) + left->y;
        rightX = (rand() % right->width) + right->x;
        rightY = (rand() % right->height) + right->y;
        //calculate vertical and horizontal distance between the points
        int w = rightX - leftX;
        int h = rightY - leftY;
        //create any vertical and/or horizontal hallways needed to connect the points
        if (w == 0 || h == 0) {
            //the points are on the same line and only need a single hallway between them
            hall1->x1 = leftX;
            hall1->y1 = leftY;
            hall1->x2 = rightX;
            hall1->y2 = rightY;
            mapHall(hall1, FloorMap);

        }
        else {
            //create 2 hallways connecting at a right angle
            if (rand() % 2 == 1) {
                hall1->x1 = leftX;
                hall1->y1 = leftY;
                hall1->x2 = leftX;
                hall1->y2 = rightY;
                mapHall(hall1, FloorMap);
                hall2->x1 = rightX;
                hall2->y1 = rightY;
                hall2->x2 = leftX;
                hall2->y2 = rightY;
                mapHall(hall2, FloorMap);
            }
            else {
                hall1->x1 = leftX;
                hall1->y1 = leftY;
                hall1->x2 = rightX;
                hall1->y2 = leftY;
                mapHall(hall1, FloorMap);
                hall2->x1 = rightX;
                hall2->y1 = rightY;
                hall2->x2 = rightX;
                hall2->y2 = leftY;
                mapHall(hall2, FloorMap);
            }
        }
    }

    void BSPLeaf::mapHall(struct Hall* hall, int **FloorMap){
        //Vertical hall
        if(hall->x1==hall->x2){
            if(hall->y1 < hall->y2){
                for(int i=hall->y1;i<=hall->y2;i++){
                    FloorMap[hall->x1][i]=FLOOR_TILE;
                }
            }
            else{
                for(int i=hall->y2;i<=hall->y1;i++){
                    FloorMap[hall->x1][i]=FLOOR_TILE;
                }
            }
        }
        //Horizontal hall
        else{
            if(hall->x1 < hall->x2){
                for(int i=hall->x1;i<=hall->x2;i++){
                    FloorMap[i][hall->y1]=FLOOR_TILE;
                }
            }
            else{
                for(int i=hall->x2;i<=hall->x1;i++){
                    FloorMap[i][hall->y1]=FLOOR_TILE;
                }
            }
        }
    }

