#include "Renderer.h"

#include "glad/glad.h"
#include <SDL_image.h>
#include <chrono>

#include "Colision.h"
#include "TextureManager.h"

SDL_Surface* FlipSurfaceVertical(SDL_Surface* surface) {
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

SDL_GLContext MT::Innit(SDL_Window *window) {

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

    MT::Texture *metTex = new MT::Texture;
    metTex->texture = texture;
    if (!surf) {
        std::cout << "Failed to load image MT::LoadTexture: " << IMG_GetError() << "\n";
        return metTex;
    }
    else {
        SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0); // Aby się nie crashowało jak jest zły format
        SDL_FreeSurface(surf);
        surf = formatted;
        surf = FlipSurfaceVertical(surf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels); // RGBA dla png
        metTex->w = surf->w;
        metTex->h = surf->h;
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    SDL_FreeSurface(surf);

    return metTex;
}

void MT::DeleteTexture(Texture* tex) {
    glDeleteTextures(1,&tex->texture);
    tex->texture = 0;
    tex->w = 0;
    tex->h = 0;
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
        std::cout << "Empty surface in MT::LoadTextureFromSurface: " << IMG_GetError() << std::endl;
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

    //Rectangle
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // powierzchnie
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2* sizeof(float))); // kolory + alpha

    //Render Copy
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // powierzchnie
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float))); // tekstury + alpha


    // Koło
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); // powierzchnie + radius
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))); // kolory

    //Rectangle Filtered
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // powierzchnie
    glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float))); // tekstury
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float))); // filtr koloru tekstury + alpha

    // Koło Render Copy
    glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // powierzchnie + radius
    glVertexAttribPointer(12, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // uv + alpha


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);
    glEnableVertexAttribArray(8);
    glEnableVertexAttribArray(9);
    glEnableVertexAttribArray(10);
    glEnableVertexAttribArray(11);
    glEnableVertexAttribArray(12);


    if (!loader.IsProgram("RenderRect")) {
        constexpr const char * vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec4 aColor;

            out vec3 ourColor;
            out float vAlpha;

            void main() {
                gl_Position = vec4(aPos,0.0, 1.0);
                ourColor = aColor.xyz;
                vAlpha = aColor.a;    
            }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec3 ourColor;
        in float vAlpha;

        void main(){
	        FragColor = vec4(ourColor,1.0 * vAlpha);
        }
        )glsl";

        loader.CreateProgramStr("RenderRect", vertexStr, fragmentStr);
    }

    if (!loader.IsProgram("RenderCopy")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout (location = 3) in vec2 aPos;
        layout (location = 4) in vec3 aTexCordAlpha;

        out vec2 texCord;
        out float vAlpha;

        void main(){
	        gl_Position = vec4(aPos, 0.0 ,1.0);

	        texCord = aTexCordAlpha.xy;
            vAlpha = aTexCordAlpha.z;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec2 texCord;
        in float vAlpha;

        uniform sampler2D texture1;


        void main(){
	        vec4 texcolor = texture(texture1,texCord);
	        texcolor.a *= vAlpha;
	        FragColor = texcolor;
        }
        )glsl";

        loader.CreateProgramStr("RenderCopy", vertexStr, fragmentStr);
    }

    if (!loader.IsProgram("RenderCopyCircle")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout (location = 11) in vec3 aPosR;
        layout (location = 12) in vec3 aTexCordAlpha;

        out vec3 texCord;
        out float radius;       

        void main(){
	        gl_Position = vec4(aPosR.xy,0.0 ,1.0);

	        texCord = aTexCordAlpha;
            radius = aPosR.z;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core
        in vec3 texCord;
        in float radius;
        out vec4 FragColor;

        uniform sampler2D texture0;

        void main(){
            vec2 center = vec2(0.5, 0.5);

            float dist = distance(texCord.xy, center);
            if (dist > radius)
                discard;

            vec4 texColor = texture(texture0, texCord.xy);
            FragColor = vec4(texColor.rgb, texColor.a * texCord.z);
        }
        )glsl";

        loader.CreateProgramStr("RenderCopyCircle", vertexStr, fragmentStr);
    }

    if (!loader.IsProgram("RenderCircle")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 6) in vec3 aPosRad;
        layout(location = 7) in vec4 aColor;

        out vec4 ourColor;
        out vec3 uvR;

        vec2 uvFromVertexID(int id) {
            if      (id == 0) return vec2(0.0, 0.0);
            else if (id == 1) return vec2(0.0, 1.0);
            else if (id == 2) return vec2(1.0, 0.0);
            else if (id == 3) return vec2(0.0, 1.0);
            else if (id == 4) return vec2(1.0, 1.0);
            else              return vec2(1.0, 0.0);
        }

        void main() {
            gl_Position = vec4(aPosRad.xy, 0.0, 1.0);
            ourColor = aColor;
            uvR.xy = uvFromVertexID(gl_VertexID % 6);
            uvR.z = aPosRad.z;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        in vec4 ourColor;
        in vec3 uvR;
        out vec4 FragColor;

        void main(){
            vec2 center = vec2(0.5, 0.5);
            float dist = distance(uvR.xy, center);
            if (dist > uvR.z)
                discard;

            FragColor = vec4(ourColor);
        }
        )glsl";

        loader.CreateProgramStr("RenderCircle", vertexStr, fragmentStr);
    }

    if (!loader.IsProgram("RenderCopyFilter")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout (location = 8) in vec2 aPos;
        layout (location = 9) in vec2 aTexCord;
        layout (location = 10) in vec4 aColor;

        out vec2 texCord;
        out vec4 cFilter;

        void main(){
	        gl_Position = vec4(aPos, 0.0 ,1.0);
	        texCord = aTexCord;
            cFilter = aColor;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec2 texCord;
        in vec4 cFilter;

        uniform sampler2D texture1;


        void main(){
	        vec4 texcolor = texture(texture1,texCord);
            texcolor.x *= cFilter.r; 
            texcolor.y *= cFilter.g; 
            texcolor.z *= cFilter.b; 
	        texcolor.a *= cFilter.a; 
	        FragColor = texcolor;
        }
        )glsl";

        loader.CreateProgramStr("RenderCopyFilter", vertexStr, fragmentStr);
    }

    if (!loader.IsProgram("RenderRoundedRectangle")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec4 aColor;

        out vec4 ourColor;
        out vec2 uv;

        vec2 uvFromVertexID(int id) {
            if      (id == 0) return vec2(0.0, 0.0);
            else if (id == 1) return vec2(0.0, 1.0);
            else if (id == 2) return vec2(1.0, 0.0);
            else if (id == 3) return vec2(0.0, 1.0);
            else if (id == 4) return vec2(1.0, 1.0);
            else              return vec2(1.0, 0.0);
        }

        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            ourColor = aColor;
            uv = uvFromVertexID(gl_VertexID % 6);
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        in vec4 ourColor;
        in vec2 uv; 
        out vec4 FragColor;

        uniform vec2 uPixelSize; // W and H

        float roundedBoxSDF(vec2 p, vec2 size, float r)
        {
            vec2 d = abs(p - size * 0.5) - (size * 0.5 - vec2(r));
            return length(max(d, 0.0)) - r;
        }

        void main()
        {
            vec2 p_px = uv * uPixelSize;

            float d = roundedBoxSDF(p_px, uPixelSize, 8.0); // 8.0 is the size of a curve if nedded the change uniform is requied

            float alpha = 1.0 - smoothstep(0.0, 1.0, d);

            if (alpha <= 0.001) discard;

            FragColor = vec4(ourColor.rgb, ourColor.a * alpha);
        }
        )glsl";

        loader.CreateProgramStr("RenderRoundedRectangle", vertexStr, fragmentStr);
    }

    if (!loader.IsProgram("RenderCopyRoundedRectangle")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout (location = 3) in vec2 aPos;
        layout (location = 4) in vec3 aTexCordAlpha;

        out vec4 ourColor;
        out vec2 uv;
        out float vAlpha;

        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            uv = aTexCordAlpha.xy;
            vAlpha = aTexCordAlpha.z;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        in vec2 uv;
        in float vAlpha; 
        out vec4 FragColor;

        uniform sampler2D texture1;
        uniform vec2 uPixelSize; // W and H

        float roundedBoxSDF(vec2 p, vec2 size, float r)
        {
            vec2 d = abs(p - size * 0.5) - (size * 0.5 - vec2(r));
            return length(max(d, 0.0)) - r;
        }

        void main()
        {

            vec2 p_px = uv * uPixelSize;
            float d = roundedBoxSDF(p_px, uPixelSize, 8.0); // 8.0 is the size of a curve if nedded the change uniform is requied

            float alpha = 1.0 - smoothstep(0.0, 1.0, d);

            if (alpha <= 0.001) discard;
            
           	vec4 texcolor = texture(texture1,uv);
	        texcolor.a *= vAlpha;

	        FragColor = texcolor;
        }
        )glsl";

        loader.CreateProgramStr("RenderCopyRoundedRectangle", vertexStr, fragmentStr);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderCopyId = loader.GetProgram("RenderCopy");
    renderRectId = loader.GetProgram("RenderRect");
    renderCopyCircleId = loader.GetProgram("RenderCopyCircle");
    renderCircleId = loader.GetProgram("RenderCircle");
    renderCopyFilterId = loader.GetProgram("RenderCopyFilter");
    renderRoundedRectId = loader.GetProgram("RenderRoundedRectangle");
    renderCopyRoundedRectId = loader.GetProgram("RenderCopyRoundedRectangle");
    
    roundRectRadius = glGetUniformLocation(renderRoundedRectId, "uPixelSize");
    roundRectCopyRadius = glGetUniformLocation(renderCopyRoundedRectId, "uPixelSize");

    globalVertices.reserve(1'000'000);
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
        RenderPresent(false);
        currentProgram = renderRectId;
        glUseProgram(renderRectId);
    }
    currentSize = renderRectSize;
    float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    float w = (static_cast<float>(rect.w) / W) * 2.0f;
    float h = (static_cast<float>(rect.h) / H) * 2.0f;

    const float fR = float(col.R) / 255;
    const float fG = float(col.G) / 255;
    const float fB = float(col.B) / 255;

    float floatAlpha = float(alpha) / 255;
    // pos.x, pos.y, col.r, col.g, col.b
    const float vertices[] = {
        x,     y - h, fR, fG, fB, floatAlpha,
        x,     y,     fR, fG, fB, floatAlpha,
        x + w, y - h, fR, fG, fB, floatAlpha,
        x,     y,     fR, fG, fB, floatAlpha,
        x + w, y,     fR, fG, fB, floatAlpha,
        x + w, y - h, fR, fG, fB, floatAlpha
    };
    constexpr int N = 36;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertices, N * sizeof(float));
}

