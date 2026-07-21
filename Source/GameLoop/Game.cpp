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

void Game::Start() {
	MethaneVersion();
	MT::SetSeed(static_cast<unsigned int>(time(0)));

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	Global::windowWidth = 1280;
	Global::windowHeight = 720;

	window.Init("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Global::windowWidth, Global::windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	renderer = new MT::Renderer();
	renderer->Start(window);

	TexMan::Start(renderer);
	TexMan::DeepLoad("Textures");
	SoundMan::Init();
	SoundMan::DeepLoad("Sounds");

	ui = std::make_unique<UI>(renderer);

	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial12");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 20, "arial20");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 40, "arial40");

	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 8, "arial8");

	renderer->FLatRenderCopySetUp();

	cam = new Camera(false);
}

void Game::LogicUpdate() {
	Global::frameCounter++;
	const Uint8* state = SDL_GetKeyboardState(nullptr);
	cam->UpdatePosition(state);
}

void Game::FrameUpdate() {
	Input();
	ui->FrameUpdate();
	Render();
}

void Game::Input() {
	while (SDL_PollEvent(&event)) {
		ui->ManageInput(event);
		cam->UpdateZoom(event);
		Exit();
	}
	Global::inputDelay++;
}

void Game::Render() {
	renderer->ClearFrame(255, 255, 255);
	MT::Rect rect{ 200,200,100,100 };
	Point mousePos = GetMousePos();
	MT::Rect rect1{ 10,10,100,100 };
	MT::Rect rect2{ 300,300,100,100 };
	MT::Rect rect3{ 500,500,20,20 };
	MT::Texture* tex = TexMan::GetTex("tree1");
	rect1 = cam->Transform(rect1);
	rect2 = cam->Transform(rect2);
	rect3 = cam->Transform(rect3);
	renderer->RenderCopy(rect1, tex);
	renderer->RenderCopy(rect2, tex);
	renderer->RenderCopy(rect3, tex);
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
