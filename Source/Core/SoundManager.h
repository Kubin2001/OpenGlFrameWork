#pragma once
#include "SDL_mixer.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <filesystem>

enum class MusicPlayType {
	Once = 1,
	Two = 2,
	Loop = -1
};

// Uses wav format for sounds and ogg or mp3 for music
class SoundMan{
	private:
		inline static std::unordered_map<std::string, Mix_Chunk*> Sounds;
		inline static std::unordered_map<std::string, Mix_Music*> Musics;

		static void RefreshSoundsInFolder(const std::string& directory, bool removeInvalid, std::unordered_set<std::string>& namesCollector);

	public:
		static void Init();

		static void Print();

		static void Load(const std::filesystem::directory_entry& entry);

		static void LoadDir(const std::string& directory);

		static void DeepLoad(const std::string& directory);

		static void PlaySound(const std::string& name, int volume = 100);

		static void PlaySound(Mix_Chunk* sound, int volume = 100);

		static void PlaySoundStereo(const std::string& name, int left, int right, int volume = 100);

		static void PlaySoundStereo(Mix_Chunk* sound, int left, int right, int volume = 100);

		static void PlayMusic(const std::string& name, int volume = 100, MusicPlayType playType = MusicPlayType::Loop);

		static void PauseMusic() {
			Mix_PauseMusic();
		}
		static void ResumeMusic() {
			Mix_ResumeMusic();
		}

		static void ToggleMusic() {
			if (Mix_PausedMusic()) {
				Mix_ResumeMusic();
			}
			else {
				Mix_PauseMusic();
			}
		}

		static Mix_Chunk* GetSound(const std::string& name);

		static std::unordered_map<std::string, Mix_Chunk*> &GetSounds();

		static void SetVolume(const std::string& soundKey, unsigned char volume);

		static bool DeleteSound(const std::string& name);

		static bool DeleteMusic(const std::string& name);

		// Loads new previously unloaded sounds in slected folder and all recursive folders
		// removeInvalid flag will remove sounds that no longer exist WARNING this will break exsting pointers
		static void RefreshSounds(const std::string& directory, bool removeInvalid = false);

		static void Clear();
};
