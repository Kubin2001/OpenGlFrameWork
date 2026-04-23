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

	Font* font = ui->GetFont("arial12");

	Label* btn = ui->CreateLabel("test1", 10, 10, 100, 100, nullptr, font, "text1");
	btn->SetColor(255, 0, 0);
	btn->SetRenderTextType(TextRenderType::Centered);
	btn->SetBorder(10, 0, 0, 255);

	btn = ui->CreateLabel("test2", 10, 120, 100, 100, nullptr, font, "text2");
	btn->SetColor(30, 30, 30);
	btn->SetRenderTextType(TextRenderType::Centered);
	btn->SetBorder(10, 255, 255, 255);

	btn = ui->CreateLabel("test3", 10, 240, 100, 100, nullptr, font, "text3");
	btn->SetColor(30, 30, 30);
	btn->SetRenderTextType(TextRenderType::Centered);
	btn->SetBorder(10, 100, 100, 100);
	btn->SetRenderType(RenderType::Rounded);

	btn = ui->CreateLabel("test4", 10, 360, 100, 100, TexMan::GetTex("grass"), font, "text4");
	btn->SetColor(30, 30, 30);
	btn->SetRenderTextType(TextRenderType::Centered);
	btn->SetBorder(10, 100, 100, 100);
	btn->SetRenderType(RenderType::Rounded);
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
	MT::Timer::Tic();
	ui->Render();
	renderer->Present();
	auto time = MT::Timer::Tac<std::chrono::microseconds>();


	std::println("Time {} mikrosec", time);


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
