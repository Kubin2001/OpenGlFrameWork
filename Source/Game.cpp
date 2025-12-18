#include "Game.h"

#include <SDL.h>
#include <print>

#include "GlobalVariables.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "Addons.h"
#include "Files.h"

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

	Global::defaultDrawColor[0] = 255;
	Global::defaultDrawColor[1] = 255;
	Global::defaultDrawColor[2] = 255;

	TexMan::Start(renderer);
	TexMan::DeepLoad("Textures");
	SoundMan::Innit();
	SoundMan::DeppLoad("Sounds");

	ui = std::make_unique<UI>(renderer);

	ui->CreateFont("arial40px", TexMan::GetTex("arial40px"), "Textures/Interface/Fonts/arial40px.json");
	ui->CreateFont("arial20px", TexMan::GetTex("arial20px"), "Textures/Interface/Fonts/arial20px.json");
	ui->CreateFont("arial12px", TexMan::GetTex("arial12px"), "Textures/Interface/Fonts/arial12px.json");

	//ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial30px");

	ClickBox *cb = ui->CreateClickBoxF("CB1", 100, 100, 100, 100, nullptr,"arial20px","Click 1");
	cb->SetColor(30, 30, 30);
	cb = ui->CreateClickBoxF("CB2", 300, 100, 100, 100, nullptr, "arial20px", "Click 2");
	cb->SetColor(30, 30, 30);
}

void Game::LogicUpdate() {
	Global::frameCounter++;
}

void Game::FrameUpdate() {
	Input();
	//if (ui->GetClickBox("CB1")->ConsumeStatus()) {
	//	std::println("clickbox 1");
	//}
	//if (ui->GetClickBox("CB2")->ConsumeStatus()) {
	//	std::println("clickbox 2");
	//}
	//if (ui->GetClickBox("CB3")->ConsumeStatus()) {
	//	std::println("clickbox 3");
	//}
	if (ui->ConsumeIfExist("CB1")) {
		std::println("clickbox 1");
	}
	if (ui->ConsumeIfExist("CB2")) {
		std::println("clickbox 2");
	}
	if (ui->ConsumeIfExist("CB3")) {
		std::println("clickbox 3");
	}

	ui->FrameUpdate();
	Render();
}

void Game::Input() {
	while (SDL_PollEvent(&event)) {
		ui->ManageInput(event);
		Exit();

		if (event.type == SDL_KEYUP) {
			if (event.key.keysym.scancode == SDL_SCANCODE_R) {
				MT::Timer::Tic();
				TexMan::RefreshTextures("Textures",true);
				std::println("Textures Refreshed in {} mickroSec", MT::Timer::Tac<std::chrono::microseconds>());
			}

			if (event.key.keysym.scancode == SDL_SCANCODE_L) {
				TexMan::DeepLoad("Textures");
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_C) {
				TexMan::Clear();
			}
		}
	}
	Global::inputDelay++;
}

void Game::Render() {
	renderer->ClearFrame(Global::defaultDrawColor[0],Global::defaultDrawColor[1],Global::defaultDrawColor[2]);


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
