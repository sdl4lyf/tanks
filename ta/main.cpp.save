#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <Windows.h>
#include <cmath>
#include <chrono>
#include <string>
#include "Object.h"
#include "Maze.h"

using namespace std;

#define D2R 3.14159265359/180

const int SCREEN_WIDTH = 618;
const int SCREEN_HEIGHT = 625;
const char* RED_PATH = "./resources/redtank.png";
const char* GREEN_PATH = "./resources/greentank.png";
const char* WALL_PATH = "./resources/wall.png";
const char* BULLET_PATH = "./resources/bullet.png";

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Point point = { 14,23 };
SDL_Point wallPoint = { 0,0 };
Object redTank;
Object wall;
Object bullet;
Maze* maze = NULL;

const double ROTATION_SPEED = 240.0; // deg/s
const double SPEED = 150.0; // p/s
const double ROTATION_CLAMP = 5.0;
const Uint8* states = 0;

bool quit = false;
SDL_Event e;

SDL_Texture* LoadTexture(const char* path);
void Init();
void Loop();
void Close();
void Render(Object object);
void CheckForQuit();
void RedMovement();
void FixRotations();
bool CollisionCheck(Object* object, int** arr);
double ClampRotation(double angle, double value);
bool PointSquareCheck(int x1,int y1,int x2,int y2,int px,int py);
void Line(int x1, int y1, int x2, int y2);



long long timeStart = chrono::time_point_cast<chrono::microseconds>(chrono::high_resolution_clock::now()).time_since_epoch().count();
long long deltaTime = chrono::time_point_cast<chrono::microseconds>(chrono::high_resolution_clock::now()).time_since_epoch().count();

void Init() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Projektas", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	IMG_Init(IMG_INIT_PNG);

	redTank = Object(LoadTexture(RED_PATH), (double)(SCREEN_WIDTH / 2 - 35), (double)(SCREEN_HEIGHT / 2 - 53), (int)(71 * 0.4), (int)(106 * 0.4));
	redTank.rotation = 0;

	wall = Object(LoadTexture(WALL_PATH), 0, 0, 0, 0);

	bullet = Object(LoadTexture(BULLET_PATH), 100.0, 100.0,8, 8);

	maze = new Maze(8, 8);
}

SDL_Texture* LoadTexture(const char* path) {
	SDL_Texture* tempTexture = NULL;
	SDL_Surface* tempSurface = IMG_Load(path);
	tempTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	SDL_FreeSurface(tempSurface);
	return tempTexture;
}

void CheckForQuit() {
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			quit = true;
		}
		if (e.type == SDL_KEYUP) {
			if (e.key.keysym.sym == SDLK_r) {
				delete maze;
				maze = new Maze(8, 8);
			}
			if (e.key.keysym.sym == SDLK_j) {
				for (int i = 0; i < 87; i++) {
					for (int k = 0; k < 4; k++) {
						cout << maze->collisions[i][k] << ' ';
					}
					cout << '\n';
				}
			}
		}
	}
}