void MT::Renderer::RenderRectEX(const Rect& rect, const Color &col, const float rotation, const int alpha) {
    if (!vievPort.IsColliding(rect)) {
        return;
    }

    if (currentProgram != renderRectId) {
        RenderPresent(false);
        currentProgram = renderRectId;
        glUseProgram(renderRectId);
    }

    currentSize = renderRectSize;
    RectF temp;

    float aspect = static_cast<float>(H) / static_cast<float>(W);
    temp.x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    temp.y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    temp.w = (static_cast<float>(rect.w) / W) * 2.0f;
    temp.h = (static_cast<float>(rect.h) / H) * 2.0f * aspect;


    float halfW = temp.w / 2.0f;
    float halfH = temp.h / 2.0f;

    const float fR = float(col.R) / 255;
    const float fG = float(col.G) / 255;
    const float fB = float(col.B) / 255;

    float rad = glm::radians(rotation);
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    glm::vec2 center = { temp.x + halfW, temp.y - halfH };

    glm::vec2 p0 = RotateAndTranslate2D(-halfW, -halfH, center, cosA, sinA);
    glm::vec2 p1 = RotateAndTranslate2D(-halfW, halfH, center, cosA, sinA);
    glm::vec2 p2 = RotateAndTranslate2D(halfW, halfH, center, cosA, sinA);
    glm::vec2 p3 = RotateAndTranslate2D(-halfW, -halfH, center, cosA, sinA);
    glm::vec2 p4 = RotateAndTranslate2D(halfW, halfH, center, cosA, sinA);
    glm::vec2 p5 = RotateAndTranslate2D(halfW, -halfH, center, cosA, sinA);

    float floatAlpha = float(alpha) / 255;

    const float vertex[] = {
        p0.x, p0.y, fR, fG, fB, floatAlpha,
        p1.x, p1.y, fR, fG, fB, floatAlpha,
        p2.x, p2.y, fR, fG, fB, floatAlpha,
        p3.x, p3.y, fR, fG, fB, floatAlpha,
        p4.x, p4.y, fR, fG, fB, floatAlpha,
        p5.x, p5.y, fR, fG, fB, floatAlpha,
    };
    constexpr int N = 36;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}


