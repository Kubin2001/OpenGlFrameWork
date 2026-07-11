#include "Renderer.h"

#include "glad/glad.h"
#include <SDL_image.h>
#include <chrono>
#include <algorithm>

#include "Colision.h"
#include "TextureManager.h"

SDL_Surface * FlipSurfaceVertical(SDL_Surface * surface) {
    SDL_Surface* flipped = SDL_CreateRGBSurfaceWithFormat(0, surface->w, surface->h,
        surface->format->BitsPerPixel,
        surface->format->format);
    int pitch = surface->pitch;
    uint8_t* srcPixels = (uint8_t*)surface->pixels;
    uint8_t* dstPixels = (uint8_t*)flipped->pixels;

    for (int y = 0; y < surface->h; ++y) {
        memcpy(&dstPixels[y * pitch],
            &srcPixels[(surface->h - 1 - y) * pitch],
            pitch);
    }
    return flipped;
}

MT::Texture* MT::GenEmptyTexture(int w, int h) {
    unsigned int tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    MT::Texture* mtTex = new Texture();
    mtTex->w = w;
    mtTex->h = h;
    mtTex->texture = tex;

    return mtTex;
}

MT::Texture* MT::LoadTexture(const char* path, TextureFilter filter) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    switch (filter) {
    case TextureFilter::Nearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // Min filter tells what texture does when downscalling
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Mag filter tells what texture does when upscalling
        break;
    case TextureFilter::Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    }

    SDL_Surface* surf = IMG_Load(path);

    MT::Texture* metTex = new MT::Texture;
    metTex->texture = texture;
    if (!surf) {
        std::println("Failed to load image MT::LoadTexture: {}", IMG_GetError());
        return metTex;
    }
    else {
        SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0); // Aby się nie crashowało jak jest zły format
        SDL_FreeSurface(surf);
        surf = formatted;
        SDL_Surface *flipped = FlipSurfaceVertical(surf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flipped->w, flipped->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, flipped->pixels); // RGBA dla png
        metTex->w = flipped->w;
        metTex->h = flipped->h;
        metTex->writeTime = std::filesystem::last_write_time(path);
        glGenerateMipmap(GL_TEXTURE_2D);
        SDL_FreeSurface(flipped);
    }
    SDL_FreeSurface(surf);

    return metTex;
}


MT::Texture* MT::LoadTextureFromSurface(SDL_Surface* surf, TextureFilter filter) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // What texture does when gpu goes out of uv bounds (here it repeats texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    switch (filter) {
    case TextureFilter::Nearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // Min filter tells what texture does when downscalling
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Mag filter tells what texture does when upscalling
        break;
    case TextureFilter::Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    }

    MT::Texture* metTex = new MT::Texture;
    metTex->texture = texture;
    if (!surf) {
        std::println("Empty surface in MT::LoadTextureFromSurface: {}", IMG_GetError());
        return metTex;
    }

    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0); // Aby się nie crashowało jak jest zły format
    SDL_Surface* flipped = FlipSurfaceVertical(formatted);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flipped->w, flipped->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, flipped->pixels); // RGBA dla png
    metTex->w = flipped->w;
    metTex->h = flipped->h;
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(flipped);
    SDL_FreeSurface(formatted);

    return metTex;
}

