#include <print>

#include "SoundManager.h"


void SoundMan::Init() {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		std::println("Failed to sound manager (sdl_mixer error): {}", Mix_GetError());
		throw std::runtime_error("Sound man critical error cannot innit systems");
	}
	Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
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

void SoundMan::Load(const std::filesystem::directory_entry& entry) {

	const std::string ext = entry.path().extension().string();
	const std::string name = entry.path().stem().string();
	if (ext == ".wav") { // Sound
		auto sound = Sounds.find(name);
		if (sound != Sounds.end()) {
			std::println("Sound already loaded {} ", name);
			return;
		}
		Mix_Chunk* lSound = Mix_LoadWAV(entry.path().string().c_str());
		// When file is eiher non supported or just wrong format like txt or png
		if (lSound) {
			Sounds[name] = lSound;
		}
	}
	else if (ext == ".mp3" || ext == ".ogg") { // Music
		auto music = Musics.find(name);
		if (music != Musics.end()) {
			std::println("Music already loaded {} ", name);
			return;
		}
		Mix_Music* lMusic = Mix_LoadMUS(entry.path().string().c_str());
		if (lMusic) {
			Musics[name] = lMusic;
		}
	}
}

void SoundMan::LoadDir(const std::string& directory) {
	try {
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
			Load(entry);
		}
	}
	catch (std::exception& e) {
		std::println("SoundMan::LoadSounds Error loading directory: {}    {}", directory, e.what());
	}

}

void SoundMan::DeepLoad(const std::string& directory) {
	LoadDir(directory);
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_directory()) {
			const std::string path = entry.path().string();
			DeepLoad(path);
		}
	}
}

void SoundMan::PlaySound(const std::string& name, int volume) {
	auto it = Sounds.find(name);
	if (it != Sounds.end()) {
		PlaySound(it->second,volume);
		return;
	}
	std::println("Sound not found: {}", name);
}

void SoundMan::PlaySound(Mix_Chunk* sound, int volume) {
	if (!sound) { return; }
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
	PlaySoundStereo(sound->second, left, right,volume);
	return;
}

void SoundMan::PlaySoundStereo(Mix_Chunk* sound, int left, int right, int volume) {
	if (!sound) { return; }
	if (volume < 1) { return; }
	int channel = Mix_PlayChannel(-1, sound, 0);
	if (channel == -1) { return; }
	int SDL_Volume = (volume * MIX_MAX_VOLUME) / 100;
	int SDL_Left = (left * 255) / 100;
	int SDL_Right = (right * 255) / 100;
	Mix_SetPanning(channel, static_cast<Uint8>(SDL_Left), static_cast<Uint8>(SDL_Right));

	Mix_Volume(channel, SDL_Volume);
}

void SoundMan::PlayMusic(const std::string& name, int volume, MusicPlayType playType) {
	auto music = Musics.find(name);
	if (music == Musics.end()) {
		std::println("No music with name {} loaded SoundMan::PlayMusic", name);
		return;
	}
	Mix_PlayMusic(music->second,static_cast<int>(playType));

	int SDL_Volume = (volume * MIX_MAX_VOLUME) / 100;
	Mix_VolumeMusic(SDL_Volume);
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

void SoundMan::RefreshSoundsInFolder(const std::string& directory, bool removeInvalid, std::unordered_set<std::string>& namesCollector) {
	namespace fs = std::filesystem;
	for (fs::directory_entry entry : fs::directory_iterator(directory)) {
		if (entry.is_directory()) {
			RefreshSoundsInFolder(entry.path().string(), removeInvalid, namesCollector);
		}
		else {
			std::string stem = entry.path().stem().string();
			if (Sounds.find(stem) == Sounds.end()) {
				Load(entry);
				if (removeInvalid) {
					namesCollector.emplace(stem);
				}
			}
			else if (removeInvalid) {
				namesCollector.emplace(stem);
			}
		}
	}
}

void SoundMan::RefreshSounds(const std::string& directory, bool removeInvalid) {
	namespace fs = std::filesystem;
	if (!fs::exists(directory)) {
		std::println("SoundMan::RefreshSounds incorrect start directory");
		return;
	}
	std::unordered_set<std::string> namesCollector;
	if (removeInvalid) {
		namesCollector.reserve(Sounds.size());
	}

	RefreshSoundsInFolder(directory, removeInvalid, namesCollector);

	if (removeInvalid) {
		std::vector<std::string> soundsToErase;
		for (auto& [key, sound] : Sounds) {
			if (!namesCollector.contains(key)) {
				soundsToErase.emplace_back(key);
			}
		}
		for (auto& it : soundsToErase) {
			DeleteSound(it);
		}
	}
}

bool SoundMan::DeleteSound(const std::string& name) {
	auto it = Sounds.find(name);
	if (it != Sounds.end()) {
		Mix_FreeChunk(it->second);
		Sounds.erase(it);
		return true;
	}
	std::println("Sound not found: {} SoundMan::DeleteSound", name);
	return false;
}

bool SoundMan::DeleteMusic(const std::string& name) {
	auto it = Musics.find(name);
	if (it != Musics.end()) {
		Mix_FreeMusic(it->second);
		Musics.erase(it);
		return true;
	}
	std::println("Music not found: {} SoundMan::DeleteMusic", name);
	return false;
}

void SoundMan::Clear() {
	Mix_HaltMusic();
	for (auto& pair : Sounds) {
		Mix_FreeChunk(pair.second);
	}
	for (auto& pair : Musics) {
		Mix_FreeMusic(pair.second);
	}
	Sounds.clear();
	Musics.clear();
	Mix_CloseAudio();
	Mix_Quit();
}