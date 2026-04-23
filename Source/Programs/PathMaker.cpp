#include "PathMaker.h"

#include <fstream>

#include "Files.h"

#include <thread>

static void CreateErrorBox(UI* ui, const std::string& text) {
	PopUpBox *pb =  ui->CreatePopUpBoxF("erroxBox" + std::to_string(RandInt(0, 1000)), 120, 200, 5, 100, 40,nullptr,"arial12px");
	pb->SetColor(120, 120, 120);
	pb->SetText(text);
	pb->SetRenderTextType(TextRenderType::Centered);
}

static void CreateInfoBox(UI* ui, const std::string& text) {
	PopUpBox* pb = ui->CreatePopUpBoxF("erroxBox" + std::to_string(RandInt(0, 1000)), 120, 200, 5, 100, 40, nullptr, "arial12px");
	pb->SetColor(120, 120, 120);
	pb->SetText(text);
	pb->SetRenderTextType(TextRenderType::Centered);
}



void PathMaker::CreateSaveMenu() {
	if (!saveSection.GetClickBoxes().empty()) { return; }
	Label* lb = ui->CreateLabelF("saveBack", 50, 50, 190, 200, nullptr, "arial20px", "Saving", 1.0f, 0, 10);
	lb->SetRenderTextType(TextRenderType::CenteredX); 
	lb->SetRenderType(RenderType::Rounded);
	lb->SetColor(40, 40, 40, 255); 
	lb->SetBorder(2, 70, 160, 255); 
	lb->SetFontColor(40, 255, 255);
	saveSection.Add(lb);

	ClickBox* cb = ui->CreateClickBoxF("saveTxt", 60, 100, 40, 40, nullptr, "arial12px", "Txt", 1.0f, 0, -15);
	cb->SetRenderTextType(TextRenderType::CenteredX);
	cb->SetRenderType(RenderType::Rounded);
	cb->SetColor(0, 255, 0, 255);
	cb->SetBorder(2, 70, 160, 255);
	cb->SetFontColor(40, 255, 255);
	saveSection.Add(cb);

	cb = ui->CreateClickBoxF("saveCsv", 125, 100, 40, 40, nullptr, "arial12px", "Csv", 1.0f, 0, -15);
	cb->SetRenderTextType(TextRenderType::CenteredX);
	cb->SetRenderType(RenderType::Rounded);
	cb->SetColor(40, 40, 40, 255);
	cb->SetBorder(2, 70, 160, 255);
	cb->SetFontColor(40, 255, 255);
	saveSection.Add(cb);

	cb = ui->CreateClickBoxF("saveBin", 190, 100, 40, 40, nullptr, "arial12px", "Bin", 1.0f, 0, -15);
	cb->SetRenderTextType(TextRenderType::CenteredX);
	cb->SetRenderType(RenderType::Rounded);
	cb->SetColor(40, 40, 40, 255);
	cb->SetBorder(2, 70, 160, 255);
	cb->SetFontColor(40, 255, 255);
	saveSection.Add(cb);

	TextBox* tb = ui->CreateTextBoxF("saveName", 90, 150, 110, 40, nullptr, "arial20px", "Name"); 
	tb->SetRenderTextType(TextRenderType::Centered);
	tb->SetRenderType(RenderType::Rounded);
	tb->SetColor(70, 70, 70, 255);
	tb->SetBorder(2, 70, 160, 255);
	saveSection.Add(tb);

	cb = ui->CreateClickBoxF("saveConf", 125, 200, 40, 30, nullptr, "arial12px", "Save");
	cb->SetRenderTextType(TextRenderType::Centered);
	cb->SetRenderType(RenderType::Rounded);
	cb->SetColor(40, 40, 40, 255);
	cb->SetBorder(2, 70, 160, 255);
	cb->SetFontColor(40, 255, 255);
	cb->SetHoverFilter(true, 255, 255, 255, 120);
	saveSection.Add(cb);
}

