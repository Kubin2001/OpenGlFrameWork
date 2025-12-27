#pragma once

#include "Renderer.h"
#include "TextureManager.h"
#include "UI.h"


class PathMaker {
private:
	SDL_Window* window = nullptr;
	MT::Renderer* ren = nullptr;
	SDL_Event event = {};
	bool finished = false;

	UI* ui = nullptr;

	LocalTexMan texMan;

	std::vector<Point> path;

	std::string statusText = "Press R to draw";
	Point startPoint;
	Point currentPoint;

	UISection saveSection;

	int programState = 0; // 0 nothing happends default state 1 creating path
	int saveState = 0; // 0 txt 1 csv 2 bin

	void CreateSaveMenu();

	void InputSaveMenu();

	void SaveTxt();

	void SaveCsv();

	void SaveBin();

	void Input();

	void FrameUpdate();

	void Render();

	void Maintain();

public:
	void Open(int windowW = 500 , int windowH = 500);

	std::vector<Point> LoadTxt(const std::string &path);

	std::vector<Point> LoadCsv(const std::string& path);

	std::vector<Point> LoadBin(const std::string& path);

};