SDL_Surface* MT::TextureToSurface(Texture* texture) {
    if (texture == nullptr) { return nullptr; }

    glBindTexture(GL_TEXTURE_2D, texture->texture);
    
    unsigned char* pixels = new unsigned char[texture->w * texture->h * 4];

    glGetTexImage(GL_TEXTURE_2D, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    SDL_Surface* outSurf = SDL_CreateRGBSurfaceWithFormatFrom(pixels, texture->w, texture->h, 32, texture->w * 4, SDL_PIXELFORMAT_RGBA32);

    SDL_Surface* flipped = FlipSurfaceVertical(outSurf);
    delete[] pixels;
    SDL_FreeSurface(outSurf);

    if (!flipped) {
        return nullptr;
    }

    return flipped;

}

glm::vec2 RotateAndTranslate2D(float localX, float localY, const glm::vec2& center, float cosA, float sinA) {
    return {
        center.x + localX * cosA - localY * sinA,
        center.y + localX * sinA + localY * cosA
    };
}

bool MT::Renderer::Start(const MT::Window &mtWindow) {
    this->window = mtWindow.GetWindow();
    SDL_GL_GetDrawableSize(this->window, &W, &H);
    vievPort.Set(0, 0, W, H);
    // Deklaracja zmiennych dla Vertex Array Object (VAO) i Vertex Buffer Object (VBO)
    // Generowanie VAO (Vertex Array Object) - obiekt przechowujący konfigurację atrybutów wierzchołków
    glGenVertexArrays(1, &VAO);

    // Generowanie VBO (Vertex Buffer Object) - bufor przechowujący dane wierzchołków
    glGenBuffers(1, &VBO);

    // Bindowanie VAO - od tego momentu wszystkie operacje na VAO będą dotyczyć tego obiektu
    glBindVertexArray(VAO);

    // Bindowanie VBO - od tego momentu wszystkie operacje na VBO będą dotyczyć tego bufora
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Binding uniform block fo viev port size (screen size or renderer size)
    glGenBuffers(1, &uVievPort);
    glBindBuffer(GL_UNIFORM_BUFFER, uVievPort);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uVievPort);
    float initialViewport[2] = { static_cast<float>(W), static_cast<float>(H) };
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(initialViewport), initialViewport);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    // Konfiguracja atrybutu wierzchołka - mówi OpenGL, jak interpretować dane w buforze
    // 0 - indeks atrybutu (w shaderze odpowiada location = 0)
    // 3 - liczba składowych (x, y, z)
    // GL_FLOAT - typ danych
    // GL_FALSE - czy normalizować dane (nie w tym przypadku)
    // 3 * sizeof(float) - odległość między kolejnymi wierzchołkami (w bajtach)
    // (void*)0 - przesunięcie do pierwszego elementu w buforze



    auto DefineAtributes = [](unsigned int VBO, unsigned int& VAO, unsigned int size, const std::vector<unsigned int>& values) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        unsigned int spread = 0;
        for (unsigned int i = 0; i < values.size(); i++) {
            glVertexAttribPointer(i, values[i], GL_FLOAT, GL_FALSE, size * sizeof(float), (void*)(spread * sizeof(float)));
            glEnableVertexAttribArray(i);
            glVertexAttribDivisor(i, 1);
            spread += values[i];
        }
        glBindVertexArray(0);
    };

    DefineAtributes(VBO, rectVao, renderRectSize, { 4,2 });

    DefineAtributes(VBO, rectExVao, renderRectExSize, { 4,3,2 });

    DefineAtributes(VBO, copyVao, renderCopySize, { 4,1 });

    DefineAtributes(VBO, copyPartVao, renderCopyPartSize, { 4,4,1 });

    DefineAtributes(VBO, copyExVao, renderCopyExSize, { 4,4,2,2});

    DefineAtributes(VBO, copyCircleVao, renderCopyCircleSize, { 4,2 });

    DefineAtributes(VBO, circleVao, renderCircleSize, { 4,3 });

    DefineAtributes(VBO, roundedVao, renderRoundedSize, { 4,2,1});

    DefineAtributes(VBO, roundedCopyVao, renderCopyRoundedSize, { 4,1,1 });

    DefineAtributes(VBO, filteredVao, renderFilteredSize, { 4,4,2 });

    DefineAtributes(VBO, borderVao, renderBorderSize, { 4,2,1 });

    DefineAtributes(VBO, roundedBorderVao, renderRoundedBorderSize, { 4,2,1,1 });

    DefineAtributes(VBO, maskedVao, renderMaskedSize, { 4,4,1 });

    DefineAtributes(VBO, doubleMaskedVao, renderDoubleMaskedSize, { 4,4,4,1 });

    DefineAtributes(VBO, shapeVao, renderShapeSize, { 4,2});

    DefineAtributes(VBO, uprVao, UPRSize, { 4,4,4,1,1});

    DefineAtributes(VBO, flatVao, flatSize, {4});


    loader.LoadSavedShaders();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderRectId = loader.GetProgram("RenderRect");
    renderRectExId = loader.GetProgram("RenderRectEx");
    renderCopyId = loader.GetProgram("RenderCopy");
    renderCopyPartId = loader.GetProgram("RenderCopyPart");
    renderCopyExId = loader.GetProgram("RenderCopyEx");
    renderCopyCircleId = loader.GetProgram("RenderCopyCircle");
    renderCircleId = loader.GetProgram("RenderCircle");
    renderRoundedId = loader.GetProgram("RenderRoundedRectangle");
    renderCopyRoundedId = loader.GetProgram("RenderCopyRounded");
    renderCopyFilterId = loader.GetProgram("RenderCopyFilter");
    renderBorderId = loader.GetProgram("RenderBorder");
    renderRoundedBorderId = loader.GetProgram("RenderRoundedBorder");
    renderMaskedId = loader.GetProgram("RenderMasked");
    renderDoubleMaskedId = loader.GetProgram("RenderDoubleMasked");
    renderShapeId = loader.GetProgram("RenderShape");

    uprId = loader.GetProgram("RenderUPR");
    flatRenderCopyId = loader.GetProgram("FlatRenderCopy");

    auto AssingVievPort = [&](unsigned int shaderID) {
        unsigned int blockIndex = glGetUniformBlockIndex(shaderID, "ViewportBlock");
        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(shaderID, blockIndex, 0);
        }
    };

    AssingVievPort(renderRectId);

    AssingVievPort(renderRectExId);

    AssingVievPort(renderCopyId);

    AssingVievPort(renderCopyPartId);

    AssingVievPort(renderCopyExId);

    AssingVievPort(renderCopyCircleId);

    AssingVievPort(renderCircleId);

    AssingVievPort(renderRoundedId);

    AssingVievPort(renderCopyRoundedId);

    AssingVievPort(renderCopyFilterId);

    AssingVievPort(renderBorderId);
    AssingVievPort(renderRoundedBorderId);

    AssingVievPort(renderMaskedId);
    glUseProgram(renderMaskedId);
    glUniform1i(glGetUniformLocation(renderMaskedId, "texture1"), 0);
    glUniform1i(glGetUniformLocation(renderMaskedId, "texture2"), 1);

    AssingVievPort(renderDoubleMaskedId);
    glUseProgram(renderDoubleMaskedId);
    glUniform1i(glGetUniformLocation(renderDoubleMaskedId, "texture1"), 0);
    glUniform1i(glGetUniformLocation(renderDoubleMaskedId, "texture2"), 1);

    AssingVievPort(renderShapeId);

    AssingVievPort(uprId);
    glUseProgram(uprId);
    glUniform1i(glGetUniformLocation(uprId, "texture1"), 0);
    glUniform1i(glGetUniformLocation(uprId, "texture2"), 1);

    AssingVievPort(flatRenderCopyId);


    glActiveTexture(GL_TEXTURE0);

    renderTarget = std::make_unique<RenderTarget>(W, H);

    globalVertices.resize(batchSize);
    return true;
}


