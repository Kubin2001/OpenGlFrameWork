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
	for (size_t i = 0; i < 10'000; i++) {
		objects.emplace_back();
		Object& ob = objects.back();
		int random = rand() % 3;
		switch (random) {
			case 0:
				ob.rect.Set(40, 40, 40, 40);
				ob.col = { 255,0,0 };
				ob.Radius = 45.0f;
				break;
			case 1:
				ob.rect.Set(90, 40, 40, 40);
				ob.col = { 0,255,0 };
				ob.Radius = 90.0f;
				break;
			case 2:
				ob.rect.Set(140, 40, 40, 40);
				ob.col = { 0,0,255 };
				ob.Radius = 180.0f;
				break;
		}
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
	renderer->ClearFrame(Global::defaultDrawColor[0],Global::defaultDrawColor[1],Global::defaultDrawColor[2]);
	auto start = std::chrono::high_resolution_clock::now();

	for (auto& it : objects) {
		//renderer->RenderRect({ 40, 100, 40, 40 }, it.col);
		renderer->RenderRectEX(it.rect, it.col, 0);
		MT::Rect rect = it.rect;
		rect.y += 60;
		//renderer->RenderCopyEX(rect,TexMan::GetTex("MenuIcon"),false,Global::frameCounter);
		renderer->RenderCopyPartEX(rect, {100,0,100,200}, TexMan::GetTex("MenuIcon"), false, Global::frameCounter);
	}
	std::cout << Global::frameCounter << "\n";
	renderer->Present(false);

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	fullTime += elapsed;
	if (Global::frameCounter % 200 == 0) {
		std::cout << "Render time: " << fullTime/1000 << " ms\n";
		fullTime = 0;
	}

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
