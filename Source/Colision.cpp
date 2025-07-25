#include <SDL.h>
#include <cmath>
#include "Colision.h"
#include "Addons.h"

int Collision(const SDL_Rect rect, const SDL_Rect rect2) {

	int width = rect.w;
	int height = rect.h;

	int width2 = rect2.w;
	int height2 = rect2.h;

	if (rect.x + width >= rect2.x &&
		rect.x - width2 <= rect2.x &&
		rect.y + height >= rect2.y &&
		rect.y - height2 <= rect2.y) {

		if (rect.y + height >= rect2.y &&
			rect.y + height - 10 <= rect2.y) {
			return 2;
		}

		if (rect.y - height2 <= rect2.y &&
			rect.y - (height2 - 10) >= rect2.y) {
			return 4;
		}

		if (rect.x - width2 <= rect2.x &&
			rect.x - (width2 - 10) >= rect2.x) {
			return 1;
		}

		if (rect.x + width >= rect2.x &&
			rect.x + width - 10 <= rect2.x) {
			return 3;
		}

	}
	return 0;
}

bool SimpleCollision(const SDL_Rect rect, const SDL_Rect rect2) {

	int width = rect.w;
	int height = rect.h;

	int width2 = rect2.w;
	int height2 = rect2.h;

	if (rect.x + width >= rect2.x &&
		rect.x - width2 <= rect2.x &&
		rect.y + height >= rect2.y &&
		rect.y - height2 <= rect2.y) {
		return 1;
	}
	return 0;
}

int AdvancedCollision(const SDL_Rect rect, const SDL_Rect rect2, const int deepth) {

	int width = rect.w;
	int height = rect.h;

	int width2 = rect2.w;
	int height2 = rect2.h;

	if (rect.x + width >= rect2.x &&
		rect.x - width2 <= rect2.x &&
		rect.y + height >= rect2.y &&
		rect.y - height2 <= rect2.y) {

		if (rect.y + height >= rect2.y &&
			rect.y + height - deepth <= rect2.y) {
			return 2;
		}

		if (rect.y - height2 <= rect2.y &&
			rect.y - (height2 - deepth) >= rect2.y) {
			return 4;
		}

		if (rect.x - width2 <= rect2.x &&
			rect.x - (width2 - deepth) >= rect2.x) {
			return 1;
		}

		if (rect.x + width >= rect2.x &&
			rect.x + width - deepth <= rect2.x) {
			return 3;
		}

	}
	return 0;
}


bool CircleMouseCollision(const SDL_Rect circleRect, const SDL_Rect rect) {
	int centerCirleX = circleRect.x + (int)(circleRect.w * 0.5);
	int centerCirleY = circleRect.y + (int)(circleRect.h * 0.5);
	float radius = (float)(circleRect.h * 0.5);

	float distance = (float)CalculateEuclidean(centerCirleX, rect.x, centerCirleY, rect.y);
	if (distance <= radius) {
		return true;
	}
	else{
		return false;
	}
}


CollisonProjectile::CollisonProjectile(int x, int y, int w, int h, int xMov, int yMov) {
	GetRectangle()->x = x;
	GetRectangle()->y = y;
	GetRectangle()->w = w;
	GetRectangle()->h = h;
	this->xMov = xMov;
	this->yMov = yMov;
}

SDL_Rect* CollisonProjectile::GetRectangle() {
	return &rectangle;
}

void CollisonProjectile::setSpeedX(int temp) { xMov = temp; }

void CollisonProjectile::setSpeedY(int temp) { yMov = temp; }

int CollisonProjectile::GetTimer() {
	return timer;
}

void CollisonProjectile::SetTimer(int temp) {
	timer = temp;
}

void CollisonProjectile::MoveProjectlile() {
	GetRectangle()->x += xMov;
	GetRectangle()->x += yMov;
	timer++;
}