#pragma once
#include "Addons.h"

#include <SDL.h>

class Camera {
	private:
		MT::Rect rectangle{ 0,0,Global::windowWidth,Global::windowHeight };
		float zoom = 1.0f; // Base zoom =1 (or no zoom)
		float zoomRelativeMoveSpeed = 1.0f;
		float zoomValue = 0.05f; // Since 0.05f does not give white lines between objects
		float zoomMin = 1.0f;
		float zoomMax = 0.25f;
		int moveSpeed = 3;
		bool enabled = true;

		bool useBorders = false;

		//Borders
		int minX = 0;
		int maxX = 0;
		int minY = 0;
		int maxY = 0;

	public:
		Camera(bool useBorders);

		float GetZoom();

		MT::Rect& GetRectangle();

		void UseBorders(bool temp);

		void SetBorders(int minX, int maxX, int minY, int maxY);

		void SetZoomValue(const float val);

		void SetMaxMinZoom(const float min, const float max);

		void UpdatePosition(const Uint8* state);

		void UpdateZoom(SDL_Event& event);

		void Enable();

		void Disable();

		SDL_Rect TransformFlat(const SDL_Rect& rect);
		MT::Rect TransformFlat(const MT::Rect& rect);
		MT::RectF TransformFlat(const MT::RectF& rect);
		MT::CompositeRect TransformFlat(const MT::CompositeRect& rect);

		SDL_Rect Transform(const SDL_Rect& rect);
		MT::Rect Transform(const MT::Rect& rect);
		MT::RectF Transform(const MT::RectF& rect);
		MT::CompositeRect Transform(const MT::CompositeRect& rect);

		Point UntransformFlat(int x, int y);
		Point Untransform(int x, int y);
		Point GetMouseUntransform();

		int GetScaledWidth();

		int GetScaledHeight();

		void SetMoveSpeed(const int temp);

		int& GetMoveSpeed();

		void Focus(const int x, const int y);
};
