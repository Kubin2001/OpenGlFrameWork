#include "Addons.h"

#include <SDL_mouse.h>

void MethaneVersion() {
	std::cout << "Using Methane SDL OpenGL Framework v1.06\n";
}

MapPos::MapPos() {

}

MapPos::MapPos(int x, int y) {
	CalcAll(x, y);
}

int MapPos::minX = 0;
int MapPos::minY = 0;
int MapPos::tileSize = 0;
int MapPos::regionSize = 0;
int MapPos::tilesPerRegion = 0;
int MapPos::regionsCountWidth = 0;
int MapPos::regionsCountHeight = 0;
int MapPos::maxX = 0;
int MapPos::maxY = 0;

void MapPos::FedData(int mX, int mY, int tSize, int tilesPerReg, int regionsW, int regionsH) {
	MapPos::minX = mX;
	MapPos::minY = mY;
	MapPos::tileSize = tSize;
	MapPos::regionSize = tSize * tilesPerReg;
	MapPos::tilesPerRegion = tilesPerReg;
	MapPos::regionsCountWidth = regionsW;
	MapPos::regionsCountHeight = regionsH;
	MapPos::maxX = mX + (tSize * tilesPerReg * regionsW);
	MapPos::maxY = mY + (tSize * tilesPerReg * regionsH);
}




void MapPos::CalcRegTile(int x, int y) {
	int localY = (y - MapPos::minY) % MapPos::regionSize;
	if (localY < 0) { 
		localY += MapPos::regionSize;
	}
	rowsTile = localY / MapPos::tileSize;

	int localX = (x - MapPos::minX) % MapPos::regionSize;
	if (localX < 0) {
		localX += MapPos::regionSize;
	}
	columnTile = localX / MapPos::tileSize;
}

void MapPos::CalcRegion(int x, int y) {
	rows = (y - MapPos::minY) / MapPos::regionSize;
	if ((y - MapPos::minY) < 0) {
		rows -= 1;
	}

	column = (x - MapPos::minX) / MapPos::regionSize;
	if ((x - MapPos::minX) < 0) {
		column -= 1;
	}
}

void MapPos::CalcAbsTile(int x, int y) {
	absTileRows = (y - MapPos::minY) / MapPos::tileSize;
	if ((y - MapPos::minY) < 0) {
		absTileRows -= 1;
	}

	absTileColumn = (x - MapPos::minX) / MapPos::tileSize;
	if ((x - MapPos::minX) < 0) {
		absTileColumn -= 1;
	}
}

void MapPos::CalcAll(int x, int y) {
	CalcRegTile(x,y);
	CalcRegion(x,y);
	CalcAbsTile(x,y);
}

void MapPos::RecalculateFromAbs() {
	rows = absTileRows / MapPos::tilesPerRegion;
	if (absTileRows < 0) { --rows; }

	column = absTileColumn / MapPos::tilesPerRegion;
	if (absTileColumn < 0) { --column; }

	rowsTile = absTileRows % MapPos::tilesPerRegion;
	if (rowsTile < 0) {
		rowsTile += MapPos::tilesPerRegion;
	}
	columnTile = absTileColumn % MapPos::tilesPerRegion;
	if (columnTile < 0) {
		columnTile += MapPos::tilesPerRegion;
	}
}

bool MapPos::CorrectnessRegionTile() {
	if(rowsTile >= MapPos::tilesPerRegion){
		std::cerr << "MapPos Incorrect tile rows too big\n";
		return false;
	}
	if (rowsTile < 0) {
		std::cerr << "MapPos Incorrect tile rows too small\n";
		return false;
	}
	if (columnTile >= MapPos::tilesPerRegion) {
		std::cerr << "MapPos Incorrect tile columns too big\n";
		return false;
	}
	if (columnTile < 0) {
		std::cerr << "MapPos Incorrect tile columns too small\n";
		return false;
	}
	return true;
}

