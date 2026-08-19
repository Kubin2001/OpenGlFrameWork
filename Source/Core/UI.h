#pragma once
#include <unordered_map>
#include <vector>
#include <map>

#include "Font.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "Renderer.h"
#include "json.hpp"

enum class CastType {
	Label,
	ClickBox,
	TextBox,
	PopUpBox,
	Slider
};

enum class TextRenderType {
	Standard,
	Centered,
	FromRight,
	CenteredX,
	CenteredY
};

enum class RenderType {
	Standard,
	Rounded
};

enum class SliderSlide {
	X,
	Y
};


// Basic non interactive button
class UIElemBase{
protected:
	std::string name = "";
	void (*renderFunction)(UIElemBase* ,MT::Renderer*) = nullptr;

	TextRenderType textRenderType = TextRenderType::Standard; // Base is left up corner

	bool hidden = false;

	bool hovered = false; // Is button in collidion with mouse

	bool hoverable = false; // Is hover filter aplied with mouse collisojn

	Mix_Chunk* hoverSound = nullptr;

	int zLayer = 0; // Bazowo zawsze 0 

	void RenderText(MT::Renderer* renderer);

public:
	MT::Rect rect{};
	MT::Texture* texture = nullptr;


	// To change name use ui->renameElem
	const std::string& GetName() const { return name; }
	CastType castType = CastType::Label;
	std::string text = "";

	float textScale = 1.0f;
	int interLine = 20;

	int borderThickness = 0;

	int textStartX = 0;
	int textStartY = 0;

	Font* font = nullptr;

	MT::ColorA color{ 255,255,255,255 };
	MT::ColorA borderColor{ 255,255,255,255 };
	MT::ColorA fontColor{ 255,255,255,255 };
	MT::ColorA hoverFilter{ 0,0,0,0 };

	void SetBorder(const int width, const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A = 255) {
		borderThickness = width;
		borderColor.R = R;
		borderColor.G = G;
		borderColor.B = B;
		borderColor.A = A;
	}

	void SetColor(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A = 255) {
		color.R = R;
		color.G = G;
		color.B = B;
		color.A = A;
	}


	void SetBorderColor(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A = 255) {
		borderColor.R = R;
		borderColor.G = G;
		borderColor.B = B;
		borderColor.A = A;
	}

	void SetFontColor(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A = 255) {
		fontColor.R = R;
		fontColor.G = G;
		fontColor.B = B;
		fontColor.A = A;
	}

	// Use Enum RenderType::
	void SetRenderType(RenderType renderType) {
		if (renderType == RenderType::Standard) {
			renderFunction = &UIElemBase::Render;
		}
		else  if (renderType == RenderType::Rounded) {
			renderFunction = &UIElemBase::RenderRounded;
		}
		else {
			renderFunction = &UIElemBase::Render;
		}
	}
	
	// Use Enum TextRenderType::
	void SetRenderTextType(TextRenderType textRenderType) {this->textRenderType = textRenderType;}

	bool IsHidden() {return hidden;}

	void Hide() {hidden = true;}

	void Show() {hidden = false;}

	bool IsHovered() {return hovered;}

	void SetHover(bool temp) {hovered = temp;}

	void SetHoverFilter(const bool filter, const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A = 255, const std::string& sound = "") {
		this->hoverable = filter;
		hoverFilter.R = R;
		hoverFilter.G = G;
		hoverFilter.B = B;
		hoverFilter.A = A;
		if (sound.empty()) {
			hoverSound = nullptr;
		}
		else {
			hoverSound = SoundMan::GetSound(sound);
		}
	}

	Mix_Chunk* GetHoverSound() {return hoverSound;}

	void SetHoverSound(std::string &name){hoverSound = SoundMan::GetSound(name);}

	int GetZLayer() {return this->zLayer;}

	static void Render(UIElemBase* elem, MT::Renderer* renderer);

