#include "Game.h"

#include <SDL.h>
#include <print>

#include "GlobalVariables.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "Addons.h"
#include "Files.h"
#include "Logger.h"

#include "PathMaker.h"

#include <chrono>

void Game::Start() {
	MethaneVersion();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	Global::windowWidth = 1280;
	Global::windowHeight = 720;

	window.Init("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Global::windowWidth, Global::windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	renderer = new MT::Renderer();
	renderer->Start(window);


	TexMan::Start(renderer);
	TexMan::DeepLoad("Textures");
	SoundMan::Init();
	SoundMan::DeepLoad("Sounds");

	ui = std::make_unique<UI>(renderer);

	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial12");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 20, "arial20");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 40, "arial40");

	renderer->FLatRenderCopySetUp();

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
	renderer->ClearFrame(255, 255, 255);
	MT::Texture* tex1 = TexMan::GetTex("water");
	ui->Render();
	MT::Timer::Tic();
	if (Global::frameCounter > 5) {
		for (size_t i = 0; i < 10000; i++) {
			renderer->RenderRoundedRect({ 10,10,100,100 }, { 100,100,100 });
			//renderer->DrawLine(10, 10, 100, 100, 5, { 255,0,0 });
			//renderer->RenderCopyCircle({ 10,10,100,100 }, tex1);
		}
		totalTime += MT::Timer::Tac<std::chrono::microseconds>();
		if (Global::frameCounter > 100) {
			Global::status = false;
			std::println("Time Taken: {}us", totalTime / 95);
		}
	}

	renderer->Present();
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
	SDL_Quit();
}