bool MapPos::CorrectnessRegion() {
	if (rows > MapPos::regionsCountHeight -1) {
		std::cerr << "MapPos Incorrect rows too big\n";
		return false;
	}
	if (rows < 0) {
		std::cerr << "MapPos Incorrect rows too small\n";
		return false;
	}
	if (column > MapPos::regionsCountWidth -1) {
		std::cerr << "MapPos Incorrect columns too big\n";
		return false;
	}
	if (column < 0) {
		std::cerr << "MapPos Incorrect columns too small\n";
		return false;
	}
	return true;
}

bool MapPos::CorrectnessAbsTile() {
	int countWidth = MapPos::regionsCountWidth * MapPos::tilesPerRegion;
	int countHeight = MapPos::regionsCountHeight * MapPos::tilesPerRegion;
	if (absTileRows >= countHeight) {
		std::cerr << "MapPos Incorrect abs tile rows too big\n";
		return false;
	}
	if (absTileRows < 0) {
		std::cerr << "MapPos Incorrect abs tile rows too small\n";
		return false;
	}
	if (absTileColumn >= countWidth) {
		std::cerr << "MapPos Incorrect abs tile columns too big\n";
		return false;
	}
	if (absTileColumn < 0) {
		std::cerr << "MapPos Incorrect abs tile columns too small\n";
		return false;
	}
	return true;
}

bool MapPos::CorrectnesAbsCol() {
	int countWidth = MapPos::regionsCountWidth * MapPos::tilesPerRegion;
	if (absTileColumn >= countWidth) {
		std::cerr << "MapPos Incorrect abs tile columns too big\n";
		return false;
	}
	if (absTileColumn < 0) {
		std::cerr << "MapPos Incorrect abs tile columns too small\n";
		return false;
	}
	return true;
}

bool MapPos::CorrectnesAbsRow() {
	int countHeight = MapPos::regionsCountHeight * MapPos::tilesPerRegion;
	if (absTileRows >= countHeight) {
		std::cerr << "MapPos Incorrect abs tile rows too big\n";
		return false;
	}
	if (absTileRows < 0) {
		std::cerr << "MapPos Incorrect abs tile rows too small\n";
		return false;
	}
	return true;
}
//Silent checks

bool MapPos::CorrectnessRegionTileS() {
	if (rowsTile >= MapPos::tilesPerRegion) {
		return false;
	}
	if (rowsTile < 0) {
		return false;
	}
	if (columnTile >= MapPos::tilesPerRegion) {
		return false;
	}
	if (columnTile < 0) {
		return false;
	}
	return true;
}

bool MapPos::CorrectnessRegionS() {
	if (rows > MapPos::regionsCountHeight - 1) {
		return false;
	}
	if (rows < 0) {
		return false;
	}
	if (column > MapPos::regionsCountWidth - 1) {
		return false;
	}
	if (column < 0) {
		return false;
	}
	return true;
}

bool MapPos::CorrectnessAbsTileS() {
	int countWidth = MapPos::regionsCountWidth * MapPos::tilesPerRegion;
	int countHeight = MapPos::regionsCountHeight * MapPos::tilesPerRegion;
	if (absTileRows >= countHeight) {
		return false;
	}
	if (absTileRows < 0) {
		return false;
	}
	if (absTileColumn >= countWidth) {
		return false;
	}
	if (absTileColumn < 0) {
		return false;
	}
	return true;
}

bool MapPos::CorrectnesAbsColS() {
	int countWidth = MapPos::regionsCountWidth * MapPos::tilesPerRegion;
	if (absTileColumn >= countWidth) {
		return false;
	}
	if (absTileColumn < 0) {
		return false;
	}
	return true;
}

bool MapPos::CorrectnesAbsRowS() {
	int countHeight = MapPos::regionsCountHeight * MapPos::tilesPerRegion;
	if (absTileRows >= countHeight) {
		return false;
	}
	if (absTileRows < 0) {;
		return false;
	}
	return true;
}
double CalculateEuclidean(int x1,int x2,int y1,int y2) {
	double x = std::pow(x2 - x1, 2);
	double y = std::pow(y2 - y1, 2);;
	return std::sqrt(x+y);
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