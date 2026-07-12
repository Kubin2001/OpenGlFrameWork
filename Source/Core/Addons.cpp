#include "Addons.h"

void MethaneVersion() {
	std::println("Using Methane SDL OpenGL Framework v2.06");
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

