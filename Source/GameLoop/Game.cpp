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

	MT::Texture* tex = TexMan::GetTex("arial12");
	MT::Texture* tex2 = TexMan::GetTex("arial20");
	MT::Texture* tex3 = TexMan::GetTex("arial40");

	std::println("Arial12 Width: {} Heigth: {}", tex->w, tex->h);
	std::println("Arial20 Width: {} Heigth: {}", tex2->w, tex2->h);
	std::println("Arial40 Width: {} Heigth: {}", tex3->w, tex3->h);

	Button *btn = ui->CreateButtonF("btn1", 100, 100, 100, 100, nullptr,"arial12","Some text + = 1");
	btn->SetColor(30, 30, 30);
	btn->SetRenderTextType((int)TextRenderType::Centered);

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
	MT::Texture* tex = TexMan::GetTex("arial12");
	MT::Texture* tex2 = TexMan::GetTex("arial20");
	MT::Texture* tex3 = TexMan::GetTex("arial40");
	renderer->RenderCopyFiltered({ 10,10,(int)tex->w,(int)tex->h }, TexMan::GetTex("arial12"), {255,0,0});
	renderer->RenderCopyFiltered({ 10,200,(int)tex2->w,(int)tex2->h }, TexMan::GetTex("arial20"), { 255,0,0 });
	renderer->RenderCopyFiltered({ 10,400,(int)tex3->w,(int)tex3->h }, TexMan::GetTex("arial40"), { 255,0,0 });
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
