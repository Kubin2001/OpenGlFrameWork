#include "Font.h"

#include <fstream>
#include <print>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "TextureManager.h"
#include "json.hpp"

struct Pixel {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

Font::Font(const std::string& name, MT::Texture* texture, const std::string& jsonPath) {
	this->name = name;
	this->texture = texture;
	LoadTextInfo(jsonPath);
}

Font::Font(const std::string& name, MT::Texture* texture, const std::string& charset, std::vector<MT::Rect>& rectangles) {
	this->name = name;
	this->texture = texture;
	LoadTextCharset(charset, rectangles);
}

std::string Font::GetName() {
	return name;
}

// Loads json file (can be created with most of font generator websides you can change max vector size to accept ASCI over 200)

bool Font::LoadTextInfo(const std::string& jsonPath) {
	sourceRectangles.resize(200);
	std::ifstream file(jsonPath);
	if (file.is_open()) {
		nlohmann::json jsonData;
		file >> jsonData;

		for (auto& [key, value] : jsonData.items()) {
			if (std::stoi(key) < sourceRectangles.size()) {
				sourceRectangles[std::stoi(key)].x = value["x"].get<int>();
				sourceRectangles[std::stoi(key)].y = value["y"].get<int>();
				sourceRectangles[std::stoi(key)].w = value["width"].get<int>();
				sourceRectangles[std::stoi(key)].h = value["height"].get<int>();
				if (standardInterLine < sourceRectangles[std::stoi(key)].h) {
					standardInterLine = sourceRectangles[std::stoi(key)].h;
				}
			}
		}

	}
	else{
		std::cout << "Error font json not loaded correctly!\n";
		return false;
	}
	return true;
}

void Font::LoadTextCharset(const std::string& charset ,std::vector<MT::Rect> &rectangles) {
	sourceRectangles.resize(200);
	for (size_t i = 0; i < charset.size(); i++) {
		if (charset[i] < 0) { return; }
		sourceRectangles[charset[i]] = rectangles[i];
	}
}


void Font::RenderRawText(MT::Renderer* renderer, const int x, const int y, const std::string& text, const int interline,
	const unsigned char R, const unsigned char G, const unsigned char B) {
	rectangle.x = x;
	rectangle.y = y;
	rectangle.w = 0;
	rectangle.h = 0;
	int temp = rectangle.x;
	for (int i = 0; i < text.length(); i++) {
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = (int)(sourceRectangles[text[i]].w);
				rectangle.h = (int)(sourceRectangles[text[i]].h);
				renderer->RenderCopyPartFiltered(rectangle, sourceRectangles[text[i]], texture,
					{ R,G,B });
				rectangle.x += (int)(sourceRectangles[text[i]].w) + 1;
			}
			else {
				rectangle.y += (int)(interline);
				rectangle.x = temp;
			}
		}
	}
}

// Basic text rendering with possition in left up corner of the button
void Font::RenderText(MT::Renderer* renderer, const std::string &text, MT::Rect &btnRect, float scale, int interline, int textStartX, int textStartY) {
	rectangle.x = btnRect.x + textStartX;
	rectangle.y = btnRect.y + textStartY;
	rectangle.w = 0;
	rectangle.h = 0;
	int temp = rectangle.x;
	for (int i = 0; i < text.length(); i++){
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = (int)(sourceRectangles[text[i]].w * scale);
				rectangle.h = (int)(sourceRectangles[text[i]].h * scale);
				renderer->RenderCopyPartFiltered(rectangle, sourceRectangles[text[i]], texture,
					{ rFilter,gFilter,bFilter });
				rectangle.x += (int)(sourceRectangles[text[i]].w * scale) + 1;
			}
			else{
				rectangle.y += (int)(interline * scale);
				rectangle.x = temp;
			}
		}
	}
}

