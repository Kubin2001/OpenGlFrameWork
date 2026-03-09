#include "TextureManager.h"

#include <iostream>
#include <SDL_image.h>
#include <SDL.h>
#include <filesystem>
#include <vector>
#include <string>
#include <format>


std::unordered_map<std::string, MT::Texture*> TexMan::Textures;
std::vector<std::string> TexMan::SupportedFormats;
MT::Renderer* TexMan::renderer = nullptr;
bool TexMan::isInit = false;
MT::Texture* TexMan::defaultTex = nullptr;

void TexMan::CreateDefaultTexture() {
	SDL_Surface* surface = SDL_CreateRGBSurface(
		0, 2, 2, 32,
		0x00FF0000, 
		0x0000FF00,
		0x000000FF,
		0xFF000000
	);
	// Last for numbers 0x... are format mapping R,G,B,A not real pixels colors

	Uint32* p = (Uint32*)surface->pixels;
	p[0] = 0xFFFF00FF;
	p[1] = 0xFF000000;
	p[2] = 0xFF000000;
	p[3] = 0xFFFF00FF;

	defaultTex = MT::LoadTextureFromSurface(surface);
	SDL_FreeSurface(surface);
}


bool TexMan::Start(MT::Renderer* ren) {
	if (isInit) {
		return false;
	}
	renderer = ren;
	if (renderer != nullptr) {
		isInit = true;
		SupportedFormats.emplace_back(".png");
		SupportedFormats.emplace_back(".jpg");
		SupportedFormats.emplace_back(".jpeg");
		SupportedFormats.emplace_back(".bmp");
		SupportedFormats.emplace_back(".gif");
		SupportedFormats.emplace_back(".tif");
		SupportedFormats.emplace_back(".tiff");
		SupportedFormats.emplace_back(".tga");
		SupportedFormats.emplace_back(".ico");
		SupportedFormats.emplace_back(".cur");
		SupportedFormats.emplace_back(".pcx");
		SupportedFormats.emplace_back(".xpm");
		CreateDefaultTexture();
		return isInit;
	}

	return false;
}


bool TexMan::IsWorking() {
	return isInit;
}

void TexMan::Print() {
	std::cout << "------------------------\n";
	std::cout << "Loaded Textures Names: \n";
	std::cout << "------------------------\n";
	for (auto it = Textures.begin(); it != Textures.end(); ++it) {
		std::cout << it->first<<"\n";
	}
	std::cout << "------------------------\n";
}

bool TexMan::IsFormatSupported(const std::string& format) {
	for (const auto& it : SupportedFormats) {
		if (format == it) { return true; }
	}
	return false;
}

bool TexMan::AddTexture(MT::Texture* tex, const std::string& name) {
	if (Textures.find(name) != Textures.end()) {
		return false;
	}
	Textures[name] = tex;
	return true;
}

void TexMan::LoadSingle(const char* filePath, const std::string& name) {
	if (Textures.find(name) != Textures.end()) {
		std::cout << "Texture: " << name << " is already loaded\n";
		return;
	}
	MT::Texture* tex = MT::LoadTexture(filePath);
	Textures.insert(std::make_pair(name, tex));
}



void TexMan::LoadMultiple(const std::string& directory){
	try {
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
			if (IsFormatSupported(entry.path().extension().string())) {
				std::string pathString = entry.path().string();
				const char* path = pathString.c_str();
				std::string name = entry.path().stem().string();
				LoadSingle(path, name);
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "TexMan::LoadMultiple Error loading directory: " << directory << " " << e.what() << "\n";
	}

}

void TexMan::DeepLoad(const std::string& directory) {
	LoadMultiple(directory);
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_directory()) {
			const std::string path = entry.path().string();
			DeepLoad(path);
		}
	}
}


MT::Texture* TexMan::GetTex(const std::string& name, bool retNullOnMissing) {
	auto it = Textures.find(name);
	if (it != Textures.end()) {
		return it->second;
	}
	if (retNullOnMissing) {
		return nullptr;
	}
	return defaultTex;
}

MT::Texture* TexMan::GetDefaultTex() {
	return defaultTex;
}

bool TexMan::DeleteTexture(const std::string& name) {
	auto it = Textures.find(name);
	if (it != Textures.end()) {
		MT::DeleteTexture(it->second);
		Textures.erase(it);
		return true;
	}
	else{return false;}
}

