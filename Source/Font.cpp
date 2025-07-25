#include <iostream>
#include <SDL.h>
#include "Font.h"
#include "json.hpp"
#include <fstream>
#include <SDL_image.h>
#include <fstream>

struct Pixel {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

Font::Font(const std::string& name, SDL_Texture* texture, const std::string& jsonPath) {
	this->name = name;
	this->texture = texture;
	LoadTextInfo(jsonPath);
}


std::string Font::GetName() {
	return name;
}

SDL_Texture* Font::GetTexture() {
	return texture;
}


void Font::SetTexture(SDL_Texture* temptex) {
	texture = temptex;
}

SDL_Rect* Font::GetRectangle() {
	return &rectangle;
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

		/*for (size_t i = 0; i < sourceRectangles.size(); i++)
		{
			std::cout << i << " : X: " << sourceRectangles[i].x << " Y: " << sourceRectangles[i].y
				<< " W: " << sourceRectangles[i].w << " H: " << sourceRectangles[i].h << "\n";

		}*/
		//std::cout << "InterLine: " << standardInterLine << "\n";

	}
	else
	{
		std::cout << "Error font json not loaded correctly!\n";
		return false;
	}
	return true;
}


// Basic text rendering with possition in left up corner of the button
void Font::RenderText(SDL_Renderer* renderer, const std::string &text, SDL_Rect &btnRect, float scale, int interline, int textStartX, int textStartY) {
	rectangle.x = btnRect.x + textStartX;
	rectangle.y = btnRect.y + textStartY;
	rectangle.w = 0;
	rectangle.h = 0;
	int temp = rectangle.x;
	for (int i = 0; i < text.length(); i++)
	{
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = sourceRectangles[text[i]].w * scale;
				rectangle.h = sourceRectangles[text[i]].h * scale;
				SDL_RenderCopy(renderer, texture, &sourceRectangles[text[i]], &rectangle);
				rectangle.x += (sourceRectangles[text[i]].w * scale) + 1;
			}
			else
			{
				rectangle.y += interline * scale;
				rectangle.x = temp;
			}
		}
	}
}

void Font::RenderTextCenter(SDL_Renderer* renderer, const std::string& text, SDL_Rect &btnRect, float scale, int interline, int textStartX, int textStartY) {
	Point textSizes = CalculatePredefinedSize(text,interline);

	textSizes.x *= 0.5;
	textSizes.y *= 0.5;
	Point center = GetRectangleCenter(btnRect);
	rectangle.x = center.x + textStartX - textSizes.x;
	rectangle.y = center.y + textStartY - textSizes.y;
	rectangle.w = 0;
	rectangle.h = 0;
	int temp = rectangle.x;


	for (int i = 0; i < text.length(); i++)
	{
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = sourceRectangles[text[i]].w * scale;
				rectangle.h = sourceRectangles[text[i]].h * scale;
				SDL_RenderCopy(renderer, texture, &sourceRectangles[text[i]], &rectangle);
				rectangle.x += (sourceRectangles[text[i]].w * scale) + 1;
			}
			else
			{
				rectangle.y += interline * scale;
				rectangle.x = temp;
			}
		}
	}
}


void Font::RenderTextFromRight(SDL_Renderer* renderer, const std::string& text, SDL_Rect& btnRect, float scale, int interline, int textStartX, int textStartY) {
	if (text.empty()) {
		return;
	}

	rectangle.x = (btnRect.x + btnRect.w) - textStartX;
	rectangle.y = btnRect.y + textStartY;
	rectangle.w = 0;
	rectangle.h = 0;
	int initialX = rectangle.x; 

	for (int i = text.length() - 1; i >= 0; --i) {
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = sourceRectangles[text[i]].w * scale;
				rectangle.h = sourceRectangles[text[i]].h * scale;

				rectangle.x -= rectangle.w + 1; 
				SDL_RenderCopy(renderer, texture, &sourceRectangles[text[i]], &rectangle);
			}
			else {
				rectangle.y += interline * scale;
				rectangle.x = initialX; 
			}
		}
	}
}


//Only use if button text is static and will not change between frames
void Font::RenderTextCenterPred(SDL_Renderer* renderer, const std::string& text, SDL_Rect& btnRect, Point& textSizes, float scale, int interline, int textStartX, int textStartY) {
	textSizes.x *= 0.5;
	textSizes.y *= 0.5;
	Point center = GetRectangleCenter(btnRect);
	rectangle.x = center.x + textStartX - textSizes.x;
	rectangle.y = center.y + textStartY - textSizes.y;
	rectangle.w = 0;
	rectangle.h = 0;
	int temp = rectangle.x;


	for (int i = 0; i < text.length(); i++)
	{
		if (text[i] < sourceRectangles.size()) {
			if (text[i] != '\n') {
				rectangle.w = sourceRectangles[text[i]].w * scale;
				rectangle.h = sourceRectangles[text[i]].h * scale;
				SDL_RenderCopy(renderer, texture, &sourceRectangles[text[i]], &rectangle);
				rectangle.x += (sourceRectangles[text[i]].w * scale) + 1;
			}
			else
			{
				rectangle.y += interline * scale;
				rectangle.x = temp;
			}
		}
	}
}


Point Font::CalculatePredefinedSize(const std::string& fontText, int interline) {
	Point predSize(0, 0 + interline);
	bool firstLine = true;
	for (int i = 0; i < fontText.length(); i++)
	{
		if (fontText[i] < sourceRectangles.size()) {
			if (fontText[i] != '\n') {
				if (firstLine) {
					predSize.x += sourceRectangles[fontText[i]].w;
				}

			}
			else
			{
				predSize.y += interline;
				firstLine = false;
			}
		}
	}

	return predSize;
}

int Font::GetStandardInterline() {
	return standardInterLine;
}

void Font::SetStandardInterline(int temp) {
	standardInterLine = temp;
}


/// //////////////////

FontManager::FontManager() {
}

bool FontManager::CreateFont(const std::string& name, SDL_Texture* texture, const std::string& jsonPath) {
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

FontManager::~FontManager() {
	for (auto& it : fonts) {
		delete it;
	}
	fonts.clear();
}