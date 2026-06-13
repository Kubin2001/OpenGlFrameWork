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
		inline Camera(bool useBorders) {
			this->useBorders = useBorders;

		}

		inline float GetZoom() {
			return zoom;
		}

		inline MT::Rect& GetRectangle() {
			return rectangle;
		}

		inline void UseBorders(bool temp) {
			this->useBorders = temp;
		}

		inline void SetBorders(int minX, int maxX, int minY, int maxY) {
			this->useBorders = true;
			this->minX = minX;
			this->maxX = maxX;
			this->minY = minY;
			this->maxY = maxY;
		}

		inline void SetZoomValue(const float val) {
			zoomValue = val;
		}

		inline void SetMaxMinZoom(const float min, const float max) {
			zoomMin = min;
			zoomMax = max;
		}

		inline void Enable() {enabled = true;}

		inline void Disable() {enabled = false;}

		inline SDL_Rect TransformFlat(const SDL_Rect& rect) {
			return { rect.x - rectangle.x ,rect.y - rectangle.y ,rect.w,rect.h };
		}
		inline MT::Rect TransformFlat(const MT::Rect& rect) {
			return { rect.x - rectangle.x ,rect.y - rectangle.y ,rect.w,rect.h };
		}
		inline MT::RectF TransformFlat(const MT::RectF& rect) {
			return { (float)(rect.x - rectangle.x) ,(float)(rect.y - rectangle.y)
				,(float)(rect.w),(rect.h) };
		}
		inline MT::CompositeRect TransformFlat(const MT::CompositeRect& rect) {
			return { (float)(rect.x - rectangle.x) ,(float)(rect.y - rectangle.y) ,rect.w,rect.h };
		}

		inline SDL_Rect Transform(const SDL_Rect& rect) {
			SDL_Rect r{ 0, 0, static_cast<int>(rect.w * zoom) + 1,static_cast<int>(rect.h * zoom) + 1 }; // +1 to prevent white lines
			r.x = static_cast<int>((rect.x - rectangle.x) * zoom);
			r.y = static_cast<int>((rect.y - rectangle.y) * zoom);
			return r;
		}

		inline MT::Rect Transform(const MT::Rect& rect) {
			MT::Rect r{
				static_cast<int>((rect.x - static_cast<int>(rectangle.x)) * zoom),
				static_cast<int>((rect.y - static_cast<int>(rectangle.y)) * zoom),
				static_cast<int>(rect.w * zoom) + 1,
				static_cast<int>(rect.h * zoom) + 1
			};
			return r;
		}

		inline MT::RectF Transform(const MT::RectF& rect) {
			MT::RectF r{
				(rect.x - rectangle.x) * zoom,
				(rect.y - rectangle.y) * zoom,
				rect.w * zoom + 1.0f,
				rect.h * zoom + 1.0f
			};
			return r;
		}

		inline MT::CompositeRect Transform(const MT::CompositeRect& rect) {
			MT::CompositeRect r{
				(rect.x - rectangle.x) * zoom,
				(rect.y - rectangle.y) * zoom,
				static_cast<int>(rect.w * zoom) + 1,
				static_cast<int>(rect.h * zoom) + 1
			};
			return r;
		}

		inline Point UntransformFlat(int x, int y) {
			return { x + rectangle.x,y + rectangle.y };
		}

		inline Point Untransform(int x, int y) {
			return {
				static_cast<int>(x / zoom + rectangle.x),
				static_cast<int>(y / zoom + rectangle.y)
			};
		}

		inline Point GetMouseUntransform() {
			int x, y;
			SDL_GetMouseState(&x, &y);
			return {
			static_cast<int>(x / zoom + rectangle.x),
			static_cast<int>(y / zoom + rectangle.y)
			};
		}

		inline int GetScaledWidth() {
			return static_cast<int>(rectangle.w / zoom);
		}

		inline int GetScaledHeight() {
			return static_cast<int>(rectangle.h / zoom);
		}


		inline void SetMoveSpeed(const int temp) {
			this->moveSpeed = temp;
		}

		inline int& GetMoveSpeed() { return this->moveSpeed; }

		inline void Focus(const int x, const int y) {
			rectangle.x = x - (int)((rectangle.w / zoom) / 2);
			rectangle.y = y - (int)((rectangle.h / zoom) / 2);
		}

		void UpdatePosition(const Uint8* state);

		void UpdateZoom(SDL_Event& event);

};
