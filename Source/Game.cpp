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
		int random = rand() % 3;
		Objects.emplace_back();
		auto &back = Objects.back();
		if (random == 0) {
			back.GetRectangle().Set(40, 40, 40, 40);
			back.SetTexture(TexMan::GetTex("MenuIcon"));
		}
		else if(random == 1) {
			back.GetRectangle().Set(90, 40, 30, 30);
			back.SetTexture(TexMan::GetTex("MenuIcon"));
		}
		else if (random == 2) {
			back.GetRectangle().Set(130, 40, 20, 20);
			back.SetTexture(TexMan::GetTex("RetryIcon"));
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
	for (auto& ob : Objects) {
		renderer->RenderRoundedRectUPR(ob.GetRectangle(),{0,0,255});
		MT::Rect rect = ob.GetRectangle();
		rect.y+= 100;
		renderer->RenderRectEXUPR(rect, { 0,255,0 }, (float)Global::frameCounter);
		rect.y += 100;
		renderer->RenderCopyCircleUPR(rect, ob.GetTexture());
		rect.y += 100;
		renderer->RenderCopyRoundedRectUPR(rect, ob.GetTexture());
		rect.y += 100;
		renderer->RenderCopyFilteredUPR(rect, ob.GetTexture(), { 0,255, 0});
		rect.y += 100;
		renderer->RenderCopyPartFilteredUPR(rect, {0,0,40,60}, ob.GetTexture(), { 0,0, 255 });
		rect.y += 100;
		renderer->RenderCopyPartEXUPR(rect, { 0,0,200,200 }, ob.GetTexture(), true, (float)Global::frameCounter);
		rect.y += 100;
	}
	//ui->Render();
	renderer->Present();

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	totalTIme += elapsed;
	if(Global::frameCounter % 200 == 0) {
		std::cout << "Render time: " << totalTIme/1000 << " ms\n";
		totalTIme = 0;
	}


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
