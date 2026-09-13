#include "Game.h"
#include "GlobalVariables.h"

int main(int argc, char* argv[]){
	Game game;
	game.Start();
	unsigned int ticks = SDL_GetTicks();
	unsigned int nextLogicTick = ticks;
	unsigned int nextFrameTick = ticks;
	while (Global::running){
		SDL_Delay(1);
		ticks = SDL_GetTicks();
		if (ticks >= nextLogicTick) {
			nextLogicTick = ticks + Global::logicDelay;
			game.LogicUpdate();
		}

		if (ticks >= nextFrameTick) {
			nextFrameTick = ticks + Global::frameDelay;
			game.FrameUpdate();
		}
	}
	return 0;
}
