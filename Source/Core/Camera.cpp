#include "Camera.h"

void Camera::UpdatePosition(const Uint8* state) {
	if (!enabled) { return; }
	if (!useBorders) {
		if (state[SDL_SCANCODE_D]) {
			rectangle.x += moveSpeed + (int)(zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_A]) {
			rectangle.x -= moveSpeed + (int)(zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_S]) {
			rectangle.y += moveSpeed + (int)(zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_W]) {
			rectangle.y -= moveSpeed + (int)(zoomRelativeMoveSpeed * 15);
		}
	}
	else{
		if (state[SDL_SCANCODE_D] && (rectangle.x + GetScaledWidth()) < maxX) {
			rectangle.x += moveSpeed + (int)(zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_A] && rectangle.x > minX) {
			rectangle.x -= moveSpeed + (int)(zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_S] && (rectangle.y + GetScaledHeight()) < maxY) {
			rectangle.y += moveSpeed + (int)(zoomRelativeMoveSpeed * 15);
		}
		if (state[SDL_SCANCODE_W] && rectangle.y > minY) {
			rectangle.y -= moveSpeed + (int)(zoomRelativeMoveSpeed * 15);
		}
	}

}

void Camera::UpdateZoom(SDL_Event& event) {
	if (!enabled) { return; }
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


