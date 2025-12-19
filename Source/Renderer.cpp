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

SDL_GLContext MT::Innit(SDL_Window* window) {

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
        glGenerateMipmap(GL_TEXTURE_2D);
        SDL_FreeSurface(flipped);
    }
    SDL_FreeSurface(surf);

    return metTex;
}

void MT::DeleteTexture(Texture*& tex) { 
    if (tex == nullptr) { return; } 
    glDeleteTextures(1, &tex->texture); 
    tex->texture = 0; 
    tex->w = 0; 
    tex->h = 0; 
    delete tex; 
    tex = nullptr; }

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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // powierzchnie + RGBA RenderRectangle

    //Flat Copy
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // tylko powerzchnia
    //Render Copy Base
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // powierzchnie + aplha bez tekstur

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

    //URP Shader
    glVertexAttribPointer(13, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Nie zdefiniowane ale raczej pozycje + coś
    glVertexAttribPointer(14, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Texture lub kolory
    glVertexAttribPointer(15, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Bonus jakis


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);
    glEnableVertexAttribArray(8);
    glEnableVertexAttribArray(9);
    glEnableVertexAttribArray(10);
    glEnableVertexAttribArray(11);
    glEnableVertexAttribArray(12);

    glEnableVertexAttribArray(13);
    glEnableVertexAttribArray(14);
    glEnableVertexAttribArray(15);


    LoadShaders();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderCopyId = loader.GetProgram("RenderCopy");
    renderBaseId = loader.GetProgram("RenderCopyBase");
    flatRenderCopyId = loader.GetProgram("FlatCopyBase");
    renderRectId = loader.GetProgram("RenderRect");
    renderCopyCircleId = loader.GetProgram("RenderCopyCircle");
    renderCircleId = loader.GetProgram("RenderCircle");
    renderCopyFilterId = loader.GetProgram("RenderCopyFilter");
    renderRoundedRectId = loader.GetProgram("RenderRoundedRectangle");
    renderCopyRoundedRectId = loader.GetProgram("RenderCopyRoundedRectangle");
    uprId = loader.GetProgram("RenderUPR");

    roundRectRadius = glGetUniformLocation(renderRoundedRectId, "uPixelSize");
    roundRectCopyRadius = glGetUniformLocation(renderCopyRoundedRectId, "uPixelSize");
    return true;
}

void MT::Renderer::LoadShaders() {
    if (!loader.IsProgram("RenderRect")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aPosRGBA;

        out vec4 ourColor;
        out float vAlpha;

        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        void main() {
            gl_Position = vec4(aPosRGBA.xy,0.0, 1.0);
	        vec2 vRG = unpackHalfColor(aPosRGBA.z);
	        vec2 vBA = unpackHalfColor(aPosRGBA.w);
            ourColor.xy = vRG;
	        ourColor.zw = vBA;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec4 ourColor;

        void main(){
	        FragColor = vec4(ourColor.xyz,1.0 * ourColor.w);
        }
        )glsl";

        loader.CreateProgramStr("RenderRect", vertexStr, fragmentStr);
    }

    if (!loader.IsProgram("FlatCopyBase")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout (location = 1) in vec2 aPos;

        vec2 uvFromVertexID(int id) {
            if      (id == 0) return vec2(0.0, 0.0);
            else if (id == 1) return vec2(0.0, 1.0);
            else if (id == 2) return vec2(1.0, 0.0);
            else if (id == 3) return vec2(0.0, 1.0);
            else if (id == 4) return vec2(1.0, 1.0);
            else              return vec2(1.0, 0.0);
        }
        out vec2 vUV;

        void main(){
	        gl_Position = vec4(aPos.xy, 0.0 ,1.0);
            vUV = uvFromVertexID(gl_VertexID % 6);
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec2 vUV;

        uniform sampler2D texture1;

        void main(){
	        vec4 texcolor = texture(texture1,vUV);
	        FragColor = texcolor;
        }
        )glsl";

        loader.CreateProgramStr("FlatCopyBase", vertexStr, fragmentStr);
    }

    if (!loader.IsProgram("RenderCopyBase")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout (location = 2) in vec3 aPosA;

        vec2 uvFromVertexID(int id) {
            if      (id == 0) return vec2(0.0, 0.0);
            else if (id == 1) return vec2(0.0, 1.0);
            else if (id == 2) return vec2(1.0, 0.0);
            else if (id == 3) return vec2(0.0, 1.0);
            else if (id == 4) return vec2(1.0, 1.0);
            else              return vec2(1.0, 0.0);
        }
        out vec2 texCord;
        out float vAlpha;

        void main(){
	        gl_Position = vec4(aPosA.xy, 0.0 ,1.0);
            texCord = uvFromVertexID(gl_VertexID % 6);
            vAlpha = aPosA.z;
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

        loader.CreateProgramStr("RenderCopyBase", vertexStr, fragmentStr);
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

    if (!loader.IsProgram("RenderUPR")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core

        layout(location = 13) in vec3 vVecOne;
        layout(location = 14) in vec3 vVecTwo;
        layout(location = 15) in vec2 vVecThree; // vVecThree.y is always shader id

        out vec4 oOutVec;
        out vec4 oOutVecTwo;
        flat out int oShaderId;

        vec2 uvFromVertexID(int id) {
	        switch(id){
		        case 0: 
			        return vec2(0.0, 0.0);
			        break;
		        case 1: 
			        return vec2(0.0, 1.0);
			        break;
		        case 2: 
			        return vec2(1.0, 0.0);
			        break;
		        case 3: 
			        return vec2(0.0, 1.0);
			        break;
		        case 4: 
			        return vec2(1.0, 1.0);
			        break;
		        case 5: 
			        return vec2(1.0, 0.0);
			        break;
		        default:
			        return vec2(1.0, 0.0);
			        break;
	        }
        }

        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        void main(){
	        int vShaderId = int(vVecThree.y);
	        switch(vShaderId){
		        case 1:{ // Render Rect
			        gl_Position = vec4(vVecOne.x, vVecOne.y, 0.0, 1.0);
			        vec2 rg = unpackHalfColor(vVecOne.z);
			        vec2 ba = unpackHalfColor(vVecTwo.x);
			        oOutVec = vec4(rg.x, rg.y , ba.x, ba.y);
			        break;
			        }
		        case 2: // Render Copy
			        gl_Position = vec4(vVecOne.x, vVecOne.y,0.0 ,1.0);
			        oOutVec = vec4(vVecOne.z,vVecTwo.xy, 0.0);
			        break;
		        case 3: // Render Copy Circle
			        gl_Position = vec4(vVecOne.xy, 0.0, 1.0);
			        oOutVec = vec4(vVecOne.z, vVecTwo); // Two .xy = texCord // z vecTwo = alpha //z VecOne = radius 
			        break;
		        case 4:{ // Render Circle
                    gl_Position = vec4(vVecOne.xy, 0.0, 1.0);
			        vec2 rg = unpackHalfColor(vVecTwo.x);
			        vec2 ba = unpackHalfColor(vVecTwo.y);
			        oOutVec.rg = rg;
			        oOutVec.ba = ba;
                    oOutVecTwo.xy = uvFromVertexID(gl_VertexID % 6);
                    oOutVecTwo.z = vVecOne.z;
			        break;
			        }
		        case 5:{ // RenderRoundedRectangle
			        gl_Position = vec4(vVecOne.xy, 0.0, 1.0);
			        vec2 rg = unpackHalfColor(vVecOne.z);
			        vec2 ba = unpackHalfColor(vVecTwo.x);
                    oOutVec.rg = rg;
			        oOutVec.ba = ba;
                    oOutVecTwo.xy = uvFromVertexID(gl_VertexID % 6);
			        oOutVecTwo.zw = vVecTwo.yz;
			        break;
			        }
		        case 6:  // RenderCopyRoundedRectangle
			        gl_Position = vec4(vVecOne.xy, 0.0, 1.0);
			        oOutVec.x = vVecOne.z;      // UV.x
			        oOutVec.yz = vVecTwo.xy;    // UV.y, Alpha
			        oOutVecTwo.x = vVecTwo.z;   // Width
			        oOutVecTwo.y = vVecThree.x; // Height
			        break;
		        case 7:{ // Render Copy Filter
	                gl_Position = vec4(vVecOne.xy, 0.0 ,1.0);
			        oOutVec.x = vVecOne.z; // u
			        oOutVec.y = vVecTwo.x; // v
			        vec2 rg = unpackHalfColor(vVecTwo.y);
			        oOutVec.z = rg.r; //r
			        oOutVecTwo.x = rg.g; //g
			        oOutVecTwo.y = vVecTwo.z; //b
			        oOutVecTwo.z = vVecThree.x; //a
			        break;
		        }
	        }
	        oShaderId = vShaderId;
        }

        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        in vec4 oOutVec;
        in vec4 oOutVecTwo;
        flat in int oShaderId;

        out vec4 FragColor;

        uniform sampler2D texture1; // Only one texture slot used so uniform switch never nedded

        float roundedBoxSDF(vec2 p, vec2 size, float r){
            vec2 d = abs(p - size * 0.5) - (size * 0.5 - vec2(r));
            return length(max(d, 0.0)) - r;
        }

        void main(){
	        switch(oShaderId){
		        case 1: // Render Rect
			        FragColor = oOutVec;
			        break;
		        case 2:{ // Render Copy
			        vec4 texcolor = texture(texture1,oOutVec.xy);
	                texcolor.a *= oOutVec.z;
	                FragColor = texcolor;
			        break;
                    }
		        case 3:{ // Render Copy Circle
		            vec2 center = vec2(0.5, 0.5);

                    float dist = distance(oOutVec.yz, center);
                    if (dist > oOutVec.x)
                        discard;

                    vec4 texColor = texture(texture1, oOutVec.yz);
                    FragColor = vec4(texColor.rgb, texColor.a *  oOutVec.a);
			        break;
			        }
		        case 4:{ // Render Circle
                    vec2 center = vec2(0.5, 0.5);
                    float dist = distance(oOutVecTwo.xy, center);
                    if (dist > oOutVecTwo.z)
                        discard;

                    FragColor = oOutVec;
			        break;
			        }
		        case 5:{ // Render Rounded Rectangle
                    vec2 size = oOutVecTwo.zw; 
                    vec2 pPx = oOutVecTwo.xy * size;
                    float d = roundedBoxSDF(pPx, size, 8.0); // 8.0 is the size of a curve if nedded the change uniform is requied
                    float alpha = 1.0 - smoothstep(0.0, 1.0, d);

                    if (alpha <= 0.001) discard;

                    FragColor = vec4(oOutVec.rgb, oOutVec.a * alpha);
                    break;
                    }

		        case 6:{ // RenderCopyRoundedRectangle
                    vec2 size = oOutVecTwo.xy; // Width, Height
                    vec2 pPx = oOutVec.xy * size;
                    float d = roundedBoxSDF(pPx, size, 8.0); // 8.0 is the size of a curve if nedded the change uniform is requied

                    float alpha = 1.0 - smoothstep(0.0, 1.0, d);

                    if (alpha <= 0.001) discard;
            
           	        vec4 texcolor = texture(texture1,oOutVec.xy);
	                texcolor.a *= oOutVec.z;
	                FragColor = texcolor;
			        break;
                    }
		        case 7:{ // RenderCopy Filter
	                vec4 texcolor = texture(texture1,oOutVec.xy);
                    texcolor.r *= oOutVec.z; 
                    texcolor.g *= oOutVecTwo.x; 
                    texcolor.b *= oOutVecTwo.y; 
	                texcolor.a *= oOutVecTwo.z; 
	                FragColor = texcolor;
			        break;
                    }
	        }
        }
        )glsl";

        loader.CreateProgramStr("RenderUPR", vertexStr, fragmentStr);
    }
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
        currentProgram = renderRectId;
        glUseProgram(renderRectId);
    }
    currentSize = renderRectSize;
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;

    uint16_t iRG = col.R;
    iRG <<= 8;
    iRG += col.G;
    uint16_t iBA = col.B;
    iBA <<= 8;
    iBA += alpha;
    const float fRG = iRG;
    const float fBA = iBA;

    // pos.x, pos.y, col.r, col.g, col.b
    const float vertices[] = {
        x,     y - h, fRG, fBA,
        x,     y,     fRG, fBA,
        x + w, y - h, fRG, fBA,
        x,     y,     fRG, fBA,
        x + w, y,     fRG, fBA,
        x + w, y - h, fRG, fBA
    };
    constexpr int N = 24;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertices, N * sizeof(float));
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

    if (currentProgram != renderRectId) {
        Present(false);
        currentProgram = renderRectId;
        glUseProgram(renderRectId);
    }

    currentSize = renderRectSize;
    const float halfW = rect.w * 0.5f;
    const float halfH = rect.h * 0.5f;

    uint16_t iRG = col.R;
    iRG <<= 8;
    iRG += col.G;
    uint16_t iBA = col.B;
    iBA <<= 8;
    iBA += alpha;
    const float fRG = iRG;
    const float fBA = iBA;

    const float rad = glm::radians(rotation);
    const float cosA = cosf(rad);
    const float sinA = sinf(rad);

    glm::vec2 centerPx = { rect.x + halfW, rect.y + halfH };

    const glm::vec2 p0 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p1 = RotateNdc(-halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p2 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p3 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p4 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p5 = RotateNdc(halfW, -halfH, centerPx, cosA, sinA, W, H);

    const float vertices[] = {
        p0.x, p0.y, fRG, fBA,
        p1.x, p1.y, fRG, fBA,
        p2.x, p2.y, fRG, fBA,
        p3.x, p3.y, fRG, fBA,
        p4.x, p4.y, fRG, fBA,
        p5.x, p5.y, fRG, fBA
    };
    constexpr int N = 24;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertices, N * sizeof(float));
}


