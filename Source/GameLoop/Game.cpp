#include "Game.h"

#include <SDL.h>
#include <print>

#include "GlobalVariables.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "Addons.h"
#include "Files.h"

#include "PathMaker.h"

#include <chrono>


Game::Game() {
	window = nullptr;
	ui = nullptr;
}

void Game::Start() {
	MethaneVersion();
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		Global::windowWidth, Global::windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	renderer = new MT::Renderer();
	renderer->Start(window, MT::Init(window));

	Global::defaultDrawColor[0] = 255;
	Global::defaultDrawColor[1] = 255;
	Global::defaultDrawColor[2] = 255;

	TexMan::Start(renderer);
	TexMan::DeepLoad("Textures");
	SoundMan::Init();
	SoundMan::DeepLoad("Sounds");

	ui = std::make_unique<UI>(renderer);

	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial12");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 20, "arial20");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 40, "arial40");

	renderer->FLatRenderCopySetUp();

	FileExplorer fe;
	fe.Open();

	PathMaker pe;
	pe.Open();
}

void Game::LogicUpdate() {
	Global::frameCounter++;
}

void Game::FrameUpdate() {
	Input();
	ui->FrameUpdate();
	Render();
}

void Game::Input() {
	while (SDL_PollEvent(&event)) {
		ui->ManageInput(event);
		Exit();
	}
	Global::inputDelay++;
}

void Game::Render() {
	renderer->ClearFrame(Global::defaultDrawColor[0], Global::defaultDrawColor[1], Global::defaultDrawColor[2]);

	MT::Texture* texture = TexMan::GetTex("granite");
	MT::Texture* texture2 = TexMan::GetTex("grass");
	MT::Texture* texture3 = TexMan::GetTex("water");

	MT::Rect rect1{ 10,10,100,100 };
	MT::Rect rect2{ 10,210,100,100 };
	MT::Rect rect3{ 10,410,100,100 };

	MT::Timer::Tic();
	for (size_t i = 0; i < 1000; i++) {
		renderer->FLatRenderCopy(rect1, texture);
		renderer->FLatRenderCopy(rect2, texture2);
		renderer->FLatRenderCopy(rect3, texture3);

	}
	renderer->FLatRenderCopyPresent();
	//renderer->RenderCopy({ 500,500,200,200 }, tAtlas);
	renderer->Present();
	auto time = MT::Timer::Tac<std::chrono::microseconds>();


	std::println("Time {} mikrosec", time);

	//ui->Render();
}


void Game::Exit() {
	if (event.type == SDL_QUIT) {
		Global::status = false; 
	}
	else if (event.type == SDL_KEYUP && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
		Global::status = false;
	}
}

Game::~Game() {
	TexMan::Clear();
	SoundMan::Clear();
	SceneMan::Clear();
	renderer->Clear();
	ui->ClearAll();
	SDL_DestroyWindow(window);
	SDL_Quit();
}
