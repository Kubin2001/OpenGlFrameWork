#pragma once
#include <unordered_map>
#include <vector>
#include <map>

#include "Font.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "Renderer.h"
#include "Basics.h"

// Basic non interactive button
class UIElemBase :public GameObject {
protected:
	std::string name = "";
	std::string text = "";
	float textScale = 1.0f;
	int interLine = 20;

	bool border = false;

	int borderThickness = 0;

	int textStartX = 0;
	int textStartY = 0;

	unsigned char buttonColor[4] = { 255,255,255,255 };

	unsigned char borderRGB[3] = { 255,255,255 };

	unsigned char fontRGB[3] = { 255,255,255 };

	Font* font = nullptr;

	void (*renderFunction)(UIElemBase* ,MT::Renderer*) = nullptr;

	unsigned short textRenderType = 1;

	Point predefinedSize;

	bool hidden = false;

	bool hovered = false; // Is button in collidion with mouse

	bool hoverable = false; // Is hover filter aplied with mouse collisojn

	unsigned char hooverFilter[4] = { 0,0,0,0 };

	std::string hooverSound = "";

	int zLayer = 0; // Bazowo zawsze 0 

	bool GetBorder();

	void SetBorder(bool temp);

	void RenderItslelf(MT::Renderer* renderer);

	void RenderItslelfRounded(MT::Renderer* renderer);

	void RenderBorder(MT::Renderer* renderer);

	void RenderText(MT::Renderer* renderer);

public:
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

	Font* GetFont();

	void SetFont(Font* font);

	void SetColor(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A = 255);

	void SetBorderRGB(const unsigned char R, const unsigned char G, const unsigned char B);

	void SetFontColor(const unsigned char R, const unsigned char G, const unsigned char B);

	static void Render(UIElemBase *elem, MT::Renderer* renderer);

	static void RenderRounded(UIElemBase* elem, MT::Renderer* renderer);

	// 1 normal rectangle
	// 2 rounded rectangle
	void SetRenderType(const unsigned int renderType);
	
	// 1 Standard
	// 2 Center
	// 3 From Right
	// 4 Centered on X axis
	// 5 Centered on Y axis
	void SetRenderTextType(const unsigned short textRenderType);

	bool IsHidden();

	void Hide();

	void Show();

	bool IsHovered();

	void SetHover(bool temp);

	void SetHoverFilter(const bool filter, const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A, const std::string& sound = "");

	std::string& GetHooverSound();

	int GetZLayer();

	void SetZLayer(const int temp);

	virtual ~UIElemBase() = default;

	friend class UI;
};


class Button : public UIElemBase {

	public:
		friend class UI;
};

// A button that can be clicked with a mouse
class ClickBox : public UIElemBase {
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
class TextBox : public UIElemBase {
private:
	bool turnedOn = false;
public:
	void CheckInteraction(SDL_Event& event);

	void ManageTextInput(SDL_Event& event);
	friend class UI;
};



class PopUpBox : public UIElemBase {
	private:
		int lifeTime = 0;

	public:
		friend class UI;

		int GetLifeTime();
		void SetLifeTime(const int lifeTime);
};


class RenderingLayer {
	public:
		friend class UI;
	private:
		std::vector<Button*> Buttons;
		std::vector<TextBox*> TextBoxes;
		std::vector<ClickBox*> ClickBoxes;
		std::vector<PopUpBox*> PopUpBoxes;
};

template<typename T>
class UIList;

// To propelly start the UI you need to place manage input function in event loop and render in rendering loop
class UI{
	private:
		MT::Renderer* renderer;
		LocalTexMan* localTexMan = nullptr;

		std::vector<Button*> Buttons;
		std::vector<TextBox*> TextBoxes;
		std::vector<ClickBox*> ClickBoxes;
		std::vector<PopUpBox*> PopUpBoxes;

		std::unordered_map<std::string, UIElemBase*> UIElemMap;

		std::map<int, RenderingLayer> Zlayers;

		std::vector<UIList<Button>*> ListBtnRef;
		std::vector<UIList<TextBox>*> ListTbRef;
		std::vector<UIList<ClickBox>*> ListCbRef;

		FontManager* fontManager;

		Point lastMousePos;

		Font* baseFont = nullptr;

