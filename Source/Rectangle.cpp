#include "Rectangle.h"

Point::Point() {

}

Point::Point(int x, int y) : x(x), y(y) {}


namespace MT {

    // ----- Rect method definitions -----

    Rect::Rect() {
        this->x = 0;
        this->y = 0;
        this->w = 0;
        this->h = 0;
    }

    Rect::Rect(const int x, const int y, const int w, const int h)
        : x(x), y(y), w(w), h(h) {
    }

    void Rect::Set(const int x, const int y, const int w, const int h) noexcept {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    Point Rect::GetCenter() {
        return { x + (int)(w * 0.5f),(int)(y + h * 0.5f) };
    }

    SDL_Rect Rect::ToSDLRect() {
        return { x,y,w, h };
    }

    RectF Rect::ToRectF() {
        return { static_cast<float>(x),static_cast<float>(y),
                 static_cast<float>(w), static_cast<float>(h) };
    }

    CompositeRect Rect::ToCompositeRect() {
        return { (float)x, (float)y , w, h };
    }

    bool Rect::IsColliding(const SDL_Rect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool Rect::IsColliding(const Rect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool Rect::IsColliding(const RectF& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool Rect::IsColliding(const CompositeRect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    Rect& Rect::operator=(const SDL_Rect& rect) {
        this->x = rect.x;
        this->y = rect.y;
        this->w = rect.w;
        this->h = rect.h;
        return *this;
    }

    Rect& Rect::operator=(const RectF& rect) {
        this->x = (int)rect.x;
        this->y = (int)rect.y;
        this->w = (int)rect.w;
        this->h = (int)rect.h;
        return *this;
    }

    Rect& Rect::operator=(const CompositeRect& rect) {
        this->x = (int)rect.x;
        this->y = (int)rect.y;
        this->w = rect.w;
        this->h = rect.h;
        return *this;
    }

    bool Rect::operator==(const SDL_Rect& rect) noexcept {
        if (this->x == rect.x && this->y == rect.y &&
            this->w == rect.w && this->h == rect.h) {
            return true;
        }
        return false;
    }

    bool Rect::operator==(const Rect& rect) noexcept {
        if (this->x == rect.x && this->y == rect.y &&
            this->w == rect.w && this->h == rect.h) {
            return true;
        }
        return false;
    }

    bool Rect::operator==(const RectF& rect) noexcept {
        if (this->x == (int)rect.x && this->y == (int)rect.y &&
            this->w == (int)rect.w && this->h == (int)rect.h) {
            return true;
        }
        return false;
    }

    bool Rect::operator==(const CompositeRect& rect) noexcept {
        if (this->x == (int)rect.x && this->y == (int)rect.y &&
            this->w == rect.w && this->h == rect.h) {
            return true;
        }
        return false;
    }

    RectF::RectF() {
        this->x = 0.0f;
        this->y = 0.0f;
        this->w = 0.0f;
        this->h = 0.0f;
    }

    RectF::RectF(const float x, const float y, const float w, const float h): x(x), y(y), w(w), h(h) {
    }

    void RectF::Set(const float x, const float y, const float w, const float h) noexcept {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    Point RectF::GetCenter() {
        return { static_cast<int>(x) + static_cast<int>(w * 0.5f),
                 static_cast<int>(y) + static_cast<int>(h * 0.5f) };
    }

    SDL_Rect RectF::ToSDLRect() {
        return { static_cast<int>(x), static_cast<int>(y),
                 static_cast<int>(w), static_cast<int>(h) };
    }

    Rect RectF::ToRect() {
        return { static_cast<int>(x), static_cast<int>(y),
                 static_cast<int>(w), static_cast<int>(h) };
    }

    CompositeRect RectF::ToCompositeRect() {
        return { x, y, static_cast<int>(w), static_cast<int>(h) };
    }

    bool RectF::IsColliding(const SDL_Rect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool RectF::IsColliding(const Rect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool RectF::IsColliding(const RectF& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool RectF::IsColliding(const CompositeRect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    RectF& RectF::operator=(const SDL_Rect& rect) {
        this->x = (float)rect.x;
        this->y = (float)rect.y;
        this->w = (float)rect.w;
        this->h = (float)rect.h;
        return *this;
    }

    RectF& RectF::operator=(const Rect& rect) {
        this->x = (float)rect.x;
        this->y = (float)rect.y;
        this->w = (float)rect.w;
        this->h = (float)rect.h;
        return *this;
    }

    RectF& RectF::operator=(const CompositeRect& rect) {
        this->x = rect.x;
        this->y = rect.y;
        this->w = (float)rect.w;
        this->h = (float)rect.h;
        return *this;
    }

    bool RectF::operator==(const SDL_Rect& rect) noexcept {
        if ((int)this->x == rect.x && (int)this->y == rect.y &&
            (int)this->w == rect.w && (int)this->h == rect.h) {
            return true;
        }
        return false;
    }

    bool RectF::operator==(const Rect& rect) noexcept {
        if ((int)this->x == rect.x && (int)this->y == rect.y &&
            (int)this->w == rect.w && (int)this->h == rect.h) {
            return true;
        }
        return false;
    }

    bool RectF::operator==(const RectF& rect) noexcept {
        if (this->x == rect.x && this->y == rect.y &&
            this->w == rect.w && this->h == rect.h) {
            return true;
        }
        return false;
    }

    bool RectF::operator==(const CompositeRect& rect) noexcept {
        if (this->x == rect.x && this->y == rect.y &&
            this->w == rect.w && this->h == rect.h) {
            return true;
        }
        return false;
    }

    CompositeRect::CompositeRect() {
        this->x = 0.0f;
        this->y = 0.0f;
        this->w = 0;
        this->h = 0;
    }

    CompositeRect::CompositeRect(const float x, const float y, const int w, const int h)
        : x(x), y(y), w(w), h(h) {
    }

    void CompositeRect::Set(const float x, const float y, const int w, const int h) noexcept {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    // ----- Conversion methods -----

    Point CompositeRect::GetCenter() {
        return { (int)(x + w * 0.5f), (int)(y + h * 0.5f) };
    }

    SDL_Rect CompositeRect::ToSDLRect() {
        return { (int)x, (int)y, w, h };
    }

    RectF CompositeRect::ToRectF() {
        return { x, y, static_cast<float>(w), static_cast<float>(h) };
    }

    Rect CompositeRect::ToRect() {
        return { (int)x, (int)y, w, h };
    }

    // ----- Collision -----

    bool CompositeRect::IsColliding(const SDL_Rect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool CompositeRect::IsColliding(const Rect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool CompositeRect::IsColliding(const RectF& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    bool CompositeRect::IsColliding(const CompositeRect& rect) noexcept {
        if (rect.x + rect.w >= x &&
            rect.x - w <= x &&
            rect.y + rect.h >= y &&
            rect.y - h <= y) {
            return true;
        }
        return false;
    }

    // ----- Assignment -----

    CompositeRect& CompositeRect::operator=(const SDL_Rect& rect) {
        this->x = (float)rect.x;
        this->y = (float)rect.y;
        this->w = rect.w;
        this->h = rect.h;
        return *this;
    }

    CompositeRect& CompositeRect::operator=(const RectF& rect) {
        this->x = rect.x;
        this->y = rect.y;
        this->w = (int)rect.w;
        this->h = (int)rect.h;
        return *this;
    }

    CompositeRect& CompositeRect::operator=(const Rect& rect) {
        this->x = (float)rect.x;
        this->y = (float)rect.y;
        this->w = rect.w;
        this->h = rect.h;
        return *this;
    }

    // ----- Comparison -----

    bool CompositeRect::operator==(const SDL_Rect& rect) noexcept {
        if ((int)this->x == rect.x && (int)this->y == rect.y &&
            this->w == rect.w && this->h == rect.h) {
            return true;
        }
        return false;
    }

    bool CompositeRect::operator==(const Rect& rect) noexcept {
        if ((int)this->x == rect.x && (int)this->y == rect.y &&
            this->w == rect.w && this->h == rect.h) {
            return true;
        }
        return false;
    }

    bool CompositeRect::operator==(const RectF& rect) noexcept {
        if (this->x == rect.x && this->y == rect.y &&
            this->w == (int)rect.w && this->h == (int)rect.h) {
            return true;
        }
        return false;
    }

    bool CompositeRect::operator==(const CompositeRect& rect) noexcept {
        if (this->x == rect.x && this->y == rect.y &&
            this->w == rect.w && this->h == rect.h) {
            return true;
        }
        return false;
    }
} 