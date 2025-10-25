#pragma once
#include "UI.h"


std::vector<std::string> ReadCsvLine(const std::string& line, const char separator = ',');

std::vector<std::vector<std::string>> ReadCsv(const std::string& path, const char separator = ',');


class FileExplorer {
	private:
		SDL_Window* window = nullptr;
		MT::Renderer* renderer = nullptr;
		SDL_Event event = {};

		std::string currentPath = "";
		std::string retPath = "";
		bool finished = false;
		int absoluteY = 0;

		UI* ui = nullptr;

		LocalTexMan texMan;

		std::vector<ClickBox*> folderElements;
		std::vector<ClickBox*> folderElementsNames;

		Button* selectedBox = nullptr; // Background hoover to selected element

		ClickBox* selectedElement = nullptr; // Reference to already selected folder element

		std::string Maintain();

		void Input();

		void Update();

		void CreateElement(int x, int y, const std::string& dirPath, const std::string& dirName, const std::string& texture);

	public:
		FileExplorer() = default;

		std::string Open(const std::string& path = "");



};