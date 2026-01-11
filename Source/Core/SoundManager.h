#pragma once
#include "SDL_mixer.h"

#include <unordered_map>
#include <unordered_set>


class SoundMan
{
	private:
		inline static std::unordered_map<std::string, Mix_Chunk*> Sounds;

		static void RefreshSoundsInFolder(const std::string& directory, bool removeInvalid, std::unordered_set<std::string>& namesCollector);

	public:
		static void Innit();

		static void Print();

		static void LoadSound(const char* filePath, const std::string& name);

		static void LoadSounds(const std::string& directory);

		static void DeppLoad(const std::string& directory);

		static void PlaySound(const std::string& name, int volume = 100);

		static void PlayRawSound(Mix_Chunk* sound, int volume = 100);

		static void PlaySoundStereo(const std::string& name, int left, int right, int volume = 100);

		static void PlayRawSoundStereo(Mix_Chunk* sound, int left, int right, int volume = 100);

		static Mix_Chunk* GetSound(const std::string& name);

		static std::unordered_map<std::string, Mix_Chunk*> &GetSounds();

		static void SetVolume(const std::string& soundKey, unsigned char volume);

		static bool DeleteSound(const std::string& name);

		// Loads new previously unloaded sounds in slected folder and all recursive folders
		// removeInvalid flag will remove sounds that no longer exist WARNING this will break exsting pointers
		static void RefreshSounds(const std::string& directory, bool removeInvalid = false);

		static void Clear();
};