void TexMan::RefreshTexturesInFolder(const std::string& directory, bool removeInvalid, std::unordered_set<std::string>&namesCollector) {
	namespace fs = std::filesystem;
	for (fs::directory_entry entry : fs::directory_iterator(directory)) {
		if (entry.is_directory()) {
			RefreshTexturesInFolder(entry.path().string(),removeInvalid,namesCollector);
		}
		else {
			std::string stem = entry.path().stem().string();
			std::string path = entry.path().string();
			auto textureIter = Textures.find(stem);
			if (textureIter == Textures.end()) { // If texture is not loaded load it
				LoadSingle(path.c_str(), stem);
			}
			else{ // Check if it needs to be refreshed
				MT::Texture* tex = textureIter->second;
				if (tex->writeTime != std::filesystem::last_write_time(path)) { //Refresh texture
					MT::DeleteTexture(tex);
					tex = MT::LoadTexture(path.c_str());
					textureIter->second = tex;
				}
			}
			if (removeInvalid) { // Add to later check if it is missing in textures
				namesCollector.emplace(stem);
			}
		}
	}
}

void TexMan::RefreshTextures(const std::string& directory, bool removeInvalid) {
	namespace fs  = std::filesystem;
	if (!fs::exists(directory)) {
		std::println("TexMan::RefreshTextures incorrect start directory");
		return;
	}
	std::unordered_set<std::string> namesCollector;
	if (removeInvalid) {
		namesCollector.reserve(Textures.size());
	}

	RefreshTexturesInFolder(directory,removeInvalid,namesCollector);

	if (removeInvalid) {
		std::vector<std::string> texturesToErase;
		for (auto &[key,tex] : Textures) {
			if (!namesCollector.contains(key)) {
				texturesToErase.emplace_back(key);
			}
		}
		for (auto& it : texturesToErase) {
			MT::DeleteTexture(Textures[it]);
			Textures.erase(it);
		}
	}
}

Point TexMan::GetTextureSize(const std::string& name) {
	Point p(-1, -1);
	auto it = Textures.find(name);
	if (it == Textures.end()) {
		std::println("Texture not found: {}", name);
		return p;
	}
	p.x = it->second->w;
	p.y = it->second->h;
	return p;
}

size_t TexMan::GetTexturesAmount() {
	return Textures.size();
}

std::unordered_map<std::string, MT::Texture*> &TexMan::GetAllTex() {
	return Textures;
}

SDL_Surface* CopyVectorToSurface(std::vector<std::vector<SDL_Color>> &copySurfData) {
	int width = (int)copySurfData.size();
	int height = (int)copySurfData[0].size();

	SDL_Surface* newSurface = SDL_CreateRGBSurfaceWithFormat(
		0, width, height, 32, SDL_PIXELFORMAT_RGBA32);

	if (newSurface) {
		SDL_LockSurface(newSurface);
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				if (x < copySurfData.size() && y < copySurfData[x].size()) {
					SDL_Color color = copySurfData[x][y];
					Uint8* p = (Uint8*)newSurface->pixels + y * newSurface->pitch + x * 4;
					p[0] = color.r;
					p[1] = color.g;
					p[2] = color.b;
					p[3] = color.a;
				}
			}
		}
		SDL_UnlockSurface(newSurface);
		copySurfData.clear();
		return newSurface;
	}
	std::cout << "SDL_Surface* CopyVectorToSurface cannot create surface aborted\n";
	return nullptr;
}