void Font::RenderTextCenter(MT::Renderer* renderer, const std::string& text, MT::Rect &btnRect, float scale, int interline, int textStartX, int textStartY) {
	Point textSizes = CalculatePredefinedSize(text,interline,scale);

	Point center = GetRectangleCenter(btnRect);
	rectangle.x = center.x - (int)(textSizes.x * 0.5f) + textStartX;
	rectangle.y = center.y - (int)(textSizes.y * 0.5f) + textStartY;
	int temp = rectangle.x;


	for (int i = 0; i < text.length(); i++){
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = (int)(sourceRectangles[text[i]].w * scale);
				rectangle.h = (int)(sourceRectangles[text[i]].h * scale);
				renderer->RenderCopyPartFiltered(rectangle, sourceRectangles[text[i]], texture,
					{ rFilter,gFilter,bFilter });
				rectangle.x += (int)(sourceRectangles[text[i]].w * scale) + 1;
			}
			else{
				rectangle.y += (int)(interline * scale);
				rectangle.x = temp;
			}
		}
	}
}


void Font::RenderTextFromRight(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, float scale, int interline, int textStartX, int textStartY) {
	const Point textSizes = CalculatePredefinedSize(text, interline, scale);

	rectangle.x = btnRect.x + btnRect.w - textSizes.x + textStartX;
	rectangle.y = btnRect.y + textStartY;
	const int temp = rectangle.x;


	for (int i = 0; i < text.length(); ++i) {
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = (int)(sourceRectangles[text[i]].w * scale);
				rectangle.h = (int)(sourceRectangles[text[i]].h * scale);
				renderer->RenderCopyPartFiltered(rectangle, sourceRectangles[text[i]], texture,
					{ rFilter,gFilter,bFilter });
				rectangle.x += (int)(sourceRectangles[text[i]].w * scale) + 1;
			}
			else {
				rectangle.y += (int)(interline * scale);
				rectangle.x = temp;
			}
		}
	}
}

void Font::RenderTextCenterX(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, float scale, int interline, int textStartX, int textStartY) {
	Point textSizes = CalculatePredefinedSize(text, interline, scale);

	Point center = GetRectangleCenter(btnRect);
	rectangle.x = center.x - (int)(textSizes.x * 0.5f) + textStartX;
	rectangle.y = btnRect.y + textStartY;
	int temp = rectangle.x;


	for (int i = 0; i < text.length(); i++) {
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = (int)(sourceRectangles[text[i]].w * scale);
				rectangle.h = (int)(sourceRectangles[text[i]].h * scale);
				renderer->RenderCopyPartFiltered(rectangle, sourceRectangles[text[i]], texture,
					{ rFilter,gFilter,bFilter });
				rectangle.x += (int)(sourceRectangles[text[i]].w * scale) + 1;
			}
			else {
				rectangle.y += (int)(interline * scale);
				rectangle.x = temp;
			}
		}
	}
}

void Font::RenderTextCenterY(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, float scale, int interline, int textStartX, int textStartY) {
	Point textSizes = CalculatePredefinedSize(text, interline, scale);

	Point center = GetRectangleCenter(btnRect);
	rectangle.x = btnRect.x + textStartX;
	rectangle.y = center.y - (int)(textSizes.y * 0.5f) + textStartY;
	int temp = rectangle.x;


	for (int i = 0; i < text.length(); i++) {
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = (int)(sourceRectangles[text[i]].w * scale);
				rectangle.h = (int)(sourceRectangles[text[i]].h * scale);
				renderer->RenderCopyPartFiltered(rectangle, sourceRectangles[text[i]], texture,
					{ rFilter,gFilter,bFilter });
				rectangle.x += (int)(sourceRectangles[text[i]].w * scale) + 1;
			}
			else {
				rectangle.y += (int)(interline * scale);
				rectangle.x = temp;
			}
		}
	}
}

Point Font::CalculatePredefinedSize(const std::string& fontText, const int interline, const float scale) {
	Point predSize(0, interline); // x width y height

	int longest = 0;
	int currentLenght = 0;
	for (int i = 0; i < fontText.length(); ++i) {
		if (fontText[i] > sourceRectangles.size()) { continue; }
		if (fontText[i] == '\n') {
			predSize.y += interline;
			if (currentLenght > longest) {
				longest = currentLenght;
				currentLenght = 0;
			}
			continue;
		}

		currentLenght += (int)(sourceRectangles[fontText[i]].w * scale) + 1;

	}
	if (currentLenght > longest) {
		longest = currentLenght;
	}
	predSize.x = longest;
	return predSize;
}