void PathMaker::InputSaveMenu() {
	if (ui->ConsumeIfExist("saveTxt")) {
		ui->GetClickBox("saveTxt")->SetColor(0, 255, 0);
		ui->GetClickBox("saveCsv")->SetColor(40, 40, 40);
		ui->GetClickBox("saveBin")->SetColor(40, 40, 40);
		saveState = 0;
	}
	if (ui->ConsumeIfExist("saveCsv")) {
		ui->GetClickBox("saveTxt")->SetColor(40, 40, 40);
		ui->GetClickBox("saveCsv")->SetColor(0, 255, 0);
		ui->GetClickBox("saveBin")->SetColor(40, 40, 40);
		saveState = 1;
	}
	if (ui->ConsumeIfExist("saveBin")) {
		ui->GetClickBox("saveTxt")->SetColor(40, 40, 40);
		ui->GetClickBox("saveCsv")->SetColor(40, 40, 40);
		ui->GetClickBox("saveBin")->SetColor(0, 255, 0);
		saveState = 2;
	}
	if (ui->ConsumeIfExist("saveConf")) {
		switch (saveState) {
			case 0: //txt
				SaveTxt();
				break;
			case 1: //csv
				SaveCsv();
				break;
			case 2: //bin
				SaveBin();
				break;
		}
	}
}

void PathMaker::SaveTxt() {
	std::ofstream file(ui->GetTextBox("saveName")->GetText() + ".txt");
	if (!file.is_open()) {
		CreateErrorBox(ui, "Cannot save to txt");
		return;
	}
	for (auto& point : path) {
		file << std::to_string(point.x)<<"\n";
		file << std::to_string(point.y)<<"\n";
	}
	CreateInfoBox(ui, "Saved to txt");
}

void PathMaker::SaveCsv() {
	std::ofstream file(ui->GetTextBox("saveName")->GetText() + ".csv");
	if (!file.is_open()) {
		CreateErrorBox(ui, "Cannot save to csv");
		return;
	}
	for (auto& point : path) {
		file << std::to_string(point.x) << ","<< std::to_string(point.y) << "\n";
	}
	CreateInfoBox(ui, "Saved to csv");
}

void PathMaker::SaveBin() {
	std::ofstream file(
		ui->GetTextBox("saveName")->GetText() + ".bin",
		std::ios::binary
	);

	if (!file.is_open()) {
		CreateErrorBox(ui, "Cannot save to bin");
		return;
	}
	size_t count = path.size();
	file.write(reinterpret_cast<char*>(&count), sizeof(count));

	file.write(
		reinterpret_cast<char*>(path.data()),
		sizeof(Point) * count
	);
	CreateInfoBox(ui, "Saved to bin");
}

std::vector<Point> PathMaker::LoadTxt(const std::string& path){
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("PathMaker::LoadTxt Cannot open file: " + path);
	}

	std::vector<Point> retPath;
	std::string line;
	while (std::getline(file, line)) {
		try {
			retPath.emplace_back();
			retPath.back().x = std::stoi(line);
			std::getline(file, line);
			retPath.back().y = std::stoi(line);
		}
		catch (const std::exception& e) {
			std::println("Cannot convert this file to path vector {} ERROR: {}", path, e.what());
			return{};
		}
	}
	return retPath;
}


std::vector<Point> PathMaker::LoadCsv(const std::string& path) {
	std::vector<std::vector<std::string>> csv = ReadCsv(path, ',');
	std::vector<Point> retPath;

	try {
		for (auto& line : csv) {
			retPath.emplace_back();
			retPath.back().x = std::stoi(line.at(0));
			retPath.back().y = std::stoi(line.at(1));
		}
	}
	catch (const std::exception&) {
		std::println("Cannot convert this file to path vector {}", path);
		return{};
	}
	return retPath;
}


std::vector<Point> PathMaker::LoadBin(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("PathMaker::LoadBin Cannot open file: " + path);
	}

	size_t count = 0;
	file.read(reinterpret_cast<char*>(&count), sizeof(count));

	if (!file || count == 0) {
		return {};
	}

	std::vector<Point> retPath;
	retPath.resize(count);

	file.read(reinterpret_cast<char*>(retPath.data()),sizeof(Point) * count);

	if (!file) {
		std::println("Cannot convert this file to path vector {}", path);
		return {};
	}

	return retPath;
}

