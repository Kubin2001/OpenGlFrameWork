#include <print>

#include "SoundManager.h"
#include <filesystem>

void SoundMan::Innit() {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::println("Failed to sound manager (sdl_mixer error): {}", Mix_GetError());
	}
	Mix_AllocateChannels(32);
}

void SoundMan::Print() {
	std::println("------------------------");
	std::println("Loaded Sounds Names");
	std::println("------------------------");
	for (auto& sound : Sounds) {
		std::println("{}", sound.first);
	}
	std::println("------------------------");
}

void SoundMan::LoadSound(const char* filePath, const std::string& name) {
	auto sound = Sounds.find(name);
	if (sound != Sounds.end()) {
		std::println("Sound already loaded {} ", name);
		return;
	}
	Sounds[name] = Mix_LoadWAV(filePath);
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
		std::println(" SoundMan::LoadSounds Error loading directory: {}    {}", directory, e.what());
	}

}

void SoundMan::DeppLoad(const std::string& directory) {
	LoadSounds(directory);
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_directory()) {
			const std::string path = entry.path().string();
			LoadSounds(path);
		}
	}
}

void SoundMan::PlaySound(const std::string& name, int volume) {
	auto it = Sounds.find(name);
	if (it != Sounds.end()) {
		PlayRawSound(it->second,volume);
		return;
	}
	std::println("Sound not found: {}", name);
}

void SoundMan::PlayRawSound(Mix_Chunk* sound, int volume) {
	if (volume < 1) { return; } // Good for conserving cpu when sound is to far in some map
	int channel = Mix_PlayChannel(-1, sound, 0);
	if (channel == -1) { return; }
	int SDL_Volume = (volume * MIX_MAX_VOLUME) / 100;

	Mix_Volume(channel, SDL_Volume);
}

void SoundMan::PlaySoundStereo(const std::string& name, int left, int right, int volume) {
	auto sound = Sounds.find(name);
	if (sound == Sounds.end()) {
		std::println("Sound not found: {}", name);
		return;
	}
	PlayRawSoundStereo(sound->second, left, right,volume);
	return;
}

void SoundMan::PlayRawSoundStereo(Mix_Chunk* sound, int left, int right, int volume) {
	if (volume < 1) { return; }
	int channel = Mix_PlayChannel(-1, sound, 0);
	if (channel == -1) { return; }
	int SDL_Volume = (volume * MIX_MAX_VOLUME) / 100;
	int SDL_Left = (left * 255) / 100;
	int SDL_Right = (right * 255) / 100;
	Mix_SetPanning(channel, SDL_Left, SDL_Right);

	Mix_Volume(channel, SDL_Volume);
}

Mix_Chunk *SoundMan::GetSound(const std::string& name) {
	auto it = Sounds.find(name);
	if (it != Sounds.end()) {
		return it->second;
	}
	std::println("Sound not found: {}", name);
	return nullptr;
}

std::unordered_map<std::string, Mix_Chunk*> &SoundMan::GetSounds() {
	return SoundMan::Sounds;
}

void SoundMan::SetVolume(const std::string& soundKey, unsigned char volume) {
	auto sound = Sounds.find(soundKey);
	if (sound == Sounds.end()) {
		std::println("Wrong sound key in SoundMan::SetVolume function: {}",soundKey);
		return;
	}
	unsigned char newVolume = (volume * MIX_MAX_VOLUME) / 100;
	if (newVolume > MIX_MAX_VOLUME) { newVolume = MIX_MAX_VOLUME; }
	else if (newVolume < 0) { newVolume = 0; }
	Mix_VolumeChunk(Sounds[soundKey], newVolume);
}

bool SoundMan::DeleteSound(const std::string& name) {
	auto it = Sounds.find(name);
	if (it != Sounds.end()) {
		Mix_FreeChunk(it->second);
		Sounds.erase(it);
		return true;
	}
	std::println("Sound not found: {}", name);
	return false;
}

void SoundMan::Clear() {
	for (auto& pair : Sounds) {
		Mix_FreeChunk(pair.second);
	}
	Sounds.clear();
}