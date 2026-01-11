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

//class SceneOne : public Scene {
//	public:
//	void Init() override {
//		std::cout << ">>> [SceneOne] Init\n";
//	}
//
//	void Clear() override {
//		std::cout << ">>> [SceneOne] Clear\n";
//	}
//
//
//	void LogicUpdate() override {}
//	void FrameUpdate() override {}
//	void Input(SDL_Event& event) override {}
//	void Render() override {}
//};
//
//class SceneTwo : public Scene {
//	public:
//	void Init() override {
//		std::cout << ">>> [SceneTwo] Init\n";
//	}
//
//	void Clear() override {
//		std::cout << ">>> [SceneTwo] clear\n";
//	}
//
//
//	void LogicUpdate() override {}
//	void FrameUpdate() override {}
//	void Input(SDL_Event& event) override {}
//	void Render() override {}
//};

class SceneMan {
	private:
		inline static Scene* currentScene = nullptr;

		inline static std::unordered_map<std::string, std::unique_ptr<Scene>> Scenes;

		inline static std::unordered_map<std::string, std::unique_ptr<AnyData>> SharedData;

	public:

		static void Print();

		template <typename T>
		static void AddScene(const std::string& sceneName) {
			static_assert(std::is_base_of_v<Scene, T>, "Type must be base of Scene class");
			if (Scenes.find(sceneName) == Scenes.end()) {
				std::unique_ptr<T> scene = std::make_unique<T>();
				scene->name = sceneName;
				Scenes[sceneName] = std::move(scene);
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
				foundScene->second = std::make_unique<T>();
				foundScene->second->name = sceneName;
			}
			if(currentScene != nullptr){
				currentScene->Clear();
				if (reset) {
					Scenes[currentScene->name].reset();
					currentScene = nullptr;
				}
			}

			currentScene = foundScene->second.get();
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
