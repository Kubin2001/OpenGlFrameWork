#include <iostream>
#include <SDL.h>
#include "Camera.h"

Camera::Camera(bool useBorders) {
	this->useBorders = useBorders;

}
float Camera::GetZoom() {
	return zoom;
}

SDL_Rect* Camera::GetRectangle() {
	return &rectangle;
}

void Camera::UseBorders(bool temp) {
	this->useBorders = temp;
}

void Camera::SetBorders(int minX, int maxX, int minY, int maxY) {
	this->useBorders = true;
	this->minX = minX;
	this->maxX = maxX;
	this->minY = minY;
	this->maxY = maxY;
}

void Camera::SetZoomValue(const float val) {
	zoomValue = val;
}

void Camera::SetMaxMinZoom(const float min, const float max) {
	zoomMin = min;
	zoomMax = max;
}

void Camera::UpdatePosition(const Uint8* state) {
	if (!useBorders) {
		if (state[SDL_SCANCODE_D]) {
			rectangle.x += moveSpeed + (zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_A]) {
			rectangle.x -= moveSpeed + (zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_S]) {
			rectangle.y += moveSpeed + (zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_W]) {
			rectangle.y -= moveSpeed + (zoomRelativeMoveSpeed * 15);
		}
	}
	else{
		if (state[SDL_SCANCODE_D] && (rectangle.x + GetScaledWidth()) < maxX) {
			rectangle.x += moveSpeed + (zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_A] && rectangle.x > minX) {
			rectangle.x -= moveSpeed + (zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_S] && (rectangle.y + GetScaledHeight()) < maxY) {
			rectangle.y += moveSpeed + (zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_W] && rectangle.y > minY) {
			rectangle.y -= moveSpeed + (zoomRelativeMoveSpeed * 15);
		}


	}

}

void Camera::UpdateZoom(SDL_Event& event) {
	if (event.type == SDL_MOUSEWHEEL) {
		if (event.wheel.y > 0) { // Scroll Up (Zoom In)
			if (zoom < zoomMin) {
				zoom += zoomValue;
				if (zoomRelativeMoveSpeed > 0.06f) {
					zoomRelativeMoveSpeed -= 0.05f;
				}
			}
		}
		else if (event.wheel.y < 0) { // Scroll Down (Zoom Out)
			if (zoom > zoomMax) {
				zoom -= zoomValue;;
				zoomRelativeMoveSpeed += 0.05f;
			}
		}
	}
}

SDL_Rect Camera::TransformFlat(const SDL_Rect& rect) {
	SDL_Rect r{ 0,0,rect.w,rect.h };
	r.x = rect.x - rectangle.x;
	r.y = rect.y - rectangle.y;
	return r;
}


SDL_Rect Camera::Transform(const SDL_Rect& rect) {
	SDL_Rect r{0, 0, static_cast<int>(rect.w * zoom) + 1,static_cast<int>(rect.h * zoom) + 1}; // +1 to prevent white lines
	r.x = static_cast<int>((rect.x - rectangle.x) * zoom);
	r.y = static_cast<int>((rect.y - rectangle.y) * zoom);
	return r;
}

Point Camera::UntransformFlat(int x, int y) {
	return { x + rectangle.x,y + rectangle.y };
}

Point Camera::Untransform(int x, int y) {
	return {
		static_cast<int>(x / zoom + rectangle.x),
		static_cast<int>(y / zoom + rectangle.y)
	};
}

int Camera::GetScaledWidth() {
	return static_cast<int>(rectangle.w / zoom);
}

int Camera::GetScaledHeight() {
	return static_cast<int>(rectangle.h / zoom);
}


void Camera::SetMoveSpeed(const int temp) {
	this->moveSpeed = temp;
}

int& Camera::GetMoveSpeed() { return this->moveSpeed; }

