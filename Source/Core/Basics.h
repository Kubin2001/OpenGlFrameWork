#pragma once
#include "Renderer.h"

class GameObjectBasic {
	protected:
		MT::Rect rectangle;

	public:
		GameObjectBasic();

		GameObjectBasic(int x, int y, int w, int h);

		MT::Rect &GetRectangle();


		void Init(int x, int y, int w, int h);
};

class GameObject {
	protected:
		MT::Rect rectangle;
		MT::Texture* texture = nullptr;

	public:
		GameObject();

		GameObject(int x, int y, int w, int h, MT::Texture* texture);

		MT::Rect &GetRectangle();

		MT::Texture* GetTexture();

		void SetTexture(MT::Texture* texture);

		void Init(int x, int y, int w, int h, MT::Texture* texture);
};