#include "SceneManager.h"


void SceneMan::Print() {
	std::cout << "------------------------\n";
	std::cout << "Loaded Scenes Names: \n";
	std::cout << "------------------------\n";
	for (auto it = Scenes.begin(); it != Scenes.end(); ++it) {
		std::cout << it->first << "\n";
	}
	std::cout << "------------------------\n";
}


void SceneMan::Clear() {
	for (auto& pair : Scenes) {
		delete pair.second;
	}
	Scenes.clear();
	currentScene = nullptr;
}


Scene* SceneMan::GetCurrentScene() {
	return currentScene;
}

bool SceneMan::IsData(const std::string& key) {
	if (SharedData.find(key) != SharedData.end()) {
		return true;
	}
	return false;
}

void SceneMan::ClearData(const std::string& key) {
	if (SharedData.find(key) != SharedData.end()) {
		SharedData.erase(key);
	}
}

void SceneMan::ClearAllData() {
	SharedData.clear();
}
