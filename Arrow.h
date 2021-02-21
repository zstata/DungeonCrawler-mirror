#include "Projectile.h"
#include <vector>

#ifndef ARROW_H
#define ARROW_H

class Arrow:public Projectile{
    public:
        using Projectile::Projectile;
        const double SPEED = 350;
        double DAMAGE = 20;
        static std::vector<SDL_Texture*> sprites;
        int x_size=25;
        int y_size=25;
        SDL_Rect hitBox;

        void setHitbox();
        double getSpeed() {return SPEED;}
        double getDamage(){return DAMAGE;}
        void setDamage(double val){DAMAGE=val;}
        int getXSize(){return x_size;}
        int getYSize(){return y_size;}
        SDL_Rect* getHitBox(){return &hitBox;}
        SDL_Texture* getSprite(){return Arrow::sprites[spriteIndex];}
        void setMovebox(){dstrect={static_cast<int>(x_pos), static_cast<int>(y_pos), getXSize(), getYSize()};}
};
#endif
