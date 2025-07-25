#include "Animator.h"
#include "GlobalVariables.h"
#include <iostream>

std::unordered_map<std::string, Animation*> Animator::Animations;

void Animator::CreateNew(const std::string& name, const int clipsAmount, const short frameWidth, 
	const short frameHeight, const int frameDelay, const int type) {

	auto mapAnim = Animations.find(name);
	if (mapAnim != Animations.end()) {
		std::cout << "Creation aborted animation " << name << " already exist\n";
		return;
	}

	Animation* anim = new Animation();
	anim->name = name;
	anim->firstFrame = Global::frameCounter;
	anim->frameDelay = frameDelay;
	anim->lastFrame = frameDelay * clipsAmount;
	anim->type = type;
	anim->clips.reserve(clipsAmount);
	int x = 0;
	int y = 0;
	int w = frameWidth;
	int h = frameHeight;
	for (size_t i = 0; i < clipsAmount; i++){
		anim->clips.emplace_back(x,y,w,h);
		x += (frameWidth + 1);
	}

	Animations[name] = anim;
}

SDL_Rect* Animator::Get(const std::string& key) {
	auto mapAnim = Animations.find(key);
	if (mapAnim == Animations.end()) {
		std::cout << "This animation does not exist returned empty rectangle\n";
		return nullptr;
	}
	auto& anim = mapAnim->second;
	int currentFrame = Global::frameCounter - anim->firstFrame;
	switch (anim->type){
		case 0:
			if (currentFrame >= anim->lastFrame) {
				anim->firstFrame = Global::frameCounter;
				return &anim->clips[0];
			}
			return &anim->clips[currentFrame / anim->frameDelay];
		case 1:
			if (currentFrame >= anim->lastFrame) {
				return &anim->clips.back();
			}
			return &anim->clips[currentFrame / anim->frameDelay];

		case 2:
			if (currentFrame >= anim->lastFrame) {
				anim->firstFrame = Global::frameCounter;
				return &anim->clips.back();
			}
			return &anim->clips[(anim->clips.size() -1) - (currentFrame / anim->frameDelay)];

		case 3:
			if (currentFrame >= anim->lastFrame) {
				return &anim->clips[0];
			}
			return &anim->clips[(anim->clips.size() - 1) - (currentFrame / anim->frameDelay)];

		case 4:
			if (currentFrame >= anim->lastFrame) {
				anim->firstFrame = Global::frameCounter;
				anim->type = 5;
				return &anim->clips[0];
			}
			return &anim->clips[(anim->clips.size() - 1) - (currentFrame / anim->frameDelay)];

		case 5:
			if (currentFrame >= anim->lastFrame) {
				anim->firstFrame = Global::frameCounter;
				anim->type = 4;
				return &anim->clips.back();
			}
			return &anim->clips[currentFrame / anim->frameDelay];

		case 6:
			if (currentFrame >= anim->lastFrame) {
				anim->firstFrame = Global::frameCounter;
				anim->type = 8;
				return &anim->clips[0];
			}
			return &anim->clips[(anim->clips.size() - 1) - (currentFrame / anim->frameDelay)];

		case 7:
			if (currentFrame >= anim->lastFrame) {
				anim->firstFrame = Global::frameCounter;
				anim->type = 9;
				return &anim->clips.back();
			}
			return &anim->clips[currentFrame / anim->frameDelay];

		case 8:
			if (currentFrame >= anim->lastFrame) {
				return &anim->clips.back();
			}
			return &anim->clips[currentFrame / anim->frameDelay];

		case 9:
			if (currentFrame >= anim->lastFrame) {
				return &anim->clips[0];
			}
			return &anim->clips[(anim->clips.size() - 1) - (currentFrame / anim->frameDelay)];

		default:
			if (currentFrame >= anim->lastFrame) {
				anim->firstFrame = Global::frameCounter;
				return &anim->clips[0];
			}
			return &anim->clips[currentFrame / anim->frameDelay];
	}
}

Animation* Animator::GetAnim(const std::string& key) {
	auto mapAnim = Animations.find(key);
	if (mapAnim == Animations.end()) {
		std::cout << "This animation does not exist returned nullptr\n";
		return nullptr;
	}
	else {
		return mapAnim->second;
	}
}

void Animator::Reset(const std::string& key) {
	auto mapAnim = Animations.find(key);
	if (mapAnim == Animations.end()) {
		std::cout << "This animation does not exist: "<<key << " reset aborted\n";
		return;
	}
	mapAnim->second->firstFrame = Global::frameCounter;

	if (mapAnim->second->type == 9) { mapAnim->second->type = 7; }
	if (mapAnim->second->type == 8) { mapAnim->second->type = 6; }
}

bool Animator::ClearSingle(const std::string& key) {
	return Animations.erase(key);
}

bool Animator::CloneFrame(const std::string& key, const unsigned int frame, const unsigned int index, const unsigned int count) {
	auto mapAnim = Animations.find(key);
	if (mapAnim == Animations.end()) {
		std::cout << "This animation does not exist impossible to copy frame\n";
		return false;
	}
	auto& anim = mapAnim->second;
	SDL_Rect copyClip =  anim->clips[frame];
	std::vector<SDL_Rect> copyVec;
	copyVec.reserve(count);
	for (size_t i = 0; i < count; ++i){
		copyVec.emplace_back(copyClip);
	}

	anim->clips.insert(anim->clips.begin() + index, copyVec.begin(), copyVec.end());

	anim->lastFrame += copyVec.size() * anim->frameDelay;
	return true;
}

bool Animator::DeleteAnimation(const std::string& key) {
	auto mapAnim = Animations.find(key);
	if (mapAnim == Animations.end()) {
		std::cout << "This animation does not exist Animator::DeleteAnimation\n";
		return false;
	}
	delete Animations[key];
	Animations.erase(key);
	return true;
}

void Animator::ClearAll() {
	for (auto& elem : Animations) {
		delete elem.second;
	}
	Animations.clear();
}