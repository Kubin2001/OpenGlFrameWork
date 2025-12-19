#pragma once

#include "glm.hpp"
#include <SDL.h>
#include <unordered_map>
#include "ShaderLoader.h"
#include "Rectangle.h"

namespace MT {

	SDL_GLContext Innit(SDL_Window* window);

	struct Color {
		unsigned char R, G, B;

		Color() : R(0), G(0), B(0) {}

		Color(const unsigned char R, const unsigned char G, const unsigned char B) : R(R), G(G), B(B) {}
	};

	struct ColorA {
		unsigned char R, G, B, A;

		ColorA() : R(0), G(0), B(0), A(0) {}

		ColorA(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A) : R(R), G(G), B(B), A(A) {}
	};

	struct ColorF {
		float R, G, B;

		ColorF() : R(0), G(0), B(0) {}

		ColorF(const float R, const float G, float B) : R(R), G(G), B(B) {}
	};

	class Texture {
		private:
		unsigned int batchIndex = 0;
		public:
		unsigned int w, h, texture;
		float alpha = 1.0f;


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
		unsigned int uprId = 0;

		//Uniforms Ids
		unsigned int currentTexture = 0;
		unsigned int roundRectRadius = 0;
		unsigned int roundRectCopyRadius = 0;

		//Uniforms Values
		glm::vec2 roundRectRadiusVal = { 0.0f,0.0f };
		glm::vec2 roundRectCopyRadiusVal = { 0.0f,0.0f };


		//Veretex Sizes
		unsigned int currentSize = 0;
		unsigned int renderRectSize = 4; // Wszystkie renderowania bez tesktur
		unsigned int renderCopySize = 5; // Wszystkie renderowania tekstur
		unsigned int flatRenderCopySize = 2;
		unsigned int renderCopyBaseSize = 3;
		unsigned int renderCircleSize = 7;
		unsigned int renderCopyCircleSize = 6;
		unsigned int renderRoundedSize = 6;
		unsigned int renderCopyRoundedSize = 5;
		unsigned int renderFilteredSize = 8;
		unsigned int renderUPRSize = 8; // 9 is historical base size

		std::vector<float> globalVertices = {};

		//Agressive Batching Rendering
		std::vector<FlatRenderLayer> flatRenderVec = {};

		void LoadShaders();

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

		void RenderCopyRoundedRect(const Rect& rect, const Texture* texture);

		void RenderCopyFiltered(const Rect& rect, const Texture* texture, const Color& filter);

		void RenderCopyPartFiltered(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter);

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

		void RenderCopyRoundedRectUPR(const Rect& rect, const Texture* texture);

		void RenderCopyFilteredUPR(const Rect& rect, const Texture* texture, const Color& filter);

		void RenderCopyPartFilteredUPR(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter);
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