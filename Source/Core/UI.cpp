#include "UI.h"

#include <fstream>

#include "json.hpp"
#include "Colision.h"
#include "GlobalVariables.h"

//UIElemBase
std::string& UIElemBase::GetName() {
	return name;
}

//void UIElemBase::SetName(const std::string &value) {
//	name = value;
//}

void UIElemBase::SetText(const std::string &temptext) {
	text = temptext;
}

std::string& UIElemBase::GetText() {
	return text;
}

float UIElemBase::GetTextScale() {
	return textScale;
}
void UIElemBase::SetTextScale(float temp) {
	textScale = temp;
}
int UIElemBase::GetInterLine() {
	return interLine;
}
void UIElemBase::SetInterLine(int temp) {
	interLine = temp;
}

bool UIElemBase::GetBorder() {
	return border;
}

Font* UIElemBase::GetFont() {
	return font;
}

void UIElemBase::SetFont(Font* font) {
	this->font = font;
}

int UIElemBase::GetBorderThickness() {
	return borderThickness;
}

void UIElemBase::SetBorderThickness(const int temp) {
	borderThickness = temp;
	border = true;
}

void UIElemBase::SetBorder(const int width, const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A) {
	border = true;
	borderThickness = width;
	borderRGB.R = R;
	borderRGB.G = G;
	borderRGB.B = B;
	borderRGB.A = A;
}

int UIElemBase::GetTextStartX() {
	return textStartX;
}
void UIElemBase::SetTextStartX(int temp) {
	textStartX = temp;
}
int UIElemBase::GetTextStartY() {
	return textStartY;
}
void UIElemBase::SetTextStartY(int temp) {
	textStartY = temp;
}

void UIElemBase::SetColor(const unsigned char R, const unsigned char G, const unsigned char B , const unsigned char A) {
	buttonColor.R = R;
	buttonColor.G = G;
	buttonColor.B = B;
	buttonColor.A = A;
}


void UIElemBase::SetBorderColor(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A) {
	borderRGB.R = R;
	borderRGB.G = G;
	borderRGB.B = B;
	borderRGB.A = A;
}

void UIElemBase::SetFontColor(const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A) {
	if (font != nullptr) {
		if (font->GetTexture() != nullptr) {
			fontRGB.R = R;
			fontRGB.G = G;
			fontRGB.B = B;
			fontRGB.A = A;
		}
	}
}

void UIElemBase::Render(UIElemBase* elem, MT::Renderer* renderer) {
	if (!elem->hidden) {
		if (elem->GetTexture() == nullptr) {
			renderer->RenderRectUPR(elem->rectangle, { elem->buttonColor.R, elem->buttonColor.G, elem->buttonColor.B }, elem->buttonColor.A);
		}
		else {
			renderer->RenderCopyUPR(elem->rectangle, elem->texture);
		}
		if (elem->GetBorder()) {
			renderer->RenderBorderUPR(elem->rectangle, { elem->borderRGB.R, elem->borderRGB.G, elem->borderRGB.B }
			,elem->borderThickness, elem->borderRGB.A);
		}
		if (elem->hovered && elem->hoverable) {
			renderer->RenderRectUPR(elem->rectangle,
				{ elem->hoverFilter.R, elem->hoverFilter.G, elem->hoverFilter.B }, elem->hoverFilter.A);
		}
		elem->RenderText(renderer);
	}
}

void UIElemBase::RenderRounded(UIElemBase* elem, MT::Renderer* renderer) {
	if (!elem->hidden) {
		if (elem->GetTexture() == nullptr) {
			renderer->RenderRoundedRectUPR(elem->rectangle, { elem->buttonColor.R, elem->buttonColor.G, elem->buttonColor.B }, elem->buttonColor.A);
		}
		else {
			renderer->RenderCopyRoundedUPR(elem->rectangle, elem->texture);
		}
		if (elem->GetBorder()) {
			renderer->RenderRoundedBorderUPR(elem->rectangle, { elem->borderRGB.R, elem->borderRGB.G, elem->borderRGB.B }
			, elem->borderThickness, elem->borderRGB.A);
		}
		if (elem->hovered && elem->hoverable) {
			renderer->RenderRoundedRectUPR(elem->rectangle,
				{ elem->hoverFilter.R, elem->hoverFilter.G, elem->hoverFilter.B }, elem->hoverFilter.A);
		}
		elem->RenderText(renderer);
	}
}


