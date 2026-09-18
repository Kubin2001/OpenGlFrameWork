#pragma once
#include <SDL.h>
#include <memory>

#include "UI.h"
#include "Window.h"
#include "Camera.h"

class Game {
private:
	MT::Window window{};
	std::unique_ptr<MT::Renderer> ren = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;

	unsigned int test1Time = 0;
	unsigned int test2Time = 0;
	unsigned int test3Time = 0;


public:
	void Start();

	void LogicUpdate();

	void FrameUpdate();

	void Input();

	void Exit();

	void Render();

	~Game();

};
