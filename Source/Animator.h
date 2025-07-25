#pragma once
#include <SDL.h>
#include <unordered_map>
#include <string>
#include <vector>

class Animation {
	public:
		std::string name = "";
		std::vector<SDL_Rect> clips;
		unsigned int firstFrame = 0;
		unsigned int lastFrame = 0;
		unsigned short frameDelay = 20;
		int type = 0; // 0 looped 1 singular 2 looped Back 3 singular Back 
		//4 back-up looped 5 up-back looped
		//6 back - up singular 8 up - back singullar
		//7 up - back singular 9 back - up singullar

};

class Animator {
	private:
		static std::unordered_map<std::string, Animation*> Animations;

	public:
		static void CreateNew(const std::string& name, const int clipsAmount, const short frameWidth,
			const short frameHeight, const int frameDelay, const int type = 1);

		static SDL_Rect* Get(const std::string& key);

		static Animation* GetAnim(const std::string& key);

		static void Reset(const std::string& key);

		static bool ClearSingle(const std::string& key);

		static bool CloneFrame(const std::string& key, const unsigned int frame, const unsigned int index, const unsigned int count);

		static bool DeleteAnimation(const std::string& key);

		static void ClearAll();
};
