#pragma once
#include <iostream>
#include <SDL.h>
#include <vector>
#include <optional>
#include <map>
#include "Font.h"
#include "TextureManager.h"
#include "unordered_map"
#include "SoundManager.h"

class TemplateUIElement {
protected:
	SDL_Texture* texture = nullptr;
	SDL_Rect rectangle = {0,0,0,0};
	std::string name = "";
	std::string text = "";
	float textScale = 1.0f;
	int interLine = 20;

	bool border = false;

	int borderThickness = 0;

	int textStartX = 0;
	int textStartY = 0;

	bool buttonTransparent = false;
	unsigned char buttonColor[3] = { 255,255,255 };

	unsigned char borderRGB[3] = { 255,255,255 };

	unsigned char fontRGB[3] = { 255,255,255 };

	Font* font = nullptr;

	unsigned short textRenderType = 1;

	Point predefinedSize;

	bool hidden = false;

	bool hovered = false; // Is button in collidion with mouse

	bool hoverable = false; // Is hover filter aplied with mouse collisojn

	unsigned char hooverFilter[4] = { 0,0,0,0 };

	std::string hooverSound = "";

	bool GetBorder();

	void SetBorder(bool temp);

public:

	SDL_Texture* GetTexture();

	void SetTexture(SDL_Texture* temp);

	SDL_Rect* GetRectangle();

	std::string& GetName();

	void SetName(const std::string value);

	std::string& GetText();
	void SetText(std::string temptext);

	float GetTextScale();
	void SetTextScale(float temp);
	int GetInterLine();
	void SetInterLine(int temp);

	int GetBorderThickness();

	void SetBorderThickness(const int temp);

	void SetBorder(const int width, const unsigned char R, const unsigned char G, const unsigned char B);

	int GetTextStartX();
	void SetTextStartX(int temp);
	int GetTextStartY();
	void SetTextStartY(int temp);

	bool GetTransparent();
	void SetTransparent(bool temp);

	Font* GetFont();

	void SetFont(Font* font);

	void SetColor(const unsigned char R, const unsigned char G, const unsigned char B);

	void SetBorderRGB(const unsigned char R, const unsigned char G, const unsigned char B);

	void SetFontColor(const unsigned char R, const unsigned char G, const unsigned char B);

	void Render(SDL_Renderer *renderer);

	void RenderItslelf(SDL_Renderer* renderer);

	void RenderBorder(SDL_Renderer* renderer);

	void RenderText(SDL_Renderer* renderer);

	void SetRenderTextType(const unsigned short textRenderType);

	bool IsHidden();

	void Hide();

	void Show();

	bool IsHovered();

	void SetHover(bool temp);

	void SetHoverFilter(const bool filter,const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A, const std::string &sound = "");

	std::string& GetHooverSound();

};

// A button that can be clicked with a mouse
class ClickBox : public TemplateUIElement {
private:
	bool status = false;
	bool turnedOn = true;

	std::string clickSound = "";
public:
	bool GetStatus();

	void SetStatus(bool value);

	bool ConsumeStatus();

	void TurnOn();

	void TurnOff();

	bool IsOn();

	void SetClickSound(const std::string &temp);

	std::string &GetClickSound();

	friend class UI;
};


// Button that can accept text input
class TextBox : public TemplateUIElement {
private:
	bool turnedOn = false;
public:
	void CheckInteraction(SDL_Event& event);

	void ManageTextInput(SDL_Event& event);
	friend class UI;
};

// Basic non interactive button
class Button : public TemplateUIElement {
	public:
		friend class UI;

};

class ClickBoxList {
	private:
		UI* ui = nullptr;

		ClickBox* mainElement = nullptr;

		std::vector<ClickBox*> Elements = {};

		std::vector<std::string> names = {};

		bool initalized = false;

		bool expanded = false;

	public:

		void Innit(UI* ui, ClickBox* main, std::vector<std::string> names, int w, int h, int R, int G, int B, std::string* texts, short space = 0);

		bool IsInitialized();

		bool IsExpanded();

		ClickBox* Get(short index);

		ClickBox* Main();

		std::vector<ClickBox*>& GetAll();

		void Expand();

		void Hide();

		void Clear();

		ClickBox* operator [](const size_t index) {
			return Elements[index];
		}
};

// To propelly start the UI you need to pleace manage input function in event loop and render in rendering loop
class UI{
private:
	SDL_Renderer* renderer;
	LocalTexMan* localTexMan = nullptr;

	std::vector<Button*> Buttons;
	std::vector<TextBox*> TextBoxes;
	std::vector<ClickBox*> ClickBoxes;

	std::unordered_map<std::string, Button*> ButtonsMap;
	std::unordered_map<std::string, TextBox*> TextBoxesMap;
	std::unordered_map<std::string, ClickBox*> ClickBoxesMap;

	std::vector<ClickBoxList*> ListReferences;

	FontManager* fontManager;

	Point lastMousePos;

public:

	UI(SDL_Renderer* renderer);

	Button* CreateButton(std::string name, int x, int y, int w, int h, SDL_Texture* texture, Font* font = nullptr,
		std::string text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

	TextBox* CreateTextBox(std::string name, int x, int y, int w, int h, SDL_Texture* texture, Font* font = nullptr,
		std::string text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

	ClickBox* CreateClickBox(std::string name, int x, int y, int w, int h, SDL_Texture* texture, Font* font = nullptr,
		std::string text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

	void AddListRef(ClickBoxList *ref);

	void RemoveListRef(ClickBoxList* ref);

	void CheckHover();

	void CheckTextBoxInteraction(SDL_Event& event);

	void ManageTextBoxTextInput(SDL_Event& event);

	void CheckClickBoxes(SDL_Event& event);

	Button* GetButton(const std::string& name);
	TextBox* GetTextBox(const std::string& name);
	ClickBox* GetClickBox(const std::string& name);

	void SetElementColor(const std::string& name, const unsigned char R, unsigned char G, unsigned char B);

	void SetElementBorderColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B);
	void SetElementFontColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B);

	void ManageInput(SDL_Event& event);

	bool DeleteButton(const std::string& name);

	bool DeleteTextBox(const std::string& name);

	bool DeleteClickBox(const std::string& name);

	bool DeleteAnyButton(const std::string& name);

	void Render();

	std::vector<Button*>& GetButtons();

	std::vector<TextBox*>& GetTextBoxes();

	std::vector<ClickBox*>& GetClickBoxes();

	// You need to provide not name (made up by you) texture (needs to be already loaded by texture manager) path to pregenerated json file
	void CreateFont(const std::string& name, SDL_Texture* texture, const std::string& jsonPath);

	Font* GetFont(const std::string& name);

	//Function to create json file for font png file that contains charcter glyps separated by lines in other color than the font
	//Requires provided txt file with ordered glyps
	//Example:
	//A
	//B
	//C
	//And so on...
	void ScanFont(const std::string& texturePath, const std::string& charactersDataPath,
		unsigned char fR, unsigned char fG, unsigned char fB, unsigned char bR, unsigned char bG, unsigned char bB, Point size,
		const std::string& outputPath);

	void ClearAll(bool clearLists = true);


	~UI();
};