	static void RenderRounded(UIElemBase* elem, MT::Renderer* renderer);

	virtual ~UIElemBase() = default;

	friend class UI;
};


class Label : public UIElemBase {

	public:
		friend class UI;
};

// An element that can be clicked with a mouse
class ClickBox : public UIElemBase {
private:
	bool status = false;
	bool turnedOn = true;

	std::string clickSound = "";
public:
	bool GetStatus() {return status;}

	void SetStatus(bool value) {status = value;}

	bool ConsumeStatus() {
		if (status) {
			status = false;
			return true;
		}
		return false;
	}

	void TurnOn() {turnedOn = true;}

	void TurnOff() {turnedOn = false;}

	bool IsOn() {return turnedOn;}

	void SetClickSound(const std::string& temp) {this->clickSound = temp;}

	std::string& GetClickSound() {return clickSound;}

	friend class UI;
};


// Button that can accept text input
class TextBox : public UIElemBase {
private:
	bool isUsed = false;
	bool turnedOn = true;
public:
	unsigned int maxTextLength = 1'000'000;

	bool IsUsed() {return this->isUsed;}

	void TurnOn() {turnedOn = true;}

	void TurnOff() {
		turnedOn = false;
		isUsed = false;
	}


	bool IsOn() {return turnedOn;}


	friend class UI;
};



class PopUpBox : public UIElemBase {
	public:
		int lifeTime = 0;

		friend class UI;
};

class Slider : public UIElemBase {
	public:

		SliderSlide slideType = SliderSlide::X; //Enmum class SliderSlide
		int min = 0; // Min X or Y slider can go
		int max = 0; // Max X or Y slider can go

		float GetPercent() {
			int radius = 0;
			int pos = 0;
			if (slideType == SliderSlide::X) {
				radius = max - min - rect.w;
				pos = rect.x - min;
			}
			else {
				radius = max - min - rect.h;
				pos = rect.y - min;
			}
			return (float)pos / radius;
		}

		void SetPosAtPercent(float percent) {
			percent = std::clamp(percent, 0.0f, 1.0f);

			if (slideType == SliderSlide::X) {
				int radius = max - min - rect.w;
				rect.x = min + static_cast<int>(radius * percent);
			}
			else {
				int radius = max - min - rect.h;
				rect.y = min + static_cast<int>(radius * percent);
			}
		}

		friend class UI;
};


template<typename T>
class UIList;

class UISection;


struct UILayer {
	bool clipTest = false;
	MT::Rect clipRect{ 0,0,0,0 };
	std::vector<UIElemBase*> elements;

};

// To propelly start the UI you need to place manage input function in event loop and render in rendering loop
class UI{
	private:
		MT::Renderer* renderer;
		LocalTexMan* localTexMan = nullptr;

		std::vector<UIElemBase*> UiElemVec;
		std::vector<UILayer> LayerVec;

		std::unordered_map<std::string, UIElemBase*> UIElemMap;

		std::vector<UIList<Label>*> ListLbRef;
		std::vector<UIList<TextBox>*> ListTbRef;
		std::vector<UIList<ClickBox>*> ListCbRef;

		FontManager* fontManager;

		int popupBoxesCount = 0;
		MT::Rect lastMousePos;

		Font* baseFont = nullptr;

