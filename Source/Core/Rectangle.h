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

struct FPoint {
	float x = 0.0f;
	float y = 0.0;
	FPoint();
	FPoint(float x, float y);

	bool operator==(const FPoint& other) const {
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

		Point GetCenter() const noexcept;
		SDL_Rect ToSDLRect() const noexcept;
		RectF ToRectF() const noexcept;
		CompositeRect ToCompositeRect() const noexcept;

		bool IsColliding(const SDL_Rect& rect) const noexcept;
		bool IsColliding(const Rect& rect) const noexcept;
		bool IsColliding(const RectF& rect) const noexcept;
		bool IsColliding(const CompositeRect& rect) const noexcept;

		Rect& operator=(const SDL_Rect& rect) noexcept;
		Rect& operator=(const RectF& rect) noexcept;
		Rect& operator=(const CompositeRect& rect) noexcept;

		bool operator==(const SDL_Rect& rect) const noexcept;
		bool operator==(const Rect& rect) const noexcept;
		bool operator==(const RectF& rect) const noexcept;
		bool operator==(const CompositeRect& rect) const noexcept;
	};

	struct RectF {
		float x;
		float y;
		float w;
		float h;

		RectF();
		RectF(const float x, const float y, const float w, const float h);

		void Set(const float x, const float y, const float w, const float h) noexcept;

		Point GetCenter() const noexcept;
		SDL_Rect ToSDLRect() const noexcept;
		Rect ToRect() const noexcept;
		CompositeRect ToCompositeRect() const noexcept;

		bool IsColliding(const SDL_Rect& rect) const noexcept;
		bool IsColliding(const Rect& rect) const noexcept;
		bool IsColliding(const RectF& rect) const noexcept;
		bool IsColliding(const CompositeRect& rect) const noexcept;

		RectF& operator=(const SDL_Rect& rect) noexcept;
		RectF& operator=(const Rect& rect) noexcept;
		RectF& operator=(const CompositeRect& rect) noexcept;

		bool operator==(const SDL_Rect& rect) const noexcept;
		bool operator==(const Rect& rect) const noexcept;
		bool operator==(const RectF& rect) const noexcept;
		bool operator==(const CompositeRect& rect) const noexcept;
	};

	struct CompositeRect {
		float x;
		float y;
		int w;
		int h;

		CompositeRect();
		CompositeRect(const float x, const float y, const int w, const int h);

		void Set(const float x, const float y, const int w, const int h) noexcept;

		Point GetCenter()const noexcept;
		SDL_Rect ToSDLRect()const noexcept;
		RectF ToRectF()const noexcept;
		Rect ToRect()const noexcept;

		bool IsColliding(const SDL_Rect& rect) const noexcept;
		bool IsColliding(const Rect& rect) const noexcept;
		bool IsColliding(const RectF& rect) const noexcept;
		bool IsColliding(const CompositeRect& rect) const noexcept;

		CompositeRect& operator=(const SDL_Rect& rect) noexcept;
		CompositeRect& operator=(const RectF& rect) noexcept;
		CompositeRect& operator=(const Rect& rect) noexcept;

		bool operator==(const SDL_Rect& rect) const noexcept;
		bool operator==(const Rect& rect) const noexcept;
		bool operator==(const RectF& rect) const noexcept;
		bool operator==(const CompositeRect& rect) const noexcept;
	};

}
