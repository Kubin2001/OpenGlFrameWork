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

	SceneMan::AddScene<SceneOne>("SceneOne");

	SceneMan::AddScene<SceneTwo>("SceneTwo");

	SceneMan::SwitchScene<SceneOne>("SceneOne");

	SceneMan::SwitchScene<SceneTwo>("SceneTwo");

	SceneMan::SwitchScene<SceneOne>("SceneOne");

	SceneMan::SwitchScene<SceneTwo>("SceneTwo");

	SceneMan::SwitchScene<SceneOne>("SceneOne");

	SceneMan::SwitchScene<SceneTwo>("SceneTwo");

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
		if (event.type == SDL_KEYUP) {
			if (event.key.keysym.scancode == SDL_SCANCODE_R) {
				TexMan::RefreshTextures("Textures/Examples",true);
			}
		}
		Exit();
	}
	Global::inputDelay++;
}

void Game::Render() {
	renderer->ClearFrame(Global::defaultDrawColor[0], Global::defaultDrawColor[1], Global::defaultDrawColor[2]);
	ui->Render();
	renderer->RenderCopy({ 50,50,50,50 }, TexMan::GetTex("granite"));
	renderer->RenderCopy({ 50,150,50,50 }, TexMan::GetTex("grass",true));
	renderer->RenderCopy({ 50,250,50,50 }, TexMan::GetTex("stone"));
	renderer->RenderCopy({ 50,350,50,50 }, TexMan::GetTex("tree1"));
	renderer->RenderCopy({ 50,450,50,50 }, TexMan::GetTex("water"));
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