void UIElemBase::RenderText(MT::Renderer* renderer) {
	if (font == nullptr || font->GetTexture() == nullptr || text.empty()) { return; }

	font->SetFilter(fontRGB.R, fontRGB.G, fontRGB.B);
	font->GetTexture()->SetAlphaBending(fontRGB.A);
	switch (textRenderType) {
		case (int)TextRenderType::Standard:
			font->RenderText(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		case (int)TextRenderType::Centered:
			font->RenderTextCenter(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		case (int)TextRenderType::FromRight:
			font->RenderTextFromRight(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		case (int)TextRenderType::CenteredX:
			font->RenderTextCenterX(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		case (int)TextRenderType::CenteredY:
			font->RenderTextCenterY(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		default: // Standardowa opcja
			font->RenderText(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
	}
	font->GetTexture()->SetAlphaBending(255);
}

void UIElemBase::SetRenderType(const unsigned int renderType) {
	if (renderType == (int)RenderType::Standard) {
		renderFunction = &UIElemBase::Render;
	}
	else  if (renderType == (int)RenderType::Rounded) {
		renderFunction = &UIElemBase::RenderRounded;
	}
	else {
		renderFunction = &UIElemBase::Render;
	}
}

void UIElemBase::SetRenderTextType(const unsigned short textRenderType) {
	this->textRenderType = textRenderType;
}


bool UIElemBase::IsHidden() {
	return hidden;
}

void UIElemBase::Hide() {
	hidden = true;
}

void UIElemBase::Show() {
	hidden = false;
}

bool UIElemBase::IsHovered() {
	return hovered;
}

void UIElemBase::SetHover(bool temp) {
	hovered = temp;
}

void UIElemBase::SetHoverFilter(const bool filter, const unsigned char R, const unsigned char G, const unsigned char B, const unsigned char A, const std::string& sound) {
	this->hoverable = filter;
	hoverFilter.R = R;
	hoverFilter.G = G;
	hoverFilter.B = B;
	hoverFilter.A = A;
	if (sound == "") {
		hoverSound = nullptr;
	}
	else {
		hoverSound = SoundMan::GetSound(sound);
	}
}

Mix_Chunk* UIElemBase::GetHoverSound() {
	return hoverSound;
}

int UIElemBase::GetZLayer() {
	return this->zLayer;
}

void UIElemBase::SetZLayer(const int temp) {
	int layer = temp;
	if (layer < 0) { layer = 0; }
	else if (layer > 100) { layer = 100; }
	zLayer = temp;
}

//BUTTON
//TextBox
bool TextBox::IsUsed() {
	return this->isUsed;
}

void TextBox::TurnOn() {
	turnedOn = true;
}

void TextBox::TurnOff() {
	turnedOn = false;
	isUsed = false;
}

bool TextBox::IsOn() {
	return turnedOn;
}

void TextBox::SetTextLength(unsigned int val) {
	if (val > 1'000'000) {
		this->maxTextLength = 1'000'000;
	}
	else {
		this->maxTextLength = val;
	}
}

unsigned int TextBox::GetTextLength() {
	return this->maxTextLength;
}


//ClickBox
bool ClickBox::GetStatus() {
	return status;
}

void ClickBox::SetStatus(bool value) {
	status = value;
}

bool ClickBox::ConsumeStatus() {
	if (status) {
		status = false;
		return true;
	}
	return false;
}

void ClickBox::TurnOn() {
	turnedOn = true;
}

void ClickBox::TurnOff() {
	turnedOn = false;
}

bool ClickBox::IsOn() {
	return turnedOn;
}

void ClickBox::SetClickSound(const std::string& temp) {
	this->clickSound = temp;
}

std::string& ClickBox::GetClickSound() {
	return clickSound;
}
//ClickBox
//Pop Up Box

int PopUpBox::GetLifeTime() {
	return this->lifeTime;
}

void PopUpBox::SetLifeTime(const int lifeTime) {
	this->lifeTime = lifeTime;
}

//Pop Up Box

//Slider

//Slider

UI::UI(MT::Renderer* renderer) {
	fontManager = new FontManager();
	this->renderer = renderer;
	lastMousePos.x = -10000000;
	lastMousePos.y = -10000000;
	ZElemVec.resize(101); // 100 leayers (101 in theory if you count 0)
	for (auto& elem : UiElemVec) {
		ZElemVec[elem->zLayer].emplace_back(elem);
	}
}



void UI::Render() {
	if (settings.useLayersInRendering) {
		for (auto& layer : ZElemVec) {
			for (auto& elem : layer) {
				elem->renderFunction(elem, renderer);
			}
		}
	}
	else {
		for (const auto& elem : UiElemVec) {
			elem->renderFunction(elem, renderer);
		}
	}
}


void UI::RenderRawText(Font* font, const int x, const int y, const std::string& text,const int interline,
		const unsigned char R, const unsigned char G, const unsigned char B) {
	font->RenderRawText(renderer, x, y, text, interline, R, G, B);
}

void UI::UseLayerInRendering(bool use) {
	if (use) {
		if (settings.useLayersInRendering) {
			return;
		}
		else {
			for (auto& elem : UiElemVec) {
				ZElemVec[elem->zLayer].emplace_back(elem);
			}
			UiElemVec.clear();
			settings.useLayersInRendering = true;
		}
	}
	else {
		if (!settings.useLayersInRendering) {
			return;
		}
		else {
			for (auto& layer : ZElemVec) {
				for (auto& elem : layer) {
					UiElemVec.emplace_back(elem);
				}
				layer.clear();
			}
			settings.useLayersInRendering = false;
		}
	}
}

void UI::FillElem(UIElemBase *elem ,const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	elem->name = name;
	elem->GetRectangle().Set(x, y, w, h);
	elem->SetRenderType((int)RenderType::Standard);

	elem->SetTexture(texture);

	elem->SetText(text);
	elem->SetTextScale(textScale);
	elem->SetFont(font);
	if (font != nullptr) {
		elem->SetInterLine(font->GetStandardInterline());
	}
	elem->SetTextStartX(textStartX);
	elem->SetTextStartY(textStartY);
}

Button* UI::LCreateButton(int layer, const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (!settings.useLayersInRendering) {
		throw std::exception("This should not be used without layer in rendering set on");
	}

	if (GetButton(name) != nullptr) {
		std::println("Warning name collision Button with name: {} already exists addition abborted", name);
		return nullptr;
	}

	if (layer < 0) { layer = 0; }
	if (layer > 100) { layer = 100; }
	ZElemVec[layer].emplace_back(new Button());
	Button* btn = static_cast<Button*>(ZElemVec[layer].back());
	btn->zLayer = layer;;

	btn->castType = CastType::Button;
	FillElem(btn, name, x, y, w, h, texture, font, text, textScale, textStartX, textStartY);

	UIElemMap.emplace(btn->name, btn);
	return btn;
}

TextBox* UI::LCreateTextBox(int layer, const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (!settings.useLayersInRendering) {
		throw std::exception("This should not be used without layer in rendering set on");
	}

	if (GetTextBox(name) != nullptr) {
		std::println("Warning name collision TextBox with name: {} already exists addition abborted", name);
		return nullptr;
	}

	if (layer < 0) { layer = 0; }
	if (layer > 100) { layer = 100; }
	ZElemVec[layer].emplace_back(new TextBox());
	TextBox* tb = static_cast<TextBox*>(ZElemVec[layer].back());
	tb->zLayer = layer;

	tb->castType = CastType::TextBox;
	FillElem(tb, name, x, y, w, h, texture, font, text, textScale, textStartX, textStartY);

	UIElemMap.emplace(tb->name, tb);
	return tb;
}

ClickBox* UI::LCreateClickBox(int layer, const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (!settings.useLayersInRendering) {
		throw std::exception("This should not be used without layer in rendering set on");
	}

	if (GetClickBox(name) != nullptr) {
		std::println("Warning name collision Click Box with name: {} already exists addition abborted", name);
		return nullptr;
	}

	if (layer < 0) { layer = 0; }
	if (layer > 100) { layer = 100; }
	ZElemVec[layer].emplace_back(new ClickBox());
	ClickBox* cb = static_cast<ClickBox*>(ZElemVec[layer].back());
	cb->zLayer = layer;

	cb->castType = CastType::ClickBox;
	FillElem(cb, name, x, y, w, h, texture, font, text, textScale, textStartX, textStartY);

	UIElemMap.emplace(cb->name, cb);
	return cb;
}

PopUpBox* UI::LCreatePopUpBox(int layer, const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (!settings.useLayersInRendering) {
		throw std::exception("This should not be used without layer in rendering set on");
	}

	if (GetPopUpBox(name) != nullptr) {
		std::cout << "Warning name collision PopUpBox with name: " << name << " already exists addition abborted\n";
		return nullptr;
	}

	if (layer < 0) { layer = 0; }
	if (layer > 100) { layer = 100; }
	ZElemVec[layer].emplace_back(new PopUpBox());
	PopUpBox* pb = static_cast<PopUpBox*>(ZElemVec[layer].back());
	pb->zLayer = layer;

	pb->castType = CastType::PopUpBox;
	pb->lifeTime = lifeSpan;
	FillElem(pb, name, x, y, w, h, texture, font, text, textScale, textStartX, textStartY);

	UIElemMap.emplace(pb->name, pb);
	popupBoxesCount++;
	return pb;
}

Slider* UI::LCreateSlider(int layer, const std::string& name, int x, int y, int w, int h, int slideType, int min, int max, MT::Texture* texture) {

	if (!settings.useLayersInRendering) {
		throw std::exception("This should not be used without layer in rendering set on");
	}

	if (GetSlider(name) != nullptr) {
		std::cout << "Warning name collision Slider with name: " << name << " already exists addition abborted\n";
		return nullptr;
	}

	if (layer < 0) { layer = 0; }
	if (layer > 100) { layer = 100; }
	ZElemVec[layer].emplace_back(new Slider());
	Slider* sl = static_cast<Slider*>(ZElemVec[layer].back());
	sl->zLayer = layer;

	sl->castType = CastType::Slider;
	sl->name = name;
	sl->GetRectangle().Set(x, y, w, h);
	sl->SetRenderType((int)RenderType::Standard);
	sl->SetTexture(texture);
	sl->SetSlideType(slideType);
	sl->SetMin(min);
	sl->SetMax(max);

	return sl;
}

Button* UI::CreateButton(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (GetButton(name) != nullptr) {
		std::println("Warning name collision Button with name: {} already exists addition abborted",name);
		return nullptr;
	}

	Button* btn = nullptr;
	if (settings.useLayersInRendering) {
		ZElemVec[0].emplace_back(new Button());
		btn = static_cast<Button*>(ZElemVec[0].back());
	}
	else {
		UiElemVec.emplace_back(new Button());
		btn = static_cast<Button*>(UiElemVec.back());

	}

	btn->castType = CastType::Button;
	FillElem(btn,name, x, y, w, h, texture, font, text, textScale, textStartX, textStartY);

	UIElemMap.emplace(btn->name, btn);
	return btn;
}

TextBox* UI::CreateTextBox(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (GetTextBox(name) != nullptr) {
		std::println("Warning name collision TextBox with name: {} already exists addition abborted", name);
		return nullptr;
	}

	TextBox* tb = nullptr;
	if (settings.useLayersInRendering) {
		ZElemVec[0].emplace_back(new TextBox());
		tb = static_cast<TextBox*>(ZElemVec[0].back());
	}
	else {
		UiElemVec.emplace_back(new TextBox());
		tb = static_cast<TextBox*>(UiElemVec.back());
	}

	tb->castType = CastType::TextBox;
	FillElem(tb, name, x, y, w, h, texture, font, text, textScale, textStartX, textStartY);

	UIElemMap.emplace(tb->name, tb);
	return tb;
}

ClickBox* UI::CreateClickBox(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (GetClickBox(name) != nullptr) {
		std::println("Warning name collision Click Box with name: {} already exists addition abborted", name);
		return nullptr;
	}

	ClickBox* cb = nullptr;
	if (settings.useLayersInRendering) {
		ZElemVec[0].emplace_back(new ClickBox());
		cb = static_cast<ClickBox*>(ZElemVec[0].back());
	}
	else {
		UiElemVec.emplace_back(new ClickBox());
		cb = static_cast<ClickBox*>(UiElemVec.back());
	}

	cb->castType = CastType::ClickBox;
	FillElem(cb, name, x, y, w, h, texture, font, text, textScale, textStartX, textStartY);

	UIElemMap.emplace(cb->name, cb);
	return cb;
}

PopUpBox* UI::CreatePopUpBox(const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {
	if (GetPopUpBox(name) != nullptr) {
		std::cout << "Warning name collision PopUpBox with name: " << name << " already exists addition abborted\n";
		return nullptr;
	}

	PopUpBox* pb = nullptr;
	if (settings.useLayersInRendering) {
		ZElemVec[0].emplace_back(new PopUpBox());
		pb = static_cast<PopUpBox*>(ZElemVec[0].back());
	}
	else {
		UiElemVec.emplace_back(new PopUpBox());
		pb = static_cast<PopUpBox*>(UiElemVec.back());
	}

	pb->castType = CastType::PopUpBox;
	pb->lifeTime = lifeSpan;
	FillElem(pb, name, x, y, w, h, texture, font, text, textScale, textStartX, textStartY);

	UIElemMap.emplace(pb->name, pb);
	popupBoxesCount++;
	return pb;
}

Slider* UI::CreateSlider(const std::string& name, int x, int y, int w, int h, int slideType, int min, int max, MT::Texture* texture){
	if (GetSlider(name) != nullptr) {
		std::cout << "Warning name collision PopUpBox with name: " << name << " already exists addition abborted\n";
		return nullptr;
	}

	Slider* sl = nullptr;
	if (settings.useLayersInRendering) {
		ZElemVec[0].emplace_back(new Slider());
		sl = static_cast<Slider*>(ZElemVec[0].back());
	}
	else {
		UiElemVec.emplace_back(new Slider());
		sl = static_cast<Slider*>(UiElemVec.back());
	}

	sl->castType = CastType::Slider;
	sl->castType = CastType::Slider;
	sl->name = name;
	sl->GetRectangle().Set(x, y, w, h);
	sl->SetRenderType((int)RenderType::Standard);

	sl->SetTexture(texture);
	sl->SetSlideType(slideType);
	sl->SetMin(min);
	sl->SetMax(max);

	UIElemMap.emplace(sl->name, sl);
	return sl;
}


Button* UI::CreateButtonF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, const std::string& fontSt,
	const std::string& text, float textScale, int textStartX, int textStartY) {
	return CreateButton(name, x, y, w, h, texture, GetFont(fontSt), text, textScale, textStartX, textStartY);
}

TextBox* UI::CreateTextBoxF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, const std::string& fontSt,
	const std::string& text, float textScale, int textStartX, int textStartY) {
	return CreateTextBox(name, x, y, w, h, texture, GetFont(fontSt), text, textScale, textStartX, textStartY);
}

ClickBox* UI::CreateClickBoxF(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, const std::string& fontSt,
	const std::string& text, float textScale, int textStartX, int textStartY) {
	return CreateClickBox(name, x, y, w, h, texture, GetFont(fontSt), text, textScale, textStartX, textStartY);
}

PopUpBox* UI::CreatePopUpBoxF(const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture, const std::string& fontSt,
	const std::string& text, float textScale, int textStartX, int textStartY) {
	return CreatePopUpBox(name, lifeSpan, x, y, w, h, texture, GetFont(fontSt), text, textScale, textStartX, textStartY);
}

bool UI::RenameElem(const std::string& name, const std::string& newName) {
	auto elem = UIElemMap.find(name);

	if (elem == UIElemMap.end()) {
		return false;
	}

	auto elemTwo = UIElemMap.find(newName);

	if (elemTwo != UIElemMap.end()) {
		return false;
	}
	UIElemBase* elemPtr = elem->second;
	UIElemMap.erase(name);
	elemPtr->name = newName;
	UIElemMap[elemPtr->name] = elemPtr;
	return true;
}

void UI::CheckHover(UIElemBase *elem, bool &hoverStop) {
	//TODO When all elemnt are in future in one vec optimize this
	if (hoverStop) {
		elem->hovered = false;
		return;
	}
	Point p = GetMousePos();
	MT::Rect rect{ p.x,p.y,1,1 };
	if (!SimpleCollision(elem->GetRectangle(), rect)) {
		elem->hovered = false;
		return;
	}

	elem->hovered = true;
	if (settings.stopHoverAtFirst) {
		hoverStop = true;
	}

	// patrzenie czy mo¿e byæ wydany dŸwiêk tylko wtedy zadzia³a gdy mysz pierwszy raz jest na przycisku
	if (elem->GetHoverSound() == nullptr) { return; }

	if (!SimpleCollision(lastMousePos, elem->GetRectangle())) {
		SoundMan::PlayRawSound(elem->GetHoverSound());
	}
}

void UI::CheckTextBoxInteraction(TextBox *tb, SDL_Event& event) {
	if (!tb->turnedOn) { return; }
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
		MT::Rect temprect{ event.button.x ,event.button.y,1,1 };
		if (SimpleCollision(tb->GetRectangle(), temprect)) {
			tb->isUsed = true;
		}
		else {
			tb->isUsed = false;
		}
	}
}

void UI::ManageTextBoxTextInput(TextBox *tb, SDL_Event& event) {
	if (!tb->isUsed) { return; }
	SDL_StartTextInput();

	if (event.type == SDL_TEXTINPUT) {
		if (tb->text.length() >= tb->maxTextLength) { return; }
		tb->text += event.text.text;
	}
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) { // Enter
			if (tb->text.length() >= tb->maxTextLength) { return; }
			tb->text += '\n';
		}
		else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE && !tb->text.empty()) {
			tb->text.pop_back();
		}
	}
}

void UI::CheckClickBoxes(ClickBox *cb, int eventType, bool &forceStop, SDL_Event& event) {
	if (event.type == eventType) {
		// Checks from newest to oldest it does not really have diffence on overall order
		// since status is called when you want but if you use stopCheckAtFirst setting it will not call
		// click box behing the first even if it still colides with mouse 
		if (!cb->IsOn()) { return; }

		MT::Rect temprect{ event.button.x ,event.button.y,1,1 };
		if (!SimpleCollision(cb->GetRectangle(), temprect)) { return; }
		cb->SetStatus(true);

		if (cb->GetClickSound() != "") { 
			SoundMan::PlaySound(cb->GetClickSound());
		}
		if (settings.stopCheckAtFirst) { forceStop = true; }
	}
}


void UI::SlideSliders(Slider* slider, SDL_Event& event) {
	if (event.button.button != SDL_BUTTON_LEFT) { return; }

	MT::Rect temprect{ event.button.x ,event.button.y,1,1 };
	if (!slider->GetRectangle().IsColliding(temprect)) { return; }

	if (slider->GetSlideType() == (int)SliderSlide::X) {
		slider->GetRectangle().x = temprect.x - slider->GetRectangle().w/2;
		if (slider->GetRectangle().x < slider->min) {
			slider->GetRectangle().x = slider->min;
			return;
		}
		if (slider->GetRectangle().x > slider->max - slider->GetRectangle().w) {
			slider->GetRectangle().x = slider->max - slider->GetRectangle().w;
			return;
		}
	}
	else {
		slider->GetRectangle().y = temprect.y - slider->GetRectangle().h / 2;
		if (slider->GetRectangle().y < slider->min) {
			slider->GetRectangle().y = slider->min;
			return;
		}
		if (slider->GetRectangle().y > slider->max - slider->GetRectangle().w) {
			slider->GetRectangle().y = slider->max - slider->GetRectangle().w;
			return;
		}
	}
}

void UI::ManageInput(SDL_Event& event) {

	int eventType = SDL_MOUSEBUTTONUP;
	if (settings.clickBoxStartAtDown) {
		eventType = SDL_MOUSEBUTTONDOWN;
	}
	bool clickBoxesStopped = false;
	bool hoverStopped = false;
	

	auto CheckElem = [&](UIElemBase* elem) {
		CheckHover(elem, hoverStopped);

		if (elem->castType == CastType::ClickBox) {
			if (clickBoxesStopped) { return; }
			ClickBox* cb = static_cast<ClickBox*>(elem);
			CheckClickBoxes(cb, eventType, clickBoxesStopped, event);
		}
		else if (elem->castType == CastType::TextBox) {
			TextBox* tb = static_cast<TextBox*>(elem);
			CheckTextBoxInteraction(tb, event);
			ManageTextBoxTextInput(tb, event);
		}
		else if (elem->castType == CastType::Slider) {
			Slider* sl = static_cast<Slider*>(elem);
			SlideSliders(sl,event);
		}
	};

	if (settings.useLayersInRendering) {
		for (auto layerIter = ZElemVec.rbegin(); layerIter != ZElemVec.rend(); layerIter++) {
			for (auto elemIter = layerIter->rbegin(); elemIter != layerIter->rend(); elemIter++) {
				UIElemBase* elem = *elemIter;
				CheckElem(elem);
			}
		}
	}
	else {
		for (auto elemIter = UiElemVec.rbegin(); elemIter != UiElemVec.rend(); elemIter++) {
			UIElemBase* elem = *elemIter;
			CheckElem(elem);
		}
	}

	SDL_GetMouseState(&lastMousePos.x, &lastMousePos.y);
}

UIElemBase* UI::GetElem(const std::string& name) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return nullptr;
	}
	return iter->second;
}

