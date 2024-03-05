#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Object.h"

class Maze {
public:
	int** grid = nullptr;

	int sizeX;
	int sizeY;
	int placeHolderX;
	int placeHolderY;
	int blockSize;
	int** collisions = nullptr;
	int iCol = 0;

	Maze(int x, int y);
	void RenderMaze(SDL_Renderer* renderer, Object* wall, Object* tile, SDL_Point* point);
	int LargestArea(int(&output)[2][2]);

	void AddToColList(int x, int y, int w, int h, int angle);
};
