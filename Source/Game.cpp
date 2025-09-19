#include "Game.h"

#include <SDL.h>
#include <print>
#include <string>
#include <SDL_image.h>

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
	SoundMan::LoadSounds("Sounds");

	ui = std::make_unique<UI>(renderer);

	ui->CreateFont("arial40px", TexMan::GetTex("arial40px"), "Textures/Interface/Fonts/arial40px.json");
	ui->CreateFont("arial20px", TexMan::GetTex("arial20px"), "Textures/Interface/Fonts/arial20px.json");
	ui->CreateFont("arial12px", TexMan::GetTex("arial12px"), "Textures/Interface/Fonts/arial12px.json");
	renderer->AgresiveRenderCopySetUp();

	int x = 0;
	int y = 0;
	for (size_t i = 0; i < 500; i++) {
		objects.emplace_back();
		objects.back().GetRectangle().Set(x, y, 25, 25);
		int random = RandInt(0, 10);
		if(random == 1){
			objects.back().SetTexture(TexMan::GetTex("MenuIcon"));
		}
		else {
			objects.back().SetTexture(TexMan::GetTex("RetryIcon"));
		}
		x += 30;
		if (i % 40 == 0) {
			y += 30;
			x = 0;
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
	using namespace std::chrono;

	auto start = high_resolution_clock::now();
	for (auto& it : objects) {
		renderer->AgressiveRenderCopy(it.GetRectangle(), it.GetTexture());
	}

	renderer->AgressiveRenderCopyPresent();
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(end - start).count();

	std::println("Render took {} microsec", duration);

	

	//auto start = high_resolution_clock::now();
	//for (auto& it : objects) {
	//	renderer->RenderCopy(it.GetRectangle(), it.GetTexture());
	//}
	renderer->RenderPresent();

	//auto end = high_resolution_clock::now();
	//auto duration = duration_cast<microseconds>(end - start).count();

	//std::println("Render took {} microsec", duration);

	//ui->Render();



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
	SDL_DestroyWindow(window);
	SDL_Quit();
}
