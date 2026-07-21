#include "Camera.h"

void Camera::UpdatePosition(const Uint8* state) {
	if (!enabled) { return; }
	if (!useBorders) {
		if (state[right]) {
			rectangle.x += moveSpeed / zoom;
		}
		if (state[left]) {
			rectangle.x -= moveSpeed /zoom;
		}
		if (state[down]) {
			rectangle.y += moveSpeed / zoom;
		}
		if (state[up]) {
			rectangle.y -= moveSpeed / zoom;
		}
	}
	else{
		if (state[right] && (rectangle.x + GetScaledWidth()) < maxX) {
			rectangle.x += moveSpeed / zoom;
		}
		if (state[left] && rectangle.x > minX) {
			rectangle.x -= moveSpeed / zoom;
		}
		if (state[down] && (rectangle.y + GetScaledHeight()) < maxY) {
			rectangle.y += moveSpeed / zoom;
		}
		if (state[up] && rectangle.y > minY) {
			rectangle.y -= moveSpeed / zoom;
		}
	}
}

void Camera::UpdateZoom(SDL_Event& event) {
    if (!enabled) { return; }

    if (event.type == SDL_MOUSEWHEEL) {
        float oldZoom = zoom;

        if (event.wheel.y > 0) { // Scroll Up (Zoom In)
            if (zoom < zoomMax) {
                zoom += zoomStrength;
                if (zoom > zoomMax) { zoom = zoomMax; }
            }
        }
        else if (event.wheel.y < 0) { // Scroll Down (Zoom Out)
            if (zoom > zoomMin) {
                zoom -= zoomStrength;
                if (zoom < zoomMin) { zoom = zoomMin; }
            }
        }

        if (zoom == oldZoom) { return; }

        float oldWorldWidth = rectangle.w / oldZoom;
        float oldWorldHeight = rectangle.h / oldZoom;

        float newWorldWidth = rectangle.w / zoom;
        float newWorldHeight = rectangle.h / zoom;

        rectangle.x += (oldWorldWidth - newWorldWidth) / 2.0f;
        rectangle.y += (oldWorldHeight - newWorldHeight) / 2.0f;

        if (useBorders) {
            int scaledW = static_cast<int>(newWorldWidth);
            int scaledH = static_cast<int>(newWorldHeight);

            if (rectangle.x < minX) rectangle.x = minX;
            if (rectangle.x + scaledW > maxX) rectangle.x = maxX - scaledW;

            if (rectangle.y < minY) rectangle.y = minY;
            if (rectangle.y + scaledH > maxY) rectangle.y = maxY - scaledH;
        }
    }
}


