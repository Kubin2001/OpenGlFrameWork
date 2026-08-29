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

	ui->settings.stopHoverAtFirst = true;
	Label *lb = ui->CreateLabel("1", 10, 10, 100, 100, nullptr, ui->GetFont("arial12"), "Some text rendered \nfor example", 0.5f);
	lb->SetColor(0, 0, 0, 255);
	lb->SetHoverFilter(120, 120, 120, 120);

	lb = ui->CreateLabel("2", 50, 10, 100, 100, nullptr, ui->GetFont("arial12"), "Some text rendered \nfor example", 0.5f);
	lb->SetColor(100, 100, 100, 255);
	lb->SetHoverFilter(120, 120, 120, 120);
	lb->Hide();
	

	lb = ui->CreateLabel("3", 10, 200, 100, 100, nullptr, ui->GetFont("arial12"), "Some text rendered \nfor example");
	lb->SetColor(0, 0, 0, 255);
	lb->SetHoverFilter(120, 120, 120, 120);
}

void Game::LogicUpdate() {
	Global::frameCounter++;
	const Uint8* state = SDL_GetKeyboardState(nullptr);
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
	renderer->ClearFrame(0, 0, 0);
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
