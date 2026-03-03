#pragma once
#include <vector>
#include <memory>

#include "Renderer.h"

enum class AnimType {
	Looped,
	Singular,
	LoopedBack,
	SingularBack,

	EndStartLooped,
	StartEndLooped,

	EndStartSingular,
	StartEndSingular,

	// Do not set this one it is only used for 	EndStartSingular second stage
	EndStartSingularSecond,
	// Do not set this one it is only used for 	StartEndSingular second stage
	StartEndSingularSecond
};

class Animation {
	public:
		std::vector<MT::Rect> clips;
		unsigned int firstFrame = 0;
		unsigned int lastFrame = 0;
		unsigned int frameDelay = 20;
		AnimType type = AnimType::Singular; 

		MT::Rect &Get();

		void Reset();

		void CloneFrame(const unsigned int index, const unsigned int count);

};

Animation CreateAnimation(const int clipsAmount, const int frameWidth,
	const int frameHeight, const int frameDelay, AnimType type = AnimType::Singular, int step = 1, int xStart = 0, int yStart = 0);

Animation CreateAnimation(const std::vector<MT::Rect> &frames, const int frameDelay, AnimType type = AnimType::Singular);


Animation CopyAnimation(Animation* animation);