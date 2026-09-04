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


#include "SDL_ttf.h"

void Game::Start() {
	MethaneVersion();
	MT::SetSeed(static_cast<unsigned int>(time(0)));

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	Global::windowWidth = 1280;
	Global::windowHeight = 720;

	window.Init("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Global::windowWidth, Global::windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	ren = std::make_unique<MT::Renderer>(window);

	TexMan::Start(ren.get());
	TexMan::DeepLoad("Textures");

	SoundMan::Init();
	SoundMan::DeepLoad("Sounds");

	ui = std::make_unique<UI>(ren.get());

	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 10, "arial10");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial12");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 20, "arial20");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 40, "arial40");

	ren->FlatDrawSetUp();
	
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
	ren->ClearFrame(255, 255, 255);
	ren->DrawSprite({ 10,10,100,100 }, TexMan::GetTex("PawnLeft"));
	ui->Render();
	ren->Present();
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
	ren->Clear();
	ui->ClearAll();
	SDL_Quit();
}
