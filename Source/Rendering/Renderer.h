#pragma once

#include <SDL.h>
#include <unordered_map>
#include <filesystem>
#include <memory>

#include "glm.hpp"

#include "ShaderLoader.h"
#include "Rectangle.h"
#include "Window.h"


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


	Texture* GenEmptyTexture(int w, int h);

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

	class RenderTarget {
		unsigned int FBO = 0;
		MT::Texture* tex = nullptr;

		public: 

		RenderTarget(int W, int H) {
			if (W > 4096 || H > 4096 || W < 1 || H < 1) {
				throw std::runtime_error("Render Target size is not between 1 and 4096");
			}
			glGenFramebuffers(1 ,&FBO);

			tex = MT::GenEmptyTexture(W, H);

			glBindFramebuffer(GL_FRAMEBUFFER, FBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->texture, 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				throw std::runtime_error("Framebuffer is not complete");
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Resize(int W, int H) {
			if (W > 4096 || H > 4096 || W < 1 || H < 1) {
				throw std::runtime_error("Render Target size is not between 1 and 4096");
			}
			delete tex;
			tex = MT::GenEmptyTexture(W, H);

			glBindFramebuffer(GL_FRAMEBUFFER, FBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->texture, 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				throw std::runtime_error("Framebuffer is not complete");
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Bind() {
			glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		}
		void UnBind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		MT::Texture* GetTexture() const { return tex; }

		RenderTarget(const RenderTarget& rt) = delete;
		RenderTarget& operator = (const RenderTarget& rt) = delete;

		RenderTarget(RenderTarget&& rt) noexcept{
			if (this == &rt) {
				return;
			}
			this->FBO = rt.FBO;
			this->tex = rt.tex;
			rt.tex = nullptr;
			rt.FBO = 0;

		}

		RenderTarget& operator = (RenderTarget&& rt) noexcept{
			if (this == &rt) {
				return *this;
			}
			this->FBO = rt.FBO;
			this->tex = rt.tex;
			rt.tex = nullptr;
			rt.FBO = 0;
			return *this;
		}

		~RenderTarget() {
			if (FBO != 0) { 
				glDeleteFramebuffers(1,&FBO);
			}
			delete tex;
		}
	};

	class Renderer {

		private:
		SDL_Window* window  = nullptr;
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
		GLuint shapeVao = 0;

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
		unsigned int renderShapeId = 0;

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
		unsigned int renderShapeVievPort = 0;
		unsigned int uprVievPort = 0;
		unsigned int flatVievPort = 0;

		unsigned int currentTexture = 0;
		unsigned int currentMaskTexture = 0;

		//Veretex Sizes
		unsigned int currentSize = 0;
		inline static constexpr unsigned int renderRectSize = 6;
		inline static constexpr unsigned int renderRectExSize = 9;
		inline static constexpr unsigned int renderCopySize = 5;
		inline static constexpr unsigned int renderCopyPartSize = 9;
		inline static constexpr unsigned int renderCopyExSize = 12;
		inline static constexpr unsigned int renderCopyCircleSize = 6;
		inline static constexpr unsigned int renderCircleSize = 7;
		inline static constexpr unsigned int renderRoundedSize = 7;
		inline static constexpr unsigned int renderCopyRoundedSize = 6;
		inline static constexpr unsigned int renderFilteredSize = 10;
		inline static constexpr unsigned int renderBorderSize = 7;
		inline static constexpr unsigned int renderRoundedBorderSize = 8;
		inline static constexpr unsigned int renderMaskedSize = 9;
		inline static constexpr unsigned int renderDoubleMaskedSize = 13;
		inline static constexpr unsigned int renderShapeSize = 6;
		inline static constexpr unsigned int UPRSize = 14;
		inline static constexpr unsigned int flatSize = 4;

		inline static constexpr unsigned int batchSize = 840'000; // ~3.3 MB
		size_t currentIndex = 0;
		std::vector<float> globalVertices = {};

		//Agressive Batching Rendering
		std::vector<FlatRenderLayer> flatRenderVec = {};

		//Single frame buffer in future could be improved but for now it is more than sufficient
		std::unique_ptr<RenderTarget> renderTarget = nullptr;

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
		int W = 0, H = 0;

		bool Start(const MT::Window& mtWindow);

		inline void Present(bool switchContext = true) {
			if (currentIndex == 0) {
				if (switchContext) { SDL_GL_SwapWindow(window); }
				return;
			}
			float* start = &globalVertices[0];
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, currentIndex * sizeof(float), start, GL_DYNAMIC_DRAW);

			const GLsizei instanceCount = static_cast<GLsizei>(currentIndex / currentSize);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);

			currentIndex = 0;

			if (switchContext) { SDL_GL_SwapWindow(window); }
		}

		inline void ClearFrame(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A = 255) {
			const float fR = float(R) / 255;
			const float fG = float(G) / 255;
			const float fB = float(B) / 255;
			const float fA = float(A) / 255;
			glClearColor(fR, fG, fB, fA);
			glClear(GL_COLOR_BUFFER_BIT);
		}

		inline void RenderRect(const Rect& rect, const Color& col, const unsigned char alpha = 255) {
			if (!vievPort.IsColliding(rect)) {
				return;
			}
			if (currentProgram != renderRectId) {
				Present(false);
				glBindVertexArray(rectVao);
				currentProgram = renderRectId;
				glUseProgram(renderRectId);
			}
			currentSize = Renderer::renderRectSize;

			uint16_t iRG = col.R;
			iRG <<= 8;
			iRG += col.G;
			uint16_t iBA = col.B;
			iBA <<= 8;
			iBA += alpha;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(iRG);
			ptr[5] = static_cast<float>(iBA);

			currentIndex += currentSize;
		}

		inline void RenderRectEX(const Rect& rect, const Color& col, const float rotation, std::optional<Point> rotCenter, const unsigned char alpha = 255) {
			if (!vievPort.IsColliding(rect)) {
				return;
			}

			if (currentProgram != renderRectExId) {
				Present(false);
				currentProgram = renderRectExId;
				glUseProgram(currentProgram);
				glBindVertexArray(rectExVao);
			}

			currentSize = Renderer::renderRectExSize;

			uint16_t iRG = col.R;
			iRG <<= 8;
			iRG += col.G;
			uint16_t iBA = col.B;
			iBA <<= 8;
			iBA += alpha;

			float centerX = 0;
			float centerY = 0;

			if (!rotCenter) {
				centerX = static_cast<float>(rect.x + (rect.w >> 1));
				centerY = static_cast<float>(rect.y + (rect.h >> 1));
			}
			else {
				centerX = static_cast<float>(rotCenter->x);
				centerY = static_cast<float>(rotCenter->y);
			}

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(iRG);
			ptr[5] = static_cast<float>(iBA);
			ptr[6] = rotation;
			ptr[7] = centerX;
			ptr[8] = centerY;

			currentIndex += currentSize;
		}

		inline void DrawLine(const Point &p1, const Point& p2, const int thickness,
			const Color& col, const unsigned char alpha = 255) {
				if (currentProgram != renderRectExId) {
					Present(false);
					currentProgram = renderRectExId;
					glUseProgram(currentProgram);
					glBindVertexArray(rectExVao);
				}

				currentSize = Renderer::renderRectExSize;

				const float fx1 = static_cast<float>(p1.x);
				const float fy1 = static_cast<float>(p1.y);
				const float fx2 = static_cast<float>(p2.x);
				const float fy2 = static_cast<float>(p2.y);

				const float dx = fx2 - fx1;
				const float dy = fy2 - fy1;
				const float w = std::sqrt(dx * dx + dy * dy);

				if (w < 1e-4f) { return; }

				const float radRot = std::atan2(dy, dx);
				const float rotation = glm::degrees(radRot);

				const float centerX = (fx1 + fx2) * 0.5f;
				const float centerY = (fy1 + fy2) * 0.5f;
				const float h = static_cast<float>(thickness);

				const float rectX = centerX - (w * 0.5f);
				const float rectY = centerY - (h * 0.5f);

				uint16_t iRG = col.R;
				iRG <<= 8;
				iRG += col.G;
				uint16_t iBA = col.B;
				iBA <<= 8;
				iBA += alpha;

				if (currentIndex + currentSize > Renderer::batchSize) {
					Present(false);
				}

				float* ptr = globalVertices.data() + currentIndex;
				ptr[0] = rectX;
				ptr[1] = rectY;
				ptr[2] = w;
				ptr[3] = h;
				ptr[4] = static_cast<float>(iRG);
				ptr[5] = static_cast<float>(iBA);
				ptr[6] = rotation;
				ptr[7] = centerX;
				ptr[8] = centerY;

				currentIndex += currentSize;
		}

		inline void RenderCopy(const Rect& rect, const Texture* texture) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			if (currentProgram != renderCopyId) {
				Present(false);
				currentProgram = renderCopyId;
				glUseProgram(currentProgram);
				glBindVertexArray(copyVao);
			}
			currentSize = renderCopySize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(texture->alpha);

			currentIndex += currentSize;
		}

		inline void RenderCopyPart(const Rect& rect, const Rect& source, const Texture* texture) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			if (currentProgram != renderCopyPartId) {
				Present(false);
				glBindVertexArray(copyPartVao);
				currentProgram = renderCopyPartId;
				glUseProgram(currentProgram);
			}

			const float sourceX = static_cast<float>(source.x) / texture->w;
			const float sourceY = static_cast<float>(source.y) / texture->h;
			const float sourceW = static_cast<float>(source.w) / texture->w;
			const float sourceH = static_cast<float>(source.h) / texture->h;

			currentSize = renderCopyPartSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = sourceX;
			ptr[5] = sourceY;
			ptr[6] = sourceW;
			ptr[7] = sourceH;
			ptr[8] = texture->alpha;

			currentIndex += currentSize;
		}

		template<bool texNullCheck = true>
		inline void RenderCopyEX(const Rect& rect, const Rect& source, const Texture* texture, const float rotation = 0.0f, const bool flip = false, std::optional<Point> rotCenter = std::nullopt) {
			if constexpr (texNullCheck) {
				if (!texture) { return; }
			}
			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			if (currentProgram != renderCopyExId) {
				Present(false);
				glBindVertexArray(copyExVao);
				currentProgram = renderCopyExId;
				glUseProgram(currentProgram);
			}
			currentSize = renderCopyExSize;

			const float sourceX = static_cast<float>(source.x) / texture->w;
			const float sourceY = static_cast<float>(source.y) / texture->h;
			const float sourceW = static_cast<float>(source.w) / texture->w;
			const float sourceH = static_cast<float>(source.h) / texture->h;

			float centerX = 0.0f;
			float centerY = 0.0f;
			if (!rotCenter) {
				centerX = static_cast<float>(rect.x + (rect.w >> 1));
				centerY = static_cast<float>(rect.y + (rect.h >> 1));
			}
			else {
				centerX = static_cast<float>(rotCenter->x);
				centerY = static_cast<float>(rotCenter->y);
			}

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			if (flip) {
				ptr[4] = sourceX + sourceW;
				ptr[6] = -sourceW;
			}
			else {
				ptr[4] = sourceX;
				ptr[6] = sourceW;
			}
			ptr[5] = sourceY;
			ptr[7] = sourceH;
			ptr[8] = texture->alpha;
			ptr[9] = rotation;
			ptr[10] = centerX;
			ptr[11] = centerY;

			currentIndex += currentSize;
		}

		inline void RenderCopyEX(const Rect& rect, const Texture* texture, const float rotation = 0.0f, const bool flip = false, std::optional<Point> rotCenter = std::nullopt) {
			if (!texture) { return; }
			const Rect fullSource = { 0, 0, static_cast<int>(texture->w), static_cast<int>(texture->h) };
			RenderCopyEX<false>(rect, fullSource, texture, rotation, flip, rotCenter);
		}

		inline void RenderCopyCircle(const Rect& rect, const Texture* texture, const float radius = 0.5f) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			if (currentProgram != renderCopyCircleId) {
				Present(false);
				glBindVertexArray(copyCircleVao);
				currentProgram = renderCopyCircleId;
				glUseProgram(currentProgram);
			}


			currentSize = renderCopyCircleSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = radius;
			ptr[5] = texture->alpha;

			currentIndex += currentSize;
		}

		inline void RenderCircle(const Rect& rect, const Color& col, const unsigned char alpha = 255, const float radius = 0.5f) {
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentProgram != renderCircleId) {
				Present(false);
				glBindVertexArray(circleVao);
				currentProgram = renderCircleId;
				glUseProgram(currentProgram);
			}

			uint16_t iRG = col.R;
			iRG <<= 8;
			iRG += col.G;
			uint16_t iBA = col.B;
			iBA <<= 8;
			iBA += alpha;
			const float fRG = iRG;
			const float fBA = iBA;

			currentSize = renderCircleSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = radius;
			ptr[5] = fRG;
			ptr[6] = fBA;

			currentIndex += currentSize;
		}

		inline void RenderRoundedRect(const Rect& rect, const Color& col, const unsigned char alpha = 255, int roundingSize = 8) {
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentProgram != renderRoundedId) {
				Present(false);
				glBindVertexArray(roundedVao);
				currentProgram = renderRoundedId;
				glUseProgram(currentProgram);
			}


			uint16_t iRG = col.R;
			iRG <<= 8;
			iRG += col.G;
			uint16_t iBA = col.B;
			iBA <<= 8;
			iBA += alpha;
			const float fRG = iRG;
			const float fBA = iBA;

			currentSize = renderRoundedSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = fRG;
			ptr[5] = fBA;
			ptr[6] = static_cast<float>(roundingSize);

			currentIndex += currentSize;
		}

		inline void RenderCopyRounded(const Rect& rect, const Texture* texture, int roundingSize = 8) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentProgram != renderCopyRoundedId) {
				Present(false);
				glBindVertexArray(roundedCopyVao);
				currentProgram = renderCopyRoundedId;
				glUseProgram(currentProgram);
			}
			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			currentSize = renderCopyRoundedSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = texture->alpha;
			ptr[5] = static_cast<float>(roundingSize);

			currentIndex += currentSize;
		}

		template<bool texNullCheck = true>
		inline void RenderCopyFiltered(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter) {
			if constexpr(texNullCheck) {
				if (!texture) { return; }
			}

			if (!vievPort.IsColliding(rect)) { return; }

			if (currentProgram != renderCopyFilterId) {
				Present(false);
				glBindVertexArray(filteredVao);
				currentProgram = renderCopyFilterId;
				glUseProgram(renderCopyFilterId);
			}

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}


			uint16_t iRG = filter.R;
			iRG <<= 8;
			iRG += filter.G;
			uint16_t iBA = filter.B;
			iBA <<= 8;
			iBA += static_cast<unsigned char>(texture->alpha * 255);

			currentSize = renderFilteredSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			const float sourceX = static_cast<float>(source.x) / texture->w;
			const float sourceY = static_cast<float>(source.y) / texture->h;
			const float sourceW = static_cast<float>(source.w) / texture->w;
			const float sourceH = static_cast<float>(source.h) / texture->h;

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = sourceX;
			ptr[5] = sourceY;
			ptr[6] = sourceW;
			ptr[7] = sourceH;
			ptr[8] = static_cast<float>(iRG);
			ptr[9] = static_cast<float>(iBA);

			currentIndex += currentSize;
		}

		inline void RenderCopyFiltered(const Rect& rect, const Texture* texture, const Color& filter) {
			if (!texture) { return; }
			const Rect fullSource = { 0, 0, static_cast<int>(texture->w), static_cast<int>(texture->h) };
			RenderCopyFiltered<false>(rect, fullSource, texture, filter);
		}

		inline void RenderBorder(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255) {
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentProgram != renderBorderId) {
				Present(false);
				glBindVertexArray(borderVao);
				currentProgram = renderBorderId;
				glUseProgram(currentProgram);
			}

			uint16_t iRG = col.R;
			iRG <<= 8;
			iRG += col.G;
			uint16_t iBA = col.B;
			iBA <<= 8;
			iBA += alpha;

			currentSize = renderBorderSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(iRG);
			ptr[5] = static_cast<float>(iBA);
			ptr[6] = static_cast<float>(width);

			currentIndex += currentSize;
		}

		inline void RenderRoundedBorder(const Rect& rect, const Color& col, const int width,  const int roundingSize = 8, const unsigned char alpha = 255) {
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentProgram != renderRoundedBorderId) {
				Present(false);
				glBindVertexArray(roundedBorderVao);
				currentProgram = renderRoundedBorderId;
				glUseProgram(currentProgram);
			}

			uint16_t iRG = col.R;
			iRG <<= 8;
			iRG += col.G;
			uint16_t iBA = col.B;
			iBA <<= 8;
			iBA += alpha;

			currentSize = renderRoundedBorderSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(iRG);
			ptr[5] = static_cast<float>(iBA);
			ptr[6] = static_cast<float>(width);
			ptr[7] = static_cast<float>(roundingSize);

			currentIndex += currentSize;
		}

		inline void RenderMaskedOverlay(const Rect& rect, const Rect& source, const Texture* tex1, const Texture* tex2) {
			if (!tex1 || !tex2) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentTexture != tex1->texture) {
				Present(false);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex1->texture);
				currentTexture = tex1->texture;
			}
			if (currentMaskTexture != tex2->texture) {
				Present(false);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, tex2->texture);
				currentMaskTexture = tex2->texture;
			}

			if (currentProgram != renderMaskedId) {
				Present(false);
				glBindVertexArray(maskedVao);
				currentProgram = renderMaskedId;
				glUseProgram(currentProgram);
			}

			const float sourceX = static_cast<float>(source.x) / tex1->w;
			const float sourceY = static_cast<float>(source.y) / tex1->h;
			const float sourceW = static_cast<float>(source.w) / tex1->w;
			const float sourceH = static_cast<float>(source.h) / tex1->h;

			currentSize = renderMaskedSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = sourceX;
			ptr[5] = sourceY;
			ptr[6] = sourceW;
			ptr[7] = sourceH;
			ptr[8] = tex1->alpha;

			currentIndex += currentSize;

			glActiveTexture(GL_TEXTURE0);
		}

		inline void RenderDoubleMaskedOverlay(const Rect& rect, const Rect& source, const Rect& source2, const Texture* tex1, const Texture* tex2) {
			if (!tex1 || !tex2) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			if (currentTexture != tex1->texture) {
				Present(false);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex1->texture);
				currentTexture = tex1->texture;
			}
			if (currentMaskTexture != tex2->texture) {
				Present(false);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, tex2->texture);
				currentMaskTexture = tex2->texture;
			}

			if (currentProgram != renderDoubleMaskedId) {
				Present(false);
				glBindVertexArray(doubleMaskedVao);
				currentProgram = renderDoubleMaskedId;
				glUseProgram(currentProgram);
			}

			const float sourceX = static_cast<float>(source.x) / tex1->w;
			const float sourceY = static_cast<float>(source.y) / tex1->h;
			const float sourceW = static_cast<float>(source.w) / tex1->w;
			const float sourceH = static_cast<float>(source.h) / tex1->h;

			const float source2X = static_cast<float>(source2.x) / tex2->w;
			const float source2Y = static_cast<float>(source2.y) / tex2->h;
			const float source2W = static_cast<float>(source2.w) / tex2->w;
			const float source2H = static_cast<float>(source2.h) / tex2->h;

			currentSize = renderDoubleMaskedSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = sourceX;
			ptr[5] = sourceY;
			ptr[6] = sourceW;
			ptr[7] = sourceH;
			ptr[8] = source2X;
			ptr[9] = source2Y;
			ptr[10] = source2W;
			ptr[11] = source2H;
			ptr[12] = tex1->alpha;

			currentIndex += currentSize;

			glActiveTexture(GL_TEXTURE0);
		}

		inline void RenderShape(const Rect& rect, Texture* texture, const ColorA& filter) {
			if (!texture) { return; }

			if (!vievPort.IsColliding(rect)) { return; }

			if (currentProgram != renderShapeId) {
				Present(false);
				glBindVertexArray(shapeVao);
				currentProgram = renderShapeId;
				glUseProgram(currentProgram);
			}

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}


			uint16_t iRG = filter.R;
			iRG <<= 8;
			iRG += filter.G;
			uint16_t iBA = filter.B;
			iBA <<= 8;
			iBA += filter.A;

			currentSize = renderShapeSize;

			if (currentIndex + currentSize > Renderer::batchSize) {
				Present(false);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(iRG);
			ptr[5] = static_cast<float>(iBA);

			currentIndex += currentSize;
		}

		//UPR Universal Pipeline Render does not change shader ever so it is much faster in shader switch rendering but slower overall
		inline void RenderRectUPR(const Rect& rect, const Color& col, const unsigned char alpha = 255) {
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(col.R); ptr[5] = static_cast<float>(col.G);
			ptr[6] = static_cast<float>(col.B); ptr[7] = static_cast<float>(alpha);
			ptr[8] = 0.0f; ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 0.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderRectEXUPR(const Rect& rect, const Color& col, const float rotation, std::optional<Point> rotCenter,
			const unsigned char alpha = 255) {
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			float centerX = 0.0f;
			float centerY = 0.0f;
			if (!rotCenter) {
				centerX = static_cast<float>(rect.x + (rect.w >> 1));
				centerY = static_cast<float>(rect.y + (rect.h >> 1));
			}
			else {
				centerX = static_cast<float>(rotCenter->x);
				centerY = static_cast<float>(rotCenter->y);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(col.R); ptr[5] = static_cast<float>(col.G);
			ptr[6] = static_cast<float>(col.B); ptr[7] = static_cast<float>(alpha);
			ptr[8] = rotation; 
			ptr[9] = centerX; ptr[10] = centerY; 
			ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 1.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void DrawLineUPR(const Point &p1, const Point& p2, const int thickness,
			const Color& col, const unsigned char alpha = 255) {
			CheckUPRProgram();

			const float fx1 = static_cast<float>(p1.x);
			const float fy1 = static_cast<float>(p1.y);
			const float fx2 = static_cast<float>(p2.x);
			const float fy2 = static_cast<float>(p2.y);

			const float dx = fx2 - fx1;
			const float dy = fy2 - fy1;
			const float w = std::sqrt(dx * dx + dy * dy);

			if (w < 1e-4f) { return; }

			const float radRot = std::atan2(dy, dx);
			const float rotation = glm::degrees(radRot);

			const float centerX = (fx1 + fx2) * 0.5f;
			const float centerY = (fy1 + fy2) * 0.5f;
			const float h = static_cast<float>(thickness);

			const float rectX = centerX - (w * 0.5f);
			const float rectY = centerY - (h * 0.5f);

			float* ptr = globalVertices.data() + currentIndex;
			ptr[0] = rectX;
			ptr[1] = rectY;
			ptr[2] = w;
			ptr[3] = h;
			ptr[4] = static_cast<float>(col.R); ptr[5] = static_cast<float>(col.G);
			ptr[6] = static_cast<float>(col.B); ptr[7] = static_cast<float>(alpha);
			ptr[8] = rotation; 
			ptr[9] = centerX; ptr[10] = centerY; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 1.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderCopyUPR(const Rect& rect, const Texture* texture) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(texture->alpha); ptr[5] = 0.0f; ptr[6] = 0.0f; ptr[7] = 0.0f;
			ptr[8] = 0.0f; ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 2.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderCopyPartUPR(const Rect& rect, const Rect& source, const Texture* texture) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}


			const float sourceX = static_cast<float>(source.x) / texture->w;
			const float sourceY = static_cast<float>(source.y) / texture->h;
			const float sourceW = static_cast<float>(source.w) / texture->w;
			const float sourceH = static_cast<float>(source.h) / texture->h;

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);

			ptr[4] = sourceX; ptr[5] = sourceY; ptr[6] = sourceW; ptr[7] = sourceH;
			ptr[8] = static_cast<float>(texture->alpha); ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 3.0f; //ShaderID

			currentIndex += currentSize;
		}

		template<bool texNullCheck = true>
		inline void RenderCopyEXUPR(const Rect& rect,  const Rect& source, const Texture* texture, const float rotation = 0.0f,
			const bool flip = false, std::optional<Point> rotCenter = std::nullopt) {
			if constexpr (texNullCheck) {
				if (!texture) { return; }
			}
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}


			const float sourceX = static_cast<float>(source.x) / texture->w;
			const float sourceY = static_cast<float>(source.y) / texture->h;
			const float sourceW = static_cast<float>(source.w) / texture->w;
			const float sourceH = static_cast<float>(source.h) / texture->h;

			float centerX = 0.0f;
			float centerY = 0.0f;
			if (!rotCenter) {
				centerX = static_cast<float>(rect.x + (rect.w >> 1));
				centerY = static_cast<float>(rect.y + (rect.h >> 1));
			}
			else {
				centerX = static_cast<float>(rotCenter->x);
				centerY = static_cast<float>(rotCenter->y);
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);

			if (flip) {
				ptr[4] = sourceX + sourceW;
				ptr[6] = -sourceW;
			}
			else {
				ptr[4] = sourceX;
				ptr[6] = sourceW;
			}
			ptr[5] = sourceY;
			ptr[7] = sourceH;
			ptr[8] = static_cast<float>(texture->alpha); 
			ptr[9] = rotation; 
			ptr[10] = centerX; ptr[11] = centerY;
			ptr[12] = 0.0f;
			ptr[13] = 4.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderCopyEXUPR(const Rect& rect, const Texture* texture, const float rotation = 0.0f, const bool flip = false,
			std::optional<Point> rotCenter = std::nullopt) {
			if (!texture) { return; }
			const Rect fullSource = { 0, 0, static_cast<int>(texture->w), static_cast<int>(texture->h) };
			RenderCopyEXUPR<false>(rect,  fullSource, texture, rotation, flip, rotCenter);
		}

		void RenderCopyCircleUPR(const Rect& rect, const Texture* texture, const float radius = 0.5f) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = radius;
			ptr[5] = texture->alpha;
			ptr[6] = 0.0f; ptr[7] = 0.0f;
			ptr[8] = 0.0f; ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 5.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderCircleUPR(const Rect& rect, const Color& col, const unsigned char alpha = 255, const float radius = 0.5f) {
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(col.R);
			ptr[5] = static_cast<float>(col.G);
			ptr[6] = static_cast<float>(col.B);
			ptr[7] = static_cast<float>(alpha);
			ptr[8] = radius;
			ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 6.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderRoundedRectUPR(const Rect& rect, const Color& col, const unsigned char alpha = 255, int roundingSize = 8) {
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(col.R);
			ptr[5] = static_cast<float>(col.G);
			ptr[6] = static_cast<float>(col.B);
			ptr[7] = static_cast<float>(alpha);
			ptr[8] = static_cast<float>(roundingSize);
			ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 7.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderCopyRoundedUPR(const Rect& rect, const Texture* texture, int roundingSize = 8) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(texture->alpha);
			ptr[5] = static_cast<float>(roundingSize);
			ptr[6] = 0.0f; ptr[7] = 0.0f;
			ptr[8] = 0.0f; ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 8.0f; //ShaderID

			currentIndex += currentSize;
		}

		template<bool texNullCheck = true>
		inline void RenderCopyFilteredUPR(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter) {
			if constexpr(texNullCheck) {
				if (!texture) { return; }
			}

			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}


			const float sourceX = static_cast<float>(source.x) / texture->w;
			const float sourceY = static_cast<float>(source.y) / texture->h;
			const float sourceW = static_cast<float>(source.w) / texture->w;
			const float sourceH = static_cast<float>(source.h) / texture->h;

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);

			ptr[4] = sourceX; ptr[5] = sourceY; ptr[6] = sourceW; ptr[7] = sourceH;
			ptr[8] = static_cast<float>(filter.R);
			ptr[9] = static_cast<float>(filter.G);
			ptr[10] = static_cast<float>(filter.B);
			ptr[11] = texture->alpha;

			ptr[12] = 0.0f;
			ptr[13] = 9.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderCopyFilteredUPR(const Rect& rect, const Texture* texture, const Color& filter) {
			if (!texture) { return; }
			const Rect fullSource = { 0, 0, static_cast<int>(texture->w), static_cast<int>(texture->h) };
			RenderCopyFilteredUPR<false>(rect, fullSource, texture, filter);
		}

		inline void RenderBorderUPR(const Rect& rect, const Color& col, const int width, const unsigned char alpha = 255) {
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(col.R); ptr[5] = static_cast<float>(col.G);
			ptr[6] = static_cast<float>(col.B); ptr[7] = static_cast<float>(alpha);
			ptr[8] = static_cast<float>(width);
			ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 10.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderRoundedBorderUPR(const Rect& rect, const Color& col, const int width, const int roundingSize = 8, const unsigned char alpha = 255) {
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = static_cast<float>(col.R);
			ptr[5] = static_cast<float>(col.G);
			ptr[6] = static_cast<float>(col.B);
			ptr[7] = static_cast<float>(alpha);
			ptr[8] = static_cast<float>(width);
			ptr[9] = static_cast<float>(roundingSize); 
			ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 11.0f; //ShaderID

			currentIndex += currentSize;
		}

		inline void RenderMaskedOverlayUPR(const Rect& rect, const Rect& source, const Texture* tex1, const Texture* tex2) {
			if (!tex1 || !tex2) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != tex1->texture) {
				Present(false);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex1->texture);
				currentTexture = tex1->texture;
			}
			if (currentMaskTexture != tex2->texture) {
				Present(false);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, tex2->texture);
				currentMaskTexture = tex2->texture;
			}

			const float sourceX = static_cast<float>(source.x) / tex1->w;
			const float sourceY = static_cast<float>(source.y) / tex1->h;
			const float sourceW = static_cast<float>(source.w) / tex1->w;
			const float sourceH = static_cast<float>(source.h) / tex1->h;

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = sourceX;
			ptr[5] = sourceY;
			ptr[6] = sourceW;
			ptr[7] = sourceH;
			ptr[8] = tex1->alpha;
			ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
			ptr[12] = 0.0f;
			ptr[13] = 12.0f; //ShaderID

			currentIndex += currentSize;

			glActiveTexture(GL_TEXTURE0);
		}

		inline void RenderDoubleMaskedOverlayUPR(const Rect& rect, const Rect& source, const Rect& source2, const Texture* tex1, const Texture* tex2) {
			if (!tex1 || !tex2) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != tex1->texture) {
				Present(false);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex1->texture);
				currentTexture = tex1->texture;
			}
			if (currentMaskTexture != tex2->texture) {
				Present(false);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, tex2->texture);
				currentMaskTexture = tex2->texture;
			}

			const float sourceX = static_cast<float>(source.x) / tex1->w;
			const float sourceY = static_cast<float>(source.y) / tex1->h;
			const float sourceW = static_cast<float>(source.w) / tex1->w;
			const float sourceH = static_cast<float>(source.h) / tex1->h;

			const float source2X = static_cast<float>(source2.x) / tex2->w;
			const float source2Y = static_cast<float>(source2.y) / tex2->h;
			const float source2W = static_cast<float>(source2.w) / tex2->w;
			const float source2H = static_cast<float>(source2.h) / tex2->h;

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
			ptr[4] = sourceX;
			ptr[5] = sourceY;
			ptr[6] = sourceW;
			ptr[7] = sourceH;
			ptr[8] = source2X;
			ptr[9] = source2Y;
			ptr[10] = source2W;
			ptr[11] = source2H;
			ptr[12] = tex1->alpha;
			ptr[13] = 13.0f; //ShaderID

			currentIndex += currentSize;

			glActiveTexture(GL_TEXTURE0);
		}

		inline void RenderShapeUPR(const Rect& rect, Texture* texture, const ColorA& filter) {
			if (!texture) { return; }

			if (!vievPort.IsColliding(rect)) { return; }

			CheckUPRProgram();

			if (currentTexture != texture->texture) {
				Present(false);
				glBindTexture(GL_TEXTURE_2D, texture->texture);
				currentTexture = texture->texture;
			}

			float* ptr = &globalVertices[currentIndex];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);

			ptr[4] = static_cast<float>(filter.R);
			ptr[5] = static_cast<float>(filter.G);
			ptr[6] = static_cast<float>(filter.B);
			ptr[7] = static_cast<float>(filter.A);

			ptr[8] = 0.0f; ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f; ptr[12] = 0.0f;
			ptr[13] = 14.0f; //ShaderID

			currentIndex += currentSize;
		}

		//UPR

		void Clear();

		void Resize(const unsigned int w, const unsigned int h);

		//
		//
		// Experimental
		// Agressive Batching Rendering good for multiple texture tile rendering but does
		// not rememeber depth so new texture can be obscured by an old one 

		//Neds to be called at least once after texture load and after every texture quantity change
		void FLatRenderCopySetUp();

		inline void FLatRenderCopy(const Rect& rect, const Texture* texture) {
			if (!texture) { return; }
			if (!vievPort.IsColliding(rect)) { return; }

			constexpr int N = 4;
			std::vector<float>& vec = flatRenderVec[texture->batchIndex].vertices;
			const size_t old = vec.size();
			vec.resize(old + N);

			float* ptr = &vec[old];
			ptr[0] = static_cast<float>(rect.x);
			ptr[1] = static_cast<float>(rect.y);
			ptr[2] = static_cast<float>(rect.w);
			ptr[3] = static_cast<float>(rect.h);
		}

		// Needs to be called after all flat operations are finisched 
		// NORMAL RENDER PRESENT WILL NOT DRAW ANYTHING !!!
		inline void FLatRenderCopyPresent() {
			Present(false);
			currentProgram = flatRenderCopyId;
			glBindVertexArray(flatVao);
			glUseProgram(flatRenderCopyId);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			for (auto& entry : flatRenderVec) {
				auto& vec = entry.vertices;
				if (vec.empty()) { continue; }
				glBindTexture(GL_TEXTURE_2D, entry.textureID);
				currentTexture = entry.textureID;
				glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(float), vec.data(), GL_DYNAMIC_DRAW);
				const GLsizei instanceCount = static_cast<GLsizei>(vec.size() / flatSize);
				glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);
				vec.clear();
			}
		}

		void SetClipSize(const Rect& rect);

		void ResetClipSize();

		void BindFrameBuffer() {
			Present(false);
			renderTarget->Bind();
		}
		void UnBindFrameBuffer() {
			Present(false);
			renderTarget->UnBind();
		}

		MT::Texture* GetFrameBuffer() {
			return renderTarget->GetTexture();
		}

		template<unsigned int size>
		friend class CustomShader;
	};

	template<unsigned int size>
	class CustomShader {
	private:
		inline static unsigned int globalID = 0;

		unsigned int VAO = 0;
		std::unordered_map<std::string,int> uniforms;
		unsigned int shaderID = 0;
		Renderer* ren = nullptr;

	public:
		bool Create(Renderer *ren, const char* vertexStr, const char* fragmentStr, const std::vector<unsigned int> &VAOAligment) {
			if(ren == nullptr){
				return false;
			}
			this->ren = ren;
			std::string strID = std::to_string(globalID);
			if (!ren->loader.CreateProgramStr("Custom" + strID, vertexStr, fragmentStr)) {
				return false;
			}
			try {
				shaderID = ren->loader.GetProgram("Custom" + strID);
			}
			catch (const std::exception&) {
				return false;
			}
			unsigned int vaoSize = 0;
			for (auto& val : VAOAligment) {
				vaoSize += val;
			}
			if (vaoSize != size) {
				return false;
			}

			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, ren->VBO);

			unsigned int spread = 0;
			for (unsigned int i = 0; i < VAOAligment.size(); i++) {
				glVertexAttribPointer(i, VAOAligment[i], GL_FLOAT, GL_FALSE, size * sizeof(float), (void*)(spread * sizeof(float)));
				glEnableVertexAttribArray(i);
				glVertexAttribDivisor(i, 1);
				spread += VAOAligment[i];
			}
			glBindVertexArray(0);


			return true;
		}

		// Creates unifrom with selected name and binds it to the shaderProgram
		int AddUniform(const std::string &name) {
			glUseProgram(shaderID);
			int  uniformID = glGetUniformLocation(shaderID, name.c_str());
			if (uniformID == -1) {
				throw("Cannot get uniform in Custom Shader::AddUniform");
			}
			uniforms[name] = uniformID;
			return uniforms[name];
		}

		int GetUnifrom(const std::string& name, bool setProg = false) {
			if (setProg) {
				glUseProgram(shaderID);
			}
			auto uniIter = uniforms.find(name);
			if (uniIter == uniforms.end()) {
				return 0;
			}
			return uniIter->second;
		}

		void Render(std::array<float,size> elements, MT::Texture* tex1 = nullptr, MT::Texture* tex2 = nullptr) {
			if (ren->currentProgram != shaderID) {
				ren->Present(false);
				glBindVertexArray(VAO);
				ren->currentProgram = shaderID;
				glUseProgram(shaderID);
			}
			if (tex1 && ren->currentTexture != tex1->texture) {
				ren->Present(false);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex1->texture);
				ren->currentTexture = tex1->texture;
			}
			if (tex2 && ren->currentMaskTexture != tex2->texture) {
				ren->Present(false);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, tex2->texture);
				ren->currentMaskTexture = tex2->texture;
			}

			ren->currentSize = size;
			if (ren->currentIndex + ren->currentSize > Renderer::batchSize) {
				ren->Present(false);
			}

			float* ptr = &ren->globalVertices[ren->currentIndex];
			for (size_t i = 0; i < elements.size(); i++) {
				ptr[i] = elements[i];
			}

			ren->currentIndex += ren->currentSize;
		}

		CustomShader() = default;

		CustomShader(const CustomShader& other) = delete;
		CustomShader& operator=(const CustomShader& other) = delete;

		CustomShader(CustomShader<size>&& other) noexcept{
			this->VAO = other.VAO;
			other.VAO = 0;
			this->shaderID = other.shaderID;
			this->uniforms = std::move(other.uniforms);
			this->ren = other.ren;
		}

		CustomShader& operator = (CustomShader<size>&& other) noexcept {
			if (this == &other) { return *this; }
			if (this->VAO != 0) {
				glDeleteVertexArrays(1, &(this->VAO));
			}
			this->VAO = other.VAO;
			other.VAO = 0;
			this->shaderID = other.shaderID;
			this->uniforms = std::move(other.uniforms);
			this->ren = other.ren;
			return *this;
		}

		~CustomShader() {
			if (VAO != 0) {
				glDeleteVertexArrays(1, &VAO);;
			}
		}
	};

}