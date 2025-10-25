#pragma once
#include <iostream>
#include <vector>
#include "GlobalVariables.h"
#include "Rectangle.h"

void MethaneVersion();

struct MapPos {
	int rows = 0; //Du¿e Regiony
	int column = 0;

	int rowsTile = 0; //Pole w regionie 
	int columnTile = 0;

	int absTileRows = 0;
	int absTileColumn = 0;

	MapPos();

	MapPos(int x, int y);

	void CalcRegTile(int x, int y);

	void CalcRegion(int x, int y);

	void CalcAbsTile(int x, int y);

	void CalcAll(int x, int y);

	void RecalculateFromAbs();


	// It will never work with automaitic check cause like with mouse cause it always is between 0 and tile per region
	// it is for manual change
	bool CorrectnessRegionTile();

	bool CorrectnessRegion();

	bool CorrectnessAbsTile();

	bool CorrectnesAbsCol();

	bool CorrectnesAbsRow();

	//Silent checks without cout

	bool CorrectnessRegionTileS();

	bool CorrectnessRegionS();

	bool CorrectnessAbsTileS();

	bool CorrectnesAbsColS();

	bool CorrectnesAbsRowS();



	static int minX;
	static int minY;
	static int tileSize;
	static int regionSize;
	static int tilesPerRegion;
	static int regionsCountWidth;
	static int regionsCountHeight;
	static int maxX;
	static int maxY;

	static void FedData(int mX, int mY, int tSize, int tilesPerReg, int regionsW, int regionsH);


};

double CalculateEuclidean(int x1, int x2, int y1, int y2);

Point GetRectangleCenter(const MT::Rect rect);

unsigned int RectanglePointDistance(const MT::Rect rect, const Point point);

unsigned int PointsDistance(const Point point, const Point point2);


class AnyData {
	public:
	virtual ~AnyData() = default;

	template <typename T>
	T& Get();

	template <typename T>
	void Set(T tempData);
};





template <typename T>
class AnyContatiner : public AnyData {
	public:
	T data;

	AnyContatiner() = default;

	AnyContatiner(T temp) {
		data = temp;
	}


};

template <typename T>
T& AnyData::Get() {
	auto temp = static_cast<AnyContatiner<T>*>(this); // Can be static not dynamic cause only one class is using inheritance
	if (!temp) {
		std::cerr << "Error: Wrong cast in AnyDataGet deafault value returned\n";
	}
	return temp->data;
}


template <typename T>
void AnyData::Set(T tempData) {
	auto temp = static_cast<AnyContatiner<T>*>(this); // Can be static not dynamic cause only one class is using inheritance
	if (!temp) {
		std::cerr << "Error: Data set uncorrectly in any data\n";
		return;
	}
	temp->data = tempData;
}


void ScaleRectanglesToScreen(std::vector<MT::Rect>& vec, int count, int desiredY, int width, int height, int xSpace, bool clearVec = true);

std::vector<std::string> SplitString(const std::string &str, const char seperator, const bool keepSep = false);

int RandInt(int min, int max);

Point GetMousePos();

// They work you can uncomment them if you wish but for some reason they sometimes break Intelli Sense



////Use only if you dont care about elements order
//template <typename T>
//void EraseSwitch(std::vector<T> &vec, const T &elem) {
//	for (size_t i = 0; i < vec.size(); i++) {
//		if (elem == vec[i]) {
//			vec[i] = vec.back();
//			vec.pop_back();
//			return;
//		}
//	}
//}
//
////Use only if you dont care about elements order
//template <typename T>
//void EraseSwitchDel(std::vector<T> &vec, const T &elem) {
//	for (size_t i = 0; i < vec.size(); i++) {
//		if (elem == vec[i]) {
//			delete vec[i];
//			vec[i] = vec.back();
//			vec.pop_back();
//			return;
//		}
//	}
//}
//
////Use only if you dont care about elements order
//template <typename T>
//void EraseSwitchDelArr(std::vector<T> &vec, const T &elem) {
//	for (size_t i = 0; i < vec.size(); i++) {
//		if (elem == vec[i]) {
//			delete[] vec[i];
//			vec[i] = vec.back();
//			vec.pop_back();
//			return;
//		}
//	}
//}



//Use only if you dont care about elements order
//template <typename T>
//void EraseSwitchIndex(std::vector<T>& vec, const size_t i) {
//	vec[i] = vec.back();
//	vec.pop_back();
//}
//
////Use only if you dont care about elements order
//template <typename T>
//void EraseSwitchDelIndex(std::vector<T>& vec, const size_t i) {
//	delete vec[i];
//	vec[i] = vec.back();
//	vec.pop_back();
//}
//
////Use only if you dont care about elements order
//template <typename T>
//void EraseSwitchDelArrIndex(std::vector<T>& vec, const size_t i) {
//	delete[] vec[i];
//	vec[i] = vec.back();
//	vec.pop_back();
//}

///////////////////////////////////////////////////////////