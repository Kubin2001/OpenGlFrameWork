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
		case 1:
			font->RenderText(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		case 2:
			font->RenderTextCenter(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		case 3:
			font->RenderTextFromRight(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		case 4:
			font->RenderTextCenterX(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		case 5:
			font->RenderTextCenterY(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
		default: // Standardowa opcja
			font->RenderText(renderer, text, rectangle, textScale, interLine, textStartX, textStartY);
			break;
	}
	font->GetTexture()->SetAlphaBending(255);
}

void UIElemBase::SetRenderType(const unsigned int renderType) {
	if (renderType == 1) {
		renderFunction = &UIElemBase::Render;
	}
	else  if (renderType == 2) {
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

Mix_Chunk* UIElemBase::GetHooverSound() {
	return hoverSound;
}

int UIElemBase::GetZLayer() {
	return this->zLayer;
}

void UIElemBase::SetZLayer(const int temp) {
	zLayer = temp;
}

//BUTTON
//MassageBox
void TextBox::CheckInteraction(SDL_Event& event) {
	if (event.button.button == SDL_BUTTON_LEFT) {
		MT::Rect temprect{ event.button.x ,event.button.y,1,1 };
		if (SimpleCollision(GetRectangle(), temprect)) {
			turnedOn = true;
		}
		else if (!SimpleCollision(GetRectangle(), temprect) && turnedOn) {
			turnedOn = false;
		}
	}
}

void TextBox::ManageTextInput(SDL_Event& event) {
	if (turnedOn) {
		SDL_StartTextInput();

		if (event.type == SDL_TEXTINPUT) {
			GetText() += event.text.text;
		}
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
				GetText() += '\n';
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE && !text.empty()) {
				GetText().pop_back();
			}
		}

	}

}

//MassageBox
//InteractionBox
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
//InteractionBox
//Pop Up Box

int PopUpBox::GetLifeTime() {
	return this->lifeTime;
}

void PopUpBox::SetLifeTime(const int lifeTime) {
	this->lifeTime = lifeTime;
}

//Pop Up Box
UI::UI(MT::Renderer* renderer) {
	fontManager = new FontManager();
	this->renderer = renderer;
	lastMousePos.x = -10000000;
	lastMousePos.y = -10000000;
}



void UI::Render() {
	if (!settings.useLayersInRendering) {
		for (const auto& it : Buttons) {
			it->renderFunction(it,renderer);
		}
		for (const auto& it : TextBoxes) {
			it->renderFunction(it, renderer);
		}
		for (const auto& it : ClickBoxes) {
			it->renderFunction(it, renderer);
		}
		for (const auto& it : PopUpBoxes) {
			it->renderFunction(it, renderer);
		}
	}
	else {
		for (const auto& it : Buttons) {
			Zlayers[it->zLayer].Buttons.emplace_back(it);
		}
		for (const auto& it : TextBoxes) {
			Zlayers[it->zLayer].TextBoxes.emplace_back(it);
		}
		for (const auto& it : ClickBoxes) {
			Zlayers[it->zLayer].ClickBoxes.emplace_back(it);
		}
		for (const auto& it : PopUpBoxes) {
			Zlayers[it->zLayer].PopUpBoxes.emplace_back(it);
		}
		for (auto& it : Zlayers) {
			auto& layer = it.second;
			for (auto& btn : layer.Buttons) {
				btn->renderFunction(btn, renderer);
			}
			for (auto& btn : layer.ClickBoxes) {
				btn->renderFunction(btn, renderer);
			}
			for (auto& btn : layer.TextBoxes) {
				btn->renderFunction(btn, renderer);
			}
			for (auto& btn : layer.PopUpBoxes) {
				btn->renderFunction(btn, renderer);
			}
		}
		Zlayers.clear();
	}
}


void UI::RenderRawText(Font* font, const int x, const int y, const std::string& text,const int interline,
		const unsigned char R, const unsigned char G, const unsigned char B) {
	font->RenderRawText(renderer, x, y, text, interline, R, G, B);
}

Button* UI::CreateButton(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (GetButton(name) != nullptr) {
		std::cout << "Warning name collision Button with name: " << name << " already exists addition abborted\n";
		return nullptr;
	}

	Buttons.emplace_back(new Button());
	Button* btn = Buttons.back();
	btn->castType = (int)CastType::Button;
	btn->name = name;
	btn->GetRectangle().Set(x, y, w, h);
	btn->SetRenderType(1);

	btn->SetTexture(texture);

	btn->SetText(text);
	btn->SetTextScale(textScale);
	btn->SetFont(font);
	if (font != nullptr) {
		btn->SetInterLine(font->GetStandardInterline());
	}

	btn->SetTextStartX(textStartX);
	btn->SetTextStartY(textStartY);

	UIElemMap.emplace(btn->GetName(), btn);
	return btn;
}

TextBox* UI::CreateTextBox(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (GetTextBox(name) != nullptr) {
		std::cout << "Warning name collision TextBox with name: " << name << " already exists addition abborted\n";
		return nullptr;
	}

	TextBoxes.emplace_back(new TextBox());
	TextBox* tb = TextBoxes.back();
	tb->castType = (int)CastType::TextBox;
	tb->name = name;
	tb->GetRectangle().Set(x, y, w, h);
	tb->SetRenderType(1);

	tb->SetTexture(texture);

	tb->SetText(text);

	tb->SetTextScale(textScale);
	tb->SetFont(font);
	if (font != nullptr) {
		tb->SetInterLine(font->GetStandardInterline());
	}

	tb->SetTextStartX(textStartX);
	tb->SetTextStartY(textStartY);

	UIElemMap.emplace(tb->GetName(), tb);
	return tb;
}

ClickBox* UI::CreateClickBox(const std::string& name, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {

	if (GetClickBox(name) != nullptr) {
		std::cout << "Warning name collision click box with name: " << name << " already exists addition abborted\n";
		return nullptr;
	}

	ClickBoxes.emplace_back(new ClickBox());
	ClickBox* cb = ClickBoxes.back();
	cb->castType = (int)CastType::ClickBox;
	cb->name = name;
	cb->GetRectangle().Set(x, y, w, h);
	cb->SetRenderType(1);

	cb->SetTexture(texture);

	cb->SetText(text);

	cb->SetTextScale(textScale);
	cb->SetFont(font);
	if (font != nullptr) {
		cb->SetInterLine(font->GetStandardInterline());
	}

	cb->SetTextStartX(textStartX);
	cb->SetTextStartY(textStartY);

	UIElemMap.emplace(cb->GetName(), cb);
	return cb;
}

PopUpBox* UI::CreatePopUpBox(const std::string& name, int lifeSpan, int x, int y, int w, int h, MT::Texture* texture, Font* font,
	const std::string& text, float textScale, int textStartX, int textStartY) {
	if (GetPopUpBox(name) != nullptr) {
		std::cout << "Warning name collision PopUpBox with name: " << name << " already exists addition abborted\n";
		return nullptr;
	}

	PopUpBoxes.emplace_back(new PopUpBox());
	PopUpBox* pb = PopUpBoxes.back();
	pb->castType = (int)CastType::PopUpBox;
	pb->name = name;
	pb->SetLifeTime(lifeSpan);
	pb->GetRectangle().Set(x, y, w, h);
	pb->SetRenderType(1);

	pb->SetTexture(texture);

	pb->SetText(text);

	pb->SetTextScale(textScale);
	pb->SetFont(font);
	if (font != nullptr) {
		pb->SetInterLine(font->GetStandardInterline());
	}

	pb->SetTextStartX(textStartX);
	pb->SetTextStartY(textStartY);

	UIElemMap.emplace(pb->GetName(), pb);
	return pb;
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
	UIElemBase* elemPtr = elem->second;
	UIElemMap.erase(name);
	elemPtr->name = newName;
	UIElemMap[elemPtr->name] = elemPtr;
	return true;
}

void UI::CheckHover() {
	//TODO When all elemnt are in future in one vec optimize this
	Point p = GetMousePos();
	MT::Rect rect{ p.x,p.y,1,1 };
	for (auto& it : Buttons) {
		if (SimpleCollision(it->GetRectangle(), rect)) {
			it->SetHover(true);
			// patrzenie czy mo¿e byæ wydany dŸwiêk tylko wtedy zadzia³a gdy mysz pierwszy raz jest na przycisku
			if (it->GetHooverSound() != nullptr) { 
				MT::Rect prevMousePos{ lastMousePos.x,lastMousePos.y,1,1 };
				if (!SimpleCollision(prevMousePos, it->GetRectangle())) {
					SoundMan::PlayRawSound(it->GetHooverSound());
				}
			}
		}
		else{
			it->SetHover(false);
		}
	}
	for (auto& it : TextBoxes) {
		if (SimpleCollision(it->GetRectangle(), rect)) {
			it->SetHover(true);
			// patrzenie czy mo¿e byæ wydany dŸwiêk tylko wtedy zadzia³a gdy mysz pierwszy raz jest na przycisku
			if (it->GetHooverSound() != nullptr) {
				MT::Rect prevMousePos{ lastMousePos.x,lastMousePos.y,1,1 };
				if (!SimpleCollision(prevMousePos, it->GetRectangle())) {
					SoundMan::PlayRawSound(it->GetHooverSound());
				}
			}
		}
		else{
			it->SetHover(false);
		}
	}
	for (auto& it : ClickBoxes) {
		if (SimpleCollision(it->GetRectangle(), rect)) {
			it->SetHover(true);
			// patrzenie czy mo¿e byæ wydany dŸwiêk tylko wtedy zadzia³a gdy mysz pierwszy raz jest na przycisku
			if (it->GetHooverSound() != nullptr) {
				MT::Rect prevMousePos{ lastMousePos.x,lastMousePos.y,1,1 };
				if (!SimpleCollision(prevMousePos, it->GetRectangle())) {
					SoundMan::PlayRawSound(it->GetHooverSound());
				}
			}
		}
		else{
			it->SetHover(false);
		}
	}
}

void  UI::CheckTextBoxInteraction(SDL_Event& event) {
	for (auto& it : TextBoxes) {
		it->CheckInteraction(event);
	}
}

void UI::ManageTextBoxTextInput(SDL_Event& event) {
	for (auto& it : TextBoxes) {
		it->ManageTextInput(event);
	}
}

void UI::CheckClickBoxes(SDL_Event& event) {
	int eventType = SDL_MOUSEBUTTONUP;
	if (settings.clickBoxStartAtDown) {
		eventType = SDL_MOUSEBUTTONDOWN;
	}

	if (event.type == eventType) {
		// Checks from newest to oldest it does not really have diffence on overall order
		// since status is called when you want but if you use stopCheckAtFirst setting it will not call
		// click box behing the first even if it still colides with mouse 
		for (auto cbIt = ClickBoxes.rbegin(); cbIt != ClickBoxes.rend(); cbIt++) { 
			ClickBox* cb = *cbIt;
			if (!cb->IsOn()) { continue; }

			MT::Rect temprect{ event.button.x ,event.button.y,1,1 };
			if (!SimpleCollision(cb->GetRectangle(), temprect)) { continue; }
			cb->SetStatus(true);

			if (cb->GetClickSound() != "") { 
				SoundMan::PlaySound(cb->GetClickSound());
			}
			if (settings.stopCheckAtFirst) { break; }
		}
	}
}

Button* UI::GetButton(const std::string& name) {
	auto iter = UIElemMap.find(name);
	if (iter == UIElemMap.end()) {
		return nullptr;
	}
	UIElemBase* elem = iter->second;
	if (elem->castType == (int)CastType::Button) {
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
	if (elem->castType == (int)CastType::TextBox) {
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
	if (elem->castType == (int)CastType::ClickBox) {
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
	if (elem->castType == (int)CastType::PopUpBox) {
		return static_cast<PopUpBox*>(elem);
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

void UI::FrameUpdate() {
	for (auto it = PopUpBoxes.begin(); it != PopUpBoxes.end();) {
		(*it)->lifeTime--;
		if ((*it)->lifeTime < 1) {
			DeleteElement((*it)->name);
			return;
		}
		else {
			++it;
		}
	}
}

void UI::ManageInput(SDL_Event& event) {
	CheckHover();

	CheckTextBoxInteraction(event);

	ManageTextBoxTextInput(event);

	CheckClickBoxes(event);

	SDL_GetMouseState(&lastMousePos.x, &lastMousePos.y);
}


bool UI::DeleteElement(const std::string& name) {
	auto elemIter = UIElemMap.find(name);
	if (elemIter == UIElemMap.end()) { return false; }

	switch (elemIter->second->castType) {
		case (int)CastType::Button:
			if (!DeleteButton(static_cast<Button*>(elemIter->second))) {return false;}
			break;
		case (int)CastType::ClickBox:
			if (!DeleteClickBox(static_cast<ClickBox*>(elemIter->second))) { return false; }
			break;
		case (int)CastType::TextBox:
			if (!DeleteTextBox(static_cast<TextBox*>(elemIter->second))) { return false; }
			break;
		case (int)CastType::PopUpBox:
			if (!DeletePopUpBox(static_cast<PopUpBox*>(elemIter->second))) { return false; }
			break;
	}
	UIElemMap.erase(elemIter);
	return true;
}

bool UI::DeleteButton(Button* btn) {
	auto vecIter = std::find(Buttons.begin(), Buttons.end(),btn);
	if (vecIter != Buttons.end()) {
		delete *vecIter;
		Buttons.erase(vecIter);
		return true;
	}
	return false;
}

bool UI::DeleteTextBox(TextBox* tb) {
	auto vecIter = std::find(TextBoxes.begin(), TextBoxes.end(), tb);
	if (vecIter != TextBoxes.end()) {
		delete* vecIter;
		TextBoxes.erase(vecIter);
		return true;
	}
	return false;
}

bool UI::DeleteClickBox(ClickBox* cb) {
	auto vecIter = std::find(ClickBoxes.begin(), ClickBoxes.end(), cb);
	if (vecIter != ClickBoxes.end()) {
		delete* vecIter;
		ClickBoxes.erase(vecIter);
		return true;
	}
	return false;
}

bool UI::DeletePopUpBox(PopUpBox* pb) {
	auto vecIter = std::find(PopUpBoxes.begin(), PopUpBoxes.end(), pb);
	if (vecIter != PopUpBoxes.end()) {
		delete* vecIter;
		PopUpBoxes.erase(vecIter);
		return true;
	}
	return false;
}

std::vector<Button*>& UI::GetButtons() {
	return Buttons;
}

std::vector<TextBox*>& UI::GetTextBoxes() {
	return TextBoxes;
}

std::vector<ClickBox*>& UI::GetClickBoxes() {
	return ClickBoxes;
}

std::vector<PopUpBox*>& UI::GetPopUpBoxes() {
	return PopUpBoxes;
}

void UI::CreateFont(const std::string& name, MT::Texture* texture, const std::string& jsonPath) {
	fontManager->CreateFont(name, texture, jsonPath);
}

void UI::CrateTempFontFromTTF(const char* ttfPath, const int size, const std::string& name) {
	fontManager->CrateTempFontFromTTF(ttfPath, size, name);
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
	fontManager->ScanFont(texturePath, charactersDataPath, fR, fG, fB, bR, bG, bB, size.x, size.y);
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
		jsonElem["RenderType"] = 1;
	}
	else {
		jsonElem["RenderType"] = 2;
	}
	jsonElem["TextRenderType"] = elem->textRenderType;
	jsonElem["Hidden"] = elem->hidden;
	jsonElem["Hovered"] = elem->hovered;
	jsonElem["Hoverable"] = elem->hoverable;

	jsonElem["HoverFilterR"] = elem->hoverFilter.R;
	jsonElem["HoverFilterG"] = elem->hoverFilter.G;
	jsonElem["HoverFilterB"] = elem->hoverFilter.B;
	jsonElem["HoverFilterA"] = elem->hoverFilter.A;

	std::string hooverSoundStr = "";
	for (auto& sound : SoundMan::GetSounds()) {
		if (elem->hoverSound == sound.second) {
			hooverSoundStr = sound.first;
			break;
		}
	}
	if (!hooverSoundStr.empty()) {
		jsonElem["HoverSound"] = hooverSoundStr;
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
}

void UI::DumpPopUpBox(nlohmann::ordered_json& json, PopUpBox* pb) {
	auto& jsonElem = json[pb->GetName()];
	jsonElem["LifeTime"] = pb->lifeTime;
}

void UI::DumpToJson(const std::string &fileName, const std::vector<UIElemBase*>& elements) {
	std::ofstream file(fileName + ".json");
	if (!file.is_open()) {
		return;
	}
	
	nlohmann::ordered_json jsonFile;
	for (auto& elem : elements) {
		switch (elem->castType) {
			case (int)CastType::Button:
				DumpButton(jsonFile, elem);
				break;
			case (int)CastType::ClickBox:
				DumpButton(jsonFile, elem);
				DumpClickBox(jsonFile, static_cast<ClickBox*>(elem));
				break;
			case (int)CastType::TextBox:
				DumpButton(jsonFile, elem);
				DumpTextBox(jsonFile, static_cast<TextBox*>(elem));
				break;
			case (int)CastType::PopUpBox:
				DumpButton(jsonFile, elem);
				DumpPopUpBox(jsonFile, static_cast<PopUpBox*>(elem));
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
			Button* btn = CreateButton(key, 0, 0, 0, 0);
			*btn = *static_cast<Button*>(elem.get());
			loadedElements.emplace_back(btn);
		}
		else if (type == (int)CastType::ClickBox) {
			ClickBox* cb = CreateClickBox(key, 0, 0, 0, 0);
			*cb = *static_cast<ClickBox*>(elem.get());
			cb->turnedOn = val["TurnedOn"];
			cb->clickSound = val["ClickSound"];
			loadedElements.emplace_back(cb);
		}
		else if (type == (int)CastType::TextBox) {
			TextBox* tb = CreateTextBox(key, 0, 0, 0, 0);
			*tb = *static_cast<TextBox*>(elem.get());
			tb->turnedOn = val["TurnedOn"];
			loadedElements.emplace_back(tb);;
		}
		else if (type == (int)CastType::PopUpBox) {
			PopUpBox* pb = CreatePopUpBox(key, 0, 0, 0, 0, 0);
			*pb = *static_cast<PopUpBox*>(elem.get());
			pb->lifeTime = val["LifeTime"];
			loadedElements.emplace_back(pb);
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

	for (auto& it : Buttons) {
		delete it;
	}
	for (auto& it : TextBoxes) {
		delete it;
	}
	for (auto& it : ClickBoxes) {
		delete it;
	}
	for (auto& it : PopUpBoxes) {
		delete it;
	}
	Buttons.clear();
	TextBoxes.clear();
	ClickBoxes.clear();
	PopUpBoxes.clear();

	UIElemMap.clear();
}



UI::~UI() {
	ClearAll();
	delete fontManager;
}
