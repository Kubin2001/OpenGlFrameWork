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

	tex = TexMan::GetTex("test1");
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

	Point mouse = GetMousePos();

	// Rozgrzewka
	//for (size_t i = 0; i < 10'000; i++) {
	//	ren->DrawRect({ 10,10,100,100 }, { 30,30,30 },100);
	//}

	MT::Timer::Tic();
	//for (size_t i = 0; i < 10'000; i++) {
		//ren->DrawRectEX({ 10,10,100,100 }, { 255,0,0 }, 0.0f, std::nullopt);
		//ren->DrawLine({ 10,10 }, { 100,100 }, 10, { 255,0,0 });
		//ren->DrawRoundedRect({ 10,10,100,100 }, { 255,0,0 },255,20);
		ren->DrawShadow({ 10,10,100,100 }, TexMan::GetTex("tree1"), { 255,0,0,255 }, {mouse.x, mouse.y,1.0f});
	//}

	time1 += MT::Timer::Tac<std::chrono::microseconds>();

	MT::Timer::Tic();
	//for (size_t i = 0; i < 10'000; i++) {
		//ren->DrawRectEX({ 10,200,100,100 }, { 0,0,0 }, (float)(Global::frameCount), center);
		//ren->DrawLine({ 110,110 }, { 210,210 }, 5, { 0,0,0 });
		//ren->DrawRoundedRect({ 10,200,100,100 }, { 0,0,0 },255, ((float)(Global::frameCount)) /100);
		ren->DrawShadow({ 10,200,100,100 }, TexMan::GetTex("tree1"), { 255,0,0,100 }, { mouse.x, mouse.y,1.0f });

	//}

	time2 += MT::Timer::Tac<std::chrono::microseconds>();

	if (Global::frameCount % 100 == 0) {
		std::println("TimeOld: {}us TimeNew: {}us", static_cast<double>(time1)/100, static_cast<double>(time2) /100);
		time1 = 0;
		time2 = 0;
	}

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