void MT::Renderer::Clear() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &uVievPort);
    VAO = 0;
    VBO = 0;
    uVievPort = 0;

    currentProgram = 0;
    currentTexture = 0;
    currentMaskTexture = 0;
    renderCopyId = 0;
    renderRectId = 0;
    renderCopyCircleId = 0;
    renderCircleId = 0;
    renderCopyFilterId = 0;


    globalVertices.clear();
    flatRenderVec.clear();
}

void MT::Renderer::Resize(const unsigned int w, const unsigned int h) {
    if (this->W == static_cast<int>(w) && this->H == static_cast<int>(h)) {
        return;
    }
    Present(false);
    W = w;
    H = h;
    vievPort.w = W;
    vievPort.h = H;

    currentProgram = 0;
    currentTexture = 0;
    currentMaskTexture = 0;
    glUseProgram(0);

    renderTarget->Resize(W, H);

    int tempW, tempH;
    SDL_GL_GetDrawableSize(window, &tempW, &tempH);
    glViewport(0, 0, tempW, tempH);

    // Updating shader viec ports
    float newViewport[2] = { static_cast<float>(W), static_cast<float>(H) };

    glBindBuffer(GL_UNIFORM_BUFFER, uVievPort);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(newViewport), newViewport);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MT::Renderer::FLatRenderCopySetUp() {
    flatRenderVec.clear();
    int i = 0;
    TexMan::GetDefaultTex()->batchIndex = i++;
    flatRenderVec.emplace_back(TexMan::GetDefaultTex()->texture);
    for (auto& tex : TexMan::GetAllTex()) {
        tex.second->batchIndex = i++;
        flatRenderVec.emplace_back(tex.second->texture);
    }
}

void MT::Renderer::SetClipSize(const MT::Rect& rect) {
    Present(false);
    glEnable(GL_SCISSOR_TEST);
    int glY = H - (rect.y + rect.h);
    glScissor(rect.x, glY, rect.w, rect.h);
}


void MT::Renderer::ResetClipSize() {
    Present(false);
    glScissor(0, 0, W, H);
    glDisable(GL_SCISSOR_TEST);
}