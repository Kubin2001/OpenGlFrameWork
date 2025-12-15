#pragma once
#include <SDL.h>
#include <memory>

#include "UI.h"

struct Ob {
	MT::Rect rect;
	MT::Color color;

	Ob(MT::Rect rect, MT::Color color) {
		this->rect = rect;
		this->color = color;
	}
};

class Game {
private:
	SDL_Window* window = nullptr;
	MT::Renderer *renderer = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;
	std::vector<Ob> vecToRender = {};
	long long totalTime = 0;
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