int Font::GetStandardInterline() {
	return standardInterLine;
}

void Font::SetStandardInterline(int temp) {
	standardInterLine = temp;
}

void Font::SetFilter(const unsigned char r, const unsigned char g, const unsigned char b) {
	this->rFilter = r;
	this->gFilter = g;
	this->bFilter = b;
}


/// //////////////////

FontManager::FontManager() {
}

bool FontManager::CreateFont(const std::string& name, MT::Texture* texture, const std::string& jsonPath) {
	if (fonts.size() > 0) {
		for (auto& it : fonts) {
			if (it->GetName() == name) {
				return false;
			}
		}
	}
	fonts.emplace_back(new Font(name, texture, jsonPath));
	return true;
}

Font* FontManager::GetFont(const std::string& name) {
	if (fonts.size() > 0) {
		for (auto& it : fonts) {
			if (it->GetName() == name) {
				return it;
			}
		}
	}
	return nullptr;
}

void FontManager::ScanFont(const std::string& texturePath, const std::string& charactersDataPath,
	unsigned char fR, unsigned char fG, unsigned char fB, unsigned char bR, unsigned char bG, unsigned char bB, int width, int height
	, const std::string& outputPath) {

	// Loading data from texture
	const char* texturepathArray = texturePath.c_str();
	SDL_Surface* surface = IMG_Load(texturepathArray);

	std::vector<std::vector<Pixel>> mappedFont(width,std::vector<Pixel>(height));



	Uint8 r, g, b, a;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Uint32* pixelPtr = (Uint32*)((Uint8*)surface->pixels + y * surface->pitch) + x;
			SDL_GetRGBA(*pixelPtr, surface->format, &r, &g, &b, &a);

			Pixel pixel{ r,g,b,a };
			mappedFont[x][y] = pixel;
		}
	}

	SDL_FreeSurface(surface);

	//Reading glyps data from texture
	Pixel border{ bR,bG,bB,255};

	Pixel font{ fR,fG,fB,255};

	short maxX = -1;
	short minX = 10000;

	short maxY = -1;
	short minY = 10000;


	std::vector<SDL_Rect> jsonRectangles;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (mappedFont[i][j].R == border.R && mappedFont[i][j].G == border.G && mappedFont[i][j].B == border.B && mappedFont[i][j].A == 255) {
				++i;
				SDL_Rect rect = { minX, minY, maxX - minX, maxY - minY };
				jsonRectangles.push_back(rect);

				maxX = -1;
				minX = 10000;

				maxY = -1;
				minY = 10000;
				if (i == width) { break; }

			}
			else if(mappedFont[i][j].R == font.R && mappedFont[i][j].G == font.G && mappedFont[i][j].B == font.B){

				if (maxX < i) { maxX = i; }    

				if (maxY < j) { maxY = j; }

				if (i < minX) { minX = i; }

				if (j < minY) { minY = j; }
			}
		}
	}

	// Height adjustment to make all rectangles render in same line
	int maxH = 0;
	for (int i = 0; i < jsonRectangles.size(); i++)
	{
		if (jsonRectangles[i].h > maxH) { maxH = jsonRectangles[i].h; }
	}

	for (int i = 0; i < jsonRectangles.size(); i++)
	{
		if (jsonRectangles[i].h < maxH) { 
			jsonRectangles[i].y -= (maxH - jsonRectangles[i].h);
			jsonRectangles[i].h = maxH;
		}
	}

	//Reading charaters from provided txt file
	std::vector<int> Characters;

	std::fstream declarations(charactersDataPath);

	std::string line;

	while (std::getline(declarations,line)){
		Characters.emplace_back(line[0]);

	}

	for (size_t i = 0; i < Characters.size(); i++)
	{
		std::cout << static_cast<char>(Characters[i]) << " X: " << jsonRectangles[i].x <<" Y: "<<jsonRectangles[i].y 
			<< " W: " << jsonRectangles[i].w<<" H: "<<jsonRectangles[i].h<<"\n";
	}
	declarations.close();

	// Creating json file for texture
	nlohmann::ordered_json fontJSON;

	for (size_t i = 0; i < Characters.size(); ++i) {

		fontJSON[std::to_string(Characters[i])] = {
			{"text", std::string(1, static_cast<char>(Characters[i]))}, 
			{"x", jsonRectangles[i].x},                                             
			{"y", jsonRectangles[i].y},                                             
			{"width", jsonRectangles[i].w},                                         
			{"height", jsonRectangles[i].h},                                        
			{"baseline", 0}                              
		};
	}

	std::ofstream file(outputPath);
	if (file.is_open()) {
		file << fontJSON.dump(4); 
		file.close();
		std::cout << "Font JSON generated successfully: " << outputPath << "\n";
	}
	else {
		std::cerr << "Failed to open file for writing: " << outputPath << "\n";
	}

	std::cout << "Font succesfully scanned\n";
}