void MT::Renderer::DrawLine(const int x1, const int y1, const int x2, const int y2, const int thickness,
    const Color& col, const unsigned char alpha) {

    if (currentProgram != renderRectId) {
        Present(false);
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

    uint16_t iRG = col.R;
    iRG <<= 8;
    iRG += col.G;
    uint16_t iBA = col.B;
    iBA <<= 8;
    iBA += alpha;
    const float fRG = iRG;
    const float fBA = iBA;

    const float vertices[] = {
        v0.x, v0.y, fRG, fBA,
        v1.x, v1.y, fRG, fBA,
        v2.x, v2.y, fRG, fBA,
        v0.x, v0.y, fRG, fBA,
        v2.x, v2.y, fRG, fBA,
        v3.x, v3.y, fRG, fBA
    };
    constexpr int N = 24;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertices, N * sizeof(float));
}


void MT::Renderer::RenderCopyAS(const Rect& rect, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderBaseId) {
        Present(false);
        currentProgram = renderBaseId;
        glUseProgram(currentProgram);
    }
    currentSize = renderCopyBaseSize;

    //    // pos.x, pos.y tex.u, tex.v
    const float verticles[] = {
        x,     y - h ,texture->alpha,
        x,     y     ,texture->alpha,
        x + w, y - h ,texture->alpha,
        x,     y     ,texture->alpha,
        x + w, y     ,texture->alpha,
        x + w, y - h ,texture->alpha
    };

    constexpr int N = 18;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, verticles, N * sizeof(float));
}

