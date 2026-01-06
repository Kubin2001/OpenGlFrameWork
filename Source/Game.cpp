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
	renderer->Start(window, MT::Innit(window));
	renderer->FLatRenderCopySetUp();

	Global::defaultDrawColor[0] = 255;
	Global::defaultDrawColor[1] = 255;
	Global::defaultDrawColor[2] = 255;

	TexMan::Start(renderer);
	TexMan::DeepLoad("Textures");
	SoundMan::Innit();
	SoundMan::DeppLoad("Sounds");

	ui = std::make_unique<UI>(renderer);

	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial12");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 20, "arial20");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 40, "arial40");

	ui->settings.stopHoverAtFirst = true;
	ui->settings.clickBoxStartAtDown = true;
	ui->settings.stopCheckAtFirst = true;
	ui->UseLayerInRendering(true);

	int x = 50;
	Font* font = ui->GetFont("arial12");
	for (size_t i = 0; i < 10; i++) {
		Button* btn = ui->CreateLayeredButton(13-i,"button" + std::to_string(i), x, 50, 200, 200, nullptr, font, "SomeText");
		btn->SetColor(30, 30, 30);
		btn->SetRenderTextType(2);
		btn->SetHoverFilter(true, 255, 255, 255, 120);
		btn->SetBorder(2, 255, 0, 0);
		x += 100;
	}
	x = 50;
	for (size_t i = 0; i < 10; i++) {
		ClickBox* btn = ui->CreateLayeredClickBox(13-i,"cb" + std::to_string(i), x, 350, 200, 200, nullptr, font, "SomeText");
		btn->SetColor(30, 30, 30);
		btn->SetRenderTextType(2);
		btn->SetHoverFilter(true, 255, 255, 255, 120);
		btn->SetBorder(2, 255, 0, 0);
		btn->SetClickSound("click");
		x += 100;
	}
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
	SDL_DestroyWindow(window);
	SDL_Quit();
}
