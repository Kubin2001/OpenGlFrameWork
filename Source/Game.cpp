#include "Game.h"

#include <SDL.h>
#include <print>

#include "GlobalVariables.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "SceneManager.h"
#include "Addons.h"

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
	//std::vector<UIElemBase*> elem;
	//Button *btn = ui->CreateButton("test1", 500, 100, 200, 100,nullptr,nullptr,"Some text");
	//btn->SetColor(30, 30, 30);
	//btn->SetFont(ui->GetFont("arial20px"));
	//btn->SetRenderTextType(2);
	//btn->SetBorder(4, 0, 100, 200);
	//btn->SetHoverFilter(true, 255, 255, 255, 120);
	//elem.push_back(btn);
	//btn = ui->CreateButton("test2", 500, 300, 200, 100, nullptr, nullptr, "Some text");
	//btn->SetColor(30, 30, 30);
	//btn->SetFont(ui->GetFont("arial12px"));
	//btn->SetRenderTextType(2);
	//btn->SetRenderType(2);
	//btn->SetBorder(4, 0, 100, 200);
	//btn->SetHoverFilter(true, 255, 255, 255, 120, "medKit");
	//elem.push_back(btn);

	//ui->DumpToJson("test", elem);
	//ui->LoadFromJson("test");

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
