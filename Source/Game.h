#pragma once
#include <SDL.h>
#include <memory>

#include "UI.h"

class Game {
private:
	SDL_Window* window = nullptr;
	MT::Renderer *renderer = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;
	std::vector<GameObject> Objects;
	long long totalTIme = 0;
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