void MT::Renderer::DrawLine(const int x1, const int y1, const int x2, const int y2,const int thickness,
    const Color& col, const unsigned char alpha) {

    if (currentProgram != renderRectId) {
        RenderPresent(false);
        currentProgram = renderRectId;
        glUseProgram(renderRectId);
    }

    currentSize = renderRectSize;



    glm::vec2 p1 = glm::vec2{ (static_cast<float>(x1) / W) * 2.0f - 1.0f,
    1.0f - (static_cast<float>(y1) / H) * 2.0f };

    glm::vec2 p2 = glm::vec2{ (static_cast<float>(x2) / W) * 2.0f - 1.0f,
    1.0f - (static_cast<float>(y2) / H) * 2.0f };

    glm::vec2 dir = p2 - p1;
    float len = glm::length(dir);
    if (len < 1e-6f) { return; };

    glm::vec2 normal = glm::normalize(glm::vec2(-dir.y, dir.x));

    float thick = ((float)(thickness) / H) * 2.0f;
    glm::vec2 offset = normal * (thick * 0.5f);

    glm::vec2 v0 = p1 + offset;
    glm::vec2 v1 = p1 - offset;
    glm::vec2 v2 = p2 - offset;
    glm::vec2 v3 = p2 + offset;

    const float fR = float(col.R) / 255;
    const float fG = float(col.G) / 255;
    const float fB = float(col.B) / 255;
    const float fA = float(alpha) / 255;

    const float vertex[] = {
        v0.x, v0.y, fR, fG, fB, fA,
        v1.x, v1.y, fR, fG, fB, fA,
        v2.x, v2.y, fR, fG, fB, fA,

        v0.x, v0.y, fR, fG, fB, fA,
        v2.x, v2.y, fR, fG, fB, fA,
        v3.x, v3.y, fR, fG, fB, fA
    };

    globalVertices.insert(globalVertices.end(), std::begin(vertex), std::end(vertex));
}


