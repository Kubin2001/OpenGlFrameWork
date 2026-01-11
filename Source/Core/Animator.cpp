#include "Animator.h"

#include "GlobalVariables.h"

Animation* CreateAnimation(const int clipsAmount, const short frameWidth,const short frameHeight, 
	const int frameDelay, const int type) {

	Animation* anim = new Animation();
	anim->firstFrame = Global::frameCounter;
	anim->frameDelay = frameDelay;
	anim->lastFrame = frameDelay * clipsAmount;
	anim->type = type;
	anim->clips.reserve(clipsAmount);
	int x = 0;
	int y = 0;
	int w = frameWidth;
	int h = frameHeight;
	for (size_t i = 0; i < clipsAmount; i++) {
		anim->clips.emplace_back(x, y, w, h);
		x += (frameWidth + 1);
	}

	return anim;
}

void DeleteAnimation(Animation*& animation) {
	if (animation != nullptr) {
		delete animation;
	}
	animation = nullptr;
}

MT::Rect& Animation::Get() {
	int currentFrame = Global::frameCounter - firstFrame;
	switch (type) {
		case 0:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				return clips[0];
			}
			return clips[currentFrame / frameDelay];
		case 1:
			if (currentFrame >= lastFrame) {
				return clips.back();
			}
			return clips[currentFrame / frameDelay];

		case 2:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				return clips.back();
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		case 3:
			if (currentFrame >= lastFrame) {
				return clips[0];
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		case 4:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				type = 5;
				return clips[0];
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		case 5:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				type = 4;
				return clips.back();
			}
			return clips[currentFrame / frameDelay];

		case 6:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				type = 8;
				return clips[0];
			}
			return clips[(clips.size() - 1) - (currentFrame / frameDelay)];

		case 7:
			if (currentFrame >= lastFrame) {
				firstFrame = Global::frameCounter;
				type = 9;
				return clips.back();
			}
			return clips[currentFrame / frameDelay];

		case 8:
			if (currentFrame >= lastFrame) {
				return clips.back();
			}
			return clips[currentFrame / frameDelay];

		case 9:
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
	if (type == 9) { type = 7; }
	if (type == 8) { type = 6; }
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

Animation* CopyAnimation(Animation* animation) {
	if (animation == nullptr) {
		return nullptr;
	}
	Animation* anim = new Animation();
	anim->firstFrame = Global::frameCounter;
	anim->frameDelay = animation->frameDelay;
	anim->lastFrame = animation->frameDelay * animation->clips.size();
	anim->type = animation->type;
	anim->clips = animation->clips;
	return anim;
}