void CrateFontFromTTF(const char* ttfPath, const int size, const std::string& name, const std::string& outPath) {
	TTF_Init();
	if (outPath != "") {
		if (!std::filesystem::exists(outPath)) {
			throw std::runtime_error("CrateFontFromTTF  outPath does not exist");
		}
	}

	TTF_Font* font = TTF_OpenFont(ttfPath, size);

	// Creating string containing all signs
	std::string strCharset = "";
	strCharset.reserve(100); // Nie wiem ile w sumie bo jeszcze nie wiem ile znaków trzymaæ

	for (size_t i = 32; i < 127; i++) { // od 31 do 127 bo od 31 w dól znaki kontrolne i 127 do 159 tak samo
		strCharset += charset[i];
	}

	for (size_t i = 160; i < 199; i++) { // dziwne znaki czy to wogóle zachowaæ nie wiem mo¿e opcja w funkcji?
		strCharset += charset[i];
	}


	// Creating rectangles and glyps
	std::vector<SDL_Surface*> surfaces;
	surfaces.reserve(strCharset.size());

	std::vector<SDL_Rect> sourceRectangles;
	sourceRectangles.reserve(strCharset.size());

	int x = 0;
	int y = 0;



	for (auto& it : strCharset) {
		SDL_Surface* surf = TTF_RenderGlyph32_Blended(font, it, { 255,255,255,255 });
		if (!surf) continue;

		SDL_Surface* newSurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
		SDL_FreeSurface(surf);
		if (!newSurf) continue;

		SDL_SetSurfaceBlendMode(newSurf, SDL_BLENDMODE_NONE);
		SDL_SetColorKey(newSurf, SDL_FALSE, 0);
		surfaces.emplace_back(newSurf);
		sourceRectangles.emplace_back(x, y, newSurf->w, newSurf->h);
		x += newSurf->w + 1;
	}

	// Creating texture atlas from glyphs

	int w = 0;
	int maxH = 0;

	for (auto& it : sourceRectangles) {
		w += it.w + 1;
		if (it.h > maxH) {
			maxH = it.h;
		}
	}

	SDL_Surface* atlas = SDL_CreateRGBSurfaceWithFormat(0, w, maxH, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_FillRect(atlas, nullptr, SDL_MapRGBA(atlas->format, 0, 0, 0, 0));


	for (size_t i = 0; i < sourceRectangles.size(); i++) {
		SDL_BlitSurface(surfaces[i], nullptr, atlas, &sourceRectangles[i]);
	}
	std::string pngName;
	if (outPath == "") {
		pngName = name + ".png";
	}
	else {
		pngName = outPath + "/" + name + ".png";
	}
	IMG_SavePNG(atlas, pngName.c_str());
	// Creating json
	nlohmann::ordered_json fontJSON;


	for (size_t i = 0; i < sourceRectangles.size(); ++i) {

		fontJSON[std::to_string((int)strCharset[i])] = {
			{"text", std::string(1, static_cast<char>(strCharset[i]))},
			{"x", sourceRectangles[i].x},
			{"y", sourceRectangles[i].y},
			{"width", sourceRectangles[i].w},
			{"height", sourceRectangles[i].h},
			{"baseline", 0}
		};
	}
	std::string jsonName;
	if (outPath == "") {
		jsonName = name + ".json";
	}
	else {
		jsonName = outPath + "/" + name + ".json";
	}
	std::ofstream file(jsonName);
	if (file.is_open()) {
		file << fontJSON.dump(4);
		file.close();
		std::println("Font {} JSON generated successfully", name);
	}
	else {
		std::println("ERROR cannot generate font: {}", name);
	}

	//Clean Up
	SDL_FreeSurface(atlas);
	for (auto& it : surfaces) {
		SDL_FreeSurface(it);
	}
	TTF_CloseFont(font);
	TTF_Quit();
}

void FontManager::CrateTempFontFromTTF(const char* ttfPath, const int size, const std::string& name) {
	TTF_Init();

	TTF_Font* font = TTF_OpenFont(ttfPath, size);

	// Creating string containing all signs
	std::string strCharset = "";
	strCharset.reserve(100); // Nie wiem ile w sumie bo jeszcze nie wiem ile znaków trzymaæ

	for (size_t i = 32; i < 127; i++) { // od 31 do 127 bo od 31 w dól znaki kontrolne i 127 do 159 tak samo
		strCharset += charset[i];
	}

	for (size_t i = 160; i < 199; i++) { // dziwne znaki czy to wogóle zachowaæ nie wiem mo¿e opcja w funkcji?
		strCharset += charset[i];
	}


	// Creating rectangles and glyps
	std::vector<SDL_Surface*> surfaces;
	surfaces.reserve(strCharset.size());

	std::vector<MT::Rect> sourceRectangles;
	sourceRectangles.reserve(strCharset.size());

	int x = 0;
	int y = 0;



	for (auto& it : strCharset) {
		SDL_Surface* surf = TTF_RenderGlyph32_Blended(font, it, { 255,255,255,255 });
		if (!surf) continue;

		SDL_Surface* newSurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
		SDL_FreeSurface(surf);
		if (!newSurf) continue;

		SDL_SetSurfaceBlendMode(newSurf, SDL_BLENDMODE_NONE);
		SDL_SetColorKey(newSurf, SDL_FALSE, 0);
		surfaces.emplace_back(newSurf);
		sourceRectangles.emplace_back(x, y, newSurf->w, newSurf->h);
		x += newSurf->w + 1;
	}

	// Creating texture atlas from glyphs

	int w = 0;
	int maxH = 0;

	for (auto& it : sourceRectangles) {
		w += it.w + 1;
		if (it.h > maxH) {
			maxH = it.h;
		}
	}

	SDL_Surface* atlas = SDL_CreateRGBSurfaceWithFormat(0, w, maxH, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_FillRect(atlas, nullptr, SDL_MapRGBA(atlas->format, 0, 0, 0, 0));




	for (size_t i = 0; i < sourceRectangles.size(); i++) {
		SDL_Rect tempRect = sourceRectangles[i].ToSDLRect();
		SDL_BlitSurface(surfaces[i], nullptr, atlas, &tempRect);
	}
	
	MT::Texture *tex = MT::LoadTextureFromSurface(atlas);
	
	if (!TexMan::AddTexture(tex, name)) {
		throw std::runtime_error("Texture name already taken");
	}
	if (fonts.size() > 0) {
		for (auto& it : fonts) {
			if (it->GetName() == name) {
				throw std::runtime_error("font with idenical name already exist");
				return;
			}
		}
	}
	fonts.emplace_back(new Font(name, tex, strCharset, sourceRectangles));


	//Clean Up
	SDL_FreeSurface(atlas);
	for (auto& it : surfaces) {
		SDL_FreeSurface(it);
	}
	TTF_CloseFont(font);
	TTF_Quit();
}

FontManager::~FontManager() {
	for (auto& it : fonts) {
		delete it;
	}
	fonts.clear();
}