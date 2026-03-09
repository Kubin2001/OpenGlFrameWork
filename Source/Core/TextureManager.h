#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Addons.h"
#include "Renderer.h"

namespace MT {
	struct Atlas {
		MT::Texture* tex = nullptr;
		std::unordered_map<std::string, MT::Rect> sourceRectangles = {};
		std::vector<std::string> errors = {};

		Atlas() = default;

		Atlas(std::vector<std::string>& errors) {
			this->errors = std::move(errors);
		}

		Atlas(MT::Texture* tex, std::unordered_map<std::string, MT::Rect> & sourceRectangles, std::vector<std::string>& errors) {
			this->tex = tex;
			this->sourceRectangles = std::move(sourceRectangles);
			this->errors = std::move(errors);
		}
	};


}

class TexMan {
	private:
		static std::unordered_map<std::string, MT::Texture*> Textures;
		static std::vector<std::string> SupportedFormats;
		static MT::Renderer* renderer;
		static bool isInit;

		static MT::Texture* defaultTex;

		static void RefreshTexturesInFolder(const std::string& directory, bool removeInvalid, std::unordered_set<std::string>& namesCollector);

		static void CreateDefaultTexture();

	public:

		static bool Start(MT::Renderer* ren);

		static bool IsWorking();

		static void Print();

		// You need to pass extenstion with . example .png it supports all formats supported by sdl_image IMG_LOAD
		static bool IsFormatSupported(const std::string& format);

		static bool AddTexture(MT::Texture* tex, const std::string& name);

		static void LoadSingle(const char* filePath, const std::string& name);

		//Loads single folder and uses file names (without extensions) as keys
		// Example tree.png key = tree
		static void LoadMultiple(const std::string& directory);

		// Recursive loading of every folder and subfolder in directory uses file names (without extensions) as keys
		// Example tree.png key = tree
		static void DeepLoad(const std::string& directory);

		static MT::Texture* GetTex(const std::string& name, bool retNullOnMissing = false);

		static MT::Texture* GetDefaultTex();

		static bool DeleteTexture(const std::string& name);

		// Loads new previously unloaded textures in slected folder and all recursive folders
		// removeInvalid flag will remove textures that no longer exist WARNING this will break exsting pointers
		static void RefreshTextures(const std::string& directory, bool removeInvalid = true);

		static Point GetTextureSize(const std::string& name);

		static size_t GetTexturesAmount();

		static std::unordered_map<std::string, MT::Texture*> &GetAllTex();

		// Function to split texture into multiple smaller textures at runtime
		// Original texture needs to have seperator color at higest pixel at the row and cant use it anywere else
		static void SplitTexture(const char* path, const std::vector<std::string>& names,
			const unsigned char r = 0, const unsigned char g = 0, const unsigned char b = 0, const unsigned char a = 255);

		static MT::Atlas CreateAtlas(int tileSize, const std::vector<std::string>& textureNames, bool deleteOriginals = false);

		static void Clear();
};

class LocalTexMan {
	private:
		std::unordered_map<std::string, MT::Texture*> Textures = {};
		std::vector<std::string> SupportedFormats = {};
		MT::Renderer* renderer = nullptr;
		bool isInit = false;

	public:
		bool Start(MT::Renderer* ren);

		bool IsWorking();

		void Print();

		// You need to pass extenstion with . example .png it supports all formats supported by sdl_image IMG_LOAD
		bool IsFormatSupported(const std::string& format);

		bool AddTexture(MT::Texture* tex, const std::string& name);

		void LoadSingle(const char* filePath, const std::string& name);

		//Loads single folder and uses file names (without extensions) as keys
		// Example tree.png key = tree
		void LoadMultiple(const std::string& directory);

		// Recursive loading of every folder and subfolder in directory uses file names (without extensions) as keys
		// Example tree.png key = tree
		void DeepLoad(const std::string& directory);

		MT::Texture* GetTex(const std::string& name);

		bool DeleteTexture(const std::string& name);

		Point GetTextureSize(const std::string& name);

		// Function to split texture into multiple smaller textures at runtime
		// Original texture needs to have seperator color at higest pixel at the row and cant use it anywere else
		void SplitTexture(const char* path, const std::vector<std::string>& names,
			const unsigned char r = 0, const unsigned char g = 0, const unsigned char b = 0, const unsigned char a = 255);

		void Clear();
};
