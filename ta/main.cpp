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

#define D2R 3.14159265359/180.0

const int SCREEN_WIDTH = 618;
const int SCREEN_HEIGHT = 625;
const char* RED_PATH = "./resources/redtank.png";
const char* GREEN_PATH = "./resources/greentank.png";
const char* WALL_PATH = "./resources/wall.png";
const char* BULLET_PATH = "./resources/bullet.png";

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Point point = { 14,27 };
SDL_Point wallPoint = { 0,0 };
Object redTank;
Object greenTank;
Object wall;
Maze* maze = NULL;

bool gAlive = true;
bool rAlive = true;

const double ROTATION_SPEED = 240.0; // deg/s
const double SPEED = 150.0; // p/s
const double ROTATION_CLAMP = 5.0;
const Uint8* states = 0;
Object* bullets[200];
int ind = 0;

double xa1, ya1;

bool quit = false;
SDL_Event e;

SDL_Texture* LoadTexture(const char* path);
void Init();
void Loop();
void Close();
void Render(Object object);
void Keybinds();
void RedMovement();
void FixRotations();
bool CollisionCheck(Object* object, int** arr);
double ClampRotation(double angle, double value);
bool PointSquareCheck(int x1, int y1, int x2, int y2, int px, int py);
void Line(int x1, int y1, int x2, int y2);
void MoveBullet();
void EraseBullets();
int* BulletOrigin();
void CheckForDeath();


long long timeStart = chrono::time_point_cast<chrono::microseconds>(chrono::high_resolution_clock::now()).time_since_epoch().count();
long long deltaTime = chrono::time_point_cast<chrono::microseconds>(chrono::high_resolution_clock::now()).time_since_epoch().count();

void Init() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Projektas", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	IMG_Init(IMG_INIT_PNG);

	redTank = Object(LoadTexture(RED_PATH), (double)(200), (double)(160), (int)(71 * 0.4), (int)(106 * 0.4));
	redTank.rotation = 0;

	greenTank = Object(LoadTexture(GREEN_PATH), (double)(500), (double)(400), (int)(71 * 0.4), (int)(106 * 0.4));
	greenTank.rotation = 0;

	wall = Object(LoadTexture(WALL_PATH), 0, 0, 0, 0);

	maze = new Maze(8, 8);
}

SDL_Texture* LoadTexture(const char* path) {
	SDL_Texture* tempTexture = NULL;
	SDL_Surface* tempSurface = IMG_Load(path);
	tempTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	SDL_FreeSurface(tempSurface);
	return tempTexture;
}

void Keybinds() {
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
				EraseBullets();
				redTank = Object(LoadTexture(RED_PATH), (double)(200), (double)(160), (int)(71 * 0.4), (int)(106 * 0.4));
				redTank.rotation = 0;
				greenTank = Object(LoadTexture(GREEN_PATH), (double)(500), (double)(400), (int)(71 * 0.4), (int)(106 * 0.4));
				greenTank.rotation = 0;
				gAlive = true;
				rAlive = true;

			}
			if (e.key.keysym.sym == SDLK_e) {
				bullets[ind++] = new Object(LoadTexture(BULLET_PATH), redTank.rect.x +24 - cos(-D2R * (redTank.rotation)), redTank.rect.y + sin(-D2R * (redTank.rotation)), 8, 8, ClampRotation(-redTank.rotation, ROTATION_CLAMP));
			}
			if (e.key.keysym.sym == SDLK_u) {
				bullets[ind++] = new Object(LoadTexture(BULLET_PATH), greenTank.rect.x + 24 - cos(-D2R * (greenTank.rotation)), greenTank.rect.y + sin(-D2R * (greenTank.rotation)), 8, 7, ClampRotation(-greenTank.rotation, ROTATION_CLAMP));
			}
		}
	}
}

int* BulletOrigin(int x,int y,double angle) {
	int x1, y1;
	return NULL;
}

void EraseBullets() {
	for (int i = 0; i < ind; i++) {
		delete bullets[i];
		bullets[i] = NULL;
	}
}

