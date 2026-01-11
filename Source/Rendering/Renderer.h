#pragma once

#include "glm.hpp"
#include <SDL.h>
#include <unordered_map>
#include <filesystem>
#include "ShaderLoader.h"
#include "Rectangle.h"

enum class ColorType {     
	White,      
	Black,     
	Red,       
	Green,     
	Blue,       
	Yellow,     
	Cyan,      
	Magenta,    
	Orange,     
	Purple,     
	Gray,       
	LightGray, 
	DarkGray    
};

namespace MT {

	SDL_GLContext Innit(SDL_Window* window);

	struct Color {
		unsigned char R = 0, G = 0, B = 0;

		Color() = default;

		Color(const unsigned char R, const unsigned char G, const unsigned char B) : R(R), G(G), B(B) {}

		Color(ColorType type);
	};

	struct ColorA {
		unsigned char R = 0, G = 0, B = 0, A = 255;

		ColorA() = default;

		ColorA(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A) : R(R), G(G), B(B), A(A) {}

		ColorA(ColorType type);
	};

	struct ColorF {
		float R = 0.0f, G = 0.0f, B = 0.0f;

		ColorF() = default;

		ColorF(const float R, const float G, float B) : R(R), G(G), B(B) {}
	};

	inline Color SetColor(ColorType type) {
		switch (type) {
		case ColorType::White:     return { 255, 255, 255 };
		case ColorType::Black:     return { 0,   0,   0 };
		case ColorType::Red:       return { 255,   0,   0 };
		case ColorType::Green:     return { 0, 255,   0 };
		case ColorType::Blue:      return { 0,   0, 255 };
		case ColorType::Yellow:    return { 255, 255,   0 };
		case ColorType::Cyan:      return { 0, 255, 255 };
		case ColorType::Magenta:   return { 255,   0, 255 };
		case ColorType::Orange:    return { 255, 165,   0 };
		case ColorType::Purple:    return { 128,   0, 128 };
		case ColorType::Gray:      return { 128, 128, 128 };
		case ColorType::LightGray: return { 211, 211, 211 };
		case ColorType::DarkGray:  return { 169, 169, 169 };
		default:                   return { 0,   0,   0 };
		}
	}

	// Linker goes crazy without inline
	inline Color::Color(ColorType type) {
		*this = SetColor(type);
	}

	inline ColorA::ColorA(ColorType type) {
		Color col = SetColor(type);
		R = col.R;
		G = col.G;
		B = col.B;
	}

	class Texture {
		private:
		unsigned int batchIndex = 0;
		float alpha = 1.0f;
		public:
		unsigned int w, h, texture;
		std::filesystem::file_time_type writeTime;

		void SetAlphaBending(const unsigned char A) {
			alpha = float(A) / 255;
		}
		friend class Renderer;
	};


	Texture* LoadTexture(const char* path);

	void DeleteTexture(Texture*& tex);

	Texture* LoadTextureFromSurface(SDL_Surface* surf);

	struct ConstextGuard {
		SDL_Window* window;
		SDL_GLContext context;

		ConstextGuard() {
			window = SDL_GL_GetCurrentWindow();
			context = SDL_GL_GetCurrentContext();
		}
		~ConstextGuard() {
			SDL_GL_MakeCurrent(window, context);
		}
	};


	struct FlatRenderLayer {
		unsigned int textureID = 0;

		std::vector<float> vertices = {};

		FlatRenderLayer(int texID) {
			this->textureID = texID;
		}
	};

	class Renderer {

		private:
		SDL_Window* window = nullptr;
		Rect vievPort;
		unsigned int VAO, VBO;
		ShaderLoader loader;
		//Shaders IDs
		unsigned int currentProgram;

		unsigned int renderBaseId = 0;
		unsigned int renderCopyId = 0;
		unsigned int flatRenderCopyId = 0;
		unsigned int renderRectId = 0;
		unsigned int renderCopyCircleId = 0;
		unsigned int renderCircleId = 0;
		unsigned int renderCopyFilterId = 0;
		unsigned int renderRoundedRectId = 0;
		unsigned int renderCopyRoundedRectId = 0;
		unsigned int renderBorderId = 0;
		unsigned int renderRoundedBorderId = 0;
		unsigned int uprId = 0;

		//Uniforms Ids
		unsigned int currentTexture = 0;
		unsigned int roundRectRadius = 0;
		unsigned int roundRectCopyRadius = 0;
		unsigned int roundBorderRadius = 0;
		unsigned int roundRoundedBorderRadius = 0;

		//Uniforms Values
		glm::vec2 roundRectRadiusVal = { 0.0f,0.0f };
		glm::vec2 roundRectCopyRadiusVal = { 0.0f,0.0f };
		glm::vec2 roundBorderRadiusVal = { 0.0f,0.0f };
		glm::vec2 roundRoundedBorderRadiusVal = { 0.0f,0.0f };


