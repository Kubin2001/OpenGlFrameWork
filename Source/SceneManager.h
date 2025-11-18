#pragma once

#include <unordered_map>
#include <memory>
#include <functional>
#include <stdexcept>


#include "UI.h"



class Scene {
	protected:
		UI* ui = nullptr;
		MT::Renderer *renderer = nullptr;

	public:
		std::string name;

		virtual void Init() = 0;

		virtual void LogicUpdate() = 0;

		virtual void FrameUpdate() = 0;

		virtual void Input(SDL_Event& event) = 0;

		virtual void Render() = 0;

		virtual void Clear() = 0;

		virtual ~Scene() = default;

		friend class SceneMan;
};

class SceneMan {
	private:
		inline static Scene* currentScene = nullptr;

		inline static std::unordered_map<std::string, Scene*> Scenes;

		inline static std::unordered_map<std::string, std::unique_ptr<AnyData>> SharedData;

	public:

		static void Print();

		template <typename T>
		static void AddScene(const std::string& sceneName) {
			static_assert(std::is_base_of_v<Scene, T>, "Type must be base of Scene class");
			if (Scenes.find(sceneName) == Scenes.end()) {
				T* scene = new T;
				scene->name = sceneName;
				Scenes.insert(std::make_pair(sceneName, scene));
			}
			else {
				std::cerr << "ERROR: Scene '" << sceneName << "' already exists!\n";
			}
		}

		template <typename T>
		static void SwitchScene(const std::string& sceneName, MT::Renderer* renderer = nullptr, UI* ui = nullptr, bool reset = true) {
			static_assert(std::is_base_of_v<Scene, T>, "Type must be base of Scene class");
			auto foundScene = Scenes.find(sceneName);
			if (foundScene == Scenes.end()) {
				std::cerr << "ERROR: Scene '" << sceneName << "' not found!\n";
				return;
			}
			if (foundScene->second == nullptr) {
				foundScene->second = new T;
			}
			if(currentScene != nullptr){
				currentScene->Clear();
				if (reset) {
					Scenes[currentScene->name] = nullptr;
					delete currentScene;
				}
			}

			currentScene = foundScene->second;
			currentScene->name = sceneName;
			currentScene->renderer = renderer;
			currentScene->ui = ui;
			currentScene->Init();
		}

		static void Clear();

		static Scene* GetCurrentScene();

		template <typename T>
		static void AddData(const std::string& key, T data) {
			SharedData[key] = std::make_unique <AnyContatiner<T>>();
			SharedData[key]->Set(data);
		}

		template <typename T>
		static T& GetData(const std::string& key) {
			auto data = SharedData.find(key);
			if (data == SharedData.end()) {
				throw std::runtime_error("Data does not exist in GetData");
			}
			else {
				return data->second->Get<T>();
			}
		}

		static bool IsData(const std::string& key);

		static void ClearData(const std::string& key);

		static void ClearAllData();
};
