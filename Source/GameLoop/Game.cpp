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
	Logger::SetUp("", LogOutput::Console, LogOutput::Console);
	MethaneVersion();
	MT::SetSeed(static_cast<unsigned int>(std::time(0)));

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

	Slider* sl = ui->CreateSlider("", 200, 200, 80, 40, SliderSlide::X, 100, 400, nullptr);
	sl->SetColor(30, 30, 30);

	sl = ui->CreateSlider("", 500, 200, 80, 40, SliderSlide::Y, 100, 400, nullptr);
	sl->SetColor(30, 30, 30);

	Label* lb = ui->CreateLabel("", 100 - 10, 200, 10, 10);
	lb->SetColor(255, 0, 0);
	lb = ui->CreateLabel("", 400, 200, 10, 10);
	lb->SetColor(255, 0, 0);

	lb = ui->CreateLabel("", 500, 100-10, 10, 10);
	lb->SetColor(255, 0, 0);
	lb = ui->CreateLabel("", 500, 400, 10, 10);
	lb->SetColor(255, 0, 0);

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
	ren->ClearFrame(255, 255, 255);

	// Rozgrzewka
	for (size_t i = 0; i < 10'000; i++) {
		ren->DrawRect({ 10,300,100,100 }, { 30,30,30 });
	}

	MT::Timer::Tic();;
	for (size_t i = 0; i < 10'000; i++) {
		ren->DrawRectNew({ 600,600,100,100 }, { 30,30,30 });

	}
	time2 += MT::Timer::Tac<std::chrono::microseconds>();

	MT::Timer::Tic();;
	for (size_t i = 0; i < 10'000; i++) {
		ren->DrawRect({ 10,10,100,100 }, { 30,30,30 });
	}
	time1 += MT::Timer::Tac<std::chrono::microseconds>();



	if (Global::frameCount % 100 == 0) {
		std::println("Time1: {}us Time2: {}us", time1/100, time2/100);
		time1 = 0;
		time2 = 0;
	}

	//ui->Render();
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
