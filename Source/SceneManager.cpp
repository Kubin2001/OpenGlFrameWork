#include "SceneManager.h"
#include "iostream"

std::unordered_map<std::string, Scene*> SceneMan::Scenes;
std::unordered_map<std::string, std::unique_ptr<AnyData>> SceneMan::SharedData;
Scene* SceneMan::currentScene = nullptr;

std::unordered_map<std::string, std::function<Scene* ()>> SceneFactory::factories;


void SceneMan::Print() {
	std::cout << "------------------------\n";
	std::cout << "Loaded Scenes Names: \n";
	std::cout << "------------------------\n";
	for (auto it = Scenes.begin(); it != Scenes.end(); ++it) {
		std::cout << it->first << "\n";
	}
	std::cout << "------------------------\n";
}

void SceneMan::AddScene(Scene* scene, const std::string& sceneName) {
	if (Scenes.find(sceneName) == Scenes.end()) {
		scene->name = sceneName;
		Scenes.insert(std::make_pair(sceneName, scene));
	}
	else
	{
		std::cerr << "ERROR: Scene '" << sceneName << "' already exists!\n";
	}

}

void SceneMan::AddRegisterScene(Scene* scene, const std::string& sceneName, std::function<Scene* ()> factoryFn) {
	if (Scenes.find(sceneName) == Scenes.end()) {
		scene->name = sceneName;
		Scenes.insert(std::make_pair(sceneName, scene));
		SceneFactory::RegisterScene(sceneName, factoryFn);
	}
	else
	{
		std::cerr << "ERROR: Scene '" << sceneName << "' already exists!\n";
	}

}

void SceneMan::SetScene(const std::string& sceneName) {
	if (Scenes.find(sceneName) != Scenes.end()) {
		currentScene = Scenes[sceneName];

	}
	else
	{
		std::cerr << "ERROR: Scene '" << sceneName << "' not found!\n";
	}
}

void SceneMan::SwitchScene(const std::string& sceneName, SDL_Renderer* renderer, UI* ui) {
	if (Scenes.find(sceneName) != Scenes.end()) {
		if (currentScene != nullptr) {
			currentScene->Clear();
		}
		currentScene = Scenes[sceneName];
		currentScene->Init(renderer, ui);

	}
	else
	{
		std::cerr << "ERROR: Scene '" << sceneName << "' not found!\n";
	}
}

void SceneMan::SwitchResetScene(const std::string& sceneName, SDL_Renderer* renderer, UI* ui) {
	if (Scenes.find(sceneName) != Scenes.end()) {
		if (currentScene != nullptr) { // usuwanie obecnej sceny
			currentScene->Clear();
			Scenes[currentScene->name] = nullptr;
			delete currentScene;
		}


		currentScene = Scenes[sceneName];

		if (currentScene == nullptr) {
			Scenes[sceneName] = SceneFactory::CreateScene(sceneName);
			currentScene = Scenes[sceneName];
			Scenes[sceneName]->name = sceneName;

			currentScene->name = sceneName;
		}
		currentScene->Init(renderer, ui);

	}
	else
	{
		std::cerr << "ERROR: Scene '" << sceneName << "' not found!\n";
	}
}

void SceneMan::Clear() {
	for (auto& pair : Scenes) {
		delete pair.second;
	}
	Scenes.clear();
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


void SceneFactory::RegisterScene(const std::string& shapeName, std::function<Scene* ()> factoryFn) {
	factories[shapeName] = factoryFn;
}


Scene* SceneFactory::CreateScene(const std::string& shapeName) {
	if (factories.count(shapeName)) {
		return factories[shapeName]();
	}
	return nullptr;
}