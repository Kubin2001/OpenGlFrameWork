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

	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial12");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 20, "arial20");
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 40, "arial40");

	ren->FlatDrawSetUp();
}

void Game::LogicUpdate() {
	Global::logicCount++;
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
		Exit();
	}
}

void Game::Render() {
	MT::Texture* tex1 = TexMan::GetTex("grass");
	MT::Texture* tex2 = TexMan::GetTex("stone");
	ren->ClearFrame(255, 255, 255);
	//MT::Timer::Tic();
	//for (size_t i = 0; i < 10'000; i++) {
	//	ren->DrawRect({ 10,10,100,100 }, { 30,30,30 });
	//}
	//test1Time += MT::Timer::Tac<std::chrono::microseconds>();
	//MT::Timer::Tic();
	//for (size_t i = 0; i < 10'000; i++) {
	//	ren->DrawSprite({ 200,10,100,100 },tex1);
	//}
	//test2Time += MT::Timer::Tac<std::chrono::microseconds>();
	MT::Timer::Tic();
	for (size_t i = 0; i < 5'000; i++) {
		ren->DrawSprite({ 10,200,100,100 }, tex1);
		ren->DrawSprite({ 200,200,100,100 }, tex2);
	}




	//ui->Render();
	ren->Present();
	test3Time += MT::Timer::Tac<std::chrono::microseconds>();

	if (Global::frameCount % 100 == 0) {
		std::println("Test 1: {}us    Test 2: {}us    Test 3: {}us", test1Time / 100, test2Time / 100, test3Time / 100);
		test1Time = 0;
		test2Time = 0;
		test3Time = 0;
	}

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
