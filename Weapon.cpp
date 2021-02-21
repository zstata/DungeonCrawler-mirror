#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Weapon.h"
#include <vector>

Weapon::Weapon(){

}

Weapon::Weapon(Weapon* copyWeapon){
	x_pos = copyWeapon->x_pos;
	y_pos = copyWeapon->y_pos;
	x_size = copyWeapon->x_size;
	y_size = copyWeapon->y_size;
	weapon_type = copyWeapon->weapon_type;
	picked_up = copyWeapon->picked_up;
	damage = copyWeapon->damage;
	dstrect.x = copyWeapon->dstrect.x;
	dstrect.y = copyWeapon->dstrect.y;
	dstrect.w = copyWeapon->dstrect.w;
	dstrect.h = copyWeapon->dstrect.h;
	sprites.push_back(copyWeapon->sprites.at(0));
	rarity = copyWeapon->rarity;
}

Weapon::Weapon(double xcoord, double ycoord, int width, int height, int type) :x_pos{xcoord}, y_pos{ycoord}, x_size{width}, y_size{height}, weapon_type{type} {
  dstrect = {static_cast<int>(x_pos), static_cast<int>(y_pos), x_size, y_size};
  picked_up = false;
}
