#include "PathMaker.h"
#include "UI.h"

void PathMaker::Input() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
			Uint32 myWindowID = SDL_GetWindowID(window);
			if (event.window.windowID == myWindowID) {
				finished = true;
			}
		}
		if (event.type == SDL_KEYUP) {
			if (event.key.keysym.scancode == SDL_SCANCODE_R) {
				switch (programState) {
					case 0:
						programState = 1;
						statusText = "Drawing press R to stop";
						break;
					case 1:
						programState = 2;
						statusText = "Saving";
						break;
					case 2:
						programState = 0;
						break;
				}
			}
		}
	}
}

void PathMaker::Render() {
	ren->ClearFrame(30, 30, 30);
	ui->FrameUpdate();
	ui->Render();
	ui->RenderRawText(ui->GetFont("arial12px"), 10, 10, statusText, 0, 230, 230, 230);
	ren->Present();
}

void PathMaker::Maintain() {
	while(!finished){
		Input();
		Render();
	}
}

void PathMaker::Open(int windowW, int windowH) {
	MT::ConstextGuard cg;
	window = SDL_CreateWindow("Path Maker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		windowW, windowH, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_OPENGL);
	ren = new MT::Renderer();
	ren->Start(window, MT::Innit(window));

	texMan.Start(ren);
	texMan.LoadMultiple("Textures/FileExplorer");
	ui = new UI(ren);
	ui->CreateFont("arial12px", texMan.GetTex("arial12px"), "Textures/Interface/Fonts/arial12px.json");
	statusText = "Press R to start";

	Maintain();

	ui->ClearAll();
	texMan.Clear();
	ren->Clear();
	delete ren;
	SDL_DestroyWindow(window);
}