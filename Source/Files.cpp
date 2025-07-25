#include "Files.h"
#include <fstream>
#include <iostream>

std::vector<std::string> ReadCsvLine(const std::string& line, const char separator) {
	std::vector<std::string> items;
	std::string temp = "";
	for (size_t i = 0; i < line.length(); i++) {
		if (line[i] != separator) {
			temp += line[i];
		}
		else {
			items.emplace_back(temp);
			temp.clear();
		}
	}
	items.emplace_back(temp);
	return items;
}

std::vector<std::vector<std::string>> ReadCsv(const std::string& path, const char separator) {
	std::vector<std::vector<std::string>> csvVec;
	std::ifstream csvFile(path);
	std::string line;
	if (csvFile.is_open()) {
		while (std::getline(csvFile, line)) {
			csvVec.emplace_back(ReadCsvLine(line, separator));
		}
		csvFile.close();
	}
	else {
		std::cout << "error reading file\n";
	}

	return csvVec;
}

void FileExplorer::CreateElement(int x, int y, const std::string& dirPath, const std::string& dirName, const std::string& texture) {
	folderElements.emplace_back(ui->CreateClickBox(dirPath, x, y, 20, 20,
		texMan.GetTex(texture), ui->GetFont("arial12px")
		, dirName, 1.0f, 25, 5));
	folderElements.back()->SetHoverFilter(1, 255, 255, 255, 70);

	folderElementsNames.emplace_back(ui->CreateClickBox("Name" + dirPath, x + 20, y, 300 - 75, 20,
		nullptr, ui->GetFont("arial12px")
		, "", 1.0f, 25, 5));
	folderElementsNames.back()->SetHoverFilter(1, 255, 255, 255, 70);
}

std::string FileExplorer::Open(const std::string& path) {
	window = SDL_CreateWindow("FileWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		300, 300, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);

	std::filesystem::path current = std::filesystem::current_path();
	currentPath = current.string();
	texMan.Start(renderer);
	texMan.LoadMultiple("Textures/FileExplorer");
	ui = new UI(renderer);
	ui->CreateFont("arial12px", texMan.GetTex("arial12px"), "Textures/Interface/Fonts/arial12px.json");

	int x = 50;
	int y = 10;

	for (auto &dir: std::filesystem::directory_iterator(currentPath)) {
		if (dir.is_directory()) {
			CreateElement(x, y, dir.path().string(), dir.path().filename().string(), "FeFolderIcon");
		}
		else {
			CreateElement(x, y, dir.path().string(), dir.path().filename().string(), "FeFileIcon");
		}
		y += 35;	
	}

	ui->CreateClickBox("ArrowLeft", 10, 10, 30, 20, nullptr, ui->GetFont("arial12px"), "<-");
	ui->GetClickBox("ArrowLeft")->SetColor(60, 60, 60);
	return Maintain();
}

std::string FileExplorer::Maintain() {
	while (!finished) {
		Input();
		//Update();
		SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
		SDL_RenderClear(renderer);
		ui->Render();
		SDL_RenderPresent(renderer);
		SDL_Delay(32);
	}

	ui->ClearAll();
	texMan.Clear();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return retPath;
}

void FileExplorer::Input() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_MOUSEWHEEL) {
			if (event.wheel.y > 0 && absoluteY < 50) { // up
				absoluteY += 10;
				for (auto &it:folderElements) {
					it->GetRectangle()->y+=10;
				}
			}
			else if (event.wheel.y < 0 && std::abs(absoluteY) < (folderElements.back()->GetRectangle()->y +
				folderElements.back()->GetRectangle()->h)) { //down
				absoluteY -= 10;
				for (auto& it : folderElements) {
					it->GetRectangle()->y-=10;
					
				}
			}
		}
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
			Uint32 myWindowID = SDL_GetWindowID(window);
			if (event.window.windowID == myWindowID) {
				finished = true;
			}
		}
		ui->ManageInput(event);
	}

	if (ui->GetClickBox("ArrowLeft")->ConsumeStatus()) {
		currentPath = std::filesystem::path(currentPath).parent_path().string();
		Update();
	}

	for (size_t i = 0; i < folderElements.size(); ++i) {
		if (folderElements[i]->ConsumeStatus() || folderElementsNames[i]->ConsumeStatus()) {
			if (selectedElement == nullptr) {
				selectedElement = folderElements[i];
				break;
			}
			if (folderElements[i] == selectedElement) {
				std::string temp = std::filesystem::path(folderElements[i]->GetName()).string();
				if (std::filesystem::is_directory(temp)) {
					currentPath = temp;
					retPath = temp;
					Update();
				}
				else {
					retPath = temp;
				}
				break;
			}
		}
	}
}

void FileExplorer::Update() {
	for (auto& elem : folderElements) {
		ui->DeleteClickBox(elem->GetName());
	}
	for (auto& elem : folderElementsNames) {
		ui->DeleteClickBox(elem->GetName());
	}
	folderElements.clear();
	folderElementsNames.clear();
	int x = 50;
	int y = 10;

	for (auto& dir : std::filesystem::directory_iterator(currentPath)) {
		if (dir.is_directory()) {
			CreateElement(x, y, dir.path().string(), dir.path().filename().string(), "FeFolderIcon");
		}
		else {
			CreateElement(x, y, dir.path().string(), dir.path().filename().string(), "FeFileIcon");
		}
		folderElements.back()->SetHoverFilter(1, 255, 255, 255,70);
		y += 35;
	}
	selectedElement = nullptr;
}