Button* UI::GetButton(const std::string& name) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return nullptr;
	}
	UIElemBase* elem = iter->second;
	if (elem->castType == CastType::Button) {
		return static_cast<Button*>(elem);
	}
	return nullptr;
}
TextBox* UI::GetTextBox(const std::string& name) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return nullptr;
	}
	UIElemBase* elem = iter->second;
	if (elem->castType == CastType::TextBox) {
		return static_cast<TextBox*>(elem);
	}
	return nullptr;
}
ClickBox* UI::GetClickBox(const std::string& name) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return nullptr;
	}
	UIElemBase* elem = iter->second;
	if (elem->castType == CastType::ClickBox) {
		return static_cast<ClickBox*>(elem);
	}
	return nullptr;
}

PopUpBox* UI::GetPopUpBox(const std::string& name) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return nullptr;
	}
	UIElemBase* elem = iter->second;
	if (elem->castType == CastType::PopUpBox) {
		return static_cast<PopUpBox*>(elem);
	}
	return nullptr;
}

Slider* UI::GetSlider(const std::string& name) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return nullptr;
	}
	UIElemBase* elem = iter->second;
	if (elem->castType == CastType::Slider) {
		return static_cast<Slider*>(elem);
	}
	return nullptr;
}

bool UI::ConsumeIfExist(const std::string& name) {
	ClickBox* cb = GetClickBox(name);
	if (cb == nullptr) { return false; }
	return cb->ConsumeStatus();
}

