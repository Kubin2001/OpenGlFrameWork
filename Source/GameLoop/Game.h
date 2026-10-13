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
	MT::Texture* tex = nullptr;
	unsigned int time1 = 0;
	unsigned int time2 = 0;

public:
	void Start();

	void LogicUpdate();

	void FrameUpdate();

	void Input();

	void Exit();

	void Render();

	~Game();

};
