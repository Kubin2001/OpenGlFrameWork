#pragma once
#include <SDL.h>
#include <memory>
#include "Camera.h"
#include "UI.h"
#include "Renderer.h"

#include "Animator.h"


class Game {
private:
	SDL_Window* window = nullptr;
	MT::Renderer *renderer = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;
	Animation* anim = nullptr;

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