void TexMan::SplitTexture(const char* path, const std::vector<std::string> &names,
	const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) {
	SDL_Surface* surface = IMG_Load(path);
	SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
	if (!converted) {
		std::cout << "Failed to convert surface format\n";
		SDL_FreeSurface(surface);
		return;
	}

	std::vector<std::vector<SDL_Color>> copySurfData;
	std::vector<SDL_Surface*> surfaces;

	for (int x = 0; x < converted->w; ++x) {
		copySurfData.emplace_back();
		for (int y = 0; y < converted->h; ++y) {
			Uint8* p = (Uint8*)converted->pixels + y * converted->pitch + x * 4; // 4 bajty na piksel

			SDL_Color col(p[0], p[1], p[2], p[3]);

			if (col.r == r && col.g == g && col.b == b && col.a == a) {
				bool fullColumnColored = true;
				for (int ty = 0; ty < converted->h; ++ty) { // Checking if all column has seperator color
					Uint8* pIn = (Uint8*)converted->pixels + ty * converted->pitch + x * 4;
					SDL_Color inCol(pIn[0], pIn[1], pIn[2], pIn[3]);
					if (inCol.r == r && inCol.g == g && inCol.b == b && inCol.a == a) {
						continue;
					}
					else {
						fullColumnColored = false;
						break;
					}
				}
				if (fullColumnColored) {
					surfaces.push_back(CopyVectorToSurface(copySurfData));
				}
				break;  
			}
			else {
				copySurfData.back().emplace_back(col.r, col.g, col.b, col.a);
			}
		}
	}

	SDL_FreeSurface(converted);

	if (!copySurfData.empty()) {
		surfaces.push_back(CopyVectorToSurface(copySurfData));
	}

	int index = 0;
	for (auto& elem : surfaces) {
		MT::Texture* tex = MT::LoadTextureFromSurface(elem);
		SDL_FreeSurface(elem);
		Textures[names[index]] = tex;
		index++;
	}
}

MT::Atlas TexMan::CreateAtlas(int tileSize, const std::vector<std::string>& textureNames, bool deleteOriginals) {
	std::vector<std::string> errors;
	if (tileSize < 1 || tileSize > 2048) {
		errors.emplace_back("Illogical Tile Size must be between 1 and 2048");
		return {errors};
	}

	if (textureNames.empty()) {
		errors.emplace_back("Textures names are empty cannot create empty atlas");
		return { errors };
	}

	// Creating MT::textures vector

	std::vector<MT::Texture*> texturesToMap;
	texturesToMap.reserve(textureNames.size());

	for (auto& name : textureNames) {
		auto texIter = Textures.find(name);
		if (texIter != Textures.end()) {
			texturesToMap.emplace_back(texIter->second);
		}
		else {
			errors.emplace_back(std::format(" Texture with name: {} is not loaded", texIter->first));
			return { errors };
		}
	}

	// Getting max textures sizes
	int maxWidth = 0;
	int maxHeight = 0;
	int meanWidth = 0;
	int meanHeight = 0;

	for (auto& tex : texturesToMap) {
		if (tex->w > maxWidth) {
			maxWidth = tex->w;
		}
		if (tex->h > maxHeight) {
			maxHeight = tex->h;
		}
		meanHeight += tex->h;
		meanWidth += tex->w;
	}
	meanWidth /= texturesToMap.size();
	meanHeight /= texturesToMap.size();

	// Creating TileMapSize
	int rowsSize = (std::max(maxHeight, static_cast<int>(meanHeight * (texturesToMap.size() / 2) + 1)) / tileSize +1);
	int colSize = (std::max(maxWidth, static_cast<int>(meanWidth * (texturesToMap.size() / 2) + 1)) / tileSize) +1;
	
	int maxOpenGLTexSizeFlorred = 4000;
	if (rowsSize > maxOpenGLTexSizeFlorred / tileSize || colSize > maxOpenGLTexSizeFlorred / tileSize) {
		errors.emplace_back("Too many textures for a single atlas texture size would be bigger than max OpenGl possible size 4096");
		return { errors };
	}

	std::vector<std::vector<int>> tileMap = {};
	tileMap.resize(rowsSize);
	for (auto& row : tileMap) {
		row.resize(colSize);
	}

	// Filling TileMap

	Point lastFreeCell{ 0,0 }; // X = row Y = Column

	auto tryInsert = [](std::vector<std::vector<int>>& tileMap, Point cell, int w, int h, int tileSize, int texIndex)->bool {
		int rowsTaken = (h + tileSize - 1) / tileSize;
		int colTaken = (w + tileSize - 1) / tileSize;

		if (cell.x + rowsTaken > tileMap.size() || cell.y + colTaken > tileMap[0].size()) {
			return false;
		}

		// Colison check
		for (size_t i = cell.x; i < cell.x + rowsTaken; i++) {
			for (size_t j = cell.y; j < cell.y + colTaken; j++) {
				if (tileMap[i][j] != 0) {
					return false;
				}
			}
		}

		// Inserting texture
		for (size_t i = cell.x; i < cell.x + rowsTaken; i++) {
			for (size_t j = cell.y; j < cell.y + colTaken; j++) {
				tileMap[i][j] = texIndex + 1;
			}
		}

		return true;
	};

	int currentTexIndex = 0;
	while (currentTexIndex < texturesToMap.size()) {
		MT::Texture* current = texturesToMap[currentTexIndex];
		if (tryInsert(tileMap, lastFreeCell, current->w, current->h, tileSize, currentTexIndex)) {
			currentTexIndex++;
		}
		lastFreeCell.y++;
		if (lastFreeCell.y > tileMap[0].size() - 1) {
			lastFreeCell.y = 0;
			lastFreeCell.x++;
			if (lastFreeCell.x > tileMap.size() -1) {
				errors.emplace_back("Not all textures are fitting in the atlas it is broken");
				return { errors };
			}
		}

	}
	

	// Converting all textures to surfaces for atlas
	std::vector<SDL_Surface*> surfaces;
	surfaces.reserve(texturesToMap.size());
	for (auto& tex : texturesToMap) {
		surfaces.emplace_back(MT::TextureToSurface(tex));
	}

	// Creating map to remove already inserted textures
	std::unordered_map<int, SDL_Surface*> texturesToInsert; // key texture id +1 (Just like in tile map)

	for (int i = 0; i < surfaces.size(); i++) {
		texturesToInsert[i + 1] = surfaces[i];
	}
	
	// Creating atlas itself
	SDL_Surface* atlas = SDL_CreateRGBSurfaceWithFormat(0,tileMap[0].size() * tileSize, tileMap.size() * tileSize, 32, SDL_PIXELFORMAT_RGBA32);

	SDL_FillRect(atlas, NULL, SDL_MapRGBA(atlas->format, 0, 0, 0, 0));

	// Coping surfaces to atlas

	std::unordered_map<std::string, MT::Rect> sourceRectangles;
	sourceRectangles.reserve(surfaces.size());

	for (int i = 0; i < tileMap.size(); i++) { // row
		for (int j = 0; j < tileMap[i].size(); j++) { // column
			int cell = tileMap[i][j];
			int rowPos = j * tileSize; // Swaped
			int colPow = i * tileSize;
			SDL_Rect dest{ rowPos,colPow,0,0 };
			if (cell == 0) { continue;  }
			auto mapIter = texturesToInsert.find(cell);
			if (mapIter == texturesToInsert.end()) {
				//Texture is already inserted
				continue;
			}
			
			sourceRectangles[textureNames[cell - 1]] = MT::Rect{ dest.x, dest.y, mapIter->second->w, mapIter->second->h };
			SDL_BlitSurface(mapIter->second, NULL, atlas, &dest);
			texturesToInsert.erase(mapIter);
		}
	}

	// Removing the originals
	if (deleteOriginals) {
		for (auto& name : textureNames) {
			TexMan::DeleteTexture(name);
		}
	}


	// CleanUp and return
	for (auto& surf : surfaces) {
		SDL_FreeSurface(surf);
	}
	MT::Texture *atlasTex = MT::LoadTextureFromSurface(atlas);
	SDL_FreeSurface(atlas);
	return { atlasTex,sourceRectangles,errors };
}

