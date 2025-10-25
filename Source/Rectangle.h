#pragma once
#include <SDL_rect.h>

struct Point {
	int x = 0;
	int y = 0;
	Point();
	Point(int x, int y);

	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}
};

namespace MT {
	struct Rect;
	struct RectF;
	struct CompositeRect;

	struct Rect {
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;

		Rect();
		Rect(const int x, const int y, const int w, const int h);

		void Set(const int x, const int y, const int w, const int h) noexcept;

		Point GetCenter();
		SDL_Rect ToSDLRect();
		RectF ToRectF();
		CompositeRect ToCompositeRect();

		bool IsColliding(const SDL_Rect& rect) noexcept;
		bool IsColliding(const Rect& rect) noexcept;
		bool IsColliding(const RectF& rect) noexcept;
		bool IsColliding(const CompositeRect& rect) noexcept;

		Rect& operator=(const SDL_Rect& rect);
		Rect& operator=(const RectF& rect);
		Rect& operator=(const CompositeRect& rect);

		bool operator==(const SDL_Rect& rect) noexcept;
		bool operator==(const Rect& rect) noexcept;
		bool operator==(const RectF& rect) noexcept;
		bool operator==(const CompositeRect& rect) noexcept;
	};

	struct RectF {
		float x;
		float y;
		float w;
		float h;

		RectF();
		RectF(const float x, const float y, const float w, const float h);

		void Set(const float x, const float y, const float w, const float h) noexcept;

		Point GetCenter();
		SDL_Rect ToSDLRect();
		Rect ToRect();
		CompositeRect ToCompositeRect();

		bool IsColliding(const SDL_Rect& rect) noexcept;
		bool IsColliding(const Rect& rect) noexcept;
		bool IsColliding(const RectF& rect) noexcept;
		bool IsColliding(const CompositeRect& rect) noexcept;

		RectF& operator=(const SDL_Rect& rect);
		RectF& operator=(const Rect& rect);
		RectF& operator=(const CompositeRect& rect);

		bool operator==(const SDL_Rect& rect) noexcept;
		bool operator==(const Rect& rect) noexcept;
		bool operator==(const RectF& rect) noexcept;
		bool operator==(const CompositeRect& rect) noexcept;
	};

	struct CompositeRect {
		float x;
		float y;
		int w;
		int h;

		CompositeRect();
		CompositeRect(const float x, const float y, const int w, const int h);

		void Set(const float x, const float y, const int w, const int h) noexcept;

		Point GetCenter();
		SDL_Rect ToSDLRect();
		RectF ToRectF();
		Rect ToRect();

		bool IsColliding(const SDL_Rect& rect) noexcept;
		bool IsColliding(const Rect& rect) noexcept;
		bool IsColliding(const RectF& rect) noexcept;
		bool IsColliding(const CompositeRect& rect) noexcept;

		CompositeRect& operator=(const SDL_Rect& rect);
		CompositeRect& operator=(const RectF& rect);
		CompositeRect& operator=(const Rect& rect);

		bool operator==(const SDL_Rect& rect) noexcept;
		bool operator==(const Rect& rect) noexcept;
		bool operator==(const RectF& rect) noexcept;
		bool operator==(const CompositeRect& rect) noexcept;
	};

}
