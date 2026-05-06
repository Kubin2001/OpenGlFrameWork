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

	SDL_GLContext Init(SDL_Window* window);

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
		unsigned int w = 0, h = 0, texture = 0;
		std::filesystem::file_time_type writeTime;

		Texture() = default;

		Texture(const Texture&) = delete;            
		Texture& operator=(const Texture&) = delete; 

		void SetAlphaBending(const unsigned char A) {
			alpha = float(A) / 255;
		}
		friend class Renderer;

		~Texture() {
			if (texture != 0) {
				glDeleteTextures(1, &texture);
			}
		}
	};


	Texture* LoadTexture(const char* path);


	Texture* LoadTextureFromSurface(SDL_Surface* surf);

	SDL_Surface* TextureToSurface(Texture* texture);

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
		Rect vievPort{0,0,0,0};
		unsigned int VAO = 0, VBO = 0;

		//Vaos
		GLuint rectVao = 0;
		GLuint rectExVao = 0;
		GLuint copyVao = 0;
		GLuint copyPartVao = 0;
		GLuint copyExVao = 0;
		GLuint copyCircleVao = 0;
		GLuint circleVao = 0;
		GLuint roundedVao = 0;
		GLuint roundedCopyVao = 0;
		GLuint filteredVao = 0;
		GLuint borderVao = 0;
		GLuint roundedBorderVao = 0;
		GLuint maskedVao = 0;
		GLuint doubleMaskedVao = 0;
		GLuint uprVao = 0;
		GLuint flatVao = 0;

		ShaderLoader loader;
		//Shaders IDs
		unsigned int currentProgram = 0;
		unsigned int renderRectId = 0;
		unsigned int renderRectExId = 0;
		unsigned int renderCopyId = 0;
		unsigned int renderCopyPartId = 0;
		unsigned int renderCopyExId = 0;
		unsigned int renderCopyCircleId = 0;
		unsigned int renderCircleId = 0;
		unsigned int renderRoundedId = 0;
		unsigned int renderCopyRoundedId = 0;
		unsigned int renderCopyFilterId = 0;
		unsigned int renderBorderId = 0;
		unsigned int renderRoundedBorderId = 0;
		unsigned int renderMaskedId = 0;
		unsigned int renderDoubleMaskedId = 0;

		unsigned int flatRenderCopyId = 0;
		unsigned int uprId = 0;

		//Uniforms Ids
		// VievPorts
		unsigned int renderRectVievPort = 0;
		unsigned int renderRectExVievPort = 0;
		unsigned int renderCopyVievPort = 0;
		unsigned int renderCopyPartVievPort = 0;
		unsigned int renderCopyExVievPort = 0;
		unsigned int renderCopyCircleVievPort = 0;
		unsigned int renderCircleVievPort = 0;
		unsigned int renderRoundedVievPort = 0;
		unsigned int renderCopyRoundedVievPort = 0;
		unsigned int renderFilterVievPort = 0;
		unsigned int renderBorderVievPort = 0;
		unsigned int renderRoundedBorderVievPort = 0;
		unsigned int renderMaskedVievPort = 0;
		unsigned int renderDoubleMaskedVievPort = 0;
		unsigned int uprVievPort = 0;
		unsigned int flatVievPort = 0;

		unsigned int currentTexture = 0;
		unsigned int currentMaskTexture = 0;

		//Veretex Sizes
		unsigned int currentSize = 0;
		inline static constexpr unsigned int renderRectSize = 6;
		inline static constexpr unsigned int renderRectExSize = 7;
		inline static constexpr unsigned int renderCopySize = 5;
		inline static constexpr unsigned int renderCopyPartSize = 9;
		inline static constexpr unsigned int renderCopyExSize = 10;
		inline static constexpr unsigned int renderCopyCircleSize = 6;
		inline static constexpr unsigned int renderCircleSize = 7;
		inline static constexpr unsigned int renderRoundedSize = 7;
		inline static constexpr unsigned int renderCopyRoundedSize = 6;
		inline static constexpr unsigned int renderFilteredSize = 10;
		inline static constexpr unsigned int renderBorderSize = 7;
		inline static constexpr unsigned int renderRoundedBorderSize = 7;
		inline static constexpr unsigned int renderMaskedSize = 9;
		inline static constexpr unsigned int renderDoubleMaskedSize = 13;
		inline static constexpr unsigned int UPRSize = 14;
		inline static constexpr unsigned int flatSize = 4;

		inline static constexpr unsigned int batchSize = 840'000; // ~3.3 MB
		size_t currentIndex = 0;
		std::vector<float> globalVertices = {};

		//Agressive Batching Rendering
		std::vector<FlatRenderLayer> flatRenderVec = {};

		void ExpandUpr(float* vertices);

		inline void CheckUPRProgram() {
			if (currentProgram != uprId) {
				Present(false);
				glBindVertexArray(uprVao);
				currentProgram = uprId;
				glUseProgram(currentProgram);
			}
			currentSize = UPRSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}
		}

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

		void RenderCopyEX(const Rect& rect, const Texture* texture, const bool flip = false, const float rotation = 0.0f) {
			const Rect fullSource = { 0, 0, static_cast<int>(texture->w), static_cast<int>(texture->h) };
			RenderCopyEX(rect, fullSource, texture, flip, rotation);
		}

		void RenderCopyEX(const Rect& rect, const Rect& source, const Texture* texture, const bool flip = false, const float rotation = 0.0f);

		void RenderCopyCircle(const Rect& rect, const Texture* texture, const float radius = 0.5f);

		void RenderCircle(const Rect& rect, const Color& col, const unsigned char alpha = 255, const float radius = 0.5f);

		void RenderRoundedRect(const Rect& rect, const Color& col, const unsigned char alpha = 255, int roundingSize = 8);

		void RenderCopyRounded(const Rect& rect, const Texture* texture, int roundingSize = 8);

		void RenderCopyFiltered(const Rect& rect, const Texture* texture, const Color& filter) {
			const Rect fullSource = { 0, 0, static_cast<int>(texture->w), static_cast<int>(texture->h) };
			RenderCopyFiltered(rect, fullSource, texture, filter);
		}

		void RenderCopyFiltered(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter);

		void RenderBorder(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255);

		void RenderRoundedBorder(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255);

		void RenderMaskedOverlay(const Rect& rect, const Rect &source, const Texture* tex1, const Texture* tex2);

		void RenderDoubleMaskedOverlay(const Rect& rect, const Rect& source, const Rect& source2, const Texture* tex1, const Texture* tex2);

		//UPR Universal Pipeline Render does not change shader ever so it is much faster in shader switch rendering but slower overall
		void RenderRectUPR(const Rect& rect, const Color& col, const int alpha = 255);

		void RenderRectEXUPR(const Rect& rect, const Color& col, const float rotation, const int alpha = 255);

		void DrawLineUPR(const int x1, const int y1, const int x2, const int y2, const int thickness,
			const Color& col, const unsigned char alpha = 255);

		void RenderCopyUPR(const Rect& rect, const Texture* texture);

		void RenderCopyPartUPR(const Rect& rect, const Rect& source, const Texture* texture);

		void RenderCopyEXUPR(const Rect& rect, const Texture* texture, const bool flip = false, const float rotation = 0.0f) {
			const Rect fullSource = { 0, 0, static_cast<int>(texture->w), static_cast<int>(texture->h) };
			RenderCopyEXUPR(rect, fullSource, texture, flip, rotation);
		}

		void RenderCopyEXUPR(const Rect& rect, const Rect& source, const Texture* texture, const bool flip = false, const float rotation = 0.0f);

		void RenderCopyCircleUPR(const Rect& rect, const Texture* texture, const float radius = 0.5f);

		void RenderCircleUPR(const Rect& rect, const Color& col, const unsigned char alpha = 255, const float radius = 0.5f);

		void RenderRoundedRectUPR(const Rect& rect, const Color& col, const unsigned char alpha = 255, int roundingSize = 8);

		void RenderCopyRoundedUPR(const Rect& rect, const Texture* texture, int roundingSize = 8);

		void RenderCopyFilteredUPR(const Rect& rect, const Texture* texture, const Color& filter) {
			const Rect fullSource = { 0, 0, static_cast<int>(texture->w), static_cast<int>(texture->h) };
			RenderCopyFilteredUPR(rect, fullSource, texture, filter);
		}

		void RenderCopyFilteredUPR(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter);

		void RenderBorderUPR(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255);

		void RenderRoundedBorderUPR(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255);

		void RenderMaskedOverlayUPR(const Rect& rect, const Rect &source, const Texture* tex1, const Texture* tex2);

		void RenderDoubleMaskedOverlayUPR(const Rect& rect, const Rect& source, const Rect& source2, const Texture* tex1, const Texture* tex2);

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
		void FLatRenderCopyPresent();

		void SetClipSize(const Rect& rect);

		void ResetClipSize();
	};
}