#include "Arrow.h"
#include <iostream>

std::vector<SDL_Texture*> Arrow::sprites;
void Arrow::setHitbox(){
    if(spriteIndex==2) hitBox={static_cast<int>(x_pos+2), static_cast<int>(y_pos+12), 21, 5};
    else hitBox={static_cast<int>(x_pos+12), static_cast<int>(y_pos+2), 5, 21};
}