		template<typename T>
		void AddListRef(UIList<T>* ref) {
			if constexpr (std::is_same_v<T*, Button*>) {
				ListBtnRef.emplace_back(ref);
			}
			if constexpr (std::is_same_v<T*, TextBox*>) {
				ListTbRef.emplace_back(ref);
			}
			if constexpr (std::is_same_v<T*, ClickBox*>) {
				ListCbRef.emplace_back(ref);
			}
		}
		
		template<typename T>
		void RemoveListRef(UIList<T>* ref) {
			if constexpr (std::is_same_v<T*, Button*>) {
				std::erase(ListBtnRef, ref);
			}
			if constexpr (std::is_same_v<T*, TextBox*>) {
				std::erase(ListTbRef, ref);
			}
			if constexpr (std::is_same_v<T*, ClickBox*>) {
				std::erase(ListCbRef, ref);
			}
		}


	public:
		template<typename T>
		friend class UIList;
		UI(MT::Renderer* renderer);

		Button* CreateButton(const std::string &name, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

		TextBox* CreateTextBox(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

		ClickBox* CreateClickBox(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

		PopUpBox* CreatePopUpBox(const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);


		Button* CreateButtonF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, const std::string &fontStr = "",
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

		TextBox* CreateTextBoxF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, const std::string& fontStr ="",
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

		ClickBox* CreateClickBoxF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, const std::string& fontStr = "",
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

		PopUpBox* CreatePopUpBoxF(const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture = nullptr, const std::string& fontStr= "",
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0, int borderThickness = 0);

		void CheckHover();

		void CheckTextBoxInteraction(SDL_Event& event);

		void ManageTextBoxTextInput(SDL_Event& event);

		void CheckClickBoxes(SDL_Event& event);

		template<typename T>
		T* UIGetElem(const std::string& name) {
			auto elemFind = UIElemMap.find(name);
			if (elemFind == UIElemMap.end()) {
				return nullptr;
			}

			T* elem = dynamic_cast<T*>(elemFind->second);
		#ifdef _DEBUG
			if (elem == nullptr) {
				throw std::runtime_error("UI::GetElem wrong cast for " + name);
			}
		#endif // DEBUG
			return elem;
		}

		Button* GetButton(const std::string& name);
		TextBox* GetTextBox(const std::string& name);
		ClickBox* GetClickBox(const std::string& name);
		PopUpBox* GetPopUpBox(const std::string& name);

		void SetElementColor(const std::string& name, const unsigned char R, unsigned char G, unsigned char B);

		void SetElementBorderColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B);
		void SetElementFontColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B);

		void FrameUpdate();

		void ManageInput(SDL_Event& event);


		template<typename T>
		bool EraseVec(std::vector<T*>& vec, const std::string &name) {
			for (unsigned int i = 0; i < vec.size(); i++) {
				if (vec[i]->GetName() == name) {
					delete vec[i];
					vec.erase(vec.begin() + i);
					return true;
				}
			}
			return false;
		}

		template<typename T>
		bool DeleteElem(const std::string& name) {
			static_assert(
					std::is_same_v<T,Button> ||
					std::is_same_v<T, ClickBox> ||
					std::is_same_v<T, TextBox> ||
					std::is_same_v<T, PopUpBox>, "Wrong type allowed : Button, ClickBox, TextBox, PopUpBox"
				);
			UIElemMap.erase(name);

			if constexpr (std::is_same_v<T, Button>) {
				return EraseVec(Buttons,name);
			}
			else if constexpr (std::is_same_v<T, ClickBox>) {
				return EraseVec(ClickBoxes, name);
			}
			else if constexpr (std::is_same_v<T, TextBox>) {
				return EraseVec(TextBoxes, name);
			}
			else if constexpr (std::is_same_v<T, PopUpBox>) {
				return EraseVec(PopUpBoxes, name);
			}
			return false;
		}

		bool DeleteButton(const std::string& name);
		bool DeleteTextBox(const std::string& name);
		bool DeleteClickBox(const std::string& name);
		bool DeletePopUpBox(const std::string& name);

		bool DeleteAnyElem(const std::string& name);

		void Render();

		void RenderRawText(Font* font, const int x, const int y, const std::string& text,const int interline,
			const unsigned char R, const unsigned char G, const unsigned char B);

		std::vector<Button*>& GetButtons();

		std::vector<TextBox*>& GetTextBoxes();

		std::vector<ClickBox*>& GetClickBoxes();
		std::vector<PopUpBox*>& GetPopUpBoxes();

		// You need to provide not name (made up by you) texture (needs to be already loaded by texture manager) path to pregenerated json file
		void CreateFont(const std::string& name, MT::Texture* texture, const std::string& jsonPath);

		void CrateTempFontFromTTF(const char* ttfPath, const int size, const std::string& name);

		Font* GetFont(const std::string& name);

		void SetBaseFont(Font *font);

		Font* GetBaseFont();

		std::vector<Font*>& GetAllFonts();

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

		bool useLayersInRendering = false;

		~UI();
};

template<typename T>
class UIList {
	static_assert((!std::is_pointer_v<T>) && (std::is_same_v<T, Button> || std::is_same_v<T, TextBox> ||
		std::is_same_v<T, ClickBox>), "Not a valid type you need to pass Button , TextBox or ClickBox also not a pointer type");
private:
	UI* ui = nullptr;