void UI::SetElementColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return;
	}
	iter->second->SetColor(R, G, B);
}

void UI::SetElementBorderColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return;
	}
	iter->second->SetBorderColor(R, G, B);
}

void UI::SetElementFontColor(const std::string& name, const unsigned char R, const unsigned char G, const unsigned char B) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return;
	}
	iter->second->SetFontColor(R, G, B);
}

void UI::SetElementZLayer(const std::string& name, int zlayer) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return;
	}
	UIElemBase* elem = iter->second;
	int prevLayer = elem->zLayer;
	elem->SetZLayer(zlayer);
	if (settings.useLayersInRendering) {
		auto elemIter = std::find_if(ZElemVec[prevLayer].begin(), ZElemVec[prevLayer].end(), 
			[&](UIElemBase* itElem) {return elem == itElem; });
		if (elemIter != ZElemVec[prevLayer].end()) { // If it is the end guess we are fucked
			ZElemVec[prevLayer].erase(elemIter);
		}
		ZElemVec[elem->zLayer].emplace_back(elem);
	}
}

void UI::FrameUpdate() {
	if (popupBoxesCount < 1) { return; }
	if (settings.useLayersInRendering) {
		for (auto layerIter = ZElemVec.rbegin(); layerIter != ZElemVec.rend(); layerIter++) {
			for (auto elemIt = layerIter->rbegin(); elemIt != layerIter->rend(); elemIt++) {
				if ((*elemIt)->castType != CastType::PopUpBox) {
					++elemIt;
					continue;
				}
				PopUpBox* pb = static_cast<PopUpBox*>(*elemIt);
				pb->lifeTime--;
				if (pb->lifeTime < 1) {
					DeleteElement(pb->name);
					return;
				}
				else {
					++elemIt;
				}
			}
		}
	}
	else {
		for (auto elemIt = UiElemVec.rbegin(); elemIt != UiElemVec.rend();) {
			if ((*elemIt)->castType != CastType::PopUpBox) {
				++elemIt;
				continue;
			}
			PopUpBox* pb = static_cast<PopUpBox*>(*elemIt);
			pb->lifeTime--;
			if (pb->lifeTime < 1) {
				DeleteElement(pb->name);
				return;
			}
			else {
				++elemIt;
			}
		}
	}
}