void MT::Renderer::RenderCopy(const Rect& rect, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyId) {
        Present(false);
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

void MT::Renderer::RenderCopyPart(const Rect& rect, const Rect& source, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;


    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyId) {
        Present(false);
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
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyId) {
        Present(false);
        currentProgram = renderCopyId;
        glUseProgram(renderCopyId);
    }
    currentSize = renderCopySize;

    float halfW = rect.w * 0.5f;
    float halfH = rect.h * 0.5f;

    const float rad = glm::radians(rotation);
    const float cosA = cosf(rad);
    const float sinA = sinf(rad);

    glm::vec2 centerPx = { rect.x + halfW, rect.y + halfH };

    const glm::vec2 p0 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p1 = RotateNdc(-halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p2 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p3 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p4 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p5 = RotateNdc(halfW, -halfH, centerPx, cosA, sinA, W, H);

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
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyId) {
        Present(false);
        currentProgram = renderCopyId;
        glUseProgram(renderCopyId);
    }
    currentSize = renderCopySize;


    float halfW = rect.w * 0.5f;
    float halfH = rect.h * 0.5f;

    const float rad = glm::radians(rotation);
    const float cosA = cosf(rad);
    const float sinA = sinf(rad);

    glm::vec2 centerPx = { rect.x + halfW, rect.y + halfH };

    const glm::vec2 p0 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p1 = RotateNdc(-halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p2 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p3 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p4 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p5 = RotateNdc(halfW, -halfH, centerPx, cosA, sinA, W, H);

    const float texW = static_cast<float>(texture->w);
    const float texH = static_cast<float>(texture->h);


    const float u0 = static_cast<float>(source.x) / texW;
    const float u1 = static_cast<float>(source.x + source.w) / texW;

    const float v1 = 1.0f - static_cast<float>(source.y) / texH;
    const float v0 = v1 - static_cast<float>(source.h) / texH;


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
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyCircleId) {
        Present(false);
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
        Present(false);
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
        Present(false);
        currentProgram = renderRoundedRectId;
        glUseProgram(currentProgram);
    }

    glm::vec2 rectPixelSize = { (float)rect.w,(float)rect.h };

    if (roundRectRadiusVal != rectPixelSize) {
        Present(false);
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
        Present(false);
        currentProgram = renderCopyRoundedRectId;
        glUseProgram(currentProgram);
    }
    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    glm::vec2 rectPixelSize = { (float)rect.w,(float)rect.h };

    if (roundRectCopyRadiusVal != rectPixelSize) {
        Present(false);
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
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyFilterId) {
        Present(false);
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
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != renderCopyFilterId) {
        Present(false);
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

void MT::Renderer::RenderRectUPR(const Rect& rect, const Color& col, const int alpha) {
    if (!vievPort.IsColliding(rect)) { return; }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;

    uint16_t iRG = col.R;
    iRG <<= 8;
    iRG += col.G;
    uint16_t iBA = col.B;
    iBA <<= 8;
    iBA += alpha;
    const float fRG = iRG;
    const float fBA = iBA;

    // pos.x, pos.y, col.r, col.g, col.b col.a trashVal1 trashVal2 shaderID
    const float vertices[] = {
        x,     y - h, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        x,     y,     fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        x + w, y - h, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        x,     y,     fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        x + w, y,     fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        x + w, y - h, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertices, N * sizeof(float));
}

void MT::Renderer::RenderRectEXUPR(const Rect& rect, const Color& col, const float rotation, const int alpha) {
    if (!vievPort.IsColliding(rect)) { return; }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }

    currentSize = renderUPRSize;
    const float halfW = rect.w * 0.5f;
    const float halfH = rect.h * 0.5f;

    uint16_t iRG = col.R;
    iRG <<= 8;
    iRG += col.G;
    uint16_t iBA = col.B;
    iBA <<= 8;
    iBA += alpha;
    const float fRG = iRG;
    const float fBA = iBA;

    const float rad = glm::radians(rotation);
    const float cosA = cosf(rad);
    const float sinA = sinf(rad);

    const glm::vec2 centerPx = { rect.x + halfW, rect.y + halfH };

    const glm::vec2 p0 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p1 = RotateNdc(-halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p2 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p3 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p4 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p5 = RotateNdc(halfW, -halfH, centerPx, cosA, sinA, W, H);

    const float vertices[] = {
        p0.x, p0.y, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        p1.x, p1.y, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        p2.x, p2.y, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        p3.x, p3.y, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        p4.x, p4.y, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
        p5.x, p5.y, fRG, fBA, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertices, N * sizeof(float));
}

void MT::Renderer::RenderCopyUPR(const Rect& rect, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) { return; }

    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;

    //    // pos.x, pos.y tex.u, tex.v
    const float verticles[] = {
        x,     y - h, 0.0f, 0.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
        x,     y,     0.0f, 1.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
        x + w, y - h, 1.0f, 0.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
        x,     y,     0.0f, 1.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
        x + w, y,     1.0f, 1.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
        x + w, y - h, 1.0f, 0.0f,texture->alpha, 0.0f, 0.0f, 2.0f
    };

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, verticles, N * sizeof(float));
}

