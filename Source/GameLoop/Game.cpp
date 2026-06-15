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
	MT::SetSeed(static_cast<unsigned int>(time(0)));

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
		if (event.type == SDL_KEYUP && event.key.keysym.scancode == SDL_SCANCODE_S) {
			MT::Timer::Tic();
			SoundMan::PlaySound("click");
			auto total = MT::Timer::Tac<std::chrono::microseconds>();
			std::println("Time sound: {} us", total);
		}
		else if (event.type == SDL_KEYUP && event.key.keysym.scancode == SDL_SCANCODE_E) {
			MT::Timer::Tic();
			SoundMan::PlaySoundStereo("click",100,100);
			auto total = MT::Timer::Tac<std::chrono::microseconds>();
			std::println("Time sound stereo: {} us", total);
		}
		ui->ManageInput(event);
		Exit();
	}
	Global::inputDelay++;
}

void Game::Render() {
	renderer->ClearFrame(255, 255, 255);
	renderer->RenderRect({ 300,300,50,50 }, {30,30,30});
	Point pivot = MT::Rect{ 300,300,50,50 }.GetCenter();
	renderer->RenderRectEX({ 100,100,50,50 }, { 255,0,0 }, (float)Global::frameCounter, std::nullopt);
	ui->Render();
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
