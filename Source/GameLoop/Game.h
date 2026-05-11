#pragma once
#include <SDL.h>
#include <memory>

#include "UI.h"
#include "Window.h"

class Game {
private:
	MT::Window window{};
	MT::Renderer *renderer = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;


public:
	void Start();

	void LogicUpdate();

	void FrameUpdate();

	void Input();

	void Exit();

	void Render();

	~Game();

};