void MT::Renderer::RenderCopyPartUPR(const Rect& rect, const Rect& source, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;


    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;

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
        x,     y - h, u0, v0,texture->alpha, 0.0f, 0.0f, 2.0f,
        x,     y,     u0, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
        x + w, y - h, u1, v0,texture->alpha, 0.0f, 0.0f, 2.0f,
        x,     y,     u0, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
        x + w, y,     u1, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
        x + w, y - h, u1, v0,texture->alpha, 0.0f, 0.0f, 2.0f
    };
    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, verticles, N * sizeof(float));
}

void MT::Renderer::RenderCopyEXUPR(const Rect& rect, const Texture* texture, const bool flip, const float rotation) {
    if (!texture) { return; }
    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;

    float halfW = rect.w * 0.5f;
    float halfH = rect.h * 0.5f;

    const float rad = glm::radians(rotation);
    const float cosA = cosf(rad);
    const float sinA = sinf(rad);

    glm::vec2 centerPx = { rect.x + halfW, rect.y + halfH };

    const glm::vec2 p0 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p1 = RotateNdc(-halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p2 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p3 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p4 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p5 = RotateNdc(halfW, -halfH, centerPx, cosA, sinA, W, H);

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    if (flip) {
        const float vertex[] = {
            p0.x, p0.y, 1.0f, 0.0f, texture->alpha, 0.0f, 0.0f, 2.0f,
            p1.x, p1.y, 1.0f, 1.0f, texture->alpha, 0.0f, 0.0f, 2.0f,
            p2.x, p2.y, 0.0f, 1.0f, texture->alpha, 0.0f, 0.0f, 2.0f,
            p3.x, p3.y, 1.0f, 0.0f, texture->alpha, 0.0f, 0.0f, 2.0f,
            p4.x, p4.y, 0.0f, 1.0f, texture->alpha, 0.0f, 0.0f, 2.0f,
            p5.x, p5.y, 0.0f, 0.0f, texture->alpha, 0.0f, 0.0f, 2.0f
        };
        std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
    }
    else {
        const float vertex[] = {
            p0.x, p0.y, 0.0f, 0.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
            p1.x, p1.y, 0.0f, 1.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
            p2.x, p2.y, 1.0f, 1.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
            p3.x, p3.y, 0.0f, 0.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
            p4.x, p4.y, 1.0f, 1.0f,texture->alpha, 0.0f, 0.0f, 2.0f,
            p5.x, p5.y, 1.0f, 0.0f,texture->alpha, 0.0f, 0.0f, 2.0f
        };
        std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
    }
}

void MT::Renderer::RenderCopyPartEXUPR(const Rect& rect, const Rect& source, const Texture* texture, const bool flip, const float rotation) {
    if (!texture) { return; }
    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;


    float halfW = rect.w * 0.5f;
    float halfH = rect.h * 0.5f;

    const float rad = glm::radians(rotation);
    const float cosA = cosf(rad);
    const float sinA = sinf(rad);

    glm::vec2 centerPx = { rect.x + halfW, rect.y + halfH };

    const glm::vec2 p0 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p1 = RotateNdc(-halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p2 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p3 = RotateNdc(-halfW, -halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p4 = RotateNdc(halfW, halfH, centerPx, cosA, sinA, W, H);
    const glm::vec2 p5 = RotateNdc(halfW, -halfH, centerPx, cosA, sinA, W, H);

    const float texW = static_cast<float>(texture->w);
    const float texH = static_cast<float>(texture->h);


    const float u0 = static_cast<float>(source.x) / texW;
    const float u1 = static_cast<float>(source.x + source.w) / texW;

    const float v1 = 1.0f - static_cast<float>(source.y) / texH;
    const float v0 = v1 - static_cast<float>(source.h) / texH;

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    if (flip) {
        const float vertex[] = {
            p0.x, p0.y, u1, v0,texture->alpha, 0.0f, 0.0f, 2.0f,
            p1.x, p1.y, u1, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
            p2.x, p2.y, u0, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
            p3.x, p3.y, u1, v0,texture->alpha, 0.0f, 0.0f, 2.0f,
            p4.x, p4.y, u0, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
            p5.x, p5.y, u0, v0,texture->alpha, 0.0f, 0.0f, 2.0f
        };
        std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
    }
    else {
        const float vertex[] = {
            p0.x, p0.y, u0, v0,texture->alpha, 0.0f, 0.0f, 2.0f,
            p1.x, p1.y, u0, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
            p2.x, p2.y, u1, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
            p3.x, p3.y, u0, v0,texture->alpha, 0.0f, 0.0f, 2.0f,
            p4.x, p4.y, u1, v1,texture->alpha, 0.0f, 0.0f, 2.0f,
            p5.x, p5.y, u1, v0,texture->alpha, 0.0f, 0.0f, 2.0f
        };
        std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
    }
}

void MT::Renderer::RenderCopyCircleUPR(const Rect& rect, const Texture* texture, const float radius) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) { return; }

    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;

    // pos.x, pos.y,radius tex.u, tex.v, alpha
    float vertex[] = {
        x,     y - h,radius, 0.0f, 0.0f, texture->alpha, 0.0f, 3.0f,
        x,     y,    radius, 0.0f, 1.0f, texture->alpha, 0.0f, 3.0f,
        x + w, y - h,radius, 1.0f, 0.0f, texture->alpha, 0.0f, 3.0f,
        x,     y,    radius, 0.0f, 1.0f, texture->alpha, 0.0f, 3.0f,
        x + w, y,    radius, 1.0f, 1.0f, texture->alpha, 0.0f, 3.0f,
        x + w, y - h,radius, 1.0f, 0.0f, texture->alpha, 0.0f, 3.0f
    };

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderCircleUPR(const Rect& rect, const Color& col, const unsigned char alpha, const float radius) {
    if (!vievPort.IsColliding(rect)) { return; }
    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;

    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;

    uint16_t iRG = col.R;
    iRG <<= 8;
    iRG += col.G;
    uint16_t iBA = col.B;
    iBA <<= 8;
    iBA += alpha;
    const float fRG = iRG;
    const float fBA = iBA;


    // pos.x, pos.y, pos.z,radius  col.r, col.g, col.b col.a
    const float vertex[] = {
        x,     y - h, radius, fRG, fBA, 0.0f, 0.0f, 4.0f,
        x,     y    , radius, fRG, fBA, 0.0f, 0.0f, 4.0f,
        x + w, y - h, radius, fRG, fBA, 0.0f, 0.0f, 4.0f,
        x,     y    , radius, fRG, fBA, 0.0f, 0.0f, 4.0f,
        x + w, y    , radius, fRG, fBA, 0.0f, 0.0f, 4.0f,
        x + w, y - h, radius, fRG, fBA, 0.0f, 0.0f, 4.0f
    };

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderRoundedRectUPR(const Rect& rect, const Color& col, const unsigned char alpha) {
    if (!vievPort.IsColliding(rect)) { return; }

    const glm::vec2 rectPixelSize = { (float)rect.w,(float)rect.h };

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }

    currentSize = renderUPRSize;
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;

    uint16_t iRG = col.R;
    iRG <<= 8;
    iRG += col.G;
    uint16_t iBA = col.B;
    iBA <<= 8;
    iBA += alpha;
    const float fRG = iRG;
    const float fBA = iBA;

    const float fW = (float)rect.w;
    const float fH = (float)rect.h;

    // pos.x, pos.y, pos.z,radius  col.r, col.g, col.b col.a
    const float vertex[] = {
        x,     y - h, fRG, fBA, fW, fH, 0.0f,  5.0f,
        x,     y    , fRG, fBA, fW, fH, 0.0f,  5.0f,
        x + w, y - h, fRG, fBA, fW, fH, 0.0f,  5.0f,
        x,     y    , fRG, fBA, fW, fH, 0.0f,  5.0f,
        x + w, y    , fRG, fBA, fW, fH, 0.0f,  5.0f,
        x + w, y - h, fRG, fBA, fW, fH, 0.0f,  5.0f
    };

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderCopyRoundedRectUPR(const MT::Rect& rect, const MT::Texture* texture) {
    if (!vievPort.IsColliding(rect)) { return; }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    currentSize = renderUPRSize;
    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;

    const float fW = (float)rect.w;
    const float fH = (float)rect.h;

    const float vertex[] = {
        x,     y - h, 0.0f, 0.0f,texture->alpha, fW, fH, 6.0f,
        x,     y,     0.0f, 1.0f,texture->alpha, fW, fH, 6.0f,
        x + w, y - h, 1.0f, 0.0f,texture->alpha, fW, fH, 6.0f,
        x,     y,     0.0f, 1.0f,texture->alpha, fW, fH, 6.0f,
        x + w, y,     1.0f, 1.0f,texture->alpha, fW, fH, 6.0f,
        x + w, y - h, 1.0f, 0.0f,texture->alpha, fW, fH, 6.0f,
    };

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderCopyFilteredUPR(const Rect& rect, const Texture* texture, const Color& filter) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) { return; }

    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;

    uint16_t iRG = filter.R;
    iRG <<= 8;
    iRG += filter.G;
    const float fRG = iRG;
    const float fB = float(filter.B) / 255;

    // pos.x, pos.y, tex.u, tex.v col.r,col.g,col.b
    const float vertex[] = {
        x,     y - h, 0.0f, 0.0f, fRG, fB, texture->alpha, 7.0f,
        x,     y,     0.0f, 1.0f, fRG, fB, texture->alpha, 7.0f,
        x + w, y - h, 1.0f, 0.0f, fRG, fB, texture->alpha, 7.0f,
        x,     y,     0.0f, 1.0f, fRG, fB, texture->alpha, 7.0f,
        x + w, y,     1.0f, 1.0f, fRG, fB, texture->alpha, 7.0f,
        x + w, y - h, 1.0f, 0.0f, fRG, fB, texture->alpha, 7.0f,
    };

    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::RenderCopyPartFilteredUPR(const Rect& rect, const Rect& source, const Texture* texture, const Color& filter) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) { return; }

    const float x = (static_cast<float>(rect.x) / W) * 2.0f - 1.0f;
    const float y = 1.0f - (static_cast<float>(rect.y) / H) * 2.0f;
    const float w = (static_cast<float>(rect.w) / W) * 2.0f;
    const float h = (static_cast<float>(rect.h) / H) * 2.0f;

    if (currentTexture != texture->texture) {
        Present(false);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        currentTexture = texture->texture;
    }

    if (currentProgram != uprId) {
        Present(false);
        currentProgram = uprId;
        glUseProgram(currentProgram);
    }
    currentSize = renderUPRSize;
    uint16_t iRG = filter.R;
    iRG <<= 8;
    iRG += filter.G;
    const float fRG = iRG;
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
        x,     y - h, u0, v0, fRG, fB, texture->alpha, 7.0f,
        x,     y,     u0, v1, fRG, fB, texture->alpha, 7.0f,
        x + w, y - h, u1, v0, fRG, fB, texture->alpha, 7.0f,
        x,     y,     u0, v1, fRG, fB, texture->alpha, 7.0f,
        x + w, y,     u1, v1, fRG, fB, texture->alpha, 7.0f,
        x + w, y - h, u1, v0, fRG, fB, texture->alpha, 7.0f
    };
    constexpr int N = 48;
    const size_t old = globalVertices.size();
    globalVertices.resize(old + N);
    std::memcpy(globalVertices.data() + old, vertex, N * sizeof(float));
}

