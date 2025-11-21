#pragma once
#include <SDL.h>
#include <memory>

#include "UI.h"

struct Object {
	MT::Rect rect{0,0,0,0};
	MT::Color col{ 0,0,0 };
	float Radius;
};

class Game {
private:
	SDL_Window* window = nullptr;
	MT::Renderer *renderer = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;
	std::vector<Object> objects;
	long long fullTime = 0;
public:
	Game();

	void Start();

	void LogicUpdate();

	void FrameUpdate();

	void Input();

	void Exit();

	void Render();

	~Game();

};
