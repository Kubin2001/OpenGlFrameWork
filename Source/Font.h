#pragma once
#include <vector>
#include <array>

#include "Basics.h"


constexpr std::array<char32_t, 200> MakeCharset() {
	std::array<char32_t, 200> cs{};
	for (size_t i = 0; i < cs.size(); i++) {
		cs[i] = (char32_t)(i);
	}
	return cs;
}

static constexpr std::array<char32_t, 200> charset = MakeCharset();

class Font : public GameObject{
private:
	std::string name;
	std::vector<MT::Rect> sourceRectangles;
	int standardInterLine = 0;
	unsigned char rFilter = 255;
	unsigned char gFilter = 255;
	unsigned char bFilter = 255;

public:
	Font(const std::string& name, MT::Texture* texture, const std::string& jsonPath);

	Font(const std::string& name, MT::Texture* texture, const std::string &charset, std::vector<MT::Rect>& rectangles);

	std::string GetName();

	int GetStandardInterline();

	void SetStandardInterline(int temp);

	bool LoadTextInfo(const std::string& jsonPath);

	void LoadTextCharset(const std::string& charset,std::vector<MT::Rect>& rectangles);

	void RenderRawText(MT::Renderer* renderer, const int x, const int y, const std::string& text, const int interline,
		const unsigned char R, const unsigned char G, const unsigned char B);

	void RenderText(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, float scale = 1.0,
		int interline = 20, int textStartX = 0, int textStartY = 0);

	void RenderTextCenter(MT::Renderer* renderer, const std::string& text, MT::Rect &btnRect, float scale = 1.0,
		int interline = 20, int textStartX = 0, int textStartY = 0);

	void RenderTextFromRight(MT::Renderer* renderer, const std::string& text, MT::Rect &btnRect, float scale = 1.0, int interline = 20,
		int textStartX = 0, int textStartY = 0);


	void RenderTextCenterX(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, float scale = 1.0,
		int interline = 20, int textStartX = 0, int textStartY = 0);


	void RenderTextCenterY(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, float scale = 1.0,
		int interline = 20, int textStartX = 0, int textStartY = 0);

	Point CalculatePredefinedSize(const std::string& fontText, const int interline, const float scale);

	void SetFilter(const unsigned char r, const unsigned char g, const unsigned char b);
};

class FontManager {
private:

public:
	std::vector<Font*> fonts;

	FontManager();

	bool CreateFont(const std::string& name, MT::Texture* texture, const std::string& jsonPath);

	Font* GetFont(const std::string& name);

	void ScanFont(const std::string& texturePath, const std::string& charactersDataPath,
		unsigned char fR, unsigned char fG, unsigned char fB, unsigned char bR, unsigned char bG, unsigned char bB, int width, int height
	, const std::string& outputPath = "font.json");

	void CrateTempFontFromTTF(const char* ttfPath, const int size, const std::string& name);

	~FontManager();
};

void CrateFontFromTTF(const char* ttfPath, const int size, const std::string& name, const std::string& outPath = "");
