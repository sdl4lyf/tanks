#include "Maze.h"
#include <math.h>
#include <stdlib.h>
#include <SDL.h>
#include <chrono>
#include <iostream>

#define T 1
#define R 2
#define B 3
#define L 4

Maze::Maze(const int x, const int y) {
	grid = new int* [x * y]{ 0 };
	for (int i = 0; i < x; i++) {
		grid[i] = new int[y] {0};
	}

	collisions = new int* [150 * 4]{ 0 };
	for (int i = 0; i < 150; i++) {
		collisions[i] = new int[4]{ 0 };
	}
	collisions[0][0] = 0;
	collisions[0][1] = 0;
	collisions[0][2] = 618;
	collisions[0][3] = 8;

	collisions[1][0] = 0;
	collisions[1][1] = 0;
	collisions[1][2] = 8;
	collisions[1][3] = 652;

	collisions[2][0] = 0;
	collisions[2][1] = 618;
	collisions[2][2] = 618;
	collisions[2][3] = 625;

	collisions[3][0] = 611;
	collisions[3][1] = 0;
	collisions[3][2] = 618;
	collisions[3][3] = 625;

	iCol = 4;

	for (int i = 0; i < x; i++) {
		for (int k = 0; k < y; k++) {
			SDL_Delay(1);
			srand(std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count());
			grid[i][k] = rand() % 4 + 1;
		}
	}
	sizeX = x;
	sizeY = y;
	blockSize = 70;
	placeHolderX = 1;
	placeHolderY = 1;
}

void Maze::AddToColList(int x, int y, int w, int h, int angle) { // x1 y1 x2 y2
	if (iCol == 150)return;
	int x1, y1, x2, y2;
	if (angle == 0 && h == abs(h)) {
		x1 = x;
		y1 = y;
		x2 = x1 + w;
		y2 = y + h;
		collisions[iCol][0] = x1;
		collisions[iCol][1] = y1;
		collisions[iCol][2] = x2;
		collisions[iCol++][3] = y2;
		return;
	}

	if (angle == 90 && h == abs(h)) {
		x1 = x - h;
		y1 = y;
		x2 = x;
		y2 = y + w;
		collisions[iCol][0] = x1;
		collisions[iCol][1] = y1;
		collisions[iCol][2] = x2;
		collisions[iCol++][3] = y2;
		return;
	}
	if (angle == 90 && h != abs(h)) {
		x1 = x;
		y1 = y;
		x2 = x + abs(h);
		y2 = y + w;
		collisions[iCol][0] = x1;
		collisions[iCol][1] = y1;
		collisions[iCol][2] = x2;
		collisions[iCol++][3] = y2;
		return;
	}
	if (angle == 0 && h != abs(h)) {
		x1 = x;
		y1 = y - h;
		x2 = x + w;
		y2 = y;
		collisions[iCol][0] = x1;
		collisions[iCol][1] = y1;
		collisions[iCol][2] = x2;
		collisions[iCol++][3] = y2;
		return;
	}
	/*if (angle == 180 && h != abs(h)) {
		x1 = x - w;
		y1 = y;
		x2 = x;
		y2 = y+h;
		collisions[iCol][0] = x1;
		collisions[iCol][1] = y1;
		collisions[iCol][2] = x2;
		collisions[iCol++][3] = y2;
		return;
	}*/
	if (angle == -90 && h == abs(h)) {
		x1 = x;
		y1 = y - w;
		x2 = x + h;
		y2 = y;
		collisions[iCol][0] = x1;
		collisions[iCol][1] = y1;
		collisions[iCol][2] = x2;
		collisions[iCol++][3] = y2;
		return;
	}
}

