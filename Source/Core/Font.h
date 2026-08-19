#pragma once
#include <vector>
#include <array>

#include "Renderer.h"

class LocalTexMan;


class Font{
private:
	std::string name;
	std::vector<MT::Rect> sourceRectangles;
	int standardInterLine = 0;

public:
	MT::Rect rect{};
	MT::Texture* texture = nullptr;

	Font(const std::string& name, MT::Texture* texture, const std::string& jsonPath);

	Font(const std::string& name, MT::Texture* texture, const std::string &charset, std::vector<MT::Rect>& rectangles);

	std::string GetName();

	int GetStandardInterline();

	void SetStandardInterline(int temp);

	bool LoadTextInfo(const std::string& jsonPath);

	void LoadTextCharset(const std::string& charset,std::vector<MT::Rect>& rectangles);

	void RenderRawText(MT::Renderer* renderer, const int x, const int y, const std::string& text, const int interline,
		const MT::Color &color);

	void RenderText(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, const MT::Color &color, float scale = 1.0,
		int interline = 20, int textStartX = 0, int textStartY = 0);

	void RenderTextCenter(MT::Renderer* renderer, const std::string& text, MT::Rect &btnRect, const MT::Color& color, float scale = 1.0,
		int interline = 20, int textStartX = 0, int textStartY = 0);

	void RenderTextFromRight(MT::Renderer* renderer, const std::string& text, MT::Rect &btnRect, const MT::Color& color, float scale = 1.0, int interline = 20,
		int textStartX = 0, int textStartY = 0);


	void RenderTextCenterX(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, const MT::Color& color, float scale = 1.0,
		int interline = 20, int textStartX = 0, int textStartY = 0);


	void RenderTextCenterY(MT::Renderer* renderer, const std::string& text, MT::Rect& btnRect, const MT::Color& color, float scale = 1.0,
		int interline = 20, int textStartX = 0, int textStartY = 0);

	Point CalculatePredefinedSize(const std::string& fontText, const int interline, const float scale);
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

	bool CrateTempFontFromTTF(const char* ttfPath, const int size, const std::string& name, LocalTexMan *localTexMan = nullptr);

	~FontManager();
};

void CrateFontFromTTF(const char* ttfPath, const int size, const std::string& name, const std::string& outPath = "");
