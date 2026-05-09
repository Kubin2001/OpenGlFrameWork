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

	constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec4 aSourceRect;
        layout(location = 2) in float aAlpha;

        out vec4 oTexCoord;
        out float oAlpha;

        uniform vec2 uVievPort;

        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            vec2 texUvs = uvs[gl_VertexID % 6];

            float u = aSourceRect.x + texUvs.x * aSourceRect.z;
    
            float v0 = 1.0 - aSourceRect.y - aSourceRect.w; 
            float v = v0 + texUvs.y * aSourceRect.w;        

            oTexCoord = vec4(vec2(u, v),uvs[gl_VertexID % 6]);
            oAlpha = aAlpha;
        }
        )glsl";

	constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec4 oTexCoord;
        in float oAlpha;

        uniform sampler2D texture1;
        uniform sampler2D texture2;


        void main(){
	        vec4 texcolor = texture(texture1,oTexCoord.xy);
	        vec4 texcolor2 = texture(texture2,oTexCoord.zw);
	        texcolor.a *= oAlpha;
	        FragColor = vec4(texcolor.r * texcolor2.r, texcolor.g * texcolor2.g, 
		        texcolor.b * texcolor2.b, texcolor.a);
        }
        )glsl";

	cShader.Create(renderer, vertexStr, fragmentStr, {4,4,1});
	cShader.AddUniform("uVievPort");

	unsigned int vievPort = cShader.GetUnifrom("uVievPort");
	glUniform2f(vievPort, Global::windowWidth, Global::windowHeight);

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
	MT::Texture* tex1 = TexMan::GetTex("tree1");
	MT::Texture* tex2 = TexMan::GetTex("water");

	cShader.Render({ 10.0f,10.0f,100.0f,100.0f,  0.0f,0.0f, 1.0f, 1.0f, 255.0f },tex1, tex2);
	renderer->RenderCopy({ 200,200,200,200 }, tex1);
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
