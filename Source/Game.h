#pragma once
#include <SDL.h>
#include <memory>
#include "Camera.h"
#include "UI.h"
#include "Renderer.h"


class Game {
private:
	SDL_Window* window = nullptr;
	MT::Renderer *renderer;
	SDL_Event event = {};
	std::unique_ptr<UI> ui;

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