		template<typename T>
		void AddListRef(UIList<T>* ref) {
			if constexpr (std::is_same_v<T*, Label*>) {
				ListLbRef.emplace_back(ref);
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
			if constexpr (std::is_same_v<T*, Label*>) {
				std::erase(ListLbRef, ref);
			}
			if constexpr (std::is_same_v<T*, TextBox*>) {
				std::erase(ListTbRef, ref);
			}
			if constexpr (std::is_same_v<T*, ClickBox*>) {
				std::erase(ListCbRef, ref);
			}
		}

		void DumpButton(nlohmann::ordered_json& json, UIElemBase* elem);

		void DumpClickBox(nlohmann::ordered_json& json, ClickBox *cb);

		void DumpTextBox(nlohmann::ordered_json& json, TextBox* tb);

		void DumpPopUpBox(nlohmann::ordered_json& json, PopUpBox* pb);

		void DumpSlider(nlohmann::ordered_json& json, Slider* sl);

		void FillElem(UIElemBase* elem, const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		struct Settings {
		private:
			// Render elements based on their z layer not based on which was created last
			bool useLayersInRendering = false;
		public:
			// If the newest ClickBox is clicked older ones that are also clicked will not call its status
			bool stopCheckAtFirst = false;
			// If the newest Element hovered all hover checks will stop
			bool stopHoverAtFirst = false;
			// Click boxes status will be checked at MOUSEBUTTONUP not like default MOUSEBUTTONDOWN
			bool clickBoxStartAtDown = false;

			friend class UI;
		};

	public:

		Settings settings;
		

		template<typename T>
		friend class UIList;
		UI(MT::Renderer* renderer);

		Label* LCreateLabel(int layer, const std::string& name, int x, int y, int w, int h, MT::Texture* texture =nullptr, 
			Font* font = nullptr,const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		TextBox* LCreateTextBox(int layer, const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr,
			Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		ClickBox* LCreateClickBox(int layer, const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr,
			Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		PopUpBox* LCreatePopUpBox(int layer, const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture = nullptr,
			Font* font = nullptr, const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		Slider* LCreateSlider(int layer, const std::string& name, int x, int y, int w, int h, SliderSlide slideType, int min, int max,
			MT::Texture* texture = nullptr);

		Label* CreateLabel(const std::string &name, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		TextBox* CreateTextBox(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		ClickBox* CreateClickBox(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		PopUpBox* CreatePopUpBox(const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture = nullptr, Font* font = nullptr,
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		Slider* CreateSlider(const std::string& name, int x, int y, int w, int h, SliderSlide slideType, int min, int max, MT::Texture* texture = nullptr);

		Label* CreateLabelF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, const std::string &fontStr = "",
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		TextBox* CreateTextBoxF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, const std::string& fontStr ="",
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		ClickBox* CreateClickBoxF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture = nullptr, const std::string& fontStr = "",
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		PopUpBox* CreatePopUpBoxF(const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture = nullptr, const std::string& fontStr= "",
			const std::string& text = "", float textScale = 1.0f, int textStartX = 0, int textStartY = 0);

		// Renaming and rehasing element
		bool RenameElem(const std::string& name, const std::string& newName);

		void CheckHover(UIElemBase* elem, bool &hoverStop);

		void CheckTextBoxInteraction(TextBox* tb, SDL_Event& event);

		void ManageTextBoxTextInput(TextBox* tb, SDL_Event& event);

		void CheckClickBoxes(ClickBox* cb, unsigned int eventType, bool& forceStop, SDL_Event& event);

		void SlideSliders(Slider* slider, SDL_Event& event);

		UIElemBase* GetElem(const std::string& name);
		Label* GetLabel(const std::string& name);
		TextBox* GetTextBox(const std::string& name);
		ClickBox* GetClickBox(const std::string& name);
		PopUpBox* GetPopUpBox(const std::string& name);
		Slider* GetSlider(const std::string& name);

		// Consumes click box status if click box exist safe and recomended to ui->getClickBox->ConsumeStatus()
		bool ConsumeIfExist(const std::string& name);

		void SetElementColor(const std::string& name, const unsigned char R, unsigned char G, unsigned char B);
		void SetElementBorderColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B);
		void SetElementFontColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B);

		void SetElementZLayer(const std::string& name, int zlayer);

		void SetLayerClipTest(bool test, const MT::Rect& rect, int zlayer);

		void FrameUpdate();

		void ManageInput(SDL_Event& event);
			
		bool DeleteElement(const std::string& name);

		void Render();

		void RenderRawText(Font* font, const int x, const int y, const std::string& text,const int interline, const MT::Color &color);

		// This function should be called only once at the Game::Start function since it is slow because it needs to recrate all ui rendering
		// If you use z layer from now on you should create new elements with CreateLayered (old way still works but it would be slower)
		// Layers are not dynamic you can use layers from 0 to 100 if you will give larger number it will default to 100
		// Smaller number will default to 0
		void UseLayerInRendering(bool use);

		bool UsingLayers() {
			return settings.useLayersInRendering;
		}


		// You need to provide not name (made up by you) texture (needs to be already loaded by texture manager) path to pregenerated json file
		// Strongly recomended to use  CrateTempFontFromTTF if you do not have any custom strange fonts
		void CreateFont(const std::string& name, MT::Texture* texture, const std::string& jsonPath);

		//Generates font directly form TTF file you can find this files for example in system32/Fonts
		bool CrateTempFontFromTTF(const char* ttfPath, const int size, const std::string& name, LocalTexMan* localTexMan = nullptr);

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

		void DumpToJson(const std::string& fileName, const std::vector<UIElemBase*>& elements);

		std::vector<UIElemBase*> LoadFromJson(const std::string& fileName);

		void ClearAll(bool clearLists = true);

		~UI();
};

template<typename T>
class UIList {
	static_assert((!std::is_pointer_v<T>) && (std::is_same_v<T, Label> || std::is_same_v<T, TextBox> ||
		std::is_same_v<T, ClickBox>), "Not a valid type you need to pass Label , TextBox or ClickBox also not a pointer type");
private:
	UI* ui = nullptr;

	ClickBox* mainElement = nullptr;

	std::vector<T*> Elements = {};

	bool initalized = false;

	bool expanded = false;

public:

	void Init(UI* ui, ClickBox* mainBtn, int w, int h, const MT::ColorA &color, const std::vector<std::string>& texts, short space = 0) {
		this->ui = ui;
		mainElement = mainBtn;
		Elements.reserve(texts.size());
		MT::Rect& rect = mainElement->rect;
		int y = rect.y + rect.h + space;
		Font* font = mainElement->font;
		const std::string& name = mainElement->GetName();
		for (size_t i = 0; i < texts.size(); i++) {
			T* elem = nullptr;
			if constexpr (std::is_same_v<T, Label>) {
				elem = ui->CreateLabel(name + std::to_string(i), rect.x, y, w, h, nullptr, font, texts[i]);
			}
			else if constexpr (std::is_same_v<T, TextBox>) {
				elem = ui->CreateTextBox(name + std::to_string(i), rect.x, y, w, h, nullptr, font, texts[i]);
			}
			else if constexpr (std::is_same_v<T, ClickBox>) {
				elem = ui->CreateClickBox(name + std::to_string(i), rect.x, y, w, h, nullptr, font, texts[i]);
			}
			Elements.emplace_back(elem);

			elem->SetColor(color.R, color.G, color.B, color.A);
			elem->Hide();
			y += (h + space);
			if (ui->UsingLayers()) {
				ui->SetElementZLayer(elem->GetName(), mainElement->GetZLayer());
			}
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
		for (const auto& it : Elements) {
			ui->DeleteElement(it->GetName());
		}
		Elements.clear();
		if (mainElement != nullptr) {
			ui->DeleteElement(mainElement->GetName());
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


class UISection {
	private:
		std::vector<Label*> labels = {};

		std::vector<TextBox*> textBoxes = {};

		std::vector<ClickBox*> clickBoxes = {};

		std::vector<PopUpBox*> popUpBoxes = {};

		std::vector<Slider*> sliders = {};

		UI* ui = nullptr;

	public:
		UISection() = default;

		UISection(UI* ui) {
			this->ui = ui;
		}

		void Init(UI *ui) {
			this->ui = ui;		
		}

		void Add(Label* button) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr section is not inicialized");
			}
			labels.emplace_back(button);
		}
		void Add(TextBox* textBox) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr section is not inicialized");
			}
			textBoxes.emplace_back(textBox);
		}
		void Add(ClickBox* clickBox) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr section is not inicialized");
			}
			clickBoxes.emplace_back(clickBox);
		}
		void Add(PopUpBox* popUpBox) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr section is not inicialized");
			}
			popUpBoxes.emplace_back(popUpBox);
		}
		void Add(Slider* sl) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr section is not inicialized");
			}
			sliders.emplace_back(sl);
		}

		void Clear() {
			auto clearVec = [](auto& vec, UI *ui) {
				for (auto& elem : vec) {
					ui->DeleteElement(elem->GetName());
				}
				vec.clear();
			};
			clearVec(labels, ui);
			clearVec(clickBoxes, ui);
			clearVec(textBoxes, ui);
			clearVec(popUpBoxes, ui);
			clearVec(sliders, ui);
		}	

		std::vector<Label*>& GetLabels() { return labels; }
		std::vector<TextBox*>& GetTextBoxes() { return textBoxes; }
		std::vector<ClickBox*>& GetClickBoxes() { return clickBoxes; }
		std::vector<PopUpBox*>& GetPopUpBoxes() { return popUpBoxes; }
		std::vector<Slider*>& GetSliders() { return sliders; }
};

// Designed for managing large interfaces WARING it is up to you to cast to corret type one tag should has only one type for safety
class TagUISection {
	private:
		std::unordered_map<std::string, std::vector<UIElemBase*>> TagMap;

		UI* ui = nullptr;

	public:
		TagUISection() = default;

		TagUISection(UI* ui) {
			this->ui = ui;
		}

		void Init(UI* ui) {
			this->ui = ui;
		}

		void Add(const std::string &tag, UIElemBase *elem) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr tag section is not inicialized");
			}
			TagMap[tag].emplace_back(elem);
		}