void MoveBullet() {
	if (ind == 0)return;
	for (int i = 0; i < ind; i++) {
		if (bullets[i] == NULL)continue;

		bullets[i]->IncrementY(sin(3.14159265359 / 180.0 * ((bullets[i]->projectileAngle - 90.0))) * deltaTime / 1000000.0 * SPEED * 2);
		bullets[i]->IncrementX(cos(3.14159265359 / 180.0 * ((bullets[i]->projectileAngle - 90.0))) * deltaTime / 1000000.0 * SPEED * 2);
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

void Line(int x1, int y1, int x2, int y2) {
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
	int cx = object->rect.x + w/2, cy = object->rect.y + h/2;
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

void CheckForDeath() {
	int xb, yb;
	int x1g, y1g, x2g, y2g;
	int x1r, y1r, x2r, y2r;

	x1g = greenTank.x;
	x2g = greenTank.x + 45;
	y1g = greenTank.y - 25;
	y2g = greenTank.y + 45;
	x1r = redTank.x;
	x2r = redTank.x + 45;
	y1r = redTank.y - 25;
	y2r = redTank.y + 45;

	for (int i = 0; i < ind; i++) {
		if (bullets[i] != NULL) {
			xb = bullets[i]->x + 4;
			yb = bullets[i]->y + 4;
			if (xb >= x1g && xb <= x2g && yb >= y1g && yb <= y2g && bullets[i]->rect.h!=7) {
				gAlive = false;
			}
			if (xb >= x1r && xb <= x2r && yb >= y1r && yb <= y2r && bullets[i]->rect.h == 7) {
				rAlive = false;
			}
		}
	}
}

void GreenMovement() {
	if (states[SDL_SCANCODE_J]) {
		greenTank.rotation += deltaTime / 1000000.0 * ROTATION_SPEED;
		if (!CollisionCheck(&greenTank, maze->collisions)) {
			greenTank.rotation -= deltaTime / 1000000.0 * ROTATION_SPEED;

		}
	}
	if (states[SDL_SCANCODE_L]) {
		greenTank.rotation -= deltaTime / 1000000.0 * ROTATION_SPEED;
		if (!CollisionCheck(&greenTank, maze->collisions)) {
			greenTank.rotation += deltaTime / 1000000.0 * ROTATION_SPEED;
		}
	}
	FixRotations();
	if (states[SDL_SCANCODE_I]) {
		greenTank.IncrementY(sin(3.14159265359 / 180.0 * ((ClampRotation(greenTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED);
		greenTank.IncrementX(-cos(3.14159265359 / 180.0 * ((ClampRotation(greenTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED);
		if (!CollisionCheck(&greenTank, maze->collisions)) {
			greenTank.IncrementY(sin(3.14159265359 / 180.0 * ((ClampRotation(greenTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * -SPEED);
			greenTank.IncrementX(-cos(3.14159265359 / 180.0 * ((ClampRotation(greenTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * -SPEED);
		}
	}
	if (states[SDL_SCANCODE_K]) {
		greenTank.IncrementY(-(sin(3.14159265359 / 180.0 * ((ClampRotation(greenTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED));
		greenTank.IncrementX(-(-cos(3.14159265359 / 180.0 * ((ClampRotation(greenTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED));
		if (!CollisionCheck(&greenTank, maze->collisions)) {
			greenTank.IncrementY(sin(3.14159265359 / 180.0 * ((ClampRotation(greenTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED);
			greenTank.IncrementX(-cos(3.14159265359 / 180.0 * ((ClampRotation(greenTank.rotation, ROTATION_CLAMP) - 90.0))) * deltaTime / 1000000.0 * SPEED);
		}
	}
}

void Loop() {

	states = SDL_GetKeyboardState(nullptr);

	maze->RenderMaze(renderer, &wall, &wall, &wallPoint);
	RedMovement();
	GreenMovement();
	Keybinds();
	MoveBullet();
	CheckForDeath();

	if (ind != 0) {
		for (int i = 0; i < ind; i++) {
			if (bullets[i] == NULL)continue;
			Render(*bullets[i]);
		}
		for (int i = 0; i < ind; i++) {
			if (bullets[i] == NULL)continue;
			if (bullets[i]->x > 660 || bullets[i]->y > 660 || bullets[i]->x < -20 || bullets[i]->y < -20) {
				delete bullets[i];
				bullets[i] = NULL;
			}
		}
	}

	if(rAlive)Render(redTank);
	if(gAlive)Render(greenTank);
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