bool UI::DeleteElement(const std::string& name) {
	auto elemIter = UIElemMap.find(name);
	if (elemIter == UIElemMap.end()) { return false; }
	if (settings.useLayersInRendering) {
		std::vector<UIElemBase*>& layer = ZElemVec[elemIter->second->zLayer];
		auto vecIter = std::find(layer.rbegin(), layer.rend(), elemIter->second);
		if (vecIter != layer.rend()) {
			if (elemIter->second->castType == CastType::PopUpBox) {
				popupBoxesCount--;
			}
			delete* vecIter;
			// It needs to be movedby one to base since reverse iter is not working for vectors by base
			layer.erase(std::next(vecIter).base());

			UIElemMap.erase(elemIter);
			return true;
		}
	}
	else {
		auto vecIter = std::find(UiElemVec.rbegin(), UiElemVec.rend(), elemIter->second);
		if (vecIter != UiElemVec.rend()) {
			if (elemIter->second->castType == CastType::PopUpBox) {
				popupBoxesCount--;
			}
			delete* vecIter;
			// It needs to be movedby one to base since reverse iter is not working for vectors by base
			UiElemVec.erase(std::next(vecIter).base());

			UIElemMap.erase(elemIter);
			return true;
		}
	}

	return false;
}

void UI::CreateFont(const std::string& name, MT::Texture* texture, const std::string& jsonPath) {
	fontManager->CreateFont(name, texture, jsonPath);
}

