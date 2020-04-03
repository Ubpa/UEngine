#pragma once


#include "SceneMngr.h"
#include "_deps/imgui/imgui.h"

#define UBPA_UGL_GLAD 1
#include <UGL/UGL>

struct GLFWwindow;

#include <functional>

namespace Ubpa {
	class DeferredRenderer;

	class Engine {
	public:
		static Engine& Instance() {
			static Engine instance;
			return instance;
		}

		bool Init(const std::string& title);
		void Loop(std::function<void()> func);
		void CleanUp();

	private:
		Engine() = default;

		DeferredRenderer* rtr;
		GLFWwindow* window{ nullptr };
	};
}
