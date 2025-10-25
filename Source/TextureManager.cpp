#include "TextureManager.h"

#include <iostream>
#include <SDL_image.h>
#include <SDL.h>
#include <filesystem>
#include <vector>
#include <string>



std::unordered_map<std::string, MT::Texture*> TexMan::Textures;
std::vector<std::string> TexMan::SupportedFormats;
MT::Renderer* TexMan::renderer = nullptr;
bool TexMan::isInnit = false;


bool TexMan::Start(MT::Renderer* ren) {
   renderer = ren;
   if (renderer != nullptr) {
	   isInnit = true;
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
	   return isInnit;
   }

   return false;
}


bool TexMan::IsWorking() {
	return isInnit;
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


MT::Texture* TexMan::GetTex(const std::string& name) {
	auto it = Textures.find(name);
	if (it != Textures.end()) {
		return it->second;
	}
	std::cerr << "Texture not found: " << name << "\n";
	return nullptr;
}

bool TexMan::DeleteTexture(const std::string& name) {
	auto it = Textures.find(name);
	if (it != Textures.end()) {
		delete it->second;
		Textures.erase(it);
		return true;
	}
	else{
		return false;
	}
}

Point TexMan::GetTextureSize(const std::string& name) {
	Point p(-1, -1);
	auto it = Textures.find(name);
	if (it == Textures.end()) {
		return p;
		std::cerr << "Texture not found: " << name << "\n";
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

void TexMan::Clear() {
	for (auto& pair : Textures) {
		MT::DeleteTexture(pair.second);
		delete pair.second;
	}
	Textures.clear();
}

//LocalTexMan
bool LocalTexMan::Start(MT::Renderer* ren) {
	renderer = ren;
	if (renderer != nullptr) {
		isInnit = true;
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
		return isInnit;
	}

	return false;
}


bool LocalTexMan::IsWorking() {
	return isInnit;
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
		return p;
		std::cerr << "Texture not found: " << name << "\n";
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