bool UI::CrateTempFontFromTTF(const char* ttfPath, const int size, const std::string& name, LocalTexMan* localTexMan) {
	return fontManager->CrateTempFontFromTTF(ttfPath, size, name,localTexMan);
}

Font* UI::GetFont(const std::string& name) {
	return fontManager->GetFont(name);
}

void UI::SetBaseFont(Font* font) {
	baseFont = font;
}

Font* UI::GetBaseFont() {
	return this->baseFont;
}

std::vector<Font*>& UI::GetAllFonts() {
	return this->fontManager->fonts;
}

void UI::ScanFont(const std::string& texturePath, const std::string& charactersDataPath,
	unsigned char fR, unsigned char fG, unsigned char fB, unsigned char bR, unsigned char bG, unsigned char bB, Point size,
	const std::string& outputPath) {
	fontManager->ScanFont(texturePath, charactersDataPath, fR, fG, fB, bR, bG, bB, size.x, size.y, outputPath);
}



void UI::DumpButton(nlohmann::ordered_json& json, UIElemBase* elem) {
	auto& jsonElem = json[elem->GetName()];

	jsonElem["Type"] = elem->castType;
	jsonElem["X"] = elem->GetRectangle().x;
	jsonElem["Y"] = elem->GetRectangle().y;
	jsonElem["W"] = elem->GetRectangle().w;
	jsonElem["H"] = elem->GetRectangle().h;

	std::string textureName = "";
	for (auto& texture : TexMan::GetAllTex()) {
		if (elem->texture == texture.second) {
			textureName = texture.first;
			break;
		}
	}
	if (!textureName.empty()) {
		jsonElem["Texture"] = textureName;
	}

	jsonElem["Text"] = elem->text;
	jsonElem["TextScale"] = elem->textScale;
	jsonElem["Border"] = elem->border;
	jsonElem["BorderThinkness"] = elem->borderThickness;
	jsonElem["TextStartX"] = elem->textStartX;
	jsonElem["TextStartY"] = elem->textStartY;

	jsonElem["ColorR"] = elem->buttonColor.R;
	jsonElem["ColorG"] = elem->buttonColor.G;
	jsonElem["ColorB"] = elem->buttonColor.B;
	jsonElem["ColorA"] = elem->buttonColor.A;

	jsonElem["BorderR"] = elem->borderRGB.R;
	jsonElem["BorderG"] = elem->borderRGB.G;
	jsonElem["BorderB"] = elem->borderRGB.B;

	jsonElem["FontR"] = elem->fontRGB.R;
	jsonElem["FontG"] = elem->fontRGB.G;
	jsonElem["FontB"] = elem->fontRGB.B;

	std::string fontName = "";
	for (auto& font : fontManager->fonts) {
		if (font == elem->font) {
			fontName = font->GetName();
			break;
		}
	}
	if (!fontName.empty()) {
		jsonElem["Font"] = fontName;
	}
	if (elem->renderFunction == &UIElemBase::Render) {
		jsonElem["RenderType"] = (int)RenderType::Standard;
	}
	else {
		jsonElem["RenderType"] = (int)RenderType::Rounded;
	}
	jsonElem["TextRenderType"] = elem->textRenderType;
	jsonElem["Hidden"] = elem->hidden;
	jsonElem["Hovered"] = elem->hovered;
	jsonElem["Hoverable"] = elem->hoverable;

	jsonElem["HoverFilterR"] = elem->hoverFilter.R;
	jsonElem["HoverFilterG"] = elem->hoverFilter.G;
	jsonElem["HoverFilterB"] = elem->hoverFilter.B;
	jsonElem["HoverFilterA"] = elem->hoverFilter.A;

	std::string hoverSoundStr = "";
	for (auto& sound : SoundMan::GetSounds()) {
		if (elem->hoverSound == sound.second) {
			hoverSoundStr = sound.first;
			break;
		}
	}
	if (!hoverSoundStr.empty()) {
		jsonElem["HoverSound"] = hoverSoundStr;
	}
	jsonElem["Zlayer"] = elem->zLayer;
}

