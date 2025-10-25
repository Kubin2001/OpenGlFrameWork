#include <iostream>

#include "SoundManager.h"
#include <filesystem>

std::unordered_map<std::string, Mix_Chunk*> SoundMan::Sounds;


void SoundMan::Innit() {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cerr << "Failed to sound manager (sdl_mixer error): " << Mix_GetError() << "\n";
	}
}

void SoundMan::Print() {
	std::cout << "------------------------\n";
	std::cout << "Loaded Sounds Names: \n";
	std::cout << "------------------------\n";
	for (auto it = Sounds.begin(); it != Sounds.end(); ++it) {
		std::cout << it->first << "\n";
	}
	std::cout << "------------------------\n";
}

void SoundMan::LoadSound(const char* filePath, const std::string& name) {
	if (Sounds.find(name) != Sounds.end()) {
		std::cout << "Sound already loaded: " << name << "\n";
		return;
	}
	else
	{
		Sounds.insert(std::make_pair(name, Mix_LoadWAV(filePath)));
	}
	auto it = Sounds.find(name);
	if (it->second == nullptr) {
		std::cout << "Failed to load sound from: " << filePath <<" loaded as nullptr"<< "\n";
	}
}

void SoundMan::LoadSounds(const std::string& directory) {
	try {
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.path().extension() == ".wav") {
				std::string pathString = entry.path().string();
				const char* path = pathString.c_str();
				std::string name = entry.path().stem().string();
				LoadSound(path, name);
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << " SoundMan::LoadSounds Error loading directory: " << directory << " " << e.what() << "\n";
	}

}

void SoundMan::PlaySound(const std::string& name) {
	auto it = Sounds.find(name);
	if (it != Sounds.end()) {
		Mix_PlayChannel(-1, it->second, 0);
		return;
	}
	std::cout << "Sound not found: " << name << "\n";
	return ;
}

Mix_Chunk *SoundMan::GetSound(const std::string& name) {
	auto it = Sounds.find(name);
	if (it != Sounds.end()) {
		return it->second;
	}
	std::cout << "Sound not found: "<<name<<"\n";
	return nullptr;
}

void SoundMan::SetVolume(const std::string& soundKey, unsigned char volume) {
	if (Sounds.find(soundKey) != Sounds.end()) {
		unsigned char newVolume = (volume * MIX_MAX_VOLUME) / 100;
		if (newVolume > MIX_MAX_VOLUME) { newVolume = MIX_MAX_VOLUME; }
		else if (newVolume < 0) { newVolume = 0; }
		Mix_VolumeChunk(Sounds[soundKey], newVolume);
	}
	else{
		std::cout << "Wrong sound key in SoundMan::SetVolume function: "<<soundKey<<"\n";
	}

}

bool SoundMan::DeleteSound(const std::string& name) {
	auto it = Sounds.find(name);
	if (it != Sounds.end()) {
		Mix_FreeChunk(it->second);
		Sounds.erase(it);
		return true;
	}
	std::cout << "Sound not found: " << name << "\n";
	return false;
}

void SoundMan::Clear() {
	for (auto& pair : Sounds) {
		Mix_FreeChunk(pair.second);
	}
	Sounds.clear();
}


