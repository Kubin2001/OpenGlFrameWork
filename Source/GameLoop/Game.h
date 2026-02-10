#pragma once
#include <SDL.h>
#include <memory>

#include "UI.h"
#include "Animator.h"

class Game {
private:
	SDL_Window* window = nullptr;
	MT::Renderer *renderer = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;
	Animation anim;
	Animation anim2;


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
