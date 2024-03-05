#include <SDL.h>
#include "Object.h"
#include <cmath>

Object::Object(SDL_Texture* Texture, double X, double Y, int w, int h) {
	texture = Texture;
	rect.x = (int)X;
	rect.y = (int)Y;
	rect.w = w;
	rect.h = h;
	x = X;
	y = Y;
}

Object::Object() {
	rect.x = 0;
	rect.y = 0;
	rect.w = 0;
	rect.h = 0;
	texture = NULL;
	x = 0; y = 0;
}

Object::Object(SDL_Texture* Texture, double X, double Y, int w, int h, double ProjectileAngle) {
	texture = Texture;
	rect.x = (int)X;
	rect.y = (int)Y;
	rect.w = w;
	rect.h = h;
	x = X;
	y = Y;
	projectileAngle = ProjectileAngle;
}

void Object::IncrementX(double value) {
	x += value;
	rect.x = round(x);
}

void Object::IncrementY(double value) {
	y += value;
	rect.y = round(y);
}