void MT::Renderer::Present(bool switchContext) {
    if (globalVertices.empty()) {
        if (switchContext) { SDL_GL_SwapWindow(window); }
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, globalVertices.size() * sizeof(float), globalVertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, globalVertices.size() / currentSize);

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
    flatRenderVec.clear();

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

void MT::Renderer::FLatRenderCopySetUp() {
    flatRenderVec.clear();
    int i = 0;
    for (auto& tex : TexMan::GetAllTex()) {
        tex.second->batchIndex = i++;
        flatRenderVec.emplace_back(tex.second->texture);
    }
}

void MT::Renderer::FLatRenderCopy(const Rect& rect, const Texture* texture) {
    if (!texture) { return; }
    if (!vievPort.IsColliding(rect)) {
        return;
    }
    const float x = (rect.x / static_cast<float>(W)) * 2.0f - 1.0f;
    const float y = 1.0f - (rect.y / static_cast<float>(H)) * 2.0f;
    const float w = (rect.w / static_cast<float>(W)) * 2.0f;
    const float h = (rect.h / static_cast<float>(H)) * 2.0f;

    // pos.x, pos.y
    const float verticles[] = {
        x,     y - h ,
        x,     y     ,
        x + w, y - h ,
        x,     y     ,
        x + w, y     ,
        x + w, y - h
    };

    constexpr int N = 12;
    std::vector<float>& vec = flatRenderVec[texture->batchIndex].vertices;
    const size_t old = vec.size();
    vec.resize(old + N);
    std::memcpy(vec.data() + old, verticles, N * sizeof(float));
}

void MT::Renderer::FLatRenderCopyPresent(bool clearVectors) {
    const unsigned int prevProgram = currentProgram;
    const unsigned int prevTexture = currentTexture;
    glUseProgram(flatRenderCopyId);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    for (auto& entry : flatRenderVec) {
        auto& vec = entry.vertices;
        if (vec.empty()) { continue; }
        glBindTexture(GL_TEXTURE_2D, entry.textureID);
        glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(float), vec.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, vec.size() / flatRenderCopySize);
        vec.clear();
    }
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    if (prevProgram) glUseProgram(prevProgram);
}


void MT::Renderer::SetClipSize(const MT::Rect& rect) {
    Present(false);
    glEnable(GL_SCISSOR_TEST);
    glScissor(rect.x, rect.y, rect.w, rect.h);

}

void MT::Renderer::ResetClipSize() {
    Present(false);
    glScissor(0, 0, W, H);
    glDisable(GL_SCISSOR_TEST);
}