void RedMovement() {
	if (states[SDL_SCANCODE_A]) {
		redTank.rotation += deltaTime / 1000000.0 * ROTATION_SPEED;
		if (!CollisionCheck(&redTank, maze->collisions)) {
			redTank.rotation -= deltaTime / 1000000.0 * ROTATION_SPEED;
			
		}
	}
	if (states[SDL_SCANCODE_D]) {
		redTank.rotation -= deltaTime / 1000000.0 * ROTATION_SPEED;
		if (!CollisionCheck(&redTank, maze->collisions)) {
			redTank.rotation += deltaTime / 1000000.0 * ROTATION_SPEED;
		}
	}
	FixRotations();
	if (states[SDL_SCANCODE_W]) {
		redTank.IncrementY(sin(3.14159265359 / 180.0 * ((ClampRotation(redTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED);
		redTank.IncrementX(-cos(3.14159265359 / 180.0 * ((ClampRotation(redTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED);
		if (!CollisionCheck(&redTank, maze->collisions)) {
			redTank.IncrementY(sin(3.14159265359 / 180.0 * ((ClampRotation(redTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * -SPEED);
			redTank.IncrementX(-cos(3.14159265359 / 180.0 * ((ClampRotation(redTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * -SPEED);
		}
	}
	if (states[SDL_SCANCODE_S]) {
		redTank.IncrementY(-(sin(3.14159265359 / 180.0 * ((ClampRotation(redTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED));
		redTank.IncrementX(-(-cos(3.14159265359 / 180.0 * ((ClampRotation(redTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED));
		if (!CollisionCheck(&redTank, maze->collisions)) {
			redTank.IncrementY(sin(3.14159265359 / 180.0 * ((ClampRotation(redTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED);
			redTank.IncrementX(-cos(3.14159265359 / 180.0 * ((ClampRotation(redTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED);
		}
	}
}

void FixRotations() {
	if (redTank.rotation < -180.0)redTank.rotation = 180 - (abs(redTank.rotation) - 180);
	if (redTank.rotation > 180.0) redTank.rotation = -180 + (redTank.rotation - 180);
}

void Line(int x1, int y1,int x2,int y2) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

double ClampRotation(double angle, double value) {
	if (angle > -value && angle < value) {
		return 0.0;
	}
	if (angle < -90.0 + value && angle > -90.0 - value) {
		return -90.0;
	}
	if (angle > 180.0 - value && angle < 180.0 || angle < -180.0 + value && angle > -180.0) {
		return 180;
	}
	if (angle > 90.0 - value && angle < 90.0 + value) {
		return 90.0;
	}
	return angle;
}

bool PointSquareCheck(int x1, int y1, int x2, int y2, int px, int py) {
	if (px >= x1 && px <= x2 && py >= y1 && py <= y2) {
		return true;
	}
	else {
		return false;
	}
}

bool CollisionCheck(Object* object, int** arr) {
	int w = object->rect.w, h = object->rect.h;
	double a = -D2R * (object->rotation);
	int cx = object->x+14, cy = object->y + 23;
	int x1, x2, x3, x4, y1, y2, y3, y4;
	x1 = cx + ((-10) * cos(a) - (-12) * sin(a));
	y1 = cy + ((-10) * sin(a) + (-12) * cos(a));
	x2 = cx + ((10) * cos(a) - (-12) * sin(a));
	y2 = cy + ((10) * sin(a) + (-12) * cos(a));
	x3 = cx + ((-10) * cos(a) - (17) * sin(a));
	y3 = cy + ((-10) * sin(a) + (17) * cos(a));
	x4 = cx + ((10) * cos(a) - (17) * sin(a));
	y4 = cy + ((-10) * sin(a) + (17) * cos(a));

	for (int i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			for (int k = 0; k < 87; k++) {
				if (PointSquareCheck(arr[k][0], arr[k][1], arr[k][2], arr[k][3], x1, y1)) {
					return false;
				}
			}
		case 1:
			for (int k = 0; k < 87; k++) {
				if (PointSquareCheck(arr[k][0], arr[k][1], arr[k][2], arr[k][3], x2, y2)) {
					return false;
				}
			}
		case 2:
			for (int k = 0; k < 87; k++) {
				if (PointSquareCheck(arr[k][0], arr[k][1], arr[k][2], arr[k][3], x3, y3)) {
					return false;
				}
			}
		case 4:
			for (int k = 0; k < 87; k++) {
				if (PointSquareCheck(arr[k][0], arr[k][1], arr[k][2], arr[k][3], x4, y4)) {
					return false;
				}
			}
		}
	}
	return true;
}
	


void Loop() {
	CheckForQuit();

	states = SDL_GetKeyboardState(nullptr);

	maze->RenderMaze(renderer, &wall, &wall, &wallPoint);
	RedMovement();

	Render(redTank);
	Render(bullet);
}

void Render(Object object) {
	SDL_RenderCopyEx(renderer, object.texture, NULL, &object.rect, (ClampRotation(-object.rotation, ROTATION_CLAMP)), &point, SDL_FLIP_NONE);
}

void Close() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	window = NULL;
	renderer = NULL;
}

int main(int argc, char* args[])
{
	Init();

	while (!quit) {
		timeStart = chrono::time_point_cast<chrono::microseconds>(chrono::high_resolution_clock::now()).time_since_epoch().count();
		SDL_RenderClear(renderer);
		Loop();
		SDL_RenderPresent(renderer);
		deltaTime = chrono::time_point_cast<chrono::microseconds>(chrono::high_resolution_clock::now()).time_since_epoch().count() - timeStart;
	}

	Close();

	return 0;
}



