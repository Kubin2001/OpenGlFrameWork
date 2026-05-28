#include "Addons.h"


#include <SDL_mouse.h>
#include <chrono>
#include <print>

void MethaneVersion() {
	std::println("Using Methane SDL OpenGL Framework v2.03");
}


void MapPos::FedData(int minX, int minY, int tSize, int tilesPerReg, int regionsW, int regionsH) noexcept{
	MapPos::minX = minX;
	MapPos::minY = minY;
	MapPos::tileSize = tSize;
	MapPos::regionSize = tSize * tilesPerReg;
	MapPos::tilesPerRegion = tilesPerReg;
	MapPos::regionsCountWidth = regionsW;
	MapPos::regionsCountHeight = regionsH;
	MapPos::maxX = minX + (tSize * tilesPerReg * regionsW);
	MapPos::maxY = minY + (tSize * tilesPerReg * regionsH);
}

float CalculateEuclidean(int x1,int x2,int y1,int y2) noexcept{
	const float x = ((float)x2 - (float)x1) * ((float)x2 - (float)x1);
	const float y = ((float)y2 - (float)y1) * ((float)y2 - (float)y1);
	return std::sqrt(x+y);
}

float CalculateEuclidean(const Point& target, const Point& dest) noexcept {
	const float x = ((float)dest.x - (float)target.x) * ((float)dest.x - (float)target.x);
	const float y = ((float)dest.y - (float)target.y) * ((float)dest.y - (float)target.y);
	return std::sqrt(x + y);
}

float FastEuclidean(int x1, int x2, int y1, int y2) noexcept {
	const float x = ((float)x2 - (float)x1) * ((float)x2 - (float)x1);
	const float y = ((float)y2 - (float)y1) * ((float)y2 - (float)y1);
	return x + y;
}

float FastEuclidean(const Point& target, const Point& dest) noexcept {
	const float x = ((float)dest.x - (float)target.x) * ((float)dest.x - (float)target.x);
	const float y = ((float)dest.y - (float)target.y) * ((float)dest.y - (float)target.y);
	return x + y;
}

Point GetRectangleCenter(const MT::Rect rect) {
	Point point;
	point.x = rect.x + (int)(rect.w * 0.5);
	point.y = rect.y + (int)(rect.h * 0.5);
	return point;

}

unsigned int RectanglePointDistance(const MT::Rect rect, const Point point) {
	return std::abs(rect.x - point.x) + std::abs(rect.y - point.y);

}

unsigned int PointsDistance(const Point point, const Point point2) {
	return std::abs(point.x - point2.x) + std::abs(point.y - point2.y);
}


void ScaleRectanglesToScreen(std::vector<MT::Rect>& vec, int count, int desiredY, int width, int height, int xSpace, bool clearVec) {
	if (clearVec) { vec.clear(); }
	vec.reserve(count);

	if ((count & 1) == 1) { // nieparzyste
		int centerPoint = (Global::windowWidth / 2) - (width / 2); //Center x of the screen minus half of width

		int jump = width + xSpace;
		int startX = centerPoint - (jump * (count / (2)));

		for (int i = 0; i < count; i++) {
			vec.emplace_back(startX, desiredY, width, height);
			startX += jump;

		}
	}
	else {
		int centerPoint = (Global::windowWidth / 2) - (width + (xSpace / 2)); //Possition of lower half rectangle


		int jump = width + xSpace;
		int startX = centerPoint - (jump * (count / (2 + 1)));

		for (int i = 0; i < count; i++) {
			vec.emplace_back(startX, desiredY, width, height);
			startX += jump;

		}
	}
}

std::vector<std::string> SplitString(const std::string& str, const char seperator, const bool keepSep) {
	std::vector<std::string> outVec;

	std::string tempStr = "";
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == seperator) {
			if (keepSep) {
				tempStr += str[i];
				outVec.emplace_back(tempStr);
				tempStr = "";
				continue;
			}
			outVec.emplace_back(tempStr);
			tempStr = "";
			continue;
		}
		tempStr += str[i];
	}
	if(tempStr != "") {
		outVec.emplace_back(tempStr);
	}
	return outVec;
}

int RandInt(int min, int max) {
	return min + rand() % (max - min + 1);
}

Point GetMousePos() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	return { x,y };
}