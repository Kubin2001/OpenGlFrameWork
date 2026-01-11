#pragma once
#include <vector>

#include "Renderer.h"

class Animation {
	public:
		std::vector<MT::Rect> clips;
		unsigned int firstFrame = 0;
		unsigned int lastFrame = 0;
		unsigned short frameDelay = 20;
		int type = 0; // 0 looped 1 singular 2 looped Back 3 singular Back 
		//4 back-up looped 5 up-back looped
		//6 back - up singular 8 up - back singullar
		//7 up - back singular 9 back - up singullar

		MT::Rect &Get();

		void Reset();

		void CloneFrame(const unsigned int index, const unsigned int count);

};

Animation* CreateAnimation(const int clipsAmount, const short frameWidth,
	const short frameHeight, const int frameDelay, const int type = 1);

void DeleteAnimation(Animation*& animation);

Animation* CopyAnimation(Animation* animation);