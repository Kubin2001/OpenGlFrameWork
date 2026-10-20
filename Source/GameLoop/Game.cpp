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
	MT::Texture* tex1 = TexMan::GetTex("tree1");
	MT::Texture* tex2 = TexMan::GetTex("grass");

	tex2->alpha = Global::frameCount % 255;

	//ren->DrawSpriteUPR({ 10,10,100,100 }, tex1);
	//ren->DrawSpriteUPR({ 210,10,100,100 }, tex2);

	//ren->DrawSpritePartUPR({ 10,10,100,100 }, {0,0,tex1->w, tex1->h}, tex1);
	//ren->DrawSpritePartUPR({ 210,10,100,100 }, { 0,0,tex2->w, tex2->h }, tex2);

	//ren->DrawSpriteEXUPR({ 10,10,100,100 }, { 0,0,tex1->w, tex1->h }, tex1);
	//ren->DrawSpriteEXUPR({ 210,10,100,100 }, { 0,0,tex2->w, tex2->h }, tex2);

	//ren->DrawCircleSpriteUPR({ 10,10,100,100 }, tex1);
	//ren->DrawCircleSpriteUPR({ 210,10,100,100 }, tex2);

	//ren->DrawFilteredSpriteUPR({ 10,10,100,100 }, tex1, {255,255,255});
	//ren->DrawFilteredSpriteUPR({ 210,10,100,100 }, tex2, { 255,255,255 });

	//ren->DrawRoundedSpriteUPR({ 10,10,100,100 }, tex1);
	//ren->DrawRoundedSpriteUPR({ 210,10,100,100 }, tex2);

	//ren->DrawMaskedOverlayUPR({ 10,10,100,100 }, { 0,0,tex1->w, tex1->h }, tex1, tex2);
	//ren->DrawMaskedOverlayUPR({210, 10, 100, 100}, { 0,0,tex2->w, tex2->h }, tex2, tex1);

	//ren->DrawDoubleMaskedOverlayUPR({ 10,10,100,100 }, { 0,0,tex1->w, tex1->h }, { 0,0,tex2->w, tex2->h }, tex1, tex2);
	//ren->DrawDoubleMaskedOverlayUPR({ 210, 10, 100, 100 }, { 0,0,tex2->w, tex2->h }, { 0,0,tex1->w, tex1->h }, tex2, tex1);

	
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
