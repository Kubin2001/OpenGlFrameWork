#include "Files.h"

#include <fstream>
#include <print>
#include <filesystem>

std::string BreakLines(const std::string& input, std::size_t every) {
	std::string out;
	out.reserve(input.size() + input.size() / every);

	for (size_t i = 0; i < input.size(); ++i) {
		if (i > 0 && i % every == 0) {
			out += '\n';
		}		
		out += input[i];
	}
	return out;
}

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

FileExplorer::FileExplorer(int scroolSpeed, const std::vector<std::string> extensionFilter) {
	this->scroolSpeed = scroolSpeed;
	this->extensionFilter = extensionFilter;
}

void FileExplorer::CreateElement(int x, int y, const std::string& dirPath, const std::string& dirName, const std::string& texture
	, const std::string &extension) {

	MT::Texture *usedTexture = nullptr;
	auto extIter = extensionsTextures.find(extension);
	if (extIter == extensionsTextures.end()) {
		usedTexture = texMan.GetTex(texture);
	}
	else {
		usedTexture = extIter->second;
	}

	folderElements.emplace_back(ui->CreateClickBox(dirPath, x, y, 20, 20, usedTexture));
	ClickBox* cb = folderElements.back();
	cb->SetHoverFilter(1, 255, 255, 255, 70);

	folderElementsNames.emplace_back(ui->CreateClickBox("Name" + dirPath, x + 20, y, 300 - 75, 20,
		nullptr, ui->GetFont("arial12px"), dirName,1.0f,5));
	cb = folderElementsNames.back();
	cb->SetColor(255, 255, 255, 0);
	cb->SetHoverFilter(true, 255, 255, 255, 70);
	cb->SetRenderTextType((int)TextRenderType::CenteredY);
}

static std::unordered_map<std::string, MT::Texture*> LoadExtensionTextures(LocalTexMan *texMan) {
	std::unordered_map<std::string, MT::Texture*> extensions;
	if (!std::filesystem::exists("Common")) {
		std::filesystem::create_directory("Common");
		return extensions;
	}
	if (!std::filesystem::exists("Common/fileSystemExtensions.csv")) {
		std::ofstream file("Common/fileSystemExtensions.csv");
		return extensions;
	}
	std::ifstream file("Common/fileSystemExtensions.csv");
	std::vector<std::vector<std::string>> readedTexturesNames =  ReadCsv("Common/fileSystemExtensions.csv");
	for (auto& csvLine : readedTexturesNames) {
		if (csvLine.size() != 2) { continue; }

		std::string extension = csvLine[0];
		std::string texName = csvLine[1];
		MT::Texture* tex = texMan->GetTex(texName);
		if (tex != nullptr) {
			extensions[extension] = tex;
		}
	}
	return extensions;
}

std::string FileExplorer::Open(const std::string& path) {
	MT::ConstextGuard cg;
	window = SDL_CreateWindow("FileWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		300, 300, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_OPENGL);
	renderer = new MT::Renderer();
	renderer->Start(window, MT::Init(window));

	std::filesystem::path current;
	if (std::filesystem::exists(path)) {
		current = path;
	}
	else {
		current = std::filesystem::current_path();
	}

	currentPath = current.string();
	texMan.Start(renderer);
	texMan.DeepLoad("Textures/FileExplorer");
	ui = new UI(renderer);
	ui->CrateTempFontFromTTF("Fonts/arial.ttf", 12, "arial12px",&texMan);

	extensionsTextures = LoadExtensionTextures(&texMan);

	ClickBox *cb =  ui->CreateClickBox("ArrowLeft", 10, 10, 30, 20, nullptr, ui->GetFont("arial12px"), "<-");
	cb->SetColor(60, 60, 60);
	cb = ui->CreateClickBox("RetPathCheckBox", 10, 50, 30, 30);
	cb->SetColor(255, 0, 0);
	cb->SetHoverFilter(true, 255, 255, 255, 120);

	selectedBox = ui->CreateButton("selectionButton", 50, 100, 300, 20, nullptr);
	selectedBox->SetColor(135, 206, 250, 150);

	Update();
	return Maintain();
}

std::string FileExplorer::Maintain() {
	while (!finished) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		Input();
		//Update();
		renderer->ClearFrame(30, 30, 30);
		ui->FrameUpdate();
		ui->Render();
		renderer->Present();
		SDL_Delay(32);
	}

	ui->ClearAll();
	texMan.Clear();
	renderer->Clear();
	delete renderer;
	SDL_DestroyWindow(window);
	return retPath;
}

