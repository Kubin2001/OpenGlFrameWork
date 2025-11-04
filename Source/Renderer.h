#pragma once

#include "glm.hpp"
#include <SDL.h>

#include "ShaderLoader.h"
#include "Rectangle.h"

namespace MT {

	SDL_GLContext Innit(SDL_Window* window);

	struct Color {
		unsigned char R, G, B;

		Color() : R(0), G(0), B(0) {}

		Color(const unsigned char R, const unsigned char G, const unsigned char B) : R(R), G(G), B(B) {}
	};

	struct ColorF {
		float R, G, B;

		ColorF() : R(0), G(0), B(0) {}

		ColorF(const float R, const float G, float B) : R(R), G(G), B(B) {}
	};

	class Texture {
		public:
		unsigned int w, h, texture;
		float alpha = 1.0f;

		void SetAlphaBending(const unsigned char A) {
			alpha = float(A) / 255;
		}
	};


	Texture* LoadTexture(const char* path);

	void DeleteTexture(Texture *tex);

	Texture* LoadTextureFromSurface(SDL_Surface *surf);
	
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


	class Renderer {

		private:
			 SDL_Window* window = nullptr;
			 Rect vievPort;
			 unsigned int VAO, VBO;
			 ShaderLoader loader;
			 //Shaders IDs
			 unsigned int currentProgram;
			 
			 unsigned int renderCopyId;
			 unsigned int renderRectId;
			 unsigned int renderCopyCircleId;
			 unsigned int renderCircleId;
			 unsigned int renderCopyFilterId;
			 unsigned int renderRoundedRectId;
			 unsigned int renderCopyRoundedRectId;

			 //Uniforms Ids
			 unsigned int currentTexture;
			 unsigned int roundRectRadius;
			 unsigned int roundRectCopyRadius;

			 //Uniforms Values
			 glm::vec2 roundRectRadiusVal = { 0.0f,0.0f };
			 glm::vec2 roundRectCopyRadiusVal = { 0.0f,0.0f };
		

			 //Veretex Sizes
			 unsigned int currentSize = 0;
			 unsigned int renderRectSize = 6; // Wszystkie renderowania bez tesktur
			 unsigned int renderCopySize = 5; // Wszystkie renderowania tekstur
			 unsigned int renderCircleSize = 7;
			 unsigned int renderCopyCircleSize = 6;
			 unsigned int renderRoundedSize = 6;
			 unsigned int renderCopyRoundedSize = 5;
			 unsigned int renderFilteredSize = 8;

			 std::vector<float> globalVertices;

			 //Agressive Batching Rendering
			 std::vector<std::vector<float>> agresiveRenderVec = {};

		public:
			 int W, H;
			 SDL_GLContext context;

			 bool Start(SDL_Window* window, SDL_GLContext context);

			 void ClearFrame(const unsigned char R, const unsigned char G, const unsigned char B);

			 void RenderRect(const Rect& rect, const Color& col, const int alpha = 255);

			 void RenderRectEX(const Rect& rect, const Color& col, const float rotation, const int alpha = 255);

			 void DrawLine(const int x1, const int y1, const int x2, const int y2, const int thickness,
				 const Color& col, const unsigned char alpha = 255);

			 void RenderCopy(const Rect& rect, const Texture* texture);

			 void RenderCopyPart(const Rect& rect, const Rect& source, const Texture* texture);

			 void RenderCopyEX(const Rect& rect, const Texture* texture, const bool flip = false, const float rotation = 0.0f);

			 void RenderCopyPartEX(const Rect& rect, const Rect& source, const Texture* texture, const bool flip = false, const float rotation = 0.0f);

			 void RenderCopyCircle(const Rect& rect, const Texture* texture, const float radius = 0.5f);

			 void RenderCircle(const Rect& rect, const Color& col, const unsigned char alpha = 255, const float radius = 0.5f);

			 void RenderRoundedRect(const Rect& rect, const Color& col, const unsigned char alpha);

			 void RenderCopyRoundedRect(const Rect& rect, const Texture* texture);

			 void RenderCopyFiltered(const Rect& rect, const Texture* texture, const Color& filter);

			 void RenderCopyPartFiltered(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter);

			 void RenderPresent(bool switchContext = true);

			 void Clear();

			 void Resize(const unsigned int w, const unsigned int h);

			 //
			 //
			 // Experimental
			 // Agressive Batching Rendering good for multiple texture tile rendering but does
			 // not rememeber deepth so new texture can be obscured by an old one 
			 // Might break or crash not recomemnded in fincisched product

			 //Neds to be called at least once after texture load and after every texture quantity change
			 void AgresiveRenderCopySetUp();

			 void AgressiveRenderCopy(const Rect& rect, const Texture* texture);

			 void AgressiveRenderCopyPresent(bool clearVectors = true);

			 void SetClipSize(const Rect& rect);

			 void ResetClipSize();
	};
}