void Maze::RenderMaze(SDL_Renderer* renderer, Object* wall, Object* tile, SDL_Point* point) {
	wall->rect.w = (int)(1.1 * blockSize);
	wall->rect.h = (int)(0.1 * blockSize);
	for (int i = 0; i < sizeX; i++) {
		wall->rect.y = placeHolderY;
		wall->rect.x = placeHolderX + i * wall->rect.w;
		//AddToColList(wall->rect.x, wall->rect.y, wall->rect.w, wall->rect.h,0);
		SDL_RenderCopyEx(renderer, wall->texture, NULL, &wall->rect, 0, point, SDL_FLIP_NONE);
	}
	for (int i = 0; i < sizeX; i++) {
		wall->rect.y = placeHolderY + sizeY * blockSize + (sizeY)*blockSize * 0.1;
		wall->rect.x = placeHolderX + i * wall->rect.w;
		//AddToColList(wall->rect.x, wall->rect.y, wall->rect.w, wall->rect.h, 0);
		SDL_RenderCopyEx(renderer, wall->texture, NULL, &wall->rect, 0, point, SDL_FLIP_NONE);
	}
	for (int i = 0; i < sizeY; i++) {
		wall->rect.y = placeHolderY + wall->rect.w * i;
		wall->rect.x = placeHolderX;
		wall->rect.h = abs(wall->rect.h) * -1;
		//AddToColList(wall->rect.x, wall->rect.y, wall->rect.w, wall->rect.h, 90);
		SDL_RenderCopyEx(renderer, wall->texture, NULL, &wall->rect, 90, point, SDL_FLIP_VERTICAL);
	}
	wall->rect.h = abs(wall->rect.h);
	for (int i = 0; i < sizeY; i++) {
		wall->rect.y = placeHolderY + wall->rect.h + wall->rect.w * i;
		wall->rect.x = placeHolderX + sizeX * blockSize + (sizeX)*blockSize * 0.1;
		//AddToColList(wall->rect.x, wall->rect.y, wall->rect.w, wall->rect.h, 90);
		SDL_RenderCopyEx(renderer, wall->texture, NULL, &wall->rect, 90, point, SDL_FLIP_VERTICAL);
	}

	for (int i = 0; i < sizeX - 1; i++) {
		for (int k = 0; k < sizeY; k++) {
			switch (grid[i][k]) {
			case T:
				wall->rect.x = placeHolderX + i * (wall->rect.h + blockSize);
				wall->rect.y = placeHolderY + k * (wall->rect.h + blockSize);
				wall->rect.w += wall->rect.h;
				AddToColList(wall->rect.x, wall->rect.y, wall->rect.w, wall->rect.h, 0);
				SDL_RenderCopyEx(renderer, wall->texture, NULL, &wall->rect, 0, point, SDL_FLIP_NONE);
				wall->rect.w -= wall->rect.h;
				break;
			case L:
				wall->rect.x = placeHolderX + i * (wall->rect.h + blockSize);
				wall->rect.y = placeHolderY + k * (wall->rect.h + blockSize);
				wall->rect.h = -wall->rect.h;
				wall->rect.w += abs(wall->rect.h);
				AddToColList(wall->rect.x, wall->rect.y, wall->rect.w, wall->rect.h, 90);
				SDL_RenderCopyEx(renderer, wall->texture, NULL, &wall->rect, 90, point, SDL_FLIP_NONE);
				wall->rect.h = abs(wall->rect.h);
				wall->rect.w -= wall->rect.h;
				break;
			case B:
				wall->rect.x = placeHolderX + (i * (blockSize + wall->rect.h));
				wall->rect.y = placeHolderY + wall->rect.h + blockSize + (k * (blockSize + wall->rect.h));
				wall->rect.w += abs(wall->rect.h);
				AddToColList(wall->rect.x, wall->rect.y, wall->rect.w, wall->rect.h, 0);
				SDL_RenderCopyEx(renderer, wall->texture, NULL, &wall->rect, 0, point, SDL_FLIP_NONE);
				wall->rect.w -= abs(wall->rect.h);
				break;
			case R:
				wall->rect.x = placeHolderX + blockSize + wall->rect.h + (i * (blockSize + wall->rect.h));
				wall->rect.y = placeHolderY + wall->rect.h + blockSize + (k * (blockSize + wall->rect.h));
				AddToColList(wall->rect.x, wall->rect.y, wall->rect.w, wall->rect.h, -90);
				SDL_RenderCopyEx(renderer, wall->texture, NULL, &wall->rect, -90, point, SDL_FLIP_NONE);
				break;
			}
		}
	}
}

#undef T
#undef R
#undef B
#undef L
