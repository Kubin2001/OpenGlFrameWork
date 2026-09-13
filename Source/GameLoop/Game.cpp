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
	Logger::SetUp("", LogOutput::Console, LogOutput::Console);
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

	rect.Set(0, 200, 100, 100);
}

void Game::LogicUpdate() {
	Global::logicCount++;
	rect.x++;
	if (rect.x > 600) {
		rect.x = 0;
	}
	const Uint8* state = SDL_GetKeyboardState(nullptr);
}

void Game::FrameUpdate() {
	Global::frameCount++;

	Input();
	ui->FrameUpdate();
	Render();
}

void Game::Input() {
	while (SDL_PollEvent(&event)) {
		ui->ManageInput(event);
		if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_A) {
			Global::logicDelay--;
		}
		if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_D) {
			Global::logicDelay++;
		}

		if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_O) {
			Global::frameDelay--;
		}
		if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_P) {
			Global::frameDelay++;
		}
		Exit();
	}
}

void Game::Render() {

	ren->ClearFrame(255, 255, 255);
	ren->DrawRect(rect, { 0,0,255 });
	Font* font = ui->GetFont("arial12");
	ui->DrawRawText(font, 10, 10, std::format("Logic Delay: {}", Global::logicDelay), 20, { 0,0,0 });
	ui->DrawRawText(font, 200, 10, std::format("Frame Delay: {}", Global::frameDelay), 20, { 0,0,0 });
	ui->DrawRawText(font, 10, 100, std::format("Logic Count: {}", Global::logicCount), 20, { 0,0,0 });
	ui->DrawRawText(font, 200, 100, std::format("Frame Count: {}", Global::frameCount), 20, { 0,0,0 });
	ui->Render();
	ren->Present();

}


void Game::Exit() {
	if (event.type == SDL_QUIT) {
		Global::running = false; 
	}
	else if (event.type == SDL_KEYUP && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
		Global::running = false;
	}
}

Game::~Game() {
	TexMan::Clear();
	SoundMan::Clear();
	SceneMan::Clear();
	Logger::Close();
	ren->Clear();
	ui->ClearAll();
	SDL_Quit();
}
