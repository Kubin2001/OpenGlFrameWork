#include "Window.h"



#include "glad/glad.h"

#include "Rectangle.h"
#include "GlobalVariables.h"

bool MT::Window::Init(const std::string& title, int x, int y, int w, int h, Uint32 flags) {

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    sdlWindow = SDL_CreateWindow(title.c_str(), x, y,
        w, h, flags);

    if (!sdlWindow) {
        return false;
    }

    context = SDL_GL_CreateContext(sdlWindow);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_DestroyWindow(sdlWindow);
        SDL_GL_DeleteContext(context);
        sdlWindow = nullptr;
        return false;
    }

    return true;
}