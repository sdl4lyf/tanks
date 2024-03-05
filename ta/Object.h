#include <SDL.h>
#pragma once
class Object {
public:
	double rotation = 90;
	double x;
	double y;
	SDL_Texture* texture;
	SDL_Rect rect = {0,0,0,0};

	Object(SDL_Texture* Texture, double X, double Y, int w, int h);
	Object();
	void IncrementX(double value);
	void IncrementY(double value);

};
