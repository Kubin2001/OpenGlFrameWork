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

SDL_GLContext MT::Init(SDL_Window* window) {

    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Failed to initialize GLAD");
    }
    return context;
}

MT::Texture* MT::LoadTexture(const char* path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //rozmywa piksele
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //świetne dla pixel art

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    SDL_Surface* surf = IMG_Load(path);

    MT::Texture* metTex = new MT::Texture;
    metTex->texture = texture;
    if (!surf) {
        std::cout << "Failed to load image MT::LoadTexture: " << IMG_GetError() << "\n";
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


MT::Texture* MT::LoadTextureFromSurface(SDL_Surface* surf) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //rozmywa piksele
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //świetne dla pixel art

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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

bool MT::Renderer::Start(SDL_Window* window, SDL_GLContext context) {
    this->window = window;
    SDL_GL_GetDrawableSize(window, &W, &H);
    this->context = context;
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

    DefineAtributes(VBO, rectExVao, renderRectExSize, { 4,3 });

    DefineAtributes(VBO, copyVao, renderCopySize, { 4,1 });

    DefineAtributes(VBO, copyPartVao, renderCopyPartSize, { 4,4,1 });

    DefineAtributes(VBO, copyExVao, renderCopyExSize, { 4,4,2 });

    DefineAtributes(VBO, copyCircleVao, renderCopyCircleSize, { 4,2 });

    DefineAtributes(VBO, circleVao, renderCircleSize, { 4,3 });

    DefineAtributes(VBO, roundedVao, renderRoundedSize, { 4,2 });

    DefineAtributes(VBO, roundedCopyVao, renderCopyRoundedSize, { 4,1 });

    DefineAtributes(VBO, filteredVao, renderFilteredSize, { 4,4,2 });

    DefineAtributes(VBO, borderVao, renderBorderSize, { 4,2,1 });

    DefineAtributes(VBO, roundedBorderVao, renderRoundedBorderSize, { 4,2,1 });

    DefineAtributes(VBO, maskedVao, renderMaskedSize, { 4,4,1 });

    DefineAtributes(VBO, doubleMaskedVao, renderDoubleMaskedSize, { 4,4,4,1 });

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
    renderCopyRoundedId = loader.GetProgram("RenderCopyRoundedRectangle");
    renderCopyFilterId = loader.GetProgram("RenderCopyFilter");
    renderBorderId = loader.GetProgram("RenderBorder");
    renderRoundedBorderId = loader.GetProgram("RenderRoundedBorder");
    renderMaskedId = loader.GetProgram("RenderMasked");
    renderDoubleMaskedId = loader.GetProgram("RenderDoubleMasked");
    uprId = loader.GetProgram("RenderUPR");
    flatRenderCopyId = loader.GetProgram("FlatRenderCopy");

    const GLfloat fW = static_cast<GLfloat>(W);
    const GLfloat fH = static_cast<GLfloat>(H);

    auto AssingVievPort = [&](unsigned int shaderID, unsigned int& uniformID) {
        glUseProgram(shaderID);
        uniformID = glGetUniformLocation(shaderID, "uVievPort");
        glUniform2f(uniformID, fW, fH);
    };

    AssingVievPort(renderRectId, renderRectVievPort);

    AssingVievPort(renderRectExId, renderRectExVievPort);

    AssingVievPort(renderCopyId, renderCopyVievPort);

    AssingVievPort(renderCopyPartId, renderCopyPartVievPort);

    AssingVievPort(renderCopyExId, renderCopyExVievPort);

    AssingVievPort(renderCopyCircleId, renderCopyCircleVievPort);

    AssingVievPort(renderCircleId, renderCircleVievPort);

    AssingVievPort(renderRoundedId, renderRoundedVievPort);

    AssingVievPort(renderCopyRoundedId, renderCopyRoundedVievPort);

    AssingVievPort(renderCopyFilterId, renderFilterVievPort);

    AssingVievPort(renderBorderId, renderBorderVievPort);
    AssingVievPort(renderRoundedBorderId, renderRoundedBorderVievPort);

    AssingVievPort(renderMaskedId, renderMaskedVievPort);
    glUniform1i(glGetUniformLocation(renderMaskedId, "texture1"), 0);
    glUniform1i(glGetUniformLocation(renderMaskedId, "texture2"), 1);

    AssingVievPort(renderDoubleMaskedId, renderDoubleMaskedVievPort);
    glUniform1i(glGetUniformLocation(renderDoubleMaskedId, "texture1"), 0);
    glUniform1i(glGetUniformLocation(renderDoubleMaskedId, "texture2"), 1);

    AssingVievPort(uprId, uprVievPort);
    glUniform1i(glGetUniformLocation(uprId, "texture1"), 0);
    glUniform1i(glGetUniformLocation(uprId, "texture2"), 1);

    AssingVievPort(flatRenderCopyId, flatVievPort);


    glActiveTexture(GL_TEXTURE0);

    globalVertices.resize(batchSize);
    return true;
}

void MT::Renderer::ClearFrame(const unsigned char R, const unsigned char G, const unsigned char B) {
    const float fR = float(R) / 255;
    const float fG = float(G) / 255;
    const float fB = float(B) / 255;
    glClearColor(fR, fG, fB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void MT::Renderer::RenderRect(const Rect& rect, const Color& col, const int alpha) {
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
    const float fRG = iRG;
    const float fBA = iBA;


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

glm::vec2 RotateNdc(float localX, float localY, const glm::vec2& mainCenter, float cosA, float sinA, int w, int h) {
    // Odwracamy lokalną oś Y (bo piksele rosną w dół)
    localY = -localY;

    // rotate local around origin then translate by center
    float rx = localX * cosA - localY * sinA;
    float ry = localX * sinA + localY * cosA;

    float px = mainCenter.x + rx;
    float py = mainCenter.y + ry;

    // convert to NDC
    float ndc_x = (px / float(w)) * 2.0f - 1.0f;
    float ndc_y = 1.0f - (py / float(h)) * 2.0f;

    return { ndc_x, ndc_y };
}

void MT::Renderer::RenderRectEX(const Rect& rect, const Color& col, const float rotation, const int alpha) {
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
    const float fRG = iRG;
    const float fBA = iBA;


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

    currentIndex += currentSize;
}


void MT::Renderer::DrawLine(const int x1, const int y1, const int x2, const int y2, const int thickness,
    const Color& col, const unsigned char alpha) {

    if (currentProgram != renderRectExId) {
        Present(false);
        currentProgram = renderRectExId;
        glUseProgram(currentProgram);
        glBindVertexArray(rectExVao);
    }

    currentSize = Renderer::renderRectExSize;

    const float fx1 = static_cast<float>(x1);
    const float fy1 = static_cast<float>(y1);
    const float fx2 = static_cast<float>(x2);
    const float fy2 = static_cast<float>(y2);

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

    currentIndex += currentSize;
}


void MT::Renderer::RenderCopy(const Rect& rect, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {return;}

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

void MT::Renderer::RenderCopyPart(const Rect& rect, const Rect& source, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {return;}

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


void MT::Renderer::RenderCopyEX(const Rect& rect, const Rect& source, const Texture* texture, const bool flip, const float rotation) {
    if (!texture) { return; }
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

    currentIndex += currentSize;
}


void MT::Renderer::RenderCopyCircle(const Rect& rect, const Texture* texture, const float radius) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {return;}

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


void MT::Renderer::RenderCircle(const Rect& rect, const Color& col, const unsigned char alpha, const float radius) {
    if (!vievPort.IsColliding(rect)) {return;}

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

void MT::Renderer::RenderRoundedRect(const Rect& rect, const Color& col, const unsigned char alpha) {
    if (!vievPort.IsColliding(rect)) {return;}

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

    currentIndex += currentSize;
}


void MT::Renderer::RenderCopyRounded(const MT::Rect& rect, const MT::Texture* texture) {
    if (!vievPort.IsColliding(rect)) {return;}

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

    currentIndex += currentSize;
}


void MT::Renderer::RenderCopyFiltered(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {return;}

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

void MT::Renderer::RenderBorder(const Rect& rect, const Color& col, const int width, const unsigned char alpha) {
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

void MT::Renderer::RenderRoundedBorder(const Rect& rect, const Color& col, const int width, const unsigned char alpha) {
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

    currentIndex += currentSize;
}

void MT::Renderer::RenderMaskedOverlay(const Rect& rect, const Rect &source, const Texture* tex1, const Texture* tex2) {
    if (!tex1 || ! tex2) { return; }
    if (!vievPort.IsColliding(rect)) {return;}

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

void MT::Renderer::RenderDoubleMaskedOverlay(const Rect& rect, const Rect& source, const Rect& source2, const Texture* tex1, const Texture* tex2) {
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

void MT::Renderer::ExpandUpr(float *vertices) {
    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertices, N * sizeof(float));
}


void MT::Renderer::RenderRectUPR(const Rect& rect, const Color& col, const int alpha) {
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

void MT::Renderer::RenderRectEXUPR(const Rect& rect, const Color& col, const float rotation, const int alpha) {
    if (!vievPort.IsColliding(rect)) { return; }

    CheckUPRProgram();

    float* ptr = &globalVertices[currentIndex];
    ptr[0] = static_cast<float>(rect.x);
    ptr[1] = static_cast<float>(rect.y);
    ptr[2] = static_cast<float>(rect.w);
    ptr[3] = static_cast<float>(rect.h);
    ptr[4] = static_cast<float>(col.R); ptr[5] = static_cast<float>(col.G);
    ptr[6] = static_cast<float>(col.B); ptr[7] = static_cast<float>(alpha);
    ptr[8] = rotation; ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
    ptr[12] = 0.0f;
    ptr[13] = 1.0f; //ShaderID

    currentIndex += currentSize;
}

void MT::Renderer::DrawLineUPR(const int x1, const int y1, const int x2, const int y2, const int thickness,
    const Color& col, const unsigned char alpha) {

    CheckUPRProgram();

    const float fx1 = static_cast<float>(x1);
    const float fy1 = static_cast<float>(y1);
    const float fx2 = static_cast<float>(x2);
    const float fy2 = static_cast<float>(y2);

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
    ptr[8] = rotation; ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
    ptr[12] = 0.0f;
    ptr[13] = 1.0f; //ShaderID

    currentIndex += currentSize;
}


void MT::Renderer::RenderCopyUPR(const Rect& rect, const Texture* texture) {
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

void MT::Renderer::RenderCopyPartUPR(const Rect& rect, const Rect& source, const Texture* texture) {
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

void MT::Renderer::RenderCopyEXUPR(const Rect& rect, const Rect& source, const Texture* texture, const bool flip, const float rotation) {
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
    ptr[8] = static_cast<float>(texture->alpha); ptr[9] = rotation; ptr[10] = 0.0f; ptr[11] = 0.0f;
    ptr[12] = 0.0f;
    ptr[13] = 4.0f; //ShaderID

    currentIndex += currentSize;
}

void MT::Renderer::RenderCopyCircleUPR(const Rect& rect, const Texture* texture, const float radius) {
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

void MT::Renderer::RenderCircleUPR(const Rect& rect, const Color& col, const unsigned char alpha, const float radius) {
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

void MT::Renderer::RenderRoundedRectUPR(const Rect& rect, const Color& col, const unsigned char alpha) {
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
    ptr[8] = 0.0f;
    ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
    ptr[12] = 0.0f;
    ptr[13] = 7.0f; //ShaderID

    currentIndex += currentSize;
}

void MT::Renderer::RenderCopyRoundedUPR(const MT::Rect& rect, const MT::Texture* texture) {
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
    ptr[5] = 0.0f; ptr[6] = 0.0f; ptr[7] = 0.0f;
    ptr[8] = 0.0f; ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
    ptr[12] = 0.0f;
    ptr[13] = 8.0f; //ShaderID

    currentIndex += currentSize;
}

void MT::Renderer::RenderCopyFilteredUPR(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter) {
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
    ptr[8] = static_cast<float>(filter.R); 
    ptr[9] = static_cast<float>(filter.G);
    ptr[10] = static_cast<float>(filter.B);
    ptr[11] = texture->alpha;

    ptr[12] = 0.0f;
    ptr[13] = 9.0f; //ShaderID

    currentIndex += currentSize;
}

void MT::Renderer::RenderBorderUPR(const Rect& rect, const Color& col, const int width, const unsigned char alpha) {
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

void MT::Renderer::RenderRoundedBorderUPR(const Rect& rect, const Color& col, const int width, const unsigned char alpha) {
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
    ptr[9] = 0.0f; ptr[10] = 0.0f; ptr[11] = 0.0f;
    ptr[12] = 0.0f;
    ptr[13] = 11.0f; //ShaderID

    currentIndex += currentSize;
}

void MT::Renderer::RenderMaskedOverlayUPR(const Rect& rect, const Rect& source, const Texture* tex1, const Texture* tex2) {
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

void MT::Renderer::RenderDoubleMaskedOverlayUPR(const Rect& rect, const Rect& source, const Rect& source2, const Texture* tex1, const Texture* tex2) {
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


void MT::Renderer::Present(bool switchContext) {
    if (currentIndex == 0) {
        if (switchContext) { SDL_GL_SwapWindow(window); }
        return;
    }
    float* start = &globalVertices[0];
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, currentIndex * sizeof(float), start, GL_DYNAMIC_DRAW);

    const GLsizei instanceCount = currentIndex / currentSize;
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);

    currentIndex = 0;

    if (switchContext) { SDL_GL_SwapWindow(window); }
}



void MT::Renderer::Clear() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    VAO = 0;
    VBO = 0;

    currentProgram = 0;
    currentTexture = 0;
    currentMaskTexture = 0;
    renderCopyId = 0;
    renderRectId = 0;
    renderCopyCircleId = 0;
    renderCircleId = 0;
    renderCopyFilterId = 0;


    globalVertices.clear();
    globalVertices.shrink_to_fit();
    flatRenderVec.clear();

    SDL_GL_DeleteContext(context);
}

void MT::Renderer::Resize(const unsigned int w, const unsigned int h) {
    W = w;
    H = h;
    vievPort.w = W;
    vievPort.h = H;

    const GLfloat fW = static_cast<GLfloat>(W);
    const GLfloat fH = static_cast<GLfloat>(H);

    auto AssingVievPort = [&](unsigned int shaderID, unsigned int& uniformID) {
        glUseProgram(shaderID);
        uniformID = glGetUniformLocation(shaderID, "uVievPort");
        glUniform2f(uniformID, fW, fH);
        };

    AssingVievPort(renderRectId, renderRectVievPort);

    AssingVievPort(renderRectExId, renderRectExVievPort);

    AssingVievPort(renderCopyId, renderCopyVievPort);

    AssingVievPort(renderCopyPartId, renderCopyPartVievPort);

    AssingVievPort(renderCopyExId, renderCopyExVievPort);

    AssingVievPort(renderCopyCircleId, renderCopyCircleVievPort);

    AssingVievPort(renderCircleId, renderCircleVievPort);

    AssingVievPort(renderRoundedId, renderRoundedVievPort);

    AssingVievPort(renderCopyRoundedId, renderCopyRoundedVievPort);

    AssingVievPort(renderCopyFilterId, renderFilterVievPort);

    AssingVievPort(renderBorderId, renderBorderVievPort);

    AssingVievPort(renderRoundedBorderId, renderRoundedBorderVievPort);

    AssingVievPort(renderMaskedId, renderMaskedVievPort);

    AssingVievPort(renderDoubleMaskedId, renderDoubleMaskedVievPort);

    AssingVievPort(uprId, uprVievPort);

    AssingVievPort(flatRenderCopyId, flatVievPort);

    SDL_GL_GetDrawableSize(window, &W, &H);
    glViewport(0, 0, W, H);
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

void MT::Renderer::FLatRenderCopy(const Rect& rect, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {return;}

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

void MT::Renderer::FLatRenderCopyPresent() {
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
        const GLsizei instanceCount = vec.size() / flatSize;
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instanceCount);
        vec.clear();
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