void FileExplorer::Input() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_MOUSEWHEEL) {
			if (event.wheel.y > 0 && absoluteY < 50) { // up
				absoluteY += scroolSpeed;
				for (auto &it:folderElements) {
					it->GetRectangle().y+= scroolSpeed;
				}
				for (auto& it : folderElementsNames) {
					it->GetRectangle().y += scroolSpeed;
				}
				selectedBox->GetRectangle().y += scroolSpeed;
			}
			else if (event.wheel.y < 0 && std::abs(absoluteY) < (folderElements.back()->GetRectangle().y +
				folderElements.back()->GetRectangle().h)) { //down
				absoluteY -= scroolSpeed;
				for (auto& it : folderElements) {
					it->GetRectangle().y-= scroolSpeed;
				}
				for (auto& it : folderElementsNames) {
					it->GetRectangle().y -= scroolSpeed;
				}
				selectedBox->GetRectangle().y -= scroolSpeed;
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

	ClickBox* retPathCb = ui->GetClickBox("RetPathCheckBox");
	if (retPathCb->ConsumeStatus()) {
		retPath = "";
	}
	if (retPath.empty()) {
		retPathCb->SetColor(255, 0, 0);
	}
	else {
		retPathCb->SetColor(0, 255, 0);
	}


	for (size_t i = 0; i < folderElements.size(); ++i) {
		if (folderElements[i]->ConsumeStatus() || folderElementsNames[i]->ConsumeStatus()) {
			std::string temp = std::filesystem::path(folderElements[i]->GetName()).string();
			if (selectedElement == nullptr) {
				selectedElement = folderElements[i];
				selectedBox->Show();
				selectedBox->GetRectangle().y = selectedElement->GetRectangle().y;
				retPath = temp;
				break;
			}
			if (selectedElement == folderElements[i]) {

				if (folderElements[i] != selectedElement) { continue; }

				if (std::filesystem::is_directory(temp)) {
					currentPath = temp;
					retPath = temp;
					Update();
				}
				else {
					retPath = temp;
				}
			}
			if (selectedElement != nullptr) {
				selectedElement = folderElements[i];
				selectedBox->GetRectangle().y = selectedElement->GetRectangle().y;
				selectedBox->Show();
				retPath = temp;
			}
		}
	}
	if (ClickBox* cb = ui->GetClickBox("RetPathCheckBox")) {
		if (cb->IsHovered() && !retPath.empty()) {
			Point mouse = GetMousePos();
			std::string breakPath = BreakLines(retPath,35);
			ui->RenderRawText(ui->GetFont("arial12px"), mouse.x, mouse.y, breakPath, 15, 200, 200, 200);
		}
	}
}

bool FileExplorer::ExtensionAllowed(const std::string& ext) {
	if (extensionFilter.empty()) {
		return true;
	}
	for (auto& extension : extensionFilter) {
		if (ext == extension) {
			return true;
		}
	}
	return false;
}

void FileExplorer::Update() {
	for (auto& elem : folderElements) {
		ui->DeleteElement(elem->GetName());
	}
	for (auto& elem : folderElementsNames) {
		ui->DeleteElement(elem->GetName());
	}
	folderElements.clear();
	folderElementsNames.clear();
	int x = 50;
	int y = 10;

	for (auto& dir : std::filesystem::directory_iterator(currentPath)) {
		std::filesystem::path path = dir.path();
		if (dir.is_directory()) {
			CreateElement(x, y, path.string(), path.filename().string(), "FeFolderIcon", path.extension().string());
		}
		else {
			if (ExtensionAllowed(path.extension().string())) {
				CreateElement(x, y, path.string(), path.filename().string(), "FeFileIcon", path.extension().string());
			}
			else {
				continue;
			}
		}
		folderElements.back()->SetHoverFilter(1, 255, 255, 255,70);
		y += 35;
	}
	selectedElement = nullptr;
	selectedBox->Hide();
}