	ClickBox* mainElement = nullptr;

	std::vector<T*> Elements = {};

	bool initalized = false;

	bool expanded = false;

public:

	void Innit(UI* ui, ClickBox* main, int w, int h, int R, int G, int B, const std::vector<std::string>& texts, short space = 0) {
		this->ui = ui;
		mainElement = main;
		Elements.reserve(texts.size());
		MT::Rect& rect = mainElement->GetRectangle();
		int y = rect.y + (rect.h + space);
		int counter = 0;
		for (size_t i = 0; i < texts.size(); i++) {
			if constexpr (std::is_same_v<T*, Button*>) {
				Elements.emplace_back(
					ui->CreateButton(main->GetName() + std::to_string(counter), rect.x, y,
						w, h, nullptr, ui->GetFont("arial12px"), texts[i]));
			}
			else if constexpr (std::is_same_v<T*, TextBox*>) {
				Elements.emplace_back(
					ui->CreateTextBox(main->GetName() + std::to_string(counter), rect.x, y,
						w, h, nullptr, ui->GetFont("arial12px"), texts[i]));
			}
			else if constexpr (std::is_same_v<T*, ClickBox*>) {
				Elements.emplace_back(
					ui->CreateClickBox(main->GetName() + std::to_string(counter), rect.x, y,
						w, h, nullptr, ui->GetFont("arial12px"), texts[i]));
			}
			Elements[i]->SetColor(R, G, B);
			Elements.back()->Hide();
			y += (h + space);
			counter++;
		}
		ui->AddListRef(this);
		initalized = true;
	}

	bool IsInitialized() {
		return initalized;
	}

	bool IsExpanded() {
		return expanded;
	}

	ClickBox* Main() {
		return mainElement;
	}

	std::vector<T*>& GetAll() {
		return Elements;
	}

	void Toogle() {
		if (expanded) {
			Hide();
		}
		else {
			Expand();
		}
	}

	void Expand() {
		for (const auto& it : Elements) {
			it->Show();
		}
		expanded = true;
	}

	void Hide() {
		for (const auto& it : Elements) {
			it->Hide();
		}
		expanded = false;
	}

	void Clear() {
		if (!initalized) { return; }
		if constexpr (std::is_same_v<T, Button>) {
			for (const auto& it : Elements) {
				ui->DeleteButton(it->GetName());
			}
		}
		else if constexpr (std::is_same_v<T, TextBox>) {
			for (const auto& it : Elements) {
				ui->DeleteTextBox(it->GetName());
			}

		}
		else if constexpr (std::is_same_v<T, ClickBox>) {
			for (const auto& it : Elements) {
				ui->DeleteClickBox(it->GetName());
			}
		}
		Elements.clear();
		if (mainElement != nullptr) {
			ui->DeleteClickBox(mainElement->GetName());
		}
		mainElement = nullptr;
		ui->RemoveListRef(this);
		initalized = false;
		expanded = false;
	}

	T* operator [](const size_t index) {
		if (index > Elements.size() - 1) {
			throw std::runtime_error("List element index out of bounds");
		}
		return Elements[index];
	}
};
