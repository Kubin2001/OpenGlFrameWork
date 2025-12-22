#pragma once

#include "Renderer.h"
#include "TextureManager.h"

class UI;

class PathMaker {
private:
	SDL_Window* window = nullptr;
	MT::Renderer* ren = nullptr;
	SDL_Event event = {};
	bool finished = false;

	UI* ui = nullptr;

	LocalTexMan texMan;

	std::string statusText = "";
	int programState = 0; // 0 nothing happends default state 1 creating path

	void Input();

	void Render();

	void Maintain();
public:
	void Open(int windowW = 500 , int windowH = 500);

};
