#pragma once
#include <SDL.h>
#include <memory>

#include "UI.h"

struct Object {
	MT::Rect rect = {};
	MT::Texture* tex = nullptr;
};

struct SourcedObject {
	MT::Rect rect = {};
	MT::Texture* tex = nullptr;
	MT::Rect sourceRect;
};


class Game {
private:
	SDL_Window* window = nullptr;
	MT::Renderer *renderer = nullptr;
	SDL_Event event = {};
	std::unique_ptr<UI> ui = nullptr;
	MT::Atlas atlas = {};
	
	std::vector<Object> objects = {};
	std::vector<SourcedObject> sourceObjects = {};


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