void UI::DumpClickBox(nlohmann::ordered_json& json, ClickBox* cb) {
	auto& jsonElem = json[cb->GetName()];
	jsonElem["TurnedOn"] = cb->turnedOn;
	jsonElem["ClickSound"] = cb->clickSound;
}

void UI::DumpTextBox(nlohmann::ordered_json& json, TextBox* tb) {
	auto& jsonElem = json[tb->GetName()];
	jsonElem["TurnedOn"] = tb->turnedOn;
	jsonElem["MaxTextLength"] = tb->maxTextLength;
}

void UI::DumpPopUpBox(nlohmann::ordered_json& json, PopUpBox* pb) {
	auto& jsonElem = json[pb->GetName()];
	jsonElem["LifeTime"] = pb->lifeTime;
}

void UI::DumpSlider(nlohmann::ordered_json& json, Slider* sl) {
	auto& jsonElem = json[sl->GetName()];
	jsonElem["SlideType"] = sl->slideType;
	jsonElem["Min"] = sl->min;
	jsonElem["Max"] = sl->max;
}

void UI::DumpToJson(const std::string &fileName, const std::vector<UIElemBase*>& elements) {
	std::ofstream file(fileName + ".json");
	if (!file.is_open()) {
		return;
	}
	
	nlohmann::ordered_json jsonFile;
	for (auto& elem : elements) {
		switch (elem->castType) {
			case CastType::Button:
				DumpButton(jsonFile, elem);
				break;
			case CastType::ClickBox:
				DumpButton(jsonFile, elem);
				DumpClickBox(jsonFile, static_cast<ClickBox*>(elem));
				break;
			case CastType::TextBox:
				DumpButton(jsonFile, elem);
				DumpTextBox(jsonFile, static_cast<TextBox*>(elem));
				break;
			case CastType::PopUpBox:
				DumpButton(jsonFile, elem);
				DumpPopUpBox(jsonFile, static_cast<PopUpBox*>(elem));
				break;
			case CastType::Slider:
				DumpButton(jsonFile, elem);
				DumpSlider(jsonFile, static_cast<Slider*>(elem));
				break;
		}
	}
	file << jsonFile;
}