		void ClearTag(const std::string& tag) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr tag section is not inicialized");
			}
			auto iter = TagMap.find(tag);
			if (iter != TagMap.end()) {
				TagMap.erase(iter);
			}
		}

		void DeleteTag(const std::string& tag) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr tag section is not inicialized");
			}
			auto iter = TagMap.find(tag);
			if (iter != TagMap.end()) {
				for (auto& elem : iter->second) {
					ui->DeleteElement(elem->GetName());
				}
				TagMap.erase(iter);
			}
		}

		void ClearAll() {
			TagMap.clear();
		}

		void DeleteAll() {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr tag section is not inicialized");
			}
			for (auto& tag : TagMap) {
				for (auto& elem : tag.second) {
					ui->DeleteElement(elem->GetName());
				}
			}
			TagMap.clear();
		}

		bool HasTag(const std::string& tag) {
			auto iter = TagMap.find(tag);
			if (iter == TagMap.end()) {
				return false;
			}
			return true;
		}

		std::vector<UIElemBase*>&  GetTag(const std::string& tag) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr tag section is not inicialized");
			}
			auto iter = TagMap.find(tag);
			if (iter == TagMap.end()) {
				throw std::runtime_error("This tag does not exist");
			}
			return iter->second;
		}

		std::unordered_map<std::string, std::vector<UIElemBase*>>& GetAll() {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr tag section is not inicialized");
			}
			return TagMap;
		}

		UIElemBase* GetElem(const std::string& tag, const std::string &name) {
			if (ui == nullptr) {
				throw std::runtime_error("UI is nullptr tag section is not inicialized");
			}
			auto iter = TagMap.find(tag);

			if (iter == TagMap.end()) {
				throw std::runtime_error("This tag does not exist");
			}
			for (auto& elem : iter->second) {
				if (elem->GetName() == name) {
					return elem;
				}
			}
			return nullptr;
		}
};