void TexMan::Clear() {
	for (auto& pair : Textures) {
		MT::DeleteTexture(pair.second);
		delete pair.second;
	}
	Textures.clear();
	isInit = false;
	SupportedFormats.clear();
	renderer = nullptr;
	MT::DeleteTexture(defaultTex);
}

//LocalTexMan
bool LocalTexMan::Start(MT::Renderer* ren) {
	renderer = ren;
	if (renderer != nullptr) {
		isInit = true;
		SupportedFormats.emplace_back(".png");
		SupportedFormats.emplace_back(".jpg");
		SupportedFormats.emplace_back(".jpeg");
		SupportedFormats.emplace_back(".bmp");
		SupportedFormats.emplace_back(".gif");
		SupportedFormats.emplace_back(".tif");
		SupportedFormats.emplace_back(".tiff");
		SupportedFormats.emplace_back(".tga");
		SupportedFormats.emplace_back(".ico");
		SupportedFormats.emplace_back(".cur");
		SupportedFormats.emplace_back(".pcx");
		SupportedFormats.emplace_back(".xpm");
		return isInit;
	}

	return false;
}


bool LocalTexMan::IsWorking() {
	return isInit;
}

void LocalTexMan::Print() {
	std::cout << "------------------------\n";
	std::cout << "Loaded Textures Names: \n";
	std::cout << "------------------------\n";
	for (auto it = Textures.begin(); it != Textures.end(); ++it) {
		std::cout << it->first << "\n";
	}
	std::cout << "------------------------\n";
}