std::vector<UIElemBase*> UI::LoadFromJson(const std::string& fileName) {
	std::vector<UIElemBase*> loadedElements;
	std::ifstream file(fileName);
	if (!file.is_open()) {
		return loadedElements;
	}
	nlohmann::json jsonFile;
	try {
		file >> jsonFile;
	}
	catch (const std::exception&) {
		return loadedElements;
	}

	for (auto& [key, val] : jsonFile.items()) {
		if (key.empty()) {
			continue;
		}
		int type = val["Type"];

		std::unique_ptr<UIElemBase> elem = nullptr;

		if (type == (int)CastType::Button) {
			elem = std::make_unique<Button>();
		}
		else if (type == (int)CastType::ClickBox) {
			elem = std::make_unique<ClickBox>();
		}
		else if (type == (int)CastType::TextBox) {
			elem = std::make_unique<TextBox>();
		}
		else if (type == (int)CastType::PopUpBox) {
			elem = std::make_unique<PopUpBox>();
		}

		elem->name = key;
		elem->GetRectangle().x = val["X"];
		elem->GetRectangle().y = val["Y"];
		elem->GetRectangle().w = val["W"];
		elem->GetRectangle().h = val["H"];
		if (val.contains("Texture")) {
			elem->texture = TexMan::GetTex(val["Texture"]);
		}
		elem->text = val["Text"];
		elem->textScale = val["TextScale"];
		elem->border = val["Border"];
		elem->borderThickness = val["BorderThinkness"];
		elem->textStartX = val["TextStartX"];
		elem->textStartY = val["TextStartY"];
		elem->buttonColor.R = val["ColorR"];
		elem->buttonColor.G = val["ColorG"];
		elem->buttonColor.B = val["ColorB"];
		elem->buttonColor.A = val["ColorA"];

		elem->borderRGB.R = val["BorderR"];
		elem->borderRGB.G = val["BorderG"];
		elem->borderRGB.B = val["BorderB"];

		elem->fontRGB.R = val["FontR"];
		elem->fontRGB.G = val["FontG"];
		elem->fontRGB.B = val["FontB"];
		if (val.contains("Font")) {
			elem->font = GetFont(val["Font"]);
		}
		int renderFunction = val["RenderType"];
		elem->SetRenderType(renderFunction);
		elem->textRenderType = val["TextRenderType"];
		elem->hidden = val["Hidden"];
		elem->hovered = val["Hovered"];
		elem->hoverable = val["Hoverable"];

		elem->hoverFilter.R = val["HoverFilterR"];
		elem->hoverFilter.G = val["HoverFilterG"];
		elem->hoverFilter.B = val["HoverFilterB"];
		elem->hoverFilter.A = val["HoverFilterA"];

		if (val.contains("HoverSound")) {
			elem->hoverSound = SoundMan::GetSound(val["HoverSound"]);
		}
		elem->zLayer = val["Zlayer"];


		if (type == (int)CastType::Button) {
			Button* btn = nullptr;
			if (settings.useLayersInRendering) {
				btn = LCreateButton(elem->zLayer,key, 0, 0, 0, 0);
			}
			else {
				btn = CreateButton(key, 0, 0, 0, 0);
			}
			int prevLayer = btn->zLayer;
			*btn = *static_cast<Button*>(elem.get());
			btn->zLayer = prevLayer;
			loadedElements.emplace_back(btn);
		}
		else if (type == (int)CastType::ClickBox) {
			ClickBox* cb = nullptr;
			if (settings.useLayersInRendering) {
				cb = LCreateClickBox(elem->zLayer, key, 0, 0, 0, 0);
			}
			else {
				cb = CreateClickBox(key, 0, 0, 0, 0);
			}
			int prevLayer = cb->zLayer;
			*cb = *static_cast<ClickBox*>(elem.get());
			cb->zLayer = prevLayer;
			cb->turnedOn = val["TurnedOn"];
			cb->clickSound = val["ClickSound"];
			loadedElements.emplace_back(cb);
		}
		else if (type == (int)CastType::TextBox) {
			TextBox* tb = nullptr;
			if (settings.useLayersInRendering) {
				tb = LCreateTextBox(elem->zLayer, key, 0, 0, 0, 0);
			}
			else {
				tb = CreateTextBox(key, 0, 0, 0, 0);
			}
			int prevLayer = tb->zLayer;
			*tb = *static_cast<TextBox*>(elem.get());
			tb->zLayer = prevLayer;
			tb->turnedOn = val["TurnedOn"];
			tb->maxTextLength = val["MaxTextLength"];
			loadedElements.emplace_back(tb);;
		}
		else if (type == (int)CastType::PopUpBox) {
			PopUpBox* pb = nullptr;
			if (settings.useLayersInRendering) {
				pb = LCreatePopUpBox(elem->zLayer, key, 120, 0, 0, 0, 0);
			}
			else {
				pb = CreatePopUpBox(key,120, 0, 0, 0, 0);
			}
			int prevLayer = pb->zLayer;
			*pb = *static_cast<PopUpBox*>(elem.get());
			pb->zLayer = prevLayer;
			pb->lifeTime = val["LifeTime"];
			loadedElements.emplace_back(pb);
		}
		else if (type == (int)CastType::Slider) {
			Slider* sl = nullptr;
			if (settings.useLayersInRendering) {
				sl = LCreateSlider(elem->zLayer, key, 120, 0, 0, 0, 0, 0, 0);
			}
			else {
				sl = CreateSlider(key, 120, 0, 0, 0, 0, 0, 0);
			}
			int prevLayer = sl->zLayer;
			*sl = *static_cast<Slider*>(elem.get());
			sl->zLayer = prevLayer;
			sl->slideType = val["SlideType"];
			sl->min = val["Min"];
			sl->max = val["Max"];
			loadedElements.emplace_back(sl);
		}
	}
	return loadedElements;
}

void UI::ClearAll(bool clearLists) {
	if (clearLists) {
		for (auto& it : ListBtnRef) {
			it->Clear();
		}
		ListBtnRef.clear();
		for (auto& it : ListTbRef) {
			it->Clear();
		}
		ListBtnRef.clear();
		for (auto& it : ListCbRef) {
			it->Clear();
		}
		ListBtnRef.clear();
		ListTbRef.clear();
		ListCbRef.clear();
	}
	if (settings.useLayersInRendering) {
		for (auto& layer : ZElemVec) {
			for (auto& elem : layer) {
				delete elem;
			}
			layer.clear();
		}
	}
	else {
		for (auto& elem : UiElemVec) {
			delete elem;
		}
	}

	UiElemVec.clear();
	UIElemMap.clear();
	popupBoxesCount = 0;
}



UI::~UI() {
	ClearAll();
	delete fontManager;
}
