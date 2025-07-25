#pragma once
#include <SDL.h>

class GameObjectBasic {
	protected:
		SDL_Rect rectangle;

	public:
		GameObjectBasic();

		GameObjectBasic(int x, int y, int w, int h);

		SDL_Rect* GetRectangle();


		void Innit(int x, int y, int w, int h);
};

class GameObject {
	protected:
		SDL_Rect rectangle;
		SDL_Texture* texture = nullptr;

	public:
		GameObject();

		GameObject(int x, int y, int w, int h, SDL_Texture* texture);

		SDL_Rect* GetRectangle();

		SDL_Texture* GetTexture();

		void SetTexture(SDL_Texture* texture);

		void Innit(int x, int y, int w, int h, SDL_Texture* texture);
};