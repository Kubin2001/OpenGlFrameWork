#include <SDL.h>
#include <iostream>
#include <string>
#include "SDL_image.h"
#include "Game.h"
#include "GlobalVariables.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "Animator.h"
#include "Addons.h"
#include "Files.h"




Game::Game() {
	window = nullptr;
	ui = nullptr;
}

void Game::Start() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		Global::windowWidth, Global::windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	renderer = new MT::Renderer();
	renderer->Start(window, MT::Innit(window));

	Global::defaultDrawColor[0] = 255;
	Global::defaultDrawColor[1] = 255;
	Global::defaultDrawColor[2] = 255;
	//SDL_SetRenderDrawColor(renderer, Global::defaultDrawColor[0], Global::defaultDrawColor[1], Global::defaultDrawColor[2], 255); 
	TexMan::Start(renderer);
	TexMan::DeepLoad("Textures");
	SoundMan::Innit();
	SoundMan::LoadSounds("Sounds");

	ui = std::make_unique<UI>(renderer);

	ui->CreateFont("arial40px", TexMan::GetTex("arial40px"), "Textures/Interface/Fonts/arial40px.json");
	ui->CreateFont("arial20px", TexMan::GetTex("arial20px"), "Textures/Interface/Fonts/arial20px.json");
	ui->CreateFont("arial12px", TexMan::GetTex("arial12px"), "Textures/Interface/Fonts/arial12px.json");


	//ui->CreateButton("test", 200, 200, 100, 100, TexMan::GetTex("FeFolderIcon"), ui->GetFont("arial20px"),
	//	"testowy Tekst", 1.0f);

	ClickBox* btn = ui->CreateClickBox("test2", 400, 400, 100, 100, nullptr, ui->GetFont("arial20px"),"text jakis \ntam");
	btn->SetColor(30, 30, 30);
	btn->SetHoverFilter(true, 255, 255, 255, 100);

	//btn = ui->CreateClickBox("test3", 600, 400, 100, 100, TexMan::GetTex("FeFolderIcon"), ui->GetFont("arial20px"), "text jakis \ntam");
	//btn->SetColor(30, 30, 30);
	//btn->SetHoverFilter(true, 255, 255, 255, 100);

	int x = 10;
	int y = 10;

	for (size_t i = 0; i < 4; i++) {
		for (size_t i = 0; i < 10; i++) {
			vec.emplace_back(x, y, 20, 20);
			x += 20;
		}
		x = 10;
		y += 20;
	}
}


void Game::LogicUpdate() {
	Global::frameCounter++;
}


void Game::FrameUpdate() {
	Input();
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

	renderer->RenderPresent();
	SDL_GL_SwapWindow(window);
}


void Game::Exit() {
	if (event.type == SDL_QUIT) {
		Global::status = false; 
	}
	else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
		Global::status = false;
	}
}


Game::~Game() {
	//TexMan::Clear();
	//SoundMan::Clear();
	//SceneMan::Clear();
	//SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	//std::cout << "Resources Destroyed";
}