void PathMaker::Input() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
			Uint32 myWindowID = SDL_GetWindowID(window);
			if (event.window.windowID == myWindowID) {
				finished = true;
			}
		}
		ui->ManageInput(event);
		if (event.type == SDL_KEYUP) {

			switch (programState) {
				case 0:
					if (event.key.keysym.scancode == SDL_SCANCODE_R) {
						programState = 1;
						startPoint = GetMousePos();
						currentPoint = GetMousePos();
						statusText = "Drawing press R to stop";
					}

					break;
				case 1:
					if (event.key.keysym.scancode == SDL_SCANCODE_R) {
						programState = 2;
						statusText = "Press R to cancel or S to save";
						if (compressEnd) {
							while (!path.empty()) {
								if (path.back().x == 0 && path.back().y == 0) {
									path.pop_back();
								}
								else {
									break;
								}
							}
						}
					}


					break;
				case 2:
					if (event.key.keysym.scancode == SDL_SCANCODE_R) {
						TextBox* tb = ui->GetTextBox("saveName");
						if (tb != nullptr && tb->IsUsed()) {
							break;
						}
						statusText = "Press R to draw";
						path.clear();
						programState = 0;
						saveSection.Clear();
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_S) {
						CreateSaveMenu();
					}
					break;
			}
		}
	}
}

void PathMaker::FrameUpdate() {
	ui->FrameUpdate();
	switch (programState) {
		case 0: // waiting

			break;
		case 1: { // drawing
			Point prev = currentPoint;
			currentPoint = GetMousePos();
			Point newPoint{ currentPoint.x - prev.x, currentPoint.y - prev.y };
			if (compressStart) {
				if (startPoint == currentPoint) { break; }
			}
			if (compressZeros) {
				if (newPoint.x == 0 && newPoint.y == 0) {
					break;
				}
			}
			path.emplace_back(newPoint);
			break;
		}
		case 2: // saving

			break;
	}
	InputSaveMenu();
}

void PathMaker::Render() {
	ren->ClearFrame(30, 30, 30);
	switch (programState) {
		case 0: // waiting

			break;
		case 1: { // drawing
			Point curPoint = startPoint;
			for (size_t i = 0; i < path.size(); i++) {

				if (i + 1 < path.size() - 1) {
					ren->DrawLine(curPoint.x, curPoint.y, curPoint.x + path[i + 1].x, curPoint.y + path[i + 1].y, 3, { 100,255,100 });
					curPoint.x += path[i + 1].x;
					curPoint.y += path[i + 1].y;
				}
			}
			break;
		}
		case 2: // saving
			Point curPoint = startPoint;
			for (size_t i = 0; i < path.size(); i++) {

				if (i + 1 < path.size() - 1) {
					ren->DrawLine(curPoint.x, curPoint.y, curPoint.x + path[i + 1].x, curPoint.y + path[i + 1].y, 3, { 100,255,100 });
					curPoint.x += path[i + 1].x;
					curPoint.y += path[i + 1].y;
				}
			}
			break;
	}
	ui->RenderRawText(ui->GetFont("arial12px"), 10, 10, statusText, 0, 230, 230, 230);
	ui->Render();
	ren->Present();
}

void PathMaker::Maintain() {
	while(!finished){
		Input();
		FrameUpdate();
		Render();
		std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60 frames
	}
}

void PathMaker::Open(bool compressStart, bool compressEnd, bool compressZeros, int windowW, int windowH) {
	MT::ConstextGuard cg;
	window = SDL_CreateWindow("Path Maker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		windowW, windowH, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_OPENGL);
	ren = new MT::Renderer();
	ren->Start(window, MT::Init(window));

	texMan.Start(ren);
	ui = new UI(ren);
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial12px", &texMan);
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial20px", &texMan);

	statusText = "Press R to start";
	saveSection.Init(ui);

	this->compressStart = compressStart;
	this->compressEnd = compressEnd;
	this->compressZeros = compressZeros;

	Maintain();

	ui->ClearAll();
	texMan.Clear();
	ren->Clear();
	delete ren;
	SDL_DestroyWindow(window);
}