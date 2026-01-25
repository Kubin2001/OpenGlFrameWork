#include "Colision.h"

#include <array>
#include <glm.hpp>

#include "Addons.h"

int Collision(const MT::Rect& rect, const MT::Rect& rect2) {

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

bool SimpleCollision(const MT::Rect& rect, const MT::Rect& rect2) {
	if (rect.x + rect.w >= rect2.x &&
		rect.x - rect2.w <= rect2.x &&
		rect.y + rect.h >= rect2.y &&
		rect.y - rect2.h <= rect2.y) {
		return true;
	}
	return false;
}

bool SimpleCollision(const SDL_Rect& rect, const MT::Rect& rect2) {
	if (rect.x + rect.w >= rect2.x &&
		rect.x - rect2.w <= rect2.x &&
		rect.y + rect.h >= rect2.y &&
		rect.y - rect2.h <= rect2.y) {
		return true;
	}
	return false;
}

bool SimpleCollision(const SDL_Rect& rect, const MT::RectF& rect2) {
	if (rect.x + rect.w >= rect2.x &&
		rect.x - rect2.w <= rect2.x &&
		rect.y + rect.h >= rect2.y &&
		rect.y - rect2.h <= rect2.y) {
		return true;
	}
	return false;
}

bool SimpleCollision(const SDL_Rect& rect, const MT::CompositeRect& rect2) {
	if (rect.x + rect.w >= rect2.x &&
		rect.x - rect2.w <= rect2.x &&
		rect.y + rect.h >= rect2.y &&
		rect.y - rect2.h <= rect2.y) {
		return true;
	}
	return false;
}

int AdvancedCollision(const MT::Rect &rect, const MT::Rect& rect2, const int deepth) {

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


bool CircleMouseCollision(const MT::Rect &circleRect, const MT::Rect &rect) {
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

static std::array<glm::vec2, 4> GetCorners(MT::Rect& rotRect, const float angle) {
	Point center = rotRect.GetCenter();
	glm::vec2 vecCenter = glm::vec2(static_cast<float>(center.x), static_cast<float>(center.y));
	glm::vec2 extents(rotRect.w * 0.5f, rotRect.h * 0.5f);

	float c = std::cos(angle);
	float s = std::sin(angle);

	glm::vec2 axisX(c, s);
	glm::vec2 axisY(-s, c);

	// Calculating 4 corners
	glm::vec2 halfW = axisX * extents.x;
	glm::vec2 halfH = axisY * extents.y;

	return {
		vecCenter - halfW - halfH, // Top-Left 
		vecCenter + halfW - halfH, // Top-Right
		vecCenter + halfW + halfH, // Bottom-Right
		vecCenter - halfW + halfH  // Bottom-Left
	};

}

static void Project(const glm::vec2& axis, const std::array<glm::vec2, 4>& corners, float& min, float& max) {
	min = std::numeric_limits<float>::max();
	max = std::numeric_limits<float>::lowest();

	for (const auto& p : corners) {
		float proj = glm::dot(axis, p);
		if (proj < min) min = proj;
		if (proj > max) max = proj;
	}
}

bool RotRectCollision(const MT::Rect& rotRect, const float angle, const MT::Rect& rect) {
	MT::Rect rotRectCOpy = rotRect;
	float rad = glm::radians(angle);

	std::array<glm::vec2, 4> rotCorners = GetCorners(rotRectCOpy, rad);

	const float x = static_cast<float>(rect.x);
	const float y = static_cast<float>(rect.y);
	const float w = static_cast<float>(rect.w);
	const float h = static_cast<float>(rect.h);


	std::array<glm::vec2, 4> staticCorners = { {{x, y},{x+w,y},{x+w,y+h},{x,y+h} } };


	std::array<glm::vec2, 4> axes;

	axes[0] = { 1.0f, 0.0f };
	axes[1] = { 0.0f, 1.0f };

	float c = std::cos(rad);
	float s = std::sin(rad);
	axes[2] = { c, s };
	axes[3] = { -s, c };

	for (const auto& axis : axes) {
		float minA, maxA, minB, maxB;

		Project(axis, rotCorners, minA, maxA);
		Project(axis, staticCorners, minB, maxB);

		if (maxA < minB || maxB < minA) {
			return false; 
		}
	}

	return true;
}

bool RotRectCollision(const MT::CompositeRect& rotRect, const float angle, const MT::CompositeRect& rect) {
	MT::CompositeRect rotRectCOpy = rotRect;

	MT::Rect rectCopy = rotRectCOpy.ToRect();
	float rad = glm::radians(angle);

	std::array<glm::vec2, 4> rotCorners = GetCorners(rectCopy, rad);

	const float x = rect.x;
	const float y = rect.y;
	const float w = static_cast<float>(rect.w);
	const float h = static_cast<float>(rect.h);


	std::array<glm::vec2, 4> staticCorners = { {{x, y},{x + w,y},{x + w,y + h},{x,y + h} } };


	std::array<glm::vec2, 4> axes;

	axes[0] = { 1.0f, 0.0f };
	axes[1] = { 0.0f, 1.0f };

	float c = std::cos(rad);
	float s = std::sin(rad);
	axes[2] = { c, s };
	axes[3] = { -s, c };

	for (const auto& axis : axes) {
		float minA, maxA, minB, maxB;

		Project(axis, rotCorners, minA, maxA);
		Project(axis, staticCorners, minB, maxB);

		if (maxA < minB || maxB < minA) {
			return false;
		}
	}

	return true;
}

CollisonProjectile::CollisonProjectile(int x, int y, int w, int h, int xMov, int yMov) {
	rectangle.x = x;
	rectangle.y = y;
	rectangle.w = w;
	rectangle.h = h;
	this->xMov = xMov;
	this->yMov = yMov;
}

MT::Rect& CollisonProjectile::GetRectangle() {
	return rectangle;
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
	rectangle.x += xMov;
	rectangle.y += yMov;
	timer++;
}