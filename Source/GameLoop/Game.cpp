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

void Game::Start() {
	MethaneVersion();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	Global::windowWidth = 1280;
	Global::windowHeight = 720;

	window.Init("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Global::windowWidth, Global::windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	renderer = new MT::Renderer();
	renderer->Start(window);

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

	std::vector<std::string> texturesNames = { "tree1", "granite", "water" };
	MT::Timer::Tic();
	atlasOne = TexMan::CreateAtlas("atlas1", 40, texturesNames);
	std::println("Old time : {}us", MT::Timer::Tac<std::chrono::microseconds>());
	MT::Timer::Tic();
	atlasTwo = TexMan::CreateAtlasNew("atlas2", 40, texturesNames);
	std::println("New time : {}us", MT::Timer::Tac<std::chrono::microseconds>());

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
	renderer->RenderCopyPart({ 10,10,40,40 }, atlasOne.sourceRectangles["tree1"], atlasOne.tex);
	renderer->RenderCopyPart({ 10,110,40,40 }, atlasOne.sourceRectangles["granite"], atlasOne.tex);
	renderer->RenderCopyPart({ 10,220,40,40 }, atlasOne.sourceRectangles["water"], atlasOne.tex);

	renderer->RenderCopy({ 300,300,static_cast<int>(atlasOne.tex->w), static_cast<int>(atlasOne.tex->h) }, atlasOne.tex);
	renderer->RenderBorder({ 300,300,static_cast<int>(atlasOne.tex->w), static_cast<int>(atlasOne.tex->h) }, { 30,30,30 }, 5);


	renderer->RenderCopyPart({ 200,10,40,40 }, atlasTwo.sourceRectangles["tree1"], atlasTwo.tex);
	renderer->RenderCopyPart({ 200,110,40,40 }, atlasTwo.sourceRectangles["granite"], atlasTwo.tex);
	renderer->RenderCopyPart({ 200,220,40,40 }, atlasTwo.sourceRectangles["water"], atlasTwo.tex);

	renderer->RenderCopy({ 700,300,static_cast<int>(atlasTwo.tex->w), static_cast<int>(atlasTwo.tex->h) }, atlasTwo.tex);
	renderer->RenderBorder({ 700,300,static_cast<int>(atlasTwo.tex->w), static_cast<int>(atlasTwo.tex->h) }, { 30,30,30 }, 5);


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
