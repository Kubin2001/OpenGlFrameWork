#pragma once

#include "Renderer.h"

namespace MT {
	// Base Object That Accept Any Rectangle As Template Type Everyting Is Public
	template <typename RectT = MT::Rect>
	class BaseObjectTex {
	public:
		static_assert(std::is_same_v < RectT, MT::Rect > || std::is_same_v<RectT, MT::RectF> || std::is_same_v<RectT, MT::CompositeRect>
			, "Wrong First Template Type");

		RectT rect{};
		MT::Texture* tex = nullptr;

	};

	// Base Object That Accept Any Rectangle As Template Type and any color as second one  Everyting Is Public
	template <typename RectT = MT::Rect, typename Color = MT::Color>
	class BaseObjectColor {
	public:
		static_assert(std::is_same_v < RectT, MT::Rect > || std::is_same_v<RectT, MT::RectF> || std::is_same_v<RectT, MT::CompositeRect>
			, "Wrong First Template Type");

		static_assert(std::is_same_v<Color, MT::Color> || std::is_same_v<Color, MT::ColorA> || 
			std::is_same_v<Color, MT::ColorF>, "Wrong Second Template Type");

		RectT rect{};
		Color color{};

	};

	// Base Object That Accept Any Rectangle As Template Type Everyting Is Protected
	template <typename RectT = MT::Rect>
	class BaseObjectTexPri {
	protected:
		static_assert(std::is_same_v < RectT, MT::Rect > || std::is_same_v<RectT, MT::RectF> || std::is_same_v<RectT, MT::CompositeRect>
			, "Wrong First Template Type");

		RectT rect{};
		MT::Texture* tex = nullptr;

	public:
		RectT& GetRect() { return rect; }

		MT::Texture*& GetTex() { return tex; }

		void SetTex(MT::Texture* tex) { this->tex = tex; }

	};


	// Base Object That Accept Any Rectangle As Template Type and any color as second one  Everyting Is Protected
	template <typename RectT = MT::Rect, typename Color = MT::Color>
	class BaseObjectColorPri {
	protected:
		static_assert(std::is_same_v < RectT, MT::Rect > || std::is_same_v<RectT, MT::RectF> || std::is_same_v<RectT, MT::CompositeRect>
			, "Wrong First Template Type");

		static_assert(std::is_same_v<Color, MT::Color> || std::is_same_v<Color, MT::ColorA> ||
			std::is_same_v<Color, MT::ColorF>, "Wrong Second Template Type");

		RectT rect{};
		Color color{};

	public:
		RectT& GetRect() { return rect; }

		Color& GetColor() { return color; }

		void SetColor(const Color& color) {
			this->color = color;
		}

	};
}