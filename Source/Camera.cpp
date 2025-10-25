#include "Camera.h"

Camera::Camera(bool useBorders) {
	this->useBorders = useBorders;

}
float Camera::GetZoom() {
	return zoom;
}

MT::Rect &Camera::GetRectangle() {
	return rectangle;
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

void Camera::Enable() {
	enabled = true;
}

void Camera::Disable() {
	enabled = false;
}

SDL_Rect Camera::TransformFlat(const SDL_Rect& rect) {
	return { rect.x - rectangle.x ,rect.y - rectangle.y ,rect.w,rect.h };
}
MT::Rect Camera::TransformFlat(const MT::Rect& rect) {
	return { rect.x - rectangle.x ,rect.y - rectangle.y ,rect.w,rect.h };
}
MT::RectF Camera::TransformFlat(const MT::RectF& rect) {
	return { (float)(rect.x - rectangle.x) ,(float)(rect.y - rectangle.y) 
		,(float)(rect.w),(rect.h) };
}
MT::CompositeRect Camera::TransformFlat(const MT::CompositeRect& rect) {
	return { (float)(rect.x - rectangle.x) ,(float)(rect.y - rectangle.y) ,rect.w,rect.h };
}



SDL_Rect Camera::Transform(const SDL_Rect& rect) {
	SDL_Rect r{0, 0, static_cast<int>(rect.w * zoom) + 1,static_cast<int>(rect.h * zoom) + 1}; // +1 to prevent white lines
	r.x = static_cast<int>((rect.x - rectangle.x) * zoom);
	r.y = static_cast<int>((rect.y - rectangle.y) * zoom);
	return r;
}

MT::Rect Camera::Transform(const MT::Rect& rect) {
	MT::Rect r{
		static_cast<int>((rect.x - static_cast<int>(rectangle.x)) * zoom),
		static_cast<int>((rect.y - static_cast<int>(rectangle.y)) * zoom),
		static_cast<int>(rect.w * zoom) + 1,
		static_cast<int>(rect.h * zoom) + 1
	};
	return r;
}

// MT::RectF
MT::RectF Camera::Transform(const MT::RectF& rect) {
	MT::RectF r{
		(rect.x - rectangle.x) * zoom,
		(rect.y - rectangle.y) * zoom,
		rect.w * zoom + 1.0f,
		rect.h * zoom + 1.0f
	};
	return r;
}

// MT::CompositeRect
MT::CompositeRect Camera::Transform(const MT::CompositeRect& rect) {
	MT::CompositeRect r{
		(rect.x - rectangle.x) * zoom,
		(rect.y - rectangle.y) * zoom,
		static_cast<int>(rect.w * zoom) + 1,
		static_cast<int>(rect.h * zoom) + 1
	};
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

Point Camera::GetMouseUntransform() {
	int x, y;
	SDL_GetMouseState(&x, &y);
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

void Camera::Focus(const int x, const int y) {
	rectangle.x = x - (int)((rectangle.w / zoom) / 2);
	rectangle.y = y - (int)((rectangle.h / zoom) / 2);
}

