#pragma once
#include <SDL.h>
#include <memory>

#include "UI.h"


struct Ob{
	MT::Rect rect;
	MT::Texture *tex;
	Ob(MT::Rect rect, MT::Texture* tex) {
		this->rect = rect;
		this->tex = tex;
	}

};

class Game {
private:
	SDL_Window* window = nullptr;
	MT::Renderer *renderer = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;
	std::vector<Ob> objects;


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