bool LocalTexMan::IsFormatSupported(const std::string& format) {
	for (const auto& it : SupportedFormats) {
		if (format == it) { return true; }
	}
	return false;
}

bool LocalTexMan::AddTexture(MT::Texture* tex, const std::string& name) {
	if (Textures.find(name) != Textures.end()) {
		return false;
	}
	Textures[name] = tex;
	return true;
}

void LocalTexMan::LoadSingle(const char* filePath, const std::string& name) {
	if (Textures.find(name) != Textures.end()) {
		std::cout << "Texture: " << name << " is already loaded\n";
		return;
	}
	MT::Texture* tex = MT::LoadTexture(filePath);
	Textures.insert(std::make_pair(name, tex));
}



void LocalTexMan::LoadMultiple(const std::string& directory) {
	try {
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
			if (IsFormatSupported(entry.path().extension().string())) {
				std::string pathString = entry.path().string();
				const char* path = pathString.c_str();
				std::string name = entry.path().stem().string();
				LoadSingle(path, name);
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "TexMan::LoadMultiple Error loading directory: " << directory << " " << e.what() << "\n";
	}

}

void LocalTexMan::DeepLoad(const std::string& directory) {
	LoadMultiple(directory);
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_directory()) {
			const std::string path = entry.path().string();
			DeepLoad(path);
		}
	}
}


MT::Texture* LocalTexMan::GetTex(const std::string& name) {
	auto it = Textures.find(name);
	if (it != Textures.end()) {
		return it->second;
	}
	std::cerr << "Texture not found: " << name << "\n";
	return nullptr;
}

bool LocalTexMan::DeleteTexture(const std::string& name) {
	auto it = Textures.find(name);
	if (it != Textures.end()) {
		delete it->second;
		Textures.erase(it);
		return true;
	}
	else {
		return false;
	}
}

Point LocalTexMan::GetTextureSize(const std::string& name) {
	Point p(-1, -1);
	auto it = Textures.find(name);
	if (it == Textures.end()) {
		std::println("Texture not found: {}", name);
		return p;
	}
	p.x = it->second->w;
	p.y = it->second->h;
	return p;
}

void LocalTexMan::SplitTexture(const char* path, const std::vector<std::string>& names,
	const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) {
	SDL_Surface* surface = IMG_Load(path);
	SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
	if (!converted) {
		std::cout << "Failed to convert surface format\n";
		SDL_FreeSurface(surface);
		return;
	}

	std::vector<std::vector<SDL_Color>> copySurfData;
	std::vector<SDL_Surface*> surfaces;

	for (int x = 0; x < converted->w; ++x) {
		copySurfData.emplace_back();
		for (int y = 0; y < converted->h; ++y) {
			Uint8* p = (Uint8*)converted->pixels + y * converted->pitch + x * 4; // 4 bajty na piksel

			SDL_Color col(p[0], p[1], p[2], p[3]);

			if (col.r == r && col.g == g && col.b == b && col.a == a) {
				bool fullColumnColored = true;
				for (int ty = 0; ty < converted->h; ++ty) { // Checking if all column has seperator color
					Uint8* pIn = (Uint8*)converted->pixels + ty * converted->pitch + x * 4;
					SDL_Color inCol(pIn[0], pIn[1], pIn[2], pIn[3]);
					if (inCol.r == r && inCol.g == g && inCol.b == b && inCol.a == a) {
						continue;
					}
					else {
						fullColumnColored = false;
						break;
					}
				}
				if (fullColumnColored) {
					surfaces.push_back(CopyVectorToSurface(copySurfData));
				}
				break;
			}
			else {
				copySurfData.back().emplace_back(col.r, col.g, col.b, col.a);
			}
		}
	}

	SDL_FreeSurface(converted);

	if (!copySurfData.empty()) {
		surfaces.push_back(CopyVectorToSurface(copySurfData));
	}

	int index = 0;
	for (auto& elem : surfaces) {
		MT::Texture* tex = MT::LoadTextureFromSurface(elem);
		SDL_FreeSurface(elem);
		Textures[names[index]] = tex;
		index++;
	}
}


void LocalTexMan::Clear() {
	for (auto& pair : Textures) {
		MT::DeleteTexture(pair.second);
		delete pair.second;
	}
	Textures.clear();
}
