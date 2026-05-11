#pragma once

#include <SDL.h>
#include <string>
#include <glad/glad.h>

#include "Rectangle.h"

namespace MT {
	class Window {
	private:
		SDL_Window* sdlWindow = nullptr;
		SDL_GLContext context = nullptr;
		bool fullScreen = false;
		

	public:
		bool Init(const std::string& title, int x, int y, int w, int h, Uint32 flags);

		SDL_Window* GetWindow() const { return this->sdlWindow; }

		void Resize(unsigned int w, unsigned int h) {
			SDL_SetWindowSize(sdlWindow, w, h);
		}

		Point GetSize() const {
			Point p;
			SDL_GetWindowSize(sdlWindow, &p.x, &p.y);
			return p;
		}

		void SetFullScreen(SDL_WindowFlags flag = SDL_WINDOW_FULLSCREEN_DESKTOP) {
			if (fullScreen) { return; }
			SDL_SetWindowFullscreen(sdlWindow, flag);
			fullScreen = true;
		}

		void QuitFullScreen() {
			if (!fullScreen) { return; }
			SDL_SetWindowFullscreen(sdlWindow, 0);
			fullScreen = false;
		}

		void ToggleFullScreen() {
			if (fullScreen) {
				QuitFullScreen();
			}
			else {
				SetFullScreen();
			}
		}



		~Window() {
			SDL_GL_DeleteContext(context);
			context = nullptr;
			if (sdlWindow) {
				SDL_DestroyWindow(sdlWindow);
				sdlWindow = nullptr;
			}

		}
	};

}