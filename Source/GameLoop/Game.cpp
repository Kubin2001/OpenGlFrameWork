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


	MT::Timer::Tic();
	std::vector<std::string> names = { "grass", "granite", "water"};
	MT::Atlas atlas = TexMan::CreateAtlas(40, names);
	std::println("Time taken: {} mikrosec", MT::Timer::Tac<std::chrono::microseconds>());

	TexMan::AddTexture(atlas.tex, "atlas");
	// Creating objects
	int x = 0;
	int y = 0;
	for (size_t i = 0; i < 500; i++) {
		objects.emplace_back();
		Object& ob = objects.back();
		int random = RandInt(0, 10);
		if (random < 6) {
			ob.tex = TexMan::GetTex("grass");
		}
		else if (random < 8) {
			ob.tex = TexMan::GetTex("granite");
		}
		else {
			ob.tex = TexMan::GetTex("water");
		}
		ob.rect.Set(x, y, 40, 40);
		x += 40;
		if (x > 1000) {
			x = 0;
			y += 40;
		}
	}

	// Creating sourced objects
	x = 0;
	y = 0;
	for (size_t i = 0; i < 500; i++) {
		sourceObjects.emplace_back();
		SourcedObject& ob = sourceObjects.back();
		int random = RandInt(0, 10);
		if (random < 6) {
			ob.tex = TexMan::GetTex("atlas");
			ob.sourceRect = atlas.sourceRectangles[0];
		}
		else if (random < 8) {
			ob.tex = TexMan::GetTex("atlas");
			ob.sourceRect = atlas.sourceRectangles[1];
		}
		else {
			ob.tex = TexMan::GetTex("atlas");
			ob.sourceRect = atlas.sourceRectangles[2];
		}
		ob.rect.Set(x, y, 40, 40);
		x += 40;
		if (x > 1000) {
			x = 0;
			y += 40;
		}
	}
	renderer->FLatRenderCopySetUp();
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
	//MT::Texture* atlas = TexMan::GetTex("atlas");
	//renderer->RenderCopy({ 100,100,(int)atlas->w,(int)atlas->h }, atlas);

	//Render Rect
	MT::Timer::Tic();
	for (auto& ob : objects) {
		renderer->RenderCopy(ob.rect, ob.tex);
	}
	renderer->Present(false);
	auto timeRenderCopy = MT::Timer::Tac<std::chrono::microseconds>();

	//Flat RenderCopy
	MT::Timer::Tic();
	for (auto& ob : objects) {
		renderer->FLatRenderCopy(ob.rect, ob.tex);
	}
	renderer->FLatRenderCopyPresent();
	renderer->Present(false);
	auto timeFlatRenderCopy = MT::Timer::Tac<std::chrono::microseconds>();

	//RenderCopy Part (Atlas)
	MT::Timer::Tic();
	for (auto& ob : sourceObjects) {
		renderer->RenderCopyPart(ob.rect,ob.sourceRect, ob.tex);
	}
	renderer->Present(false);
	auto timeCopyAtlas = MT::Timer::Tac<std::chrono::microseconds>();

	//RenderCopyUPR Part (Atlas)
	MT::Timer::Tic();
	for (auto& ob : sourceObjects) {
		renderer->RenderCopyPartUPR(ob.rect, ob.sourceRect, ob.tex);
	}
	renderer->Present(false);
	auto timeCopyUPRAtlas = MT::Timer::Tac<std::chrono::microseconds>();

	std::println("Render Copy {} Flat RenderCopy {} RenderCopy Part Atlas {} RenderCopy UPR Atlas {}", timeRenderCopy, timeFlatRenderCopy, timeCopyAtlas, timeCopyUPRAtlas);

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
