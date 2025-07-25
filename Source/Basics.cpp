#include "Basics.h"



GameObjectBasic::GameObjectBasic() {

}

GameObjectBasic::GameObjectBasic(int x, int y, int w, int h) {
	this->rectangle.x = x;
	this->rectangle.y = y;
	this->rectangle.w = w;
	this->rectangle.h = h;
}

SDL_Rect* GameObjectBasic::GetRectangle() {
	return &rectangle;
}


void GameObjectBasic::Innit(int x, int y, int w, int h) {
	this->rectangle.x = x;
	this->rectangle.y = y;
	this->rectangle.w = w;
	this->rectangle.h = h;
}
//////////////////////

SDL_Rect* GameObject::GetRectangle() {
	return &rectangle;
}

GameObject::GameObject() {

}

GameObject::GameObject(int x, int y, int w, int h, SDL_Texture* texture) {
	this->rectangle.x = x;
	this->rectangle.y = y;
	this->rectangle.w = w;
	this->rectangle.h = h;
	this->texture = texture;
}

SDL_Texture* GameObject::GetTexture() {
	return texture;
}

void GameObject::SetTexture(SDL_Texture* texture) {
	this->texture = texture;
}

void GameObject::Innit(int x, int y, int w, int h, SDL_Texture* texture) {
	this->rectangle.x = x;
	this->rectangle.y = y;
	this->rectangle.w = w;
	this->rectangle.h = h;
	this->texture = texture;
}