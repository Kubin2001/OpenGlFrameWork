#include "Animator.h"

#include "GlobalVariables.h"

Animation CreateAnimation(const int clipsAmount, const int frameWidth, const int frameHeight, 
	const int frameDelay, AnimType type, int step, int xStart, int yStart) {

	Animation anim;

	anim.firstFrame = Global::frameCounter;
	anim.frameDelay = frameDelay;
	anim.lastFrame = frameDelay * clipsAmount;
	anim.type = type;
	anim.clips.reserve(clipsAmount);

	int x = xStart;
	for (size_t i = 0; i < clipsAmount; i++) {
		anim.clips.emplace_back(x, yStart, frameWidth, frameHeight);
		x += (frameWidth + step);
	}

	return anim;
}


Animation CreateAnimation(const std::vector<MT::Rect>& frames, const int frameDelay, AnimType type) {
	Animation anim;
	anim.firstFrame = Global::frameCounter;
	anim.frameDelay = frameDelay;
	anim.lastFrame = frameDelay * frames.size();
	anim.type = type;
	anim.clips = frames;

	return anim;
}

MT::Rect& Animation::Get() {
	unsigned int currentFrame = static_cast<unsigned int>(Global::frameCounter - firstFrame);
	switch (type) {
		case AnimType::Looped:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				return clips[0];
			}
			return clips[currentFrame / frameDelay];
		case AnimType::Singular:
			if (currentFrame >= lastFrame) {
				return clips.back();
			}
			return clips[currentFrame / frameDelay];

		case AnimType::LoopedBack:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				return clips.back();
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		case AnimType::SingularBack:
			if (currentFrame >= lastFrame) {
				return clips[0];
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		case AnimType::EndStartLooped:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				type = AnimType::StartEndLooped;
				return clips[0];
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		case AnimType::StartEndLooped:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				type = AnimType::EndStartLooped;
				return clips.back();
			}
			return clips[currentFrame / frameDelay];

		case AnimType::EndStartSingular:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				type = AnimType::EndStartSingularSecond;
				return clips[0];
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		case AnimType::StartEndSingular:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				type = AnimType::StartEndSingularSecond;
				return clips.back();
			}
			return clips[currentFrame / frameDelay];

		case AnimType::EndStartSingularSecond:
			if (currentFrame >= lastFrame) {
				return clips.back();
			}
			return clips[currentFrame / frameDelay];

		case AnimType::StartEndSingularSecond:
			if (currentFrame >= lastFrame) {
				return clips[0];
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		default:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				return clips[0];
			}
			return clips[currentFrame / frameDelay];
	}
}

void Animation::Reset() {
	firstFrame = Global::frameCounter;
	if (type == AnimType::StartEndSingularSecond) { type = AnimType::StartEndSingular; }
	if (type == AnimType::EndStartSingularSecond) { type = AnimType::EndStartSingular; }
}

void Animation::CloneFrame(const unsigned int index, const unsigned int count) {
	MT::Rect copyClip = clips[index];
	std::vector<MT::Rect> copyVec;
	copyVec.reserve(count);
	for (size_t i = 0; i < count; ++i) {
		copyVec.emplace_back(copyClip);
	}

	clips.insert(clips.begin() + index, copyVec.begin(), copyVec.end());

	lastFrame += copyVec.size() * frameDelay;
}

Animation CopyAnimation(Animation* animation) {
	if (animation == nullptr) {
		throw std::exception("Passed animation is empty cannot copy");
	}
	Animation anim;
	anim.firstFrame = Global::frameCounter;
	anim.frameDelay = animation->frameDelay;
	anim.lastFrame = animation->frameDelay * animation->clips.size();
	anim.type = animation->type;
	anim.clips = animation->clips;
	return anim;
}