void MT::Renderer::RenderCopy(const Rect& rect, const Texture* texture){
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    if (currentTexture != texture->texture) {
        RenderPresent(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }
    
    if (currentProgram != renderCopyId) {
        RenderPresent(false);
        currentProgram = renderCopyId;
        glUseProgram(renderCopyId);
    }
    currentSize = renderCopySize; 

    //    // pos.x, pos.y tex.u, tex.v
    const float verticles[] = {
        x,     y - h, 0.0f, 0.0f,texture->alpha,
        x,     y,     0.0f, 1.0f,texture->alpha,
        x + w, y - h, 1.0f, 0.0f,texture->alpha,
        x,     y,     0.0f, 1.0f,texture->alpha,
        x + w, y,     1.0f, 1.0f,texture->alpha,
        x + w, y - h, 1.0f, 0.0f,texture->alpha
    };

    constexpr int N = 30;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, verticles, N * sizeof(float));
}

void MT::Renderer::RenderCopyPart(const Rect& rect, const Rect& source, const Texture *texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;


    if (currentTexture != texture->texture) {
        RenderPresent(false); 
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyId) {
        RenderPresent(false);
        currentProgram = renderCopyId;
        glUseProgram(renderCopyId);
    }
    currentSize = renderCopySize;

    RectF tempSource;

    tempSource.x = static_cast<float>(source.x) / texture->w;
    tempSource.y = static_cast<float>(source.y) / texture->h;
    tempSource.w = static_cast<float>(source.w) / texture->w;
    tempSource.h = static_cast<float>(source.h) / texture->h;

    float u0 = tempSource.x;
    float u1 = tempSource.x + tempSource.w;
    float v1 = 1.0f - tempSource.y;
    float v0 = v1 - tempSource.h;


    // pos.x pos.y tex.u, tex.v
    float verticles[] = {
        x,     y - h, u0, v0,texture->alpha,
        x,     y,     u0, v1,texture->alpha,
        x + w, y - h, u1, v0,texture->alpha,
        x,     y,     u0, v1,texture->alpha,
        x + w, y,     u1, v1,texture->alpha,
        x + w, y - h, u1, v0,texture->alpha
    };
    constexpr int N = 30;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, verticles, N * sizeof(float));
}