		//Veretex Sizes
		unsigned int currentSize = 0;
		unsigned int renderRectSize = 4; // Wszystkie renderowania bez tesktur
		unsigned int renderCopySize = 5; // Wszystkie renderowania tekstur
		unsigned int flatRenderCopySize = 2;
		unsigned int renderCopyBaseSize = 3;
		unsigned int renderCircleSize = 7;
		unsigned int renderCopyCircleSize = 6;
		unsigned int renderRoundedSize = 4;
		unsigned int renderCopyRoundedSize = 5;
		unsigned int renderFilteredSize = 8;
		unsigned int renderUPRSize = 8; 
		unsigned int renderBorderSize = 5; // Same for rounded border

		std::vector<float> globalVertices = {};

		//Agressive Batching Rendering
		std::vector<FlatRenderLayer> flatRenderVec = {};

		void LoadShaders();

		void ExpandUpr(float* vertices);

		public:
		int W, H;
		SDL_GLContext context;

		bool Start(SDL_Window* window, SDL_GLContext context);

		void ClearFrame(const unsigned char R, const unsigned char G, const unsigned char B);

		void RenderRect(const Rect& rect, const Color& col, const int alpha = 255);

		void RenderRectEX(const Rect& rect, const Color& col, const float rotation, const int alpha = 255);

		void DrawLine(const int x1, const int y1, const int x2, const int y2, const int thickness,
			const Color& col, const unsigned char alpha = 255);

		// Same as render copy but it uses faster shader but is extreamly slow when switching betweeen any other renderCopy functions
		void RenderCopyAS(const Rect& rect, const Texture* texture);

		void RenderCopy(const Rect& rect, const Texture* texture);

		void RenderCopyPart(const Rect& rect, const Rect& source, const Texture* texture);

		void RenderCopyEX(const Rect& rect, const Texture* texture, const bool flip = false, const float rotation = 0.0f);

		void RenderCopyPartEX(const Rect& rect, const Rect& source, const Texture* texture, const bool flip = false, const float rotation = 0.0f);

		void RenderCopyCircle(const Rect& rect, const Texture* texture, const float radius = 0.5f);

		void RenderCircle(const Rect& rect, const Color& col, const unsigned char alpha = 255, const float radius = 0.5f);

		void RenderRoundedRect(const Rect& rect, const Color& col, const unsigned char alpha = 255);

		void RenderCopyRounded(const Rect& rect, const Texture* texture);

		void RenderCopyFiltered(const Rect& rect, const Texture* texture, const Color& filter);

		void RenderCopyPartFiltered(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter);

		void RenderBorder(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255);

		void RenderRoundedBorder(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255);

		//UPR Universal Pipeline Render does not change shader ever so it is much faster in shader switch rendering but slower overall
		void RenderRectUPR(const Rect& rect, const Color& col, const int alpha = 255);

		void RenderRectEXUPR(const Rect& rect, const Color& col, const float rotation, const int alpha = 255);

		void RenderCopyUPR(const Rect& rect, const Texture* texture);

		void RenderCopyPartUPR(const Rect& rect, const Rect& source, const Texture* texture);

		void RenderCopyEXUPR(const Rect& rect, const Texture* texture, const bool flip = false, const float rotation = 0.0f);

		void RenderCopyPartEXUPR(const Rect& rect, const Rect& source, const Texture* texture, const bool flip = false, const float rotation = 0.0f);

		void RenderCopyCircleUPR(const Rect& rect, const Texture* texture, const float radius = 0.5f);

		void RenderCircleUPR(const Rect& rect, const Color& col, const unsigned char alpha = 255, const float radius = 0.5f);

		void RenderRoundedRectUPR(const Rect& rect, const Color& col, const unsigned char alpha = 255);

		void RenderCopyRoundedUPR(const Rect& rect, const Texture* texture);

		void RenderCopyFilteredUPR(const Rect& rect, const Texture* texture, const Color& filter);

		void RenderCopyPartFilteredUPR(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter);

		void RenderBorderUPR(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255);

		void RenderRoundedBorderUPR(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255);

		//UPR

		void Present(bool switchContext = true);

		void Clear();

		void Resize(const unsigned int w, const unsigned int h);

		//
		//
		// Experimental
		// Agressive Batching Rendering good for multiple texture tile rendering but does
		// not rememeber depth so new texture can be obscured by an old one 

		//Neds to be called at least once after texture load and after every texture quantity change
		void FLatRenderCopySetUp();

		void FLatRenderCopy(const Rect& rect, const Texture* texture);

		// Needs to be called after all flat operations are finisched 
		// NORMAL RENDER PRESENT WILL NOT DRAW ANYTHING !!!
		void FLatRenderCopyPresent(bool clearVectors = true);

		void SetClipSize(const Rect& rect);

		void ResetClipSize();
	};
}