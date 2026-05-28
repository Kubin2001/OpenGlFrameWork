#pragma once
#include <print>
#include <chrono>
#include <vector>
#include "GlobalVariables.h"
#include "Rectangle.h"


void MethaneVersion();

struct MapPos {
	int rows = 0; // Large Regions
	int column = 0;

	int rowsTile = 0; // Tiles In large Regions
	int columnTile = 0;

	int absTileRows = 0; // Absolute Tile Pos in whole map
	int absTileColumn = 0;


	inline static int minX = 0;
	inline static int minY = 0;
	inline static int tileSize = 0;
	inline static int regionSize = 0;
	inline static int tilesPerRegion = 0;
	inline static int regionsCountWidth = 0;
	inline static int regionsCountHeight = 0;
	inline static int maxX = 0;
	inline static int maxY = 0;

	inline MapPos() = default;

	inline MapPos(int x, int y) {
		CalcAll(x, y);
	}

	inline MapPos(const Point& p) {
		CalcAll(p.x, p.y);
	}

	inline void CalcRegTile(int x, int y) {
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

	inline void CalcRegion(int x, int y) {
		rows = (y - MapPos::minY) / MapPos::regionSize;
		if ((y - MapPos::minY) < 0) {
			rows -= 1;
		}

		column = (x - MapPos::minX) / MapPos::regionSize;
		if ((x - MapPos::minX) < 0) {
			column -= 1;
		}
	}

	inline void CalcAbsTile(int x, int y) {
		absTileRows = (y - MapPos::minY) / MapPos::tileSize;
		if ((y - MapPos::minY) < 0) {
			absTileRows -= 1;
		}

		absTileColumn = (x - MapPos::minX) / MapPos::tileSize;
		if ((x - MapPos::minX) < 0) {
			absTileColumn -= 1;
		}
	}

	inline void CalcAll(int x, int y) {
		CalcRegTile(x, y);
		CalcRegion(x, y);
		CalcAbsTile(x, y);
	}

	inline void RecalculateFromAbs() {
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



	inline bool CorrectnessRegionTile() const {
		if (rowsTile >= MapPos::tilesPerRegion) {
			std::println("MapPos Incorrect tile rows too big");
			return false;
		}
		if (rowsTile < 0) {
			std::println("MapPos Incorrect tile rows too small");
			return false;
		}
		if (columnTile >= MapPos::tilesPerRegion) {
			std::println("MapPos Incorrect tile columns too big");
			return false;
		}
		if (columnTile < 0) {
			std::println("MapPos Incorrect tile columns too small");
			return false;
		}
		return true;
	}

	inline bool CorrectnessRegion() const {
		if (rows > MapPos::regionsCountHeight - 1) {
			std::println("MapPos Incorrect rows too big");
			return false;
		}
		if (rows < 0) {
			std::println("MapPos Incorrect rows too small");
			return false;
		}
		if (column > MapPos::regionsCountWidth - 1) {
			std::println("MapPos Incorrect columns too big");
			return false;
		}
		if (column < 0) {
			std::println("MapPos Incorrect columns too small");
			return false;
		}
		return true;
	}

	inline bool CorrectnessAbsTile() const {
		int countWidth = MapPos::regionsCountWidth * MapPos::tilesPerRegion;
		int countHeight = MapPos::regionsCountHeight * MapPos::tilesPerRegion;
		if (absTileRows >= countHeight) {
			std::println("MapPos Incorrect abs tile rows too big");
			return false;
		}
		if (absTileRows < 0) {
			std::println("MapPos Incorrect abs tile rows too small");
			return false;
		}
		if (absTileColumn >= countWidth) {
			std::println("MapPos Incorrect abs tile columns too big");
			return false;
		}
		if (absTileColumn < 0) {
			std::println("MapPos Incorrect abs tile columns too small");
			return false;
		}
		return true;
	}

	inline bool CorrectnesAbsCol() const {
		int countWidth = MapPos::regionsCountWidth * MapPos::tilesPerRegion;
		if (absTileColumn >= countWidth) {
			std::println("MapPos Incorrect abs tile columns too big");
			return false;
		}
		if (absTileColumn < 0) {
			std::println("MapPos Incorrect abs tile columns too small");
			return false;
		}
		return true;
	}

	inline bool CorrectnesAbsRow() const {
		int countHeight = MapPos::regionsCountHeight * MapPos::tilesPerRegion;
		if (absTileRows >= countHeight) {
			std::println("MapPos Incorrect abs tile rows too big");
			return false;
		}
		if (absTileRows < 0) {
			std::println("MapPos Incorrect abs tile rows too small");
			return false;
		}
		return true;
	}

	//Silent checks without printing

	inline bool CorrectnessRegionTileS() const {
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

	inline bool CorrectnessRegionS() const {
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

	inline bool CorrectnessAbsTileS() const {
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

	inline bool CorrectnesAbsColS() const {
		int countWidth = MapPos::regionsCountWidth * MapPos::tilesPerRegion;
		if (absTileColumn >= countWidth) {
			return false;
		}
		if (absTileColumn < 0) {
			return false;
		}
		return true;
	}

	inline bool CorrectnesAbsRowS() const {
		int countHeight = MapPos::regionsCountHeight * MapPos::tilesPerRegion;
		if (absTileRows >= countHeight) {
			return false;
		}
		if (absTileRows < 0) {
			;
			return false;
		}
		return true;
	}

	static void FedData(int minX, int minY, int tSize, int tilesPerReg, int regionsW, int regionsH) noexcept;


};

float CalculateEuclidean(int x1, int x2, int y1, int y2)  noexcept;

float CalculateEuclidean(const Point &target, const Point &dest)  noexcept;

float FastEuclidean(int x1, int x2, int y1, int y2)  noexcept;

float FastEuclidean(const Point& target, const Point& dest)  noexcept;

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
		std::println("Error: Wrong cast in AnyDataGet deafault value returned");
	}
	return temp->data;
}


template <typename T>
void AnyData::Set(T tempData) {
	auto temp = static_cast<AnyContatiner<T>*>(this); // Can be static not dynamic cause only one class is using inheritance
	if (!temp) {
		std::println("Error: Data set uncorrectly in any data");
		return;
	}
	temp->data = tempData;
}


void ScaleRectanglesToScreen(std::vector<MT::Rect>& vec, int count, int desiredY, int width, int height, int xSpace, bool clearVec = true);

std::vector<std::string> SplitString(const std::string &str, const char seperator, const bool keepSep = false);

int RandInt(int min, int max);

Point GetMousePos();

namespace MT {
	class Timer {
		inline static std::chrono::steady_clock::time_point start{};

		inline static std::chrono::steady_clock::time_point end{};


	public:
		static void Tic() {
			start = std::chrono::steady_clock::now();
		}

		template<typename T>
		static long long Tac() {
			static_assert(std::is_same_v<T,std::chrono::hours> || std::is_same_v<T, std::chrono::minutes> ||
				std::is_same_v<T, std::chrono::seconds> || std::is_same_v<T, std::chrono::milliseconds> ||
				std::is_same_v<T, std::chrono::microseconds> || std::is_same_v<T, std::chrono::nanoseconds>,
				"Incorect template argument accepts chrono::hours - chrono::nanoseconds");

			end = std::chrono::steady_clock::now();
			return std::chrono::duration_cast<T>(end - start).count();
		}
	};
}

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