void MT::Renderer::RenderCopyEX(const Rect& rect, const Texture* texture, const bool flip, const float rotation) {
    if (!texture) { return; }
    if (currentTexture != texture->texture) {
        RenderPresent(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyId) {
        RenderPresent(false);
        currentProgram = renderCopyId;
        glUseProgram(renderCopyId);
    }
    currentSize = renderCopySize;

    float aspect = static_cast<float>(H) / static_cast<float>(W);
    float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    float w = (static_cast<float>(rect.w) / W) * 2.0f;
    float h = (static_cast<float>(rect.h) / H) * 2.0f * aspect;

    float halfW = w / 2.0f;
    float halfH = h / 2.0f;

    float rad = glm::radians(rotation);
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    glm::vec2 center = { x + halfW, y - halfH };

    glm::vec2 p0 = RotateAndTranslate2D(-halfW, -halfH, center, cosA, sinA);
    glm::vec2 p1 = RotateAndTranslate2D(-halfW, halfH, center, cosA, sinA);
    glm::vec2 p2 = RotateAndTranslate2D(halfW, halfH, center, cosA, sinA);
    glm::vec2 p3 = RotateAndTranslate2D(-halfW, -halfH, center, cosA, sinA);
    glm::vec2 p4 = RotateAndTranslate2D(halfW, halfH, center, cosA, sinA);
    glm::vec2 p5 = RotateAndTranslate2D(halfW, -halfH, center, cosA, sinA);

    if (flip) {
        const float vertex[] = {
            p0.x, p0.y, 1.0f, 0.0f, texture->alpha,
            p1.x, p1.y, 1.0f, 1.0f, texture->alpha,
            p2.x, p2.y, 0.0f, 1.0f, texture->alpha,
            p3.x, p3.y, 1.0f, 0.0f, texture->alpha,
            p4.x, p4.y, 0.0f, 1.0f, texture->alpha,
            p5.x, p5.y, 0.0f, 0.0f, texture->alpha
        };
        constexpr int N = 30;
        const size_t old = globalVertices.size();
        globalVertices.resize(old + N);
        std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
    }
    else {
        const float vertex[] = {
            p0.x, p0.y, 0.0f, 0.0f,texture->alpha,
            p1.x, p1.y, 0.0f, 1.0f,texture->alpha,
            p2.x, p2.y, 1.0f, 1.0f,texture->alpha,
            p3.x, p3.y, 0.0f, 0.0f,texture->alpha,
            p4.x, p4.y, 1.0f, 1.0f,texture->alpha,
            p5.x, p5.y, 1.0f, 0.0f,texture->alpha
        };
        constexpr int N = 30;
        const size_t old = globalVertices.size();
        globalVertices.resize(old + N);
        std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
    }
}

void MT::Renderer::RenderCopyPartEX(const Rect& rect, const Rect& source, const Texture* texture, const bool flip, const float rotation) {
    if (!texture) { return; }
    if (currentTexture != texture->texture) {
        RenderPresent(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyId) {
        RenderPresent(false);
        currentProgram = renderCopyId;
        glUseProgram(renderCopyId);
    }
    currentSize = renderCopySize;

    float aspect = static_cast<float>(H) / static_cast<float>(W);
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f *aspect;

    float halfW = w / 2.0f;
    float halfH = h / 2.0f;

    const float texW = static_cast<float>(texture->w);
    const float texH = static_cast<float>(texture->h);


    const float u0 = static_cast<float>(source.x) / texW;
    const float u1 = static_cast<float>(source.x + source.w) / texW;

    const float v1 = 1.0f - static_cast<float>(source.y) / texH;
    const float v0 = v1 - static_cast<float>(source.h) / texH;


    float rad = glm::radians(rotation);
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    glm::vec2 center = { x + halfW, y - halfH };

    glm::vec2 p0 = RotateAndTranslate2D(-halfW, -halfH, center, cosA, sinA);
    glm::vec2 p1 = RotateAndTranslate2D(-halfW, halfH, center, cosA, sinA);
    glm::vec2 p2 = RotateAndTranslate2D(halfW, halfH, center, cosA, sinA);
    glm::vec2 p3 = RotateAndTranslate2D(-halfW, -halfH, center, cosA, sinA);
    glm::vec2 p4 = RotateAndTranslate2D(halfW, halfH, center, cosA, sinA);
    glm::vec2 p5 = RotateAndTranslate2D(halfW, -halfH, center, cosA, sinA);

    if (flip) {
        const float vertex[] = {
            p0.x, p0.y, u1, v0,texture->alpha,
            p1.x, p1.y, u1, v1,texture->alpha,
            p2.x, p2.y, u0, v1,texture->alpha,
            p3.x, p3.y, u1, v0,texture->alpha,
            p4.x, p4.y, u0, v1,texture->alpha,
            p5.x, p5.y, u0, v0,texture->alpha
        };
        constexpr int N = 30;
        const size_t old = globalVertices.size();
        globalVertices.resize(old + N);
        std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
    }
    else {
        const float vertex[] = {
            p0.x, p0.y, u0, v0,texture->alpha,
            p1.x, p1.y, u0, v1,texture->alpha,
            p2.x, p2.y, u1, v1,texture->alpha,
            p3.x, p3.y, u0, v0,texture->alpha,
            p4.x, p4.y, u1, v1,texture->alpha,
            p5.x, p5.y, u1, v0,texture->alpha
        };

        constexpr int N = 30;
        const size_t old = globalVertices.size();
        globalVertices.resize(old + N);
        std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
    }
}


void MT::Renderer::RenderCopyCircle(const Rect& rect, const Texture* texture, const float radius) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    if (currentTexture != texture->texture) {
        RenderPresent(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyCircleId) {
        RenderPresent(false);
        currentProgram = renderCopyCircleId;
        glUseProgram(renderCopyCircleId);
    }

    currentSize = renderCopyCircleSize;

    // pos.x, pos.y,radius tex.u, tex.v, alpha
    float vertex[] = {
        x,     y - h,radius, 0.0f, 0.0f, texture->alpha,
        x,     y,    radius, 0.0f, 1.0f, texture->alpha,
        x + w, y - h,radius, 1.0f, 0.0f, texture->alpha,
        x,     y,    radius, 0.0f, 1.0f, texture->alpha,
        x + w, y,    radius, 1.0f, 1.0f, texture->alpha,
        x + w, y - h,radius, 1.0f, 0.0f, texture->alpha
    };
    constexpr int N = 36;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderCircle(const Rect& rect, const Color& col, const unsigned char alpha, const float radius) {
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    if (currentProgram != renderCircleId) {
        RenderPresent(false);
        currentProgram = renderCircleId;
        glUseProgram(renderCircleId);
    }

    currentSize = renderCircleSize;
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;

    const float fR = float(col.R) / 255;
    const float fG = float(col.G) / 255;
    const float fB = float(col.B) / 255;
    const float fA = float(alpha) / 255;


    // pos.x, pos.y, pos.z,radius  col.r, col.g, col.b col.a
    const float vertex[] = {
        x,     y - h, radius, fR, fG, fB, fA,
        x,     y    , radius, fR, fG, fB, fA,
        x + w, y - h, radius, fR, fG, fB, fA,
        x,     y    , radius, fR, fG, fB, fA,
        x + w, y    , radius, fR, fG, fB, fA,
        x + w, y - h, radius, fR, fG, fB, fA
    };

    constexpr int N = 42;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderRoundedRect(const Rect& rect, const Color& col, const unsigned char alpha) {
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    if (currentProgram != renderRoundedRectId) {
        RenderPresent(false);
        currentProgram = renderRoundedRectId;
        glUseProgram(currentProgram);
    }

    glm::vec2 rectPixelSize = { (float)rect.w,(float)rect.h };

    if (roundRectRadiusVal != rectPixelSize) {
        RenderPresent(false);
        roundRectRadiusVal = rectPixelSize;
        glUniform2f(roundRectRadius, rectPixelSize.x, rectPixelSize.y);
    }

    currentSize = renderRoundedSize; 
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;

    const float fR = float(col.R) / 255;
    const float fG = float(col.G) / 255;
    const float fB = float(col.B) / 255;
    const float fA = float(alpha) / 255;


    // pos.x, pos.y, pos.z,radius  col.r, col.g, col.b col.a
    const float vertex[] = {
        x,     y - h, fR, fG, fB, fA,
        x,     y    , fR, fG, fB, fA,
        x + w, y - h, fR, fG, fB, fA,
        x,     y    , fR, fG, fB, fA,
        x + w, y    , fR, fG, fB, fA,
        x + w, y - h, fR, fG, fB, fA
    };

    constexpr int N = 36;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderCopyRoundedRect(const MT::Rect& rect, const MT::Texture* texture) {
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    if (currentProgram != renderCopyRoundedRectId) {
        RenderPresent(false);
        currentProgram = renderCopyRoundedRectId;
        glUseProgram(currentProgram);
    }
    if (currentTexture != texture->texture) {
        RenderPresent(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    glm::vec2 rectPixelSize = { (float)rect.w,(float)rect.h };

    if (roundRectCopyRadiusVal != rectPixelSize) {
        RenderPresent(false);
        roundRectCopyRadiusVal = rectPixelSize;
        glUniform2f(roundRectCopyRadius, rectPixelSize.x, rectPixelSize.y);
    }

    currentSize = renderCopyRoundedSize;
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;


    const float vertex[] = {
        x,     y - h, 0.0f, 0.0f,texture->alpha,
        x,     y,     0.0f, 1.0f,texture->alpha,
        x + w, y - h, 1.0f, 0.0f,texture->alpha,
        x,     y,     0.0f, 1.0f,texture->alpha,
        x + w, y,     1.0f, 1.0f,texture->alpha,
        x + w, y - h, 1.0f, 0.0f,texture->alpha
    };

    constexpr int N = 30;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}


void MT::Renderer::RenderCopyFiltered(const Rect& rect, const Texture* texture, const Color& filter) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    // aktywacja tekstury
    if (currentTexture != texture->texture) {
        RenderPresent(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyFilterId) {
        RenderPresent(false);  
        currentProgram = renderCopyFilterId;
        glUseProgram(renderCopyFilterId);
    }
    currentSize = renderFilteredSize;
    const float fR = float(filter.R) / 255;
    const float fG = float(filter.G) / 255;
    const float fB = float(filter.B) / 255;

    // pos.x, pos.y, tex.u, tex.v col.r,col.g,col.b
    const float vertex[] = {
        x,     y - h, 0.0f, 0.0f, fR, fG, fB, texture->alpha,
        x,     y,     0.0f, 1.0f, fR, fG, fB, texture->alpha,
        x + w, y - h, 1.0f, 0.0f, fR, fG, fB, texture->alpha,
        x,     y,     0.0f, 1.0f, fR, fG, fB, texture->alpha,
        x + w, y,     1.0f, 1.0f, fR, fG, fB, texture->alpha,
        x + w, y - h, 1.0f, 0.0f, fR, fG, fB, texture->alpha
    };
    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderCopyPartFiltered(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;


    if (currentTexture != texture->texture) {
        RenderPresent(false);  
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyFilterId) {
        RenderPresent(false);
        currentProgram = renderCopyFilterId;
        glUseProgram(renderCopyFilterId);
    }
    currentSize = renderFilteredSize;
    const float fR = float(filter.R) / 255;
    const float fG = float(filter.G) / 255;
    const float fB = float(filter.B) / 255;

    const float tempSourceX = static_cast<float>(source.x) / texture->w;
    const float tempSourceY = static_cast<float>(source.y) / texture->h;
    const float tempSourceW = static_cast<float>(source.w) / texture->w;
    const float tempSourceH = static_cast<float>(source.h) / texture->h;

    const float u0 = tempSourceX;
    const float u1 = tempSourceX + tempSourceW;
    const float v1 = 1.0f - tempSourceY;
    const float v0 = v1 - tempSourceH;

    // pos.x, pos.y, tex.u, tex.v col.r,col.g,col.b
    const float vertex[] = {
        x,     y - h, u0, v0, fR, fG, fB, texture->alpha,
        x,     y,     u0, v1, fR, fG, fB, texture->alpha,
        x + w, y - h, u1, v0, fR, fG, fB, texture->alpha,
        x,     y,     u0, v1, fR, fG, fB, texture->alpha,
        x + w, y,     u1, v1, fR, fG, fB, texture->alpha,
        x + w, y - h, u1, v0, fR, fG, fB, texture->alpha
    };
    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}



void MT::Renderer::RenderPresent(bool switchContext) {
    if (globalVertices.empty()) {
        if (switchContext) { SDL_GL_SwapWindow(window); }
        return;
    }  

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, globalVertices.size() * sizeof(float), globalVertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, globalVertices.size()/currentSize);

    globalVertices.clear();

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
    renderCopyId = 0;
    renderRectId = 0;
    renderCopyCircleId = 0;
    renderCircleId = 0;
    renderCopyFilterId = 0;


    globalVertices.clear();
    globalVertices.shrink_to_fit();

    SDL_GL_DeleteContext(context);
}

void MT::Renderer::Resize(const unsigned int w, const unsigned int h) {
    W = w;
    H = h;
    vievPort.w = W;
    vievPort.h = H;
    SDL_GL_GetDrawableSize(window, &W, &H);
    glViewport(0, 0, W, H);
}

void MT::Renderer::AgresiveRenderCopySetUp() {
    agresiveRenderVec.resize(TexMan::GetTexturesAmount() +1);
}

void MT::Renderer::AgressiveRenderCopy(const Rect& rect, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    //    // pos.x, pos.y tex.u, tex.v
    float verticles[] = {
        x,     y - h, 0.0f, 0.0f,texture->alpha,
        x,     y,     0.0f, 1.0f,texture->alpha,
        x + w, y - h, 1.0f, 0.0f,texture->alpha,
        x,     y,     0.0f, 1.0f,texture->alpha,
        x + w, y,     1.0f, 1.0f,texture->alpha,
        x + w, y - h, 1.0f, 0.0f,texture->alpha
    };

    constexpr int N = 30;
    std::vector<float>& vec = agresiveRenderVec[texture->texture];
    const size_t old = vec.size();
    vec.resize(old + N);
    std::memcpy(vec.data() + old, verticles, N * sizeof(float));
}

void MT::Renderer::AgressiveRenderCopyPresent(bool clearVectors) {
    const unsigned int prevProgram = currentProgram;

    glUseProgram(renderCopyId);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    for (size_t i = 0; i < agresiveRenderVec.size(); i++) {
        std::vector<float>& vec = agresiveRenderVec[i];
        if (vec.empty()) { continue; }
        glBindTexture(GL_TEXTURE_2D, i);
        glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(float),  vec.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, vec.size() / renderCopySize);
        vec.clear();
    }

    if (prevProgram) glUseProgram(prevProgram);

}


void MT::Renderer::SetClipSize(const MT::Rect& rect) {
    RenderPresent(false);
    glEnable(GL_SCISSOR_TEST);
    glScissor(rect.x, rect.y, rect.w, rect.h);
    
}

void MT::Renderer::ResetClipSize() {
    RenderPresent(false);
    glScissor(0, 0, W, H);
    glDisable(GL_SCISSOR_TEST);
}