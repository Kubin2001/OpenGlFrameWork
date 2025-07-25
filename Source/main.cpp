#include <iostream>
#include "SDL.h"
#include "Game.h"
#include "GlobalVariables.h"

int main(int argv, char* argc[])
{
	//srand(time(0));
	srand(0);

	Game* game = new Game();

	game->Start();
	Global::TickTimer = SDL_GetTicks();
	Global::LogicTimeEnd = Global::TickTimer + Global::frameDelay;
	Global::RenderingTimeEnd = Global::TickTimer + 16;
	while (Global::status)
	{
		SDL_Delay(1);
		Global::TickTimer = SDL_GetTicks();
		if (Global::TickTimer > Global::LogicTimeEnd) {
			Global::LogicTimeEnd = Global::TickTimer + Global::frameDelay;
			game->LogicUpdate();
		}

		if (Global::TickTimer > Global::RenderingTimeEnd) {
			Global::RenderingTimeEnd = Global::TickTimer + 16;
			game->FrameUpdate();
		}
	}

	delete game;
	